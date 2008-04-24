<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

//  phpESP Authentication Library Chooser
//  By: James Flemer <jflemer@alum.rpi.edu>

	$auth_type = $GLOBALS['ESPCONFIG']['auth_type'];
	if (empty($auth_type))
		$auth_type = 'default';
	if (ereg("manage.php",$ESPCONFIG['ME']) &&
	    ($auth_type == 'ldap_both' || $auth_type == 'ldap_des')) {
		$auth_type = 'ldap';
	} else if (!ereg("manage.php",$ESPCONFIG['ME']) &&
		($auth_type == 'ldap_both' || $auth_type == 'ldap_resp')) {
		$auth_type = 'ldap';
	} else {
		$auth_type = 'default';
	}

	if (!file_exists($GLOBALS['ESPCONFIG']['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension'])) {
		echo("<b>FATAL: Unable to set up authentication for type $auth_type. Aborting.</b>");
		exit;
	}

	require($ESPCONFIG['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension']);


/* respondent authentication methods */
// {{{ authenticate_in_realm()         Determine if a <username,password> credential is valid for a given realm

function authenticate_in_realm($username, $password, $realm) {
    assert('is_callable("authenticate"); // expecting an authenticate function to be defined');
    $isAuthenticated = authenticate($username, $password, $realms);
    return ($isAuthenticated && in_array($realm, $realms) ? true : false);
}

// }}}
// {{{ set_session_authentication()    Set session authentication bit

function set_session_authentication($isAuthenticated) {
    // IE6 has a serious problem with cookies in multi-home framed sites. Force it to shut up.
    // See also: http://www.oreillynet.com/mac/blog/2002/06/p3p_in_ie6_frustrating_failure.html
    @header('P3P: CP="NOI ADM DEV PSAi COM NAV OUR OTRo STP IND DEM"');

    // start sessions if needed
    if ('' == session_name()) {
        session_start();
    }

    if ($isAuthenticated) {
        $_SESSION['authenticated'] = true;
    } else {
        // wipe out the session...
        $_SESSION = array ();
        session_destroy();

        // ... and regenerate it for next requests
        session_start();
        session_regenerate_id();

        // NOTE: for PHP < 4.3.3, since it does not put the cookie out
        setcookie(session_name(), session_id());
    }
}

// }}}
// {{{ is_session_authenticated()      Determine if this session has been authenticated

function is_session_authenticated() {
    return (array_key_exists('authenticated', $_SESSION) ? $_SESSION['authenticated'] : false);
}

// }}}
// {{{ set_current_respondent()        Set the current respondent

function set_current_respondent($username, $realm, $password = null) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    $sql = sprintf(
               'SELECT * FROM %s WHERE username = %s AND realm = %s',
               $cfg['respondent_table'], _addslashes($username), _addslashes($realm)
           );

    // execute the SQL
    $res = execute_sql($sql, null, ADODB_FETCH_ASSOC);
    if (! $res) {
        // give up, select bombed
        $GLOBALS['errmsg'] = mkerror(_('Unable to set current respondent'));
        return false;
    }   

    // put dataset into session
    if (1 === record_count($res)) {
        $row = fetch_row($res);
        $_SESSION['respondent'] = $row;
    } else {
        $GLOBALS['errmsg'] = mkerror(_('Unable to set current respondent'));
        return false;
    }
    db_close($res);

    // FIXME: For now, to bootstrap public/handler-prefix.php
    if (isset($password)) {
        $_SESSION['espuser'] = $username;
        $_SESSION['esppass'] = $password;
    }

    return true;
}

// }}}
// {{{ get_current_respondent()        Get the respondent currently logged in

function get_current_respondent(&$respondent) {
    if (isset($_SESSION['respondent'])) {
        $respondent = $_SESSION['respondent'];
        return true;
    } else {
        $respondent = null;
        return false;
    }
}

// }}}
// {{{ change_profile()                cHANGE THe profile for a user (in a given realm)

function change_profile($username, $realm, $firstName, $lastName, $emailAddress) {
    // build the change SQL
    $_username     = _addslashes($username);
    $_realm        = _addslashes($realm);
    $_firstName    = _addslashes($firstName);
    $_lastName     = _addslashes($lastName);
    $_emailAddress = _addslashes($emailAddress);
    $changed  = sys_time_stamp();
	$sql =<<<EOSQL
UPDATE {$GLOBALS['ESPCONFIG']['respondent_table']}
   SET fname={$_firstName}, lname={$_lastName}, email={$_emailAddress}, changed={$changed}
 WHERE username={$_username} AND realm={$_realm}
EOSQL;

    // throw it at the DB
    // NOTE: There is a small race condition here on some databases: see the note in change_password()
	$res = execute_sql($sql);
    if (1 === affected_rows()) {
        set_current_respondent($username, $realm);
        db_close($res);
        return true;
    } else {
        return false;
    }
}

// }}}
// {{{ change_password()               Change a password for a user (in a given realm)

function change_password($username, $realm, $password) {
    // build the change SQL
    $_username = _addslashes($username);
    $_realm    = _addslashes($realm);
    $_password = db_crypt(_addslashes($password));
    $changed  = sys_time_stamp();
	$sql =<<<EOSQL
UPDATE {$GLOBALS['ESPCONFIG']['respondent_table']}
   SET password={$_password}, changed={$changed}
 WHERE username={$_username} AND realm={$_realm}
EOSQL;

    // throw it at the DB
    // NOTE: There is a small race condition here on some databases: this will fail if the password is changed to the same password
    // NOTE: as originally within 1 second of the last change.  In other words, if you changed your password to "bob" 500ms ago,
    // NOTE: and you change it to "bob" again, the second time will fail, because the database will not update the row so
    // NOTE: affected_rows() === 0.  Prevent this case by preventing the user from changing their password to the current password
    // NOTE: in the UI code.
	$res = execute_sql($sql);
    if (1 === affected_rows()) {
        set_current_respondent($username, $realm);
        db_close($res);
        return true;
    } else {
        return false;
    }
}

// }}}

?>
