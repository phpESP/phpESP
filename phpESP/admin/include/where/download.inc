<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Download patch submitted by Matthew Gregg
// <greggmc@musc.edu>
// Modified by James Flemer
// <jflemer@alum.rpi.edu>

	$sid = -1;
	if (!empty($_GET['sid']))
		$sid = intval($_GET['sid']);
	else if(!empty($_POST['sid']))
		$sid = intval($_POST['sid']);

	$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id = $sid";
	$result = execute_sql($sql);
	if (record_count($result) < 1) {
		db_close($result);
		echo mkerror(_('Invalid survey ID.'));
		return;
	}
	$name = $result->fields[0];
    db_close($result);

	/* check ACLs for permissions */
	$srealm = auth_get_survey_realm($sid);
	if (isset($_GET['test'])) {
		/* check ACL to see if user is allowed to test
		 * _this_ survey */
		if($_SESSION['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $_SESSION['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$_SESSION['acl']['pdesign'],
						$_SESSION['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
	} else {
		/* check ACL to see if user is allowed to export
		 * _this_ survey */
		if($_SESSION['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $_SESSION['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$_SESSION['acl']['pdata'],
						$_SESSION['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
	}

    function printCommonHeaders() {
        header("Pragma: public");
        header("Expires: 0");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Cache-Control: private",false);
        header("Content-Transfer-Encoding: ascii");
    }

	$type = isset($_GET['type']) ? $_GET['type'] : 'csv_full_header';
	
	// default to csv_full_header
    switch($type) {
        case 'spss':
            //NOT FULLY IMPLEMENT YET
            header("Content-Transfer-Encoding: ascii");
            printCommonHeaders();
            header("Content-Type: text/comma-separated-values");
            header("Content-Disposition: attachment; filename={$name}.csv");
            header("Content-Type: application/txt");
            $output = survey_generate_results($type, $sid);
            //Do something with output
            foreach ($output as $row ) {
                echo(implode(',', $row) . "\r\n");
            }
            break;

        case 'xml':
            //NOT FULLY IMPLEMENT YET
            header("Content-Transfer-Encoding: ascii");
            printCommonHeaders();
            header("Content-Disposition: attachment; filename=$name.xml");
            header("Content-Type: text/xml");
            $output = survey_generate_results($type, $sid);
            //Do something with output
            break;

        case 'tab':
            // TAB
            header("Content-Transfer-Encoding: ascii");
            printCommonHeaders();
            header("Content-Disposition: attachment; filename=$name.txt");
            header("Content-Type: text/tab-separated-values");
            $output = survey_generate_results($type, $sid);
            foreach ($output as $row ) {
                echo(implode(chr(9), $row) . "\r\n");
            }
            break;
            
        default:	
            // CSV, csv_full_header old method, csv_short_header new method
            header("Content-Transfer-Encoding: ascii");
            printCommonHeaders();
            header("Content-Disposition: attachment; filename=$name.csv");
            header("Content-Type: text/comma-separated-values");
            $output = survey_generate_results($type, $sid);
            foreach ($output as $row ) {
                echo(implode(',', $row) . "\r\n");
            }
    }
	return;
?>
