<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if(!empty($sid)) {
		$ret = survey_copy($sid);
		if($ret) {
			echo("<font color=\"${cf_error_color}\">${ret}</font><br>\n");
			echo("<a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
			return;
		}
?>
<script language="JavaScript"><!--
window.location="<?php echo("${ME}?where=manage"); ?>"
//-->
</script>
<?php 
		echo("<noscript><a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a></noscript>\n");
		return;
	}

	$sql = 'SELECT id,name,title,status FROM surveys ORDER BY id DESC';
	$result = mysql_query($sql);
?>
<h2><?php echo(_('Copy Survey')); ?></h2>
<?php echo(_('
Chose a survey to make a copy of. The copy will have the same
status of a newly created survey. You will be able to edit the
survey, and will have to activate it before use.')); ?>
<table border="0" cellpadding="4" cellspacing="0" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
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
		
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><a href="<?php echo("${ME}?where=copy&sid=${sid}"); ?>"><?php echo($title); ?></a></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
