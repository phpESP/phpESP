<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* When using the authenticated handler you need to include
 * part of the script *before* your template so that the
 * HTTP Auth headers can be sent when needed.
 *
 * So, create your template file with this as the *first*
 * line:
 *   <?php $sid=<SID>; include('path-to-phpesp/public/authhand-prefix.php'); ?>
 * then where you want to include the actual survey use this
 * line:
 *   <?php inlcude('path-to-phpesp/public/authhand-suffix.php'); ?>
 *
 * For example, here is a very small template:
 *   <?php $sid=42; include('phpesp/public/authhand-prefix.php'); ?>
 *   <html>
 *   <head><title>Speednaked.com</title></head>
 *   <body>
 *   <table border="0" width="800">
 *   <tr><td colspan="2"><img src="speednaked-banner.png" 
 *       border="0" width="800" height="42"></td></tr>
 *   <tr><td><?php inlcude('speednaked-sidebar.php'); ?></td>
 *       <td><?php include('phpesp/public/authhand-suffix.php'); ?>
 *       </td>
 *   </tr>
 *   <tr><td colspan="2"><p align="right"><font size="-2">
 *       Copyright &copy; 2001. Speednaked.com</font></p></td></tr>
 *   </table>
 *   </body>
 *   </html>
 *   
 */


#	global $HTTP_SERVER_VARS, $HTTP_POST_VARS, $HTTP_GET_VARS;
	require($HTTP_SERVER_VARS['DOCUMENT_ROOT']. "/phpESP/admin/phpESP.ini");
	require($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);

	if(isset($HTTP_GET_VARS['sid'])) {
		echo(mkerror(_('Error processing survey: Security violation.')));
		return;
	}
	
	if(isset($HTTP_GET_VARS['results']) || isset($HTTP_POST_VARS['results'])) {
		echo(mkerror(_('Error processing survey: Security violation.')));
		return;
	}
	
	$sid = intval($sid);
	if(empty($sid))
		$sid = intval($HTTP_POST_VARS['sid']);

	if(empty($sid)) {
		echo(mkerror(_('Error processing survey: Survey not specified.')));
		return;
	}

	if(empty($HTTP_POST_VARS['userid'])) {
		// find remote user id (takes the first non-empty of the folowing)
		//  1. a GET variable named 'userid'
		//  2. the REMOTE_USER set by HTTP-Authintication
		//  3. the query string
		if (!empty($HTTP_GET_VARS['userid'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_GET_VARS['userid'];
		} elseif(!empty($HTTP_SERVER_VARS['REMOTE_USER'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_SERVER_VARS['REMOTE_USER'];
		} elseif(!empty($HTTP_SERVER_VARS['QUERY_STRING'])) {
			$HTTP_POST_VARS['userid'] = urldecode($HTTP_SERVER_VARS['QUERY_STRING']);
		} else {
			$HTTP_POST_VARS['userid'] = 'unknown';
		}
	}

	if(empty($HTTP_POST_VARS['referer']))
		$HTTP_POST_VARS['referer'] = $HTTP_SERVER_VARS['HTTP_REFERER'];

	if(empty($HTTP_POST_VARS['sec']) || $HTTP_POST_VARS['sec'] < 1) {
		$HTTP_POST_VARS['sec'] = 1;

		// check for authorization on the survey
		include($PIECES."/auth".$EXT);
		if(!survey_auth($sid,$PHP_AUTH_USER,$PHP_AUTH_PW))
			exit;
	}
	define('AUTHHAND-PREFIX',TRUE);
?>
