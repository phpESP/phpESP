<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

	// see if a survey has been selected
	if(!empty($HTTP_GET_VARS['sid'])) {
		$sid = intval($HTTP_GET_VARS['sid']);
		if(empty($HTTP_GET_VARS['details']))
			$HTTP_GET_VARS['details'] = 1; // by default, show details in report

		/* check ACL to see if user is allowed to view
		 * _this_ survey */
		$srealm = auth_get_survey_realm($sid);
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$HTTP_SESSION_VARS['acl']['pdesign'],
						$HTTP_SESSION_VARS['acl']['pall'])) &&
				!auth_no_access(_('to access this form'))) {
			return;
		}
?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
<tr><td>
<?php
		$ret = survey_report($sid, $HTTP_GET_VARS['details']);
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
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm FROM survey
			WHERE NOT (status & $statusbad)
			ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']));
		$sql = "SELECT id,name,title,owner,realm FROM survey WHERE (owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."' ||
			realm $realms)
			AND NOT (status & $statusbad)
			ORDER BY id DESC";
	}
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Form Report')); ?></h2>
<?php echo(_('Pick Form to View')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Group')); ?></th>
	</tr>
<?php
    $bg = '';
	while(list($sid, $name, $title, $owner, $realm) = mysql_fetch_row($result)) {
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=report&sid=${sid}"); ?>">
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
