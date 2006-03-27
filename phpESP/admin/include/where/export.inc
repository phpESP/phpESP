<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if(!empty($_POST['sid']))
	$sid = intval($_POST['sid']);
elseif(!empty($_GET['sid']))
	$sid = intval($_GET['sid']);
else
	$sid = '';
$bg = '';

?>
</div>
</form>
<div class="contents">
<h2><?php echo(_('Export Data')); ?></h2>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<table cellspacing="0" cellpadding="4">
<?php

/* If the Survey ID is not given, then build
 * a menu of available surveys to chose from.
 */
if(!$sid) {
?>
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
		<th><?php echo(_('Status')); ?></th>
		<th colspan="2"><?php echo(_('Format')); ?></th>
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	$statusok = (STATUS_ACTIVE | STATUS_DONE | STATUS_DELETED);
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,status,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
		WHERE (status & $statusok) ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['pdata']));
		$sql = "SELECT id,name,title,status,owner,realm
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE (status & $statusok) AND (owner = ".
			_addslashes($_SESSION['acl']['username']) ." || realm $realms) ORDER BY id DESC";
	}
	$result = execute_sql($sql);

	while(list($sid,$name,$title,$status,$owner,$realm) = fetch_row($result)) {
        $result->MoveNext();
		$stat = _('Editing');

		if($status & STATUS_DELETED) {
			$stat = _('Archived');
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
		}

		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($stat); ?></td>
		<td style="font-size: xx-small; vertical-align: top;">
    <form id="export<?php echo($sid) ?>" action="<?php echo($GLOBALS['ESPCONFIG']['ME']) ?>">
	<div>
        <select name="type">
            <option value="csv_full_header" selected="selected">CSV Full Headers</option>
            <option value="csv_short_header">CSV Short Headers</option>
            <option value="spss_csv">CSV for SPSS</option>
            <option value="tab">Tab</option>
        </select>
        <input type="hidden" name="sid" value="<?php echo($sid) ?>" />
        <input type="hidden" name="where" value="" />
        <div class="exportsave">
			<?php echo('<a href="javascript: exportSubmit(\'export\', document.getElementById(\'export'.$sid.'\'));" title="Save the results for \''.$name.'\' to the survey in CSV Format (Comma Delimted File). Column Titles are based on question text.">' . _('Save On Server') . '</a>');
            echo("\n");?>
			| <?php
			echo('<a href="javascript: exportSubmit(\'download\', document.getElementById(\'export'.$sid.'\'));" title="Download the results for \''.$name.'\' to your computer in CSV Format (Comma Delimted File). Column Titles are based on the question text. Click this link and select \'Save\' when prompted by your browser.">' . _('Download') . '</a>'); ?>
            </div>
            </div></form>
		</td>
	</tr>
<?php
	}
?>
</table>
</div>
<form method="post" id="null" action="">
<div class="contents">
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
	return;
	}

	/* sid supplied ... continue */
	$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id = $sid";
	$name = get_one($sql);

	/* check ACLs for permissions */
	$srealm = auth_get_survey_realm($sid);
	if(isset($_GET['test'])) {
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
		echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = $sid)</p>\n");
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

	$type = isset($_GET['type']) ? $_GET['type'] : 'csv_full_header';

	// prepare file name, default to csv_full_headers
    switch ($type) {
        case 'tab': 
            $file = $_SESSION['acl']['home'] . "/$name.txt";
            break;
            
        case 'xml':
            //Not implemented yet
            break;
            
        case 'spss_csv':
            $file = $_SESSION['acl']['home'] . "/$name.csv";
            break;
            
        case 'dbf':
            $file = $_SESSION['acl']['home'] . "/$name.dbf";
            if(extension_loaded('dbase')) {
                $ret = survey_export_dbf($sid, $file);
            } else {
                $ret = 0;
                echo("<tr><td>" . mkerror(_('The PHP dBase Extension is not installed.')) . "</td></tr>");
            }
            break;

        default: //csv_full_header,csv_short_header
            $file = $_SESSION['acl']['home'] . "/$name.csv";
	}
	$ret = survey_export_results($type, $sid, $file);

	echo("<tr><td>");
	if(isset($_GET['test'])) {
		echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = $sid)</p>\n");
	}
	if($ret) {
		echo(_('Survey exported as:') . " <tt>$file</tt>");
	} else {
		echo(mkwarn(_('Error exporting survey as:') . " <tt>$file</tt>"));
	}
	echo("</td></tr></table>\n");
	echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");

?>
