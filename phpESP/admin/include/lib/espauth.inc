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
function survey_auth($sid,$username,$password) {
	// Formulate the query and check whether survey requires authentication
	$sql = "SELECT * FROM access WHERE survey_id='${sid}' AND restricted='Y'";

	// Execute the query and put results in $accres 
	$accres = mysql_query( $sql );
	if(!$accres) {
		mkerror(_('Unable to execute query for access.'));
		return(FALSE);
	}

	// Get number of rows in $accres. 
	if(mysql_num_rows( $accres ) < 1) {
		// no matching rows ... no authorization required
		mysql_free_result($accres);
		return(TRUE);
	}
	
	$access = mysql_fetch_array($accres, MYSQL_ASSOC);
	mysql_free_result($accres);

	$auth = FALSE; // default to unauthorized

	// A matching row was found - the survey requires authentication. 
	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT * FROM user WHERE 
		username = '$username' AND 
		password = PASSWORD('$password') AND
		disabled = 'N' AND
		(expiration = '0' OR expiration > NOW())"; 

		// Execute the query and put results in $usrres 
		$usrres = mysql_query( $sql );
		if(!$usrres) {
			mkerror(_('Unable to execute query users.' )); 
			return(FALSE);
		}

		if (mysql_numrows( $usrres ) > 0) { 
			// A matching row was found - the user is authorized. 
			$auth = TRUE; 
			$user = mysql_fetch_array($usrres,MYSQL_ASSOC);
			mysql_free_result($usrres);
		}
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) { 
		header( 'WWW-Authenticate: Basic realm="'. $access['realm'] .'"' ); 
		header( 'HTTP/1.0 401 '. _('Unauthorized')); 
		mkerror(_('Incorrect User ID or Password, or your account has been disabled/expired.'));
		return(FALSE);
	}

	if ( $access['maxresponses'] > 0 ) {
		// see if user is over the MAX # of responses
		$sql = "SELECT COUNT(*) FROM response WHERE
		survey_id = '${sid}' AND
		complete = 'Y' AND
		username = '$username'";
		
		$numres = mysql_query( $sql );
		$numresponses = mysql_result($numres, 0, 0);
		mysql_free_result($numres);
		if($numresponses >= $access['maxresponses']) {
			header( 'WWW-Authenticate: Basic realm="'. $access['realm'] .'"' ); 
			header( 'HTTP/1.0 401 '. _('Unauthorized')); 
			mkerror(_('Your account has been disabled or you have already completed this survey.'));
			return(FALSE);
		}
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
/**/
	// see if they have already logged in.
	if (!empty($username) &&
			!empty($password) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['username']) &&
			isset($GLOBALS['HTTP_SESSION_VARS']['acl']['password']) &&
			$GLOBALS['HTTP_SESSION_VARS']['acl']['username'] == $username) {
		$sql = "SELECT PASSWORD('${password}')";
		$result = mysql_query($sql);
		$auth = (mysql_result($result, 0, 0) == $password);
		mysql_free_result($result);
		if($auth)
			return(TRUE);
	}
/**/
	
	$auth = FALSE; // default to unauthorized

	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT * FROM manager WHERE 
		username = '${username}' AND 
		password = PASSWORD('${password}') AND
		disabled = 'N' AND
		(expiration = '0' OR expiration > NOW())"; 

		// Execute the query and put results in $accres 
		$accres = mysql_query( $sql );
		if(!$accres) {
			mkerror(_('Unable to execute query users.' )); 
			return(FALSE);
		}

		// Get number of rows in $accres. 
		if (mysql_numrows( $accres ) > 0) { 
			// A matching row was found - the user is authorized. 
			$auth = TRUE; 
			$access = mysql_fetch_array($accres,MYSQL_ASSOC);
			mysql_free_result($accres);
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
	
	// set session variables
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = $access;
	
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
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array('expired' => 2);
}
/* }}} */

/* {{{ proto string where_auth(string where)
   Checks the requested location against current ACL. On
   failure (unauthorized) directs to a "unauthorized" page.
   Otherwise returns esp_where(). *
function where_auth($where) {
	$access = $GLOBALS['HTTP_SESSION_VARS']['acl'];

	if(empty($where))
		$where = 'index';
	$where = strtolower(ereg_replace(' +','_',$where));
	if(!ereg('^[A-Za-z0-9_]+$',$where))	// Valid chars are [A-Za-z0-9_]
		$where = 'index';
	if(!file_exists($GLOBALS['ESPCONFIG']['include_path'].'/where/'.$where.$GLOBALS['ESPCONFIG']['extension']))
		$where = 'index';
	
	switch($where) {
		// requires 'screate':
		case 'new':
		// requires 'sactivate':
		// requires 'send':
		// requires 'sdelete':

		case 'adduser':
		case 'cancel':
		case 'copy':
		case 'edit':
		case 'help':
		case 'index':
		case 'logout':
		case 'manage':
		case 'merge':
		case 'purge':
		case 'readme':
		case 'report':
		case 'reset':
		case 'results':
		case 'status':
		case 'tab':
		case 'test':
		case 'todo':
		case 'upgrade':
		case 'users':
	}

	return($GLOBALS['ESPCONFIG']['include_path'].'/where/'.$where.$GLOBALS['ESPCONFIG']['extension']);
}
/* }}} */

/* {{{ proto boolean auth_is_owner(int surveyId, string user)
   Returns TRUE if user owns the survey. */
function auth_is_owner($sid, $user) {
	$sql = "SELECT s.owner='$user' FROM survey s WHERE s.id='$sid'";
	$result = mysql_query($sql);
	$val = mysql_result($result, 0, 0);
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

} // end _LIB_AUTH
?>
