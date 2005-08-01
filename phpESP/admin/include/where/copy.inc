<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	/* if survey has been selected ... */
	if(!empty($_GET['sid'])) {
		$sid = intval($_GET['sid']);
		
		$srealm = auth_get_survey_realm($sid);
		/* check ACL to see if user is allowed to copy
		 * _this_ survey */
		if($_SESSION['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $_SESSION['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$_SESSION['acl']['pdesign'],
						$_SESSION['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}

		/* copy the survey */
		if(!survey_copy($sid)) {
			echo(mkerror(_('Error copying survey.') ." (". ErrorMsg() .")") . "<br>\n");
			echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
			return;
		}
?>
<script language="JavaScript"><!--
window.location="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=manage"); ?>"
//-->
</script>
<?php
		echo("<noscript><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a></noscript>\n");
		return;
	}

	/* load names and titles of all surveys available to
	 * _this_ user */
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,status,owner,realm FROM '.$GLOBALS['ESPCONFIG']['survey_table'].' ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['pdesign']));
		$sql = "SELECT id,name,title,status,owner,realm
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE NOT (status & ". STATUS_DELETED .") AND (owner = ".
			_addslashes($_SESSION['acl']['username']) ." || realm $realms) ORDER BY id DESC";
	}
	$result = execute_sql($sql);
	$bg = '';

?>
<h2><?php echo(_('Copy Survey')); ?></h2>
<?php echo(
_('Choose a survey of which to make a copy. The copy will have the same
status of a newly created survey. You will be able to edit the
survey, and will have to activate it before use.')); ?>
<table cellpadding="4" cellspacing="0">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Status')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$status,$owner,$realm) = fetch_row($result)) {
        $result->MoveNext();
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
		} else {
			$stat = _('Editing');
		}
		
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo(htmlentities("". $GLOBALS['ESPCONFIG']['ME'] ."?where=copy&sid=${sid}")); ?>"><?php echo($name); ?></a></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($stat); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
	</tr>
<?php
	}
	db_close($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
