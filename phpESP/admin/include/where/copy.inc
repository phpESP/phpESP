<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if(!empty($HTTP_GET_VARS['sid'])) {
		if(!survey_copy($HTTP_GET_VARS['sid'])) {
			echo("<font color=\"". $ESPCONFIG['error_color'] ."\">". _('Error copying survey.') ."</font><br>\n");
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

	$sql = 'SELECT id,name,title,status FROM survey ORDER BY id DESC';
	$result = mysql_query($sql);
?>
<h2><?php echo(_('Copy Survey')); ?></h2>
<?php echo(_('
Chose a survey to make a copy of. The copy will have the same
status of a newly created survey. You will be able to edit the
survey, and will have to activate it before use.')); ?>
<table border="0" cellpadding="4" cellspacing="0" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
		<td><b><?php echo(_('Status')); ?></b></td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
		} elseif($status & STATUS_DONE) {
			$stat = -('Ended');
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
		} else {
			$stat = _('Editing');
		}
		
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=copy&sid=${sid}"); ?>"><?php echo($title); ?></a></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
