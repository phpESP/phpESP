<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* When using the authentication for responses you need to include
 * part of the script *before* your template so that the
 * HTTP Auth headers can be sent when needed.
 *
 * So, create your template file with this as the *first*
 * line:
 *   <?php $sid=<SID>; include('.../phpESP/public/authhand-prefix.php'); ?>
 * then where you want to include the actual survey use this
 * line:
 *   <?php inlcude('.../phpESP/public/authhand-suffix.php'); ?>
 *
 * For example, here is a very small template:
 *   <?php $sid=42; include('phpESP/public/authhand-prefix.php'); ?>
 *   <html>
 *   <head><title>Speednaked.com</title></head>
 *   <body>
 *   <table border="0" width="800">
 *   <tr><td colspan="2"><img src="speednaked-banner.png" 
 *       border="0" width="800" height="42"></td></tr>
 *   <tr><td><?php inlcude('speednaked-sidebar.php'); ?></td>
 *       <td><?php include('phpESP/public/authhand-suffix.php'); ?>
 *       </td>
 *   </tr>
 *   <tr><td colspan="2"><p align="right"><font size="-2">
 *       Copyright &copy; 2001. Speednaked.com</font></p></td></tr>
 *   </table>
 *   </body>
 *   </html>
 *   
 */

if(!defined('AUTHHAND-PREFIX')) {
	define('AUTHHAND-PREFIX', TRUE);
	// undefine('AUTHHAND-OK');
	
	require('/usr/local/lib/php/contrib/phpESP/admin/phpESP.ini');
	require($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);
	
	$GLOBALS['errmsg'] = '';

	if(isset($HTTP_GET_VARS['sid'])) {
		$GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
		return;
	}
	
	if(isset($HTTP_GET_VARS['results']) || isset($HTTP_POST_VARS['results'])) {
		$GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
		return;
	}

	if (isset($sid) && !empty($sid))
		$sid = intval($sid);
	else if (isset($HTTP_POST_VARS['sid']) && !empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);

	if(!isset($sid) || empty($sid)) {
		$GLOBALS['errmsg'] = mkerror(_('Error processing survey: Survey not specified.'));
		return;
	}

	if(empty($HTTP_POST_VARS['userid'])) {
		// find remote user id (takes the first non-empty of the following)
		//  1. a GET variable named 'userid'
		//  2. the REMOTE_USER set by HTTP-Authentication
		//  3. the query string
		//  4. the remote ip address
		if (!empty($HTTP_GET_VARS['userid'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_GET_VARS['userid'];
		} elseif(!empty($HTTP_SERVER_VARS['REMOTE_USER'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_SERVER_VARS['REMOTE_USER'];
		} elseif(!empty($HTTP_SERVER_VARS['QUERY_STRING'])) {
			$HTTP_POST_VARS['userid'] = urldecode($HTTP_SERVER_VARS['QUERY_STRING']);
		} else {
			$HTTP_POST_VARS['userid'] = $HTTP_SERVER_VARS['REMOTE_ADDR'];
		}
	}

	if(empty($HTTP_POST_VARS['referer']))
		$HTTP_POST_VARS['referer'] = isset($HTTP_SERVER_VARS['HTTP_REFERER']) ?
			$HTTP_SERVER_VARS['HTTP_REFERER'] : '';

	if(empty($HTTP_POST_VARS['sec']) || $HTTP_POST_VARS['sec'] < 1) {
		$HTTP_POST_VARS['sec'] = 1;

		if($ESPCONFIG['auth_response']) {
			// check for authorization on the survey
			include($ESPCONFIG['include_path']."/lib/espauth".$ESPCONFIG['extension']);
			if(!survey_auth(
					$sid, 
					addslashes($HTTP_SERVER_VARS['PHP_AUTH_USER']),
					addslashes($HTTP_SERVER_VARS['PHP_AUTH_PW'])))
				return;
		}
	}
	define('AUTHHAND-OK', TRUE);
} /* !defined('AUTHHAND-PREFIX') */
?>
