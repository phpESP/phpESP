<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	/* get the sid ... if any */
	if(!empty($HTTP_GET_VARS['sid']))
	    $sid = intval($HTTP_GET_VARS['sid']);

	if(!empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);

    $bg = '';

	/* sid selected for testing */
	if(!empty($sid)) {
		$sql = "SELECT status,owner,realm,theme FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
		$result = execute_sql($sql);
		list($status,$owner,$realm,$theme) = fetch_row($result);
		db_close($result);

		/* check ACL to see if user is allowed to test
		 * _this_ survey */
		$realms = array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']);
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				$owner != $HTTP_SESSION_VARS['acl']['username'] &&
				!in_array($realm, $realms) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
		if (($status & STATUS_TEST) &&
			!($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE))) {
			/* cleared for testing ... was that enough tests? */
?>
</div>
</form>
<?php

			/* make the css style available during the testing.
			 * this should really go into the head section ...
			 * but should still work */
			if (!empty($theme))
				echo('<link rel="stylesheet" href="' .
						$GLOBALS['ESPCONFIG']['css_url'].$theme . '" type="text/css">');
?>
<b><?php echo(_('Testing Survey...')); ?></b> (<?php echo(_('SID')." = $sid"); ?>)
<p>
<?php
			/* set things up for the handler to take over ... */
			$test = true;
			unset($HTTP_GET_VARS['sid']);
			unset($HTTP_POST_VARS['sid']);
			$HTTP_POST_VARS['userid'] = 'test';
			define('ESP-HANDLER-PREFIX', true);
			define('ESP-AUTH-OK', true);
			$HTTP_POST_VARS['sec'] = empty($HTTP_POST_VARS['sec']) ? 1 : intval($HTTP_POST_VARS['sec']);
			$HTTP_POST_VARS['rid'] = empty($HTTP_POST_VARS['rid']) ? 0 : intval($HTTP_POST_VARS['rid']);
			include($ESPCONFIG['handler']);
			//undefine('ESP-HANDLER-PREFIX');
			//undefine('ESP-AUTH-OK');
?>
<a href="<?php echo(htmlentities($ESPCONFIG['ME']."?where=results&sid=".$sid)) ?>&test=1"><?php echo(_('View Results')); ?></a><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<form><div>
<?php
			db_close($result);
			return;
		}
	}

	/* no survey selected ... show a menu to choose from */

	/* check with ACL for allowed surveys */
	/* load names and titles of all surveys available to
	 * _this_ user */
	$statusbad = (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE);
	$statusok = STATUS_TEST;
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
		WHERE ".db_bin("status",$statusok) ." AND NOT ".db_bin("status", $statusbad)."
		ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']));
		$sql = "SELECT id,name,title,owner,realm
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
			WHERE ".db_bin("status",$statusok) ."AND NOT ".db_bin("status",$statusbad) ."AND
			(owner = ". _addslashes($HTTP_SESSION_VARS['acl']['username']) ." || realm $realms)
			ORDER BY id DESC";
	}
	$result = execute_sql($sql);

?>
</div>
</form>
<h2><?php echo(_('Test a Survey')); ?></h2>
<?php echo(_('Pick Survey to Test')); ?>
<table cellspacing="0" cellpadding="4">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<th><?php echo(_('Export')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$owner,$realm) = fetch_row($result)) {
        $result->MoveNext();

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
		<td><a href="<?php echo(htmlentities("". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=${sid}")); ?>"><?php echo(_('Test')); ?></a></td>
		<td><a href="<?php echo(htmlentities("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&test=1&sid=${sid}")); ?>"><?php echo(_('Results')); ?></a></td>
		<td style="font-size: xx-small; vertical-align: top;">
        <form id="export<?php echo($sid) ?>" action="<?php echo($GLOBALS['ESPCONFIG']['ME']) ?>">
	<div>
        <select name="type">
            <option value="csv_full_header" selected="selected">CSV Full Headers</option>
            <option value="csv_short_header">CSV Short Headers</option>
            <option value="tab">Tab</option>
        </select>
        <input type="hidden" name="test" value="1" />
        <input type="hidden" name="sid" value="<?php echo($sid) ?>" />
        <input type="hidden" name="where" value="" />
			<?php echo('<a href="javascript: exportSubmit(\'export\', document.getElementById(\'export'.$sid.'\'));" title="Save CSV Long Header (old method). Save the results for \''.$name.'\' to the survey in CSV Format (Comma Delimted File). Column Titles are based on question text.">' . _('Save On Server') . '</a>');
            echo("\n");?>
			| <?php
			echo('<a href="javascript: exportSubmit(\'download\', document.getElementById(\'export'.$sid.'\'));" title="Download CSV Long Header (old method). Download the results for \''.$name.'\' to your computer in CSV Format (Comma Delimted File). Column Titles are based on the question text. Click this link and select \'Save\' when prompted by your browser.">' . _('Download') . '</a>'); ?>
            </div></form>
		</td>
	</tr>
<?php
	}
?>
</table>

<form method="post" action="" id="null">
<div>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
