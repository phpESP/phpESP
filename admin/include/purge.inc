<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	function purge_survey($sid) {
		// work from the bottom up...
		// start with the survey results & question choices
		$sql = "SELECT survey_questions.id,question_types.answer_table FROM survey_questions,question_types ".
			"WHERE survey_id=$sid AND survey_questions.type_id=question_types.id";
		$result = mysql_query($sql);
		while(list($qid,$table) = mysql_fetch_row($result)) {
			$sql = "DELETE FROM $table WHERE question_id=$qid";
			mysql_query($sql); // results
			$sql = "DELETE FROM answers_other WHERE question_id=$qid";
			mysql_query($sql); // results
			$sql = "DELETE FROM question_choices WHERE question_id=$qid";
			mysql_query($sql); // choices
		}
		@mysql_free_result($result);
		$sql = "DELETE FROM survey_responses WHERE survey_id=$sid";
		mysql_query($sql);
		
		// now remove the questions
		$sql = "DELETE FROM survey_questions WHERE survey_id=$sid";
		mysql_query($sql);
		
		// and finally the survey
		$sql = "DELETE FROM surveys WHERE id=$sid";
		mysql_query($sql);
		
		return;
	}

	// clear the session
	@session_destroy();

	if(!empty($list)) {
		while(list(,$sid) = each($list)) {
			purge_survey($sid);
		}
	}

//	$sql = "SELECT * FROM surveys ORDER BY status,name";
	$sql = "SELECT * FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>Purge Surveys</h2>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td>&nbsp;</td>
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
			$stat = 'new';
		}
		
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><input type="checkbox" name="list[]" value="<?php echo($survey['id']); ?>"></td>
		<td><?php echo($survey['id']); ?></td>
		<td><?php echo($survey['name']); ?></td>
		<td><?php echo($survey['title']); ?></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
?>
	<tr bgcolor="#dddddd">
		<td colspan="5" align="center">
			<input type="reset" name="reset" value="Clear Checkboxes">
			<input type="submit" name="where" value="Purge">
		</td>
	</tr>
</table>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
