<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>


	// clear the session
	@session_destroy();

	if(!empty($list)) {
		while(list(,$sid) = each($list)) {
			survey_purge($sid);
		}
	}

	$sql ="
SELECT S.id,COUNT(*)
  FROM surveys S, survey_questions Q
 WHERE S.id=Q.survey_id AND Q.status=0
 GROUP BY S.id
 ORDER BY S.id DESC";
 	$result = mysql_query($sql);
	while(list($sid,$num) = mysql_fetch_row($result)) {
		$qnum[$sid] = $num;
	}
	mysql_free_result($result);

	$sql = "
SELECT S.id,S.name,S.title,S.status
  FROM surveys S
 GROUP BY S.id
 ORDER BY S.id DESC";
	$result = mysql_query($sql);

?>
<h2>Purge Surveys</h2>

	<p>This page is not directly on the main menu because it is
	dangerous. This <b>completly</b> removes everything about a
	survey from the database <b>forever</b>. All question info,
	general info, results, etc. are purged from the database. Do
	not do anything here that you are not completly certain
	about. There is no confirmation, there is no turning
	back.</p>

<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td>&nbsp;</td>
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
		<td><b># Q's</b></td>
		<td><b>Status</b></td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		if($status & STATUS_DELETED) {
			$stat = 'Archived';
		} elseif($status & STATUS_DONE) {
			$stat = 'Ended';
		} elseif($status & STATUS_ACTIVE) {
			$stat = 'Active';
		} elseif($status & STATUS_TEST) {
			$stat = 'Testing';
		} else {
			$stat = 'Editing';
		}
		
		if(!($num=$qnum[$sid]))
			$num = 0;
		
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><input type="checkbox" name="list[]" value="<?php echo($sid); ?>"></td>
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($num); ?></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
?>
	<tr bgcolor="#dddddd">
		<td colspan="6" align="center">
			<input type="reset" name="reset" value="Clear Checkboxes">
			<input type="submit" name="where" value="Purge">
		</td>
	</tr>
</table>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
