<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

//  phpESP LDAP Authentication Library
//  By: Christopher Zorn <zorncj@musc.edu>
//      James Flemer <jflemer@alum.rpi.edu>

	$_ext = 'ldap.so';
	if (substr(PHP_OS, 0, 3) == 'WIN') {
		$_ext = 'php_ldap.dll';
	}
	if (!extension_loaded('ldap') && !ini_get('safe_mode')
		&& ini_get('enable_dl') && !@dl($_ext)) {
		echo "<b>FATAL: Unable to load the PHP ldap extension ($_ext). Aborting.</b>";
		exit;
	}
	unset($_ext);

/* {{{ proto bool survey_auth(int survey_id, string username, string password)
   Returns true if the supplied username/password is allowed
   access to the survey. */
function survey_auth($sid, $username, $password, $raw_password, $css, $title) {
	// Default to _unauthorized_
	$auth_ldap_accept = $auth = false; // default to unauthorized
	//if (isset($GLOBALS['HTTP_GET_VARS']['auth_options']) || isset($GLOBALS['HTTP_POST_VARS']['auth_options'])) {
	if (isset($_GET['auth_options']) || isset($_POST['auth_options'])) {
		$GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
		return(FALSE);
	}
   
	$GLOBALS['auth_options'] = array();

    // make sure database is opened
    esp_init_adodb();

	// Formulate the query and check whether survey requires authentication
	$sql = "SELECT realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id = $sid AND public = 'N'";

	// Execute the query and put results in $accres
    $accres = execute_sql($sql);
	if(!$accres) {
		$GLOBALS['errmsg'] = mkerror(_('Unable to execute query for access.'));
		return(false);
	}

	// Get number of rows in $accres.
	if(record_count($accres) < 1) {
		// no matching rows ... no authorization required
		return(true);
	}

    $realm = $accres->fields[0];
    db_close($accres);

	// A matching row was found - the survey requires authentication.
    $loginAttempt = false;
    $maxlogin=0;
	if (!empty($username) && !empty($password)) {
        $loginAttempt = true;
        $auth = authenticate($username, $raw_password, $realms);
        if ($auth && 1 == count($realms)) {
		   $sql = "SELECT a.maxlogin, a.realm, a.resume, a.navigate
			FROM ".$GLOBALS['ESPCONFIG']['access_table']." a
			WHERE a.survey_id = $sid
			AND a.realm='".current($realms)."'";
        } else {
		   // Formulate the query check whether user is authorized
		   $sql = "SELECT a.maxlogin, a.realm, a.resume, a.navigate
			FROM ".$GLOBALS['ESPCONFIG']['access_table']." a, ".$GLOBALS['ESPCONFIG']['respondent_table']." r
			WHERE a.survey_id = $sid AND
				r.username = "._addslashes($username)." AND
				r.password = ".db_crypt($password) ." AND
				r.realm = a.realm AND
				r.disabled = 'N' AND
				(r.expiration = '0000-00-00 00:00:00' OR r.expiration > ".sys_time_stamp().")";
        }

		// Execute the query and put results in $usrres
		$usrres = execute_sql( $sql );
		if(!$usrres) {
			$GLOBALS['errmsg'] = mkerror(_('Unable to execute query respondents.' ));
			return(false);
		}

        if (record_count( $usrres ) > 0) {
			// A matching row was found - the user is authorized.
			$auth = true;
			list($maxlogin, $arealm, $aresume, $anavigate) = $usrres->fields;
			db_close($usrres);
			$GLOBALS['auth_options'] = array('resume' => $aresume, 'navigate' => $anavigate);
		}
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) {
        // record the fact that authentication failed
        if ($loginAttempt) {
            esp_require_once('/lib/espsurveystat');
            survey_stat_increment(SURVEY_STAT_LOGINFAIL, $sid);
        }

        if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic') {
            header( "WWW-Authenticate: Basic realm=\"$realm\"" );
            header( 'HTTP/1.0 401 '. _('Unauthorized'));
            $GLOBALS['errmsg'] = mkerror(_('Incorrect User ID or Password, or your account has been disabled/expired.'));
            return(false);
        }
        elseif ( $GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
            if (!isset($survey_name)) {
                $survey_name = "";
            }
            if (isset($GLOBALS['_GET']['name'])) {
                $survey_name = $GLOBALS['_GET']['name'];
            }
            elseif (isset($GLOBALS['HTTP_POST_VARS']['name'])) {
                $survey_name = $GLOBALS['HTTP_POST_VARS']['name'];
            }

            $error_message = "";
            if ($_SERVER['REQUEST_METHOD'] == 'POST') {
                $error_message = "Incorrect User ID or Password, or your account has been disabled/expired.";
            }
            echo show_login($survey_name, $error_message, $css, $title);
            exit;
            return(false);
        }
        else {
            return(false);
        }
    }

	if ( $maxlogin > 0 ) {
		// see if user is over the MAX # of responses
		$sql = "SELECT COUNT(*) < $maxlogin FROM ".$GLOBALS['ESPCONFIG']['response_table']." WHERE
		survey_id = ${sid} AND
		complete = 'Y' AND
		username = "._addslashes($username);

		$auth = get_one($sql);
	}
	if( !$auth ) {
        if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic') {
            header( "WWW-Authenticate: Basic realm=\"$realm\"" );
            header( 'HTTP/1.0 401 '. _('Unauthorized'));
            $GLOBALS['errmsg'] = mkerror(_('Your account has been disabled or you have already completed this survey.'));
            return(false);
        }
        elseif ( $GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
            if (!isset($survey_name)) {
                $survey_name = "";
            }
            if (isset($_GET['name'])) {
                $survey_name = $_GET['name'];
            }
            elseif (isset($_POST['name'])) {
                $survey_name = $_POST['name'];
            }
            $error_message = "";
            if ($_SERVER['REQUEST_METHOD'] == 'POST') {
                $error_message = "Your account has been disabled or you have already completed this survey.";
            }
            echo show_login($survey_name, $error_message, $css, $title);
            exit;
            return(false);
        }
    }
    return(true);
}
/* }}} */

/* {{{ proto bool manage_auth(string username, string password)
   Returns true if the supplied username/password is allowed
   access to the management interface. This sets/clears
   access control related session variables. */
function manage_auth($username, $password, $raw_password="") {

	// see if session is expired, or has been logged out
	if(isset($_SESSION['acl']) &&
	   isset($_SESSION['acl']['expired'])) {
		if($_SESSION['acl']['expired']-- > 0) {
			$username = ''; $password = '';
		}
	}

	// see if ACL is cached
	if (!empty($username) &&
			!empty($password) &&
			isset($_SESSION['acl']) &&
			isset($_SESSION['acl']['username']) &&
			$_SESSION['acl']['username'] == $username) {
		return(true);
		#$sql = "SELECT ".db_crypt($password) ."= ". $_SESSION['acl']['password'];
		#$auth = get_one($sql);
		#if($auth)
		#	return(true);
	}

	$auth_ldap_accept = $auth = false; // default to unauthorized
	$_SESSION['acl'] = array();

	if (!empty($username) && !empty($password) && !empty($raw_password)) {
		// Formulate the query check whether user is authorized
		// This only gives authorization for use of the survey.
		// Information is entered into the database after authorization.

		// Add configuration for ldap server and other things
		$tmp_ldap_filter = "(&(".$GLOBALS['ESPCONFIG']['ldap_filter'] .$username.")(".$GLOBALS['ESPCONFIG']['ldap_designer_filter']."))";

		$ds = @ldap_connect($GLOBALS['ESPCONFIG']['ldap_server'],
				$GLOBALS['ESPCONFIG']['ldap_port']);

        if ($ds && isset ($GLOBALS['ESPCONFIG']['ldap_bind_dn'])
                && isset ($GLOBALS['ESPCONFIG']['ldap_bind_password'])
                && !empty($GLOBALS['ESPCONFIG']['ldap_bind_dn']) 
                && !empty($GLOBALS['ESPCONFIG']['ldap_bind_password'])) {
            $search_bind = @ldap_bind($ds, $GLOBALS['ESPCONFIG']['ldap_bind_dn'], $GLOBALS['ESPCONFIG']['ldap_bind_password']);
            if (!$search_bind) {
                // the bind failed, so we return false
                return false;
            }
        }

		if ($ds) {
			$search_result = @ldap_search($ds, $GLOBALS['ESPCONFIG']['ldap_dn'], $tmp_ldap_filter);
			if (@ldap_count_entries($ds,$search_result)==1) {
				//$user_info = ldap_get_entries($ds, $search_result);
				$entry_id=ldap_first_entry($ds, $search_result);
				if ($GLOBALS['ESPCONFIG']['ldap_force_proto_3']) {
					ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
				}
                // bind with the raw password, unbind is not needed first
				$auth_bind = @ldap_bind($ds, ldap_get_dn($ds,$entry_id), $raw_password);
				if ($auth_bind) {
					$auth_ldap_accept = true;
					$values = @ldap_get_values($ds, $entry_id,$GLOBALS['ESPCONFIG']['ldap_realm_attr']);
					$ldap_realm=$values[0];
					$values = @ldap_get_values($ds, $entry_id,$GLOBALS['ESPCONFIG']['ldap_superuser_attr']);
					$ldap_superuser_value=$values[0];
				}
			}
		}
		ldap_close($ds);

		if ($auth_ldap_accept) {
			$auth = true;
		}
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) {
          if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic') {
             header( 'WWW-Authenticate: Basic realm="'. _('Management Interface') .'"' );
             header( 'HTTP/1.0 401 '. _('Unauthorized'));
             echo("<html>\n<head><title>401 ". _('Unauthorized') ."</title></head>\n".
             "<body><h1>401 ". _('Unauthorized') ."</h1>\n".
             mkerror(_('Incorrect User ID or Password, or your account has been disabled/expired.')) .
             "</body>\n</html>\n");
             exit;
             return(false);
          } elseif ( $GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
             echo show_login();
             exit;
             return(false);
          }
 	}

	// All tests passed ... create ACL array,
	// and stick it in the session
	$acl = array(
		'username'  => $username,
		'superuser' => 'N',
		'home' => '/tmp'
	);
	// all the fields for the ACL
	$fields = array('pdesign', 'pstatus', 'pdata', 'pall', 'pgroup', 'puser');
	// the fields that a LDAP user gets
	$fields_Y = array('pdesign', 'pstatus', 'pdata', 'pall');
	foreach($fields as $f) {
		$$f = array();
	}
	if ($ldap_superuser_value == $GLOBALS['ESPCONFIG']['ldap_superuser_value'])
		$acl['superuser'] = 'Y';
	foreach($fields_Y as $f) {
		array_push($$f, $ldap_realm);
	}

	foreach($fields as $f) {
		$acl[$f] =& $$f;
	}

	$_SESSION['acl'] =& $acl;

	// if one were to want login accounting (logs) this
	// would be the ideal place to do so...

	return(true);
}
/* }}} */

/* {{{ proto void manage_logout()
   Clears the current ACL, and will cause HTTP-Auth
   to be redisplayed. This is not fool proof; common browsers
   will continue to retry cached username & password for
   HTTP-Auth. So if the browser is not closed after logout,
   it still may be possible to get back in w/o knowing a
   valid username & password. */
function manage_logout() {
    $_SESSION['acl'];
    $_SESSION['username'] = "";
    $_SESSION['raw_password'] = "";
    session_destroy();
	$acl = array('expired' => 2);
	$_SESSION['acl'] = array('expired' => 2);
}
/* }}} */

/* {{{ proto boolean auth_is_owner(int surveyId, string user)
   Returns true if user owns the survey. */
function auth_is_owner($sid, $user) {
	$val = false;
	$sql = "SELECT s.owner = "._addslashes($user)." FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.id=$sid";
    $result = execute_sql($sql);
	if(!(list($val) = fetch_row($result)))
		$val = false;
    db_close($result);
	return $val;
}
/* }}} */

/* {{{ proto string auth_get_survey_realm(int surveyId)
   Returns the realm of the survey. */
function auth_get_survey_realm($sid) {
	$val = '';
	$sql = "SELECT s.realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.id=$sid";
    $result = execute_sql($sql);
	list($val) = fetch_row($result);
    db_close($result);
	return $val;
}
/* }}} */

/* {{{ proto boolean auth_no_access(string description)
   Handle a user trying to access an unauthorised area.
   Returns true if user should be allowed to continue.
   Returns false (or exits) if access should be denied. */
function auth_no_access($description) {
	echo(mkerror(sprintf(_('This account does not have permission %s.'), $description)));
	echo("\n<br>\n");
	echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
	return false;
}
/* }}} */

/* {{{ proto boolean auth_change_manager_passwd(string user, string old, string new)
   Change a managers password. If old password does not match
   or if there is an error, return false. Return true if
   password changed. */
function auth_change_manager_passwd($user,$old,$new) {
	$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['designer_table']." SET password=".db_crypt($new) ."
		WHERE username="._addslashes($user)." AND password=".db_crypt($old);
        return(execute_sql($sql) && affected_rows() > 0);
}
/* }}} */

/* {{{ proto boolean auth_get_option(string option)
   Returns the value of the given option. Valid options include:
   { resume, navigate } */
function auth_get_option($opt) {
	return (isset($GLOBALS['auth_options'][$opt]) && $GLOBALS['auth_options'][$opt] == 'Y');
}
/* }}} */

/* {{{ proto int auth_get_rid (int survey_id, string username, int response_id)
   Returns the RID to use for this user. */
function auth_get_rid($sid, $username, $rid = 0) {
	$rid = intval($rid);
	if (isset($rid) && $rid != 0) {
		// check for valid rid
		$sql = "SELECT (r.username = "._addslashes($username)." && r.complete = 'N') AS valid
			FROM ".$GLOBALS['ESPCONFIG']['response_table']." r
			WHERE r.id = $rid AND
			r.survey_id = $sid";
		$res = execute_sql($sql);
		$valid = FALSE;
        if ($res && record_count($res) > 0 && $res->fields[0])
			$valid = TRUE;
		if ($res)
			db_close($res);
		return ($valid) ? $rid : '';
	} elseif (auth_get_option('resume')) {
		// find latest in progress rid
		$sql = "SELECT r.id FROM ".$GLOBALS['ESPCONFIG']['response_table']." r
			WHERE r.survey_id = $sid AND
			r.complete = 'N' AND
			r.username = "._addslashes($username)."
			ORDER BY submitted DESC";
		$res = execute_sql($sql,1);
        if ($res && record_count($res) > 0)
			$rid = $res->fields[0];
		if ($res)
			db_close($res);
		return ($rid != 0) ? $rid : '';
	} else {
		return '';
	}
}
/* }}} */

/* respondent authentication methods */
// {{{ authenticate()                  See if a <username,password> credential is valid, and identify to which realms that applies
// NOTE1: returns true iff authenticated, false otherwise
// NOTE2: $realms will always be an array with at least one element.  If 2 or more, you must have an external
// NOTE2: means to disambiguate to which realms the username/password pair applies

function authenticate($username, $password, &$realms) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // if we are running mysql >4.0 need to check/upgrade passwords
    password_upgrade($username, _addslashes($password), $cfg['respondent_table']);

    // assume no authorization
    $auth_ldap_accept = false;

    // Add configuration for ldap server and other things
    #$tmp_ldap_filter = "(&(".$GLOBALS['ESPCONFIG']['ldap_filter'] .$username.")(".$GLOBALS['ESPCONFIG']['ldap_realm_attr']."=".$realm."))";
    $tmp_ldap_filter = $GLOBALS['ESPCONFIG']['ldap_filter'].$username;

    $ds = @ldap_connect($GLOBALS['ESPCONFIG']['ldap_server'], $GLOBALS['ESPCONFIG']['ldap_port']);
    if ($ds && $GLOBALS['ESPCONFIG']['ldap_force_proto_3']) {
        ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    }

    // do an authenticated bind for the search if wanted
    $search_bind=false;
    if ($ds && isset ($GLOBALS['ESPCONFIG']['ldap_bind_dn'])
        && isset ($GLOBALS['ESPCONFIG']['ldap_bind_password'])
        && !empty($GLOBALS['ESPCONFIG']['ldap_bind_dn']) 
        && !empty($GLOBALS['ESPCONFIG']['ldap_bind_password'])) {
        $search_bind = @ldap_bind($ds, $GLOBALS['ESPCONFIG']['ldap_bind_dn'], $GLOBALS['ESPCONFIG']['ldap_bind_password']);
        if (!$search_bind) {
            // the bind failed, so we return false
            return false;
        }
    }
    
    // now search for the user uid to know it's dn
    if ($ds) {
        $search_result = @ldap_search($ds, $GLOBALS['ESPCONFIG']['ldap_dn'], $tmp_ldap_filter);
        if (@ldap_count_entries($ds,$search_result)==1) {
            $entry_id=ldap_first_entry($ds, $search_result);
            // bind with the user password, unbind is not needed first
            $auth_bind = @ldap_bind($ds, ldap_get_dn($ds,$entry_id), $password);
            if ($auth_bind) {
                $auth_ldap_accept = true;
                $values = @ldap_get_values($ds, $entry_id,$GLOBALS['ESPCONFIG']['ldap_realm_attr']);
                $realms[] = $values[0];
            }
        }
        ldap_close($ds);
    }

    return ($auth_ldap_accept ? true : false);
}

// }}}

?>
