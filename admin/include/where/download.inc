<?php

# $Id$

// Download patch submitted by Matthew Gregg
// <greggmc@musc.edu>
// Modified by James Flemer
// <jflemer@acm.jhu.edu>

	$sid = intval($_REQUEST['sid']);

	$sql = "SELECT name FROM survey WHERE id = $sid";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) < 1) {
		mysql_free_result($result);
		echo mkerror(_('Invalid survey ID.'));
		return;
	}
	$name = mysql_result($result, 0, 0);
	mysql_free_result($result);

	/* check ACLs for permissions */
	$srealm = auth_get_survey_realm($sid);
	if($HTTP_GET_VARS['test']) {
		/* check ACL to see if user is allowed to test
		 * _this_ survey */
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$HTTP_SESSION_VARS['acl']['pdesign'],
						$HTTP_SESSION_VARS['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
	} else {
		/* check ACL to see if user is allowed to export
		 * _this_ survey */
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$HTTP_SESSION_VARS['acl']['pdata'],
						$HTTP_SESSION_VARS['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
	}

	$type = isset($_REQUEST['type']) ? $_REQUEST['type'] : 'csv';
	
	// Try DBF first, default to CSV
	if($type == 'dbf') {
		$file = "$name.dbf";
		if(extension_loaded('dbase')) {
			echo mkerror(_('DBF download not yet implemented.'));
		} else {
			echo mkerror(_('The PHP dBase Extension is not installed.'));
		}
		return;
	}
	// CSV
	$data = survey_generate_csv($sid);
	header("Content-Transfer-Encoding: ascii");
	header("Content-Disposition: attachment; filename=$name.csv");
	header("Content-Type: application/x-csv");
	$output = survey_generate_csv($sid);
	foreach ($output as $row ) {
		echo(join(',', $row) . "\n");
	}
	return;
?>
