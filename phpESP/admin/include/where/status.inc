<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_unset();
	@session_destroy();
	
	$sid = XADDSLASHES($HTTP_GET_VARS['sid']);

	if(!empty($HTTP_GET_VARS['op'])) {
		$sql = "SELECT status FROM survey WHERE id='${sid}'";
		$old_status = mysql_result(mysql_query($sql),0,0);
		$err = 0;
		$sql = '';
		$status = $old_status;
		// trying to perform some operation
		switch(strtolower($HTTP_GET_VARS['op'])) {
			case 'c':	// Clear
				if($HTTP_GET_VARS['god']) {
					$status = 0;
					break;
				}
			case 't':	// test
				$status = $old_status | STATUS_TEST;
				if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
					$err = 1;
				break;
			case 'm':	// Edit
				$status = $old_status & ~STATUS_TEST;
				if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
					$err = 1;
				else
					clear_results($sid);
				break;
			case 'a':	// activate
				$status = $old_status | STATUS_ACTIVE;
				if($old_status & ( STATUS_DELETED | STATUS_DONE ) )
					$err = 1;
				else
					clear_results($sid);
				break;
			case 'e':	// End
				$status = $old_status | STATUS_DONE;
				if($old_status & STATUS_DELETED )
					$err = 1;
				break;
			case 'd':	// Delete
				$status = $old_status | STATUS_DELETED;
				break;
		}
		$sql = "UPDATE survey SET status='${status}' WHERE id='${sid}'";
		if(!$err && !empty($sql))
			mysql_query($sql);
		else {
			mkwarn(_('Can not set survey status.'));
			mkerror(_('STATUS') .': '. $old_status);
		}
	}
?>
<h2><?php echo(_('Survey Status')); ?></h2>
<div align="left">

<p><?php echo(_('<b>Test</b> transitions a survey into testing mode. At which point you may
perform a live test by taking the survey, and viewing the results. You will not
be able to make any further changes to the survey once you have switched to
test mode.')); ?></p>

<p><?php echo(_('<b>Activate</b> transitions a survey into active more. In this mode the
survey is open for production use, and may be put online. This will clear any
results from testing mode (if any). No further editing of survey is
allowed.')); ?></p>

<p><?php echo(_('<b>End</b> transitions a survey into ended mode. In this mode, no edits are
possible, no users may take the survey (it is inactive), but results are still
viewable from the results menu.')); ?></p>

<p><?php echo(_('<b>Archive</b> removes this survey. It is still stored in the database, but
no further interaction is allowed. You may <b>not</b> view the results of an
archived survey.')); ?></p>

</div>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
		<td><b><?php echo(_('Status')); ?></b></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	$sql = 'SELECT id,name,title,status FROM survey ORDER BY id DESC';
	$result = mysql_query($sql);

	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		$stat = _('Editing');
		$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=t&sid=${sid}\">". _('Test') ."</a>";
		$act  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=a&sid=${sid}\">". _('Activate') ."</a>";
		$done = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=e&sid=${sid}\">". _('End') ."</a>";
		$del  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=d&sid=${sid}\">". _('Archive') ."</a>";
		
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
			$test = $act = $done = $del = '&nbsp;';
			continue;
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
			$test = $act = $done = '&nbsp;';
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
			$test = $act = '&nbsp;';
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
			$done = '&nbsp;';
			$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=m&sid=${sid}\">". _('Edit') ."</a>";
		} else {
			$done = '&nbsp;';
		}
		
		if($bg != '#ffffff')
			$bg = '#ffffff';
		else
			$bg = '#eeeeee';
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($stat); ?></td>
		<td><?php echo($test); ?></td>
		<td><?php echo($act); ?></td>
		<td><?php echo($done); ?></td>
		<td><?php echo($del); ?></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
