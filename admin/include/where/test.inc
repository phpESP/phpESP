<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_unset();
	@session_destroy();
	
	$sid = intval($HTTP_GET_VARS['sid']);
	if(!empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);

	if(!empty($sid)) {
		$sql = "SELECT status FROM survey WHERE id=${sid}";
		$status = mysql_result(mysql_query($sql),0,0);
		if(($status & STATUS_TEST) && !($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE))) {
?>
</form>
<b><?php echo(_('Testing Survey...')); ?></b> (SID = <?php echo($sid); ?>)
<p>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
			$test=TRUE;
			unset($HTTP_GET_VARS['sid']);
			unset($HTTP_POST_VARS['sid']);
			$HTTP_POST_VARS['userid'] = 'test';
			$tmp = $ESPCONFIG['ME'];
			$HTTP_SERVER_VARS['PHP_SELF'] = $ESPCONFIG['ME'].'?where=test';
			include($ESPCONFIG['handler']);
			$ESPCONFIG['ME'] = $tmp;
?>
</td></tr></table>
<a href="<?php echo($ESPCONFIG['ME']); ?>?where=results&sid=<?php echo($sid); ?>&test=1"><?php echo(_('View Results')); ?></a><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<form>
<?php
			return;
		}
	}
	
	$sql = 'SELECT id,name,title,status FROM survey ORDER BY id DESC';
	$result = mysql_query($sql);

?>
<h2><?php echo(_('Test a Survey')); ?></h2>
<?php echo(_('Pick Survey to Test')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		// only include "inactive" surveys for editing!
		if(!($status & STATUS_TEST) || 
			($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE)))
				continue;

		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=${sid}"); ?>"><?php echo(_('Test')); ?></a></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&test=1&sid=${sid}"); ?>"><?php echo(_('Results')); ?></a></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
