<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if(!empty($sid)) {
		$msg = survey_copy($sid);
		if($msg) {
?>
<font color="<?php echo($cf_error_color); ?>"><?php echo($ret); ?></font>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<?php
			return;
		}
?>
<script language="JavaScript"><!--
window.location="<?php echo("${ME}?where=manage"); ?>"
//-->
</script>
<noscript><a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a></noscript>
<?php
		return;
	}

//	$sql = "SELECT * FROM surveys ORDER BY status,name";
	$sql = "SELECT * FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);
?>
<h2>Copy Survey</h2>
Chose a survey to make a copy of. The copy will have the same status of a newly created survey. You will
be able to edit the survey, and will have to activate it before use.
<table border="0" cellpadding="4" cellspacing="0" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
		<td><b>Status</b></td>
	</tr>
<?php
	while($survey = mysql_fetch_array($result)) {
		if($survey['status'] & STATUS_DELETED) {
			$stat = 'Deleted';
		} elseif($survey['status'] & STATUS_DONE) {
			$stat = 'Ended';
		} elseif($survey['status'] & STATUS_ACTIVE) {
			$stat = 'Active';
		} else {
			$stat = 'Editing';
		}
		
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
		

?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($survey['id']); ?></td>
		<td><?php echo($survey['name']); ?></td>
		<td><a href="<?php echo($PHP_SELF); ?>?where=copy&sid=<?php echo($survey['id']); ?>"><?php echo($survey['title']); ?></a></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
?>
</table>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
