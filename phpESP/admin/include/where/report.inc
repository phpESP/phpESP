<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

	// see if a survey has been selected
	if(!empty($HTTP_GET_VARS['sid'])) {
		$sid = intval($HTTP_GET_VARS['sid']);
		if($HTTP_GET_VARS['details'] != 0)
			$HTTP_GET_VARS['details'] = 1; // by default, show details in report

		/* check ACLs for permissions */
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
				auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) ||
				($HTTP_GET_VARS['test'] && 
					$HTTP_SESSION_VARS['acl']['seeall'] == 'Y') ||
				auth_no_access('to access this survey')) {
?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
	$ret = survey_report($sid, $HTTP_GET_VARS['details']);
	if($ret != '')
		echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=report\">" . _('Go back to Report Menu') . "</a>\n"); ?><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		}
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$statusbad = (STATUS_DELETED);
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
		$HTTP_SESSION_VARS['acl']['seeall'] == 'Y')
		$sql = "SELECT id,name,title,owner FROM survey 
			WHERE NOT (status & $statusbad)
			ORDER BY id DESC";
	else
		$sql = "SELECT id,name,title,owner FROM survey WHERE owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."' 
			AND NOT (status & $statusbad)
			ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Survey Report')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
	</tr>
<?php
	while(list($sid, $name, $title, $owner) = mysql_fetch_row($result)) {
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
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
