<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

	// see if a survey has been selected
	if(!empty($_GET['sid'])) {
		$sid = intval($_GET['sid']);
		if(empty($_GET['details']))
			$_GET['details'] = 1; // by default, show details in report

		/* check ACL to see if user is allowed to view
		 * _this_ survey */
		$srealm = auth_get_survey_realm($sid);
		if($_SESSION['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $_SESSION['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$_SESSION['acl']['pdesign'],
						$_SESSION['acl']['pall'])) &&
				!auth_no_access(_('to access this form'))) {
			return;
		}
?>
<table cellspacing="0" cellpadding="4">
<tr><td>
<?php
		$ret = survey_report($sid, $_GET['details']);
		if($ret != '')
			echo(mkerror($ret));
?>
</td></tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=report\">" . _('Go back to Report Menu') . "</a>\n"); ?><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$statusbad = (STATUS_DELETED);
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
			WHERE NOT (status & $statusbad)
			ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['pdesign']));
		$sql = "SELECT id,name,title,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE (owner=".
			_addslashes($_SESSION['acl']['username']) ." ||
			realm $realms)
			AND NOT (status & $statusbad)
			ORDER BY id DESC";
	}
	$result = execute_sql($sql);

?>
<h2><?php echo(_('View Form Report')); ?></h2>
<?php echo(_('Pick Form to View')); ?>
<table cellspacing="0" cellpadding="4">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
	</tr>
<?php
    $bg = '';
	while(list($sid, $name, $title, $owner, $realm) = fetch_row($result)) {
        $result->MoveNext();
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=report&sid=${sid}")); ?>">
			<?php echo($name); ?></a>
		</td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
