<?php

# $Id$

//  phpESP Default Authentication Library
//  By: Romans Jasins <roma@latnet.lv>
//      James Flemer <jflemer@acm.jhu.edu>

if(!defined('_LIB_AUTH')) {
	define('_LIB_AUTH', TRUE);

/* {{{ proto bool survey_auth(int survey_id, string username, string password)
   Returns TRUE if the supplied username/password is allowed
   access to the survey. */
function survey_auth($sid, $username, $password) {
	// Default to _unauthorized_
	$auth = FALSE;

	// Formulate the query and check whether survey requires authentication
	$sql = "SELECT realm FROM survey WHERE id = '$sid' AND public = 'N'";

	// Execute the query and put results in $accres 
	$accres = mysql_query( $sql );
	if(!$accres) {
		$GLOBALS['errmsg'] = mkerror(_('Unable to execute query for access.'));
		return(FALSE);
	}

	// Get number of rows in $accres. 
	if(mysql_num_rows($accres) < 1) {
		// no matching rows ... no authorization required
		mysql_free_result($accres);
		return(TRUE);
	}
	
	$realm = mysql_result($accres, 0, 0);
	mysql_free_result($accres);

	// A matching row was found - the survey requires authentication. 
	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT a.maxlogin, a.realm
			FROM access a, respondent r
			WHERE a.survey_id = '$sid' AND
				r.username = '$username' AND
				r.password = PASSWORD('$password') AND
				r.realm = a.realm AND
				r.disabled = 'N' AND
				(r.expiration = '0' OR r.expiration > NOW())"; 

		// Execute the query and put results in $usrres 
		$usrres = mysql_query( $sql );
		if(!$usrres) {
			$GLOBALS['errmsg'] = mkerror(_('Unable to execute query respondents.' ));
			return(FALSE);
		}

		if (mysql_num_rows( $usrres ) > 0) { 
			// A matching row was found - the user is authorized. 
			$auth = TRUE; 
			$maxlogin = mysql_result($usrres, 0, 0);
			mysql_free_result($usrres);
		}
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) { 
		header( "WWW-Authenticate: Basic realm=\"$realm\"" ); 
		header( 'HTTP/1.0 401 '. _('Unauthorized')); 
		$GLOBALS['errmsg'] = mkerror(_('Incorrect User ID or Password, or your account has been disabled/expired.'));
		return(FALSE);
	}

	if ( $maxlogin > 0 ) {
		// see if user is over the MAX # of responses
		$sql = "SELECT COUNT(*) < '$maxlogin' FROM response WHERE
		survey_id = '${sid}' AND
		complete = 'Y' AND
		username = '$username'";
		
		$numres = mysql_query( $sql );
		$auth = mysql_result($numres, 0, 0);
		mysql_free_result($numres);
	}
	if( !$auth ) {
		header( 'WWW-Authenticate: Basic realm="'. $access['realm'] .'"' ); 
		header( 'HTTP/1.0 401 '. _('Unauthorized')); 
		$GLOBALS['errmsg'] = mkerror(_('Your account has been disabled or you have already completed this survey.'));
		return(FALSE);
	}
	return(TRUE);
}
/* }}} */

/* {{{ proto bool manage_auth(string username, string password)
   Returns TRUE if the supplied username/password is allowed
   access to the management interface. This sets/clears
   access control related session variables. */
function manage_auth($username, $password) {
	// see if session is expired, or has been logged out
	if(isset($GLOBALS['HTTP_SESSION_VARS']['acl']) &&
	   isset($GLOBALS['HTTP_SESSION_VARS']['acl']['expired'])) {
		if($GLOBALS['HTTP_SESSION_VARS']['acl']['expired']-- > 0) {
			$username = ''; $password = '';
		}
	}

	// see if ACL is cached
	if (!empty($username) &&
			!empty($password) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['username']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['password']) &&
			$GLOBALS['HTTP_SESSION_VARS']['acl']['username'] == $username) {
		$sql = "SELECT PASSWORD('$password') = '". $GLOBALS['HTTP_SESSION_VARS']['acl']['password'] ."'";
		$result = mysql_query($sql);
		$auth = mysql_result($result, 0, 0);
		mysql_free_result($result);
		if($auth)
			return(TRUE);
	}
	
	$auth = FALSE; // default to unauthorized
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array();

	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT * FROM designer WHERE 
			username = '$username' AND
			password = PASSWORD('$password') AND
			disabled = 'N' AND
			(expiration = '0' OR expiration > NOW())"; 

		// Execute the query and put results in $accres 
		$accres = mysql_query( $sql );
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
	session_register('acl');
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array('expired' => 2);
}
/* }}} */

/* {{{ proto boolean auth_is_owner(int surveyId, string user)
   Returns TRUE if user owns the survey. */
function auth_is_owner($sid, $user) {
	$val = FALSE;
	$sql = "SELECT s.owner = '$user' FROM survey s WHERE s.id='$sid'";
	$result = mysql_query($sql);
	if(!(list($val) = mysql_fetch_row($result)))
		$val = FALSE;
	mysql_free_result($result);
	return $val;
}
/* }}} */

/* {{{ proto string auth_get_survey_realm(int surveyId)
   Returns the realm of the survey. */
function auth_get_survey_realm($sid) {
	$val = '';
	$sql = "SELECT s.realm FROM survey s WHERE s.id='$sid'";
	$result = mysql_query($sql);
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
	$sql = "UPDATE designer SET password=PASSWORD('$new')
		WHERE username='$user' AND password=PASSWORD('$old')";
	return(mysql_query($sql) && mysql_affected_rows() > 0);
}
/* }}} */

} // end _LIB_AUTH
?>
