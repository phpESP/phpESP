<?php

# $Id$

//  phpESP Authentication Library specifically for survey.ucsb.edu
//  By: Steve McGhee <stevem@isber.ucsb.edu>

if(!defined('_LIB_AUTH')) {
	define('_LIB_AUTH', TRUE);


    // isber specific functions, related to common login page, etc.
include_once("/www/lib/functions.php");

/* {{{ proto bool survey_auth(int survey_id, string username, string password)
   Returns TRUE if the supplied username/password is allowed
   access to the survey. */
function survey_auth($sid, $username, $password) {

    /*
    if (defined($session_var)) echo "defined";
    else echo "not defined";
    echo $session_var[logged_in] ;
    return $session_var[logged_in] == "y";
    */
    return true;
}
/*
    //
    // NEED TO DO: ensure user only submits once ?
    // 
    //



    
	// Default to _unauthorized_
	$auth = FALSE;

	// Formulate the query and check whether survey requires authentication
    $sql = "SELECT public FROM survey WHERE id = '$sid'";

	// Execute the query and put results in $accres 
	$accres = mysql_query( $sql );
	if(!$accres) {
		$GLOBALS['errmsg'] = mkerror(_('Unable to execute query for access.'));
		return(FALSE);
	}

	// 
	if(mysql_result($accres, 0, 0) == "Y") {
		// if public==Y, no authorization required, user is authorized: return TRUE
		mysql_free_result($accres);
        $auth = TRUE;
		return(TRUE);
	}
	
	// we know the survey requires authentication. 
    // now see if this user is allowed
    echo "username:" . $username;
    echo "passwd:" . $password;
	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
        $survey_name = "phpESP_" . $sid;

		if ( verifyPass($username, $password, $survey_name) ) { 
			// A matching row was found - the user is authorized. 
			$auth = TRUE; 
		}
    }

    if ( ! $auth ) {
//        header( "WWW-Authenticate: Basic realm=\"$realm\"" );
//        header( 'HTTP/1.0 401 ' . _('Unauthorized') );
        $GLOBALS['errmsg'] = mkerror(_('Incorrect userID or password. Or your account has been disabled or has expired.'));
//        return (FALSE);
    }

    $sql = "SELECT completed_survey FROM users WHERE login='$username' AND survey_name='phpESP_$sid'";
    $result = mysql_query($sql);
    $object    = mysql_fetch_object($result);
    $completed = $object->completed_survey;

    if ( $completed != "0000-00-00 00:00:00"){
        $GLOBALS['errmsg'] = mkerror(_('You have already completed this survey: ' .  $sql . $completed));
        $auth = FALSE;
        return(FALSE);
    }
        
    if ( ! $auth ) {
        //
        $GLOBALS['errmsg'] = mkerror(_('this should go to the login page now...'));
        //
        return (FALSE);
    }

	return(TRUE);
}
*/

/* }}} */


/* {{{ proto bool manage_auth(string username, string password)
   Returns TRUE if the supplied username/password is allowed
   access to the management interface. This sets/clears
   access control related session variables. */
function manage_auth($username, $password) {

    $db_conn = $GLOBALS['ESPCONFIG']['db_conn'];
    $auth_db_conn = $GLOBALS['ESPCONFIG']['auth_db_conn'];

	// see if session is expired, or has been logged out
	if(isset($GLOBALS['HTTP_SESSION_VARS']['acl']) &&
	   isset($GLOBALS['HTTP_SESSION_VARS']['acl']['expired'])) {
		if($GLOBALS['HTTP_SESSION_VARS']['acl']['expired']-- > 0) {
			$username = ''; $password = '';
		}
	}


    $sql    = "SELECT password FROM admin_users WHERE login='$username'";
    $result = mysql_query($sql, $auth_db_conn);
    if (mysql_num_rows($result) > 0) {
        $object    = mysql_fetch_object($result);
        $stored_pw = $object->password;
    }
    $salt    = substr($stored_pw, 0, 2);
    $crypt_pass = crypt($password, $salt);

	// see if ACL is cached
	if (!empty($username) &&
			!empty($password) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['username']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['password']) &&
			$GLOBALS['HTTP_SESSION_VARS']['acl']['username'] == $username) {
        $crypt_pass = crypt($password);
		if($crypt_pass == $GLOBALS['HTTP_SESSION_VARS']['acl']['password'])
			return(TRUE);
	}
	
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array();

	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		// This only gives authorization for use of the survey. 
		// Information is entered into the database after authorization.

		$sql = "SELECT * FROM admin_users WHERE 
            login = '$username' AND
            password = '$crypt_pass' AND
            disabled = 'N' AND
            (expiration = '0' OR expiration > NOW())"; 

		// Execute the query and put results in $accres 
		$accres = mysql_query( $sql, $auth_db_conn );
		if(!$accres) {
			header( 'HTTP/1.0 503 '. _('Service Unavailable'));
			echo('<html><head><title>'. _('Service Unavailable') .
				'</title></head><body><h1>HTTP 503 '.
				_('Service Unavailable') .'</h1>'.
				mkerror(_('Unable to load ACL.') .' ('. mysql_error() .')') . 
				'</body></html>');
			return(FALSE);
		}

		// Get number of rows in $accres. 
		if (mysql_numrows( $accres ) > 0) { 
			// A matching row was found - the user is authorized. 
			$auth = TRUE; 
		} 
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) { 
		header( 'WWW-Authenticate: Basic realm="'. _('Management Interface') .'"' ); 
		header( 'HTTP/1.0 401 '. _('Unauthorized')); 
		echo("<html>\n<head><title>401 ". _('Unauthorized') ."</title></head>\n".
			"<body><h1>401 ". _('Unauthorized') ."</h1>\n".
			mkerror(_('Incorrect User ID or Password, or your account has been disabled.')) .
			"</body>\n</html>\n");
		exit;
		return(FALSE);
	}
	
	// All tests passed ... create ACL array, 
	// and stick it in the session
	$acl = array(
		'username'  => $username,
		'superuser' => 'N',
		'home' => '/tmp'
	);
	$fields = array('pdesign', 'pstatus', 'pdata', 'pall', 'pgroup', 'puser');
	foreach($fields as $f) {
		$$f = array();
	}
	while( $arr = mysql_fetch_array($accres, MYSQL_ASSOC) ) {
		if($arr['realm'] == 'superuser')
			$acl['superuser'] = 'Y';
		foreach($fields as $f) {
			if($arr[$f] == 'Y')
				array_push($$f, $arr['realm']);
		}
	}
	mysql_free_result($accres);
	
	foreach($fields as $f) {
		$acl[$f] =& $$f;
	}
	
	$GLOBALS['HTTP_SESSION_VARS']['acl'] =& $acl;
	
	// if one were to want login accounting (logs) this
	// would be the ideal place to do so...

	return(TRUE);
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
	// clear existing ACL, and set the expired flag
	session_unset();
	$acl = array('expired' => 2);
	//session_register('acl');
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array('expired' => 2);
}
/* }}} */

/* {{{ proto boolean auth_is_owner(int surveyId, string user)
   Returns TRUE if user owns the survey. */
function auth_is_owner($sid, $user) {
    $db_conn = $GLOBALS['ESPCONFIG']['db_conn'];
    $auth_db_conn = $GLOBALS['ESPCONFIG']['auth_db_conn'];
	$val = FALSE;
	$sql = "SELECT s.owner = '$user' FROM survey s WHERE s.id='$sid'";
	$result = mysql_query($sql, $db_conn);
	if(!(list($val) = mysql_fetch_row($result)))
		$val = FALSE;
	mysql_free_result($result);
	return $val;
}
/* }}} */

/* {{{ proto string auth_get_survey_realm(int surveyId)
   Returns the realm of the survey. */
function auth_get_survey_realm($sid) {
    $db_conn = $GLOBALS['ESPCONFIG']['db_conn'];
    $auth_db_conn = $GLOBALS['ESPCONFIG']['auth_db_conn'];
	$val = '';
	$sql = "SELECT s.realm FROM survey s WHERE s.id='$sid'";
	$result = mysql_query($sql, $db_conn);
	list($val) = mysql_fetch_row($result);
	mysql_free_result($result);
	return $val;
}
/* }}} */

/* {{{ proto boolean auth_no_access(string description)
   Handle a user trying to access an unauthorised area.
   Returns TRUE if user should be allowed to continue.
   Returns FALSE (or exits) if access should be denied. */
function auth_no_access($description) {
	echo(mkerror(_('This account does not have permission') .' '. $description .'.'));
	echo("\n<br>\n");
	echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
	return FALSE;
}
/* }}} */

/* {{{ proto boolean auth_change_manager_passwd(string user, string old, string new)
   Change a managers password. If old password does not match
   or if there is an error, return FALSE. Return TRUE if 
   password changed. */
function auth_change_manager_passwd($user,$old,$new) {
    $db_conn = $GLOBALS['ESPCONFIG']['db_conn'];
    $auth_db_conn = $GLOBALS['ESPCONFIG']['auth_db_conn'];
    //
    // generate the password using DES (2 letter salt)
    // using a random salt (logical AND of session_id and current milliseconds)
    // avoids cleartext passwords in database logs
    //
    $time = gettimeofday();
    $secs = (int)$time["usec"];
    $random = session_id() & $secs;
    $sql    = "SELECT password FROM admin_users WHERE login ='$user'";
    $result = mysql_query($sql, $auth_db_conn);
    if (mysql_num_rows($result) > 0) {
        $object    = mysql_fetch_object($result);
        $stored_pw = $object->password;
    }
    else {
        //echo("user doesn't exist");
        return false;
    }
    $salt    = substr($stored_pw, 0, 2);
    $crypt_old = crypt($old, $salt);
    $crypt_new = crypt($new, substr($random, 0, 2));

	$sql = "UPDATE admin_users SET password='$crypt_new'
		WHERE login ='$user' AND password='$crypt_old'";
	return(mysql_query($sql, $auth_db_conn) && mysql_affected_rows($auth_db_conn) > 0);
}
/* }}} */


function verifyPass($login, $password, $survey_name) {
    $db_conn = $GLOBALS['ESPCONFIG']['db_conn'];
    $auth_db_conn = $GLOBALS['ESPCONFIG']['auth_db_conn'];
        $sql    = "SELECT password FROM users WHERE login='$login' AND survey_name='$survey_name'";
        $result = mysql_query($sql, $auth_db_conn);
        if (mysql_num_rows($result) > 0)
        {
            $object    = mysql_fetch_object($result);
            $stored_pw = $object->password;
        }
        else
        {
            //echo("user doesn't exist");
            return false;
        }
        $salt    = substr($stored_pw, 0, 2);
        $cryptpw = crypt($password, $salt);
        $sql     = "SELECT id FROM users 
                    WHERE login='$login' AND password='$cryptpw'
                    AND survey_name='$survey_name' AND
    				disabled = 'N' AND
    				(expiration = '0' OR expiration > NOW())"; 
        $result  = mysql_query($sql, $auth_db_conn);
        if (mysql_num_rows($result) > 0)
        {
            $object = mysql_fetch_object($result);
            return true;
        }
        else
        {
            //echo("wrong password");
            return false;
        }
}

} // end _LIB_AUTH
?>
