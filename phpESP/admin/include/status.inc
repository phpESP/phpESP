<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_destroy();

	if(!empty($op)) {
		$sql = "SELECT status FROM surveys WHERE id='${sid}'";
		$old_status = mysql_result(mysql_query($sql),0,0);
		$err = 0;
		$sql = '';
		// trying to perform some operation
		switch(strtolower($op)) {
			case 'c':	// Clear
				if($god) {
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
		$sql = "UPDATE surveys SET status='${status}' WHERE id='${sid}'";
		if(!$err && !empty($sql))
			mysql_query($sql);
		else {
			mkwarn('Can not set survey status.');
			mkerror("STATUS: ${old_status}");
		}
	}

//	$sql = "SELECT * FROM surveys ORDER BY status,name";
	$sql = "SELECT * FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>Survey Status</h2>
<div align="left">

<p><b>Test</b> transitions a survey into testing mode. At which point you may
perform a live test by taking the survey, and viewing the results. You will not
be able to make any further changes to the survey once you have switched to
test mode.</p>

<p><b>Activate</b> transitions a survey into active more. In this mode the
survey is open for production use, and may be put online. This will clear any
results from testing mode (if any). No further editing of survey is
allowed.</p>

<p><b>End</b> transitions a survey into ended mode. In this mode, no edits are
possible, no users may take the survey (it is inactive), but results are still
viewable from the results menu.</p>

<p><b>Archive</b> removes this survey. It is still stored in the database, but
no further interaction is allowed. You may <b>not</b> view the results of an
archived survey.</p>

</div>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
		<td><b>Status</b></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	while($survey = mysql_fetch_array($result)) {
		$stat = 'new';
		$test = '<a href="'. $PHP_SELF .'?where=status&op=t&sid='. $survey['id'] .'">Test</a>';
		$act  = '<a href="'. $PHP_SELF .'?where=status&op=a&sid='. $survey['id'] .'">Activate</a>';
		$done = '<a href="'. $PHP_SELF .'?where=status&op=e&sid='. $survey['id'] .'">End</a>';
		$del  = '<a href="'. $PHP_SELF .'?where=status&op=d&sid='. $survey['id'] .'">Archive</a>';
		
		if($survey['status'] & STATUS_DELETED) {
			$stat = 'Archived';
			$test = $act = $done = $del = '&nbsp;';
			continue;
		} elseif($survey['status'] & STATUS_DONE) {
			$stat = 'Ended';
			$test = $act = $done = '&nbsp;';
		} elseif($survey['status'] & STATUS_ACTIVE) {
			$stat = 'Active';
			$test = $act = '&nbsp;';
		} elseif($survey['status'] & STATUS_TEST) {
			$stat = 'Testing';
			$done = '&nbsp;';
			$test = '<a href="'. $PHP_SELF .'?where=status&op=m&sid='. $survey['id'] .'">Edit</a>';
		} else {
			$done = '&nbsp;';
		}
		
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($survey['id']); ?></td>
		<td><?php echo($survey['name']); ?></td>
		<td><?php echo($survey['title']); ?></td>
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
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
