<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

	// see if a survey has been selected
	if(!empty($HTTP_GET_VARS['sid'])) {
		if(empty($HTTP_GET_VARS['details']))
			$HTTP_GET_VARS['details'] = 1; // by default, show details in report
?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
	$ret = survey_report($HTTP_GET_VARS['sid'], $HTTP_GET_VARS['details']);
	if($ret != '')
		echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=report\">" . _('Go back to Report Menu') . "</a>\n"); ?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$sql = "SELECT id,name,title,status FROM survey WHERE NOT (status & '".STATUS_DELETED."') ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Survey Report')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
	</tr>
<?php
	while(list($sid, $name, $title) = mysql_fetch_row($result)) {
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
		<td>
			<?php echo($title); ?>
		</td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
