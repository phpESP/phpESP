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
		password = '$password' AND
		disabled = 'N' AND
		expiration > NOW()"; 

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
	// see if they have already logged in.
	if($GLOBALS['HTTP_SESSION_VARS']['acl']['username'] = $username &&
			$GLOBALS['HTTP_SESSION_VARS']['acl']['password'] = $password)
		return(TRUE);
	
	$auth = FALSE; // default to unauthorized
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = array();

	if (!empty($username) && !empty($password)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT * FROM manager WHERE 
		username = '${username}' AND 
		password = '${password}' AND
		disabled = 'N' AND
		expiration > NOW()"; 

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
		mkerror(_('Incorrect User ID or Password, or your account has been disabled.'));
		return(FALSE);
	}
	
	// set session variables
	$GLOBALS['HTTP_SESSION_VARS']['acl'] = $access;
	
	// if one were to want login accounting (logs) this
	// would be the ideal place to do so...

	return(TRUE);
}
/* }}} */

} // end _LIB_AUTH
?>
