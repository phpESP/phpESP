<?php 
//  Authentication Module
//  By: Romans Jasins <roma@latnet.lv>
//      James Flemer <jflemer@acm.jhu.edu>

function survey_auth($sid,$PHP_AUTH_USER,$PHP_AUTH_PW) {
	// Formulate the query and check whether survey requires authentication
	$sql = "SELECT * FROM access WHERE survey_id='${sid}' AND uselogin='y'";

	// Execute the query and put results in $surres 
	$surres = mysql_query( $sql );
	if(!$surres) {
		mkerror(_('Unable to execute query for access.'));
		return(false);
	}

	// Get number of rows in $surres. 
	$num = mysql_num_rows( $surres ); 

	if(!$num) {
		// no matching rows ... no authorization required
		mysql_free_result($surres);
		return(true);
	}
	
	$auth_realm = mysql_result($surres,0,realm);
	mysql_free_result($surres);

	$auth = false; // default to unauthorized

	// A matching row was found - the survey requires authentication. 
	if (!empty($PHP_AUTH_USER) && !empty($PHP_AUTH_PW)) {
		// Formulate the query check whether user is authorized
		$sql = "SELECT * FROM users WHERE 
		username = '${PHP_AUTH_USER}' AND 
		password = '${PHP_AUTH_PW}' AND
		accstatus = 1"; 

		// Execute the query and put results in $accres 
		$accres = mysql_query( $sql );
		if(!$accres) {
			mkerror(_('Unable to execute query users.' )); 
			return(false);
		}

		// Get number of rows in $accres. 
		$num = mysql_numrows( $accres ); 
		if ( $num != 0 ) { 
			// A matching row was found - the user is authorized. 
			$auth = true; 
			$access = mysql_fetch_array($accres,MYSQL_ASSOC);
			mysql_free_result($accres);
		}
	}

	// no matching authorization ... send a 401
	if ( ! $auth ) { 
		header( 'WWW-Authenticate: Basic realm="'. $auth_realm .'"' ); 
		header( 'HTTP/1.0 401 '. _('Unauthorized')); 
		mkerror(_('Incorrect User ID or Password, or your account has been disabled.'));
		return(false);
	}

	// UPDATE the account status.
	// Get account id and type
	$acctype = $access['acctype'];
	$user =  $access['username'];
	if(empty($acctype))
		$acctype = 0;
	if(empty($user)) {
		mkerror(_( 'Unable to get account information.' ));
		return(false);
	}

	if ( $acctype == 1 ) {
		// see if user is over the MAX # of logins
		$maxlogin = $access['maxlogin'];
		$numlogin = $access['numlogin'];
		if(++$numlogin > $maxlogin) {
			mysql_query("UPDATE users SET accstatus = 0 WHERE username='${user}'");
			header( 'WWW-Authenticate: Basic realm="'. $auth_realm .'"' ); 
			header( 'HTTP/1.0 401 '. _('Unauthorized')); 
			mkerror(_('Your account has been disabled.'));
			return(false);
		}
		// increment the number of logins		
		mysql_query("UPDATE users SET numlogin = $numlogin WHERE username='${user}'");
	}
	return(true);
}
?>
