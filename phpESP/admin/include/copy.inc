<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	function add_squotes (&$val, $key) {
		$val = "'" . ereg_replace("'","\\'",$val) . "'";
	}

	function copy_survey ($sid) {
		$sql = "SELECT * FROM surveys WHERE id='${sid}'";
		$result = @mysql_query($sql);
		if(mysql_num_rows($result) != 1)
			return(1);
		$survey = mysql_fetch_array($result,MYSQL_ASSOC);
		@mysql_free_result($result);

		// clear the sid, clear the creation date, change the name, and clear the status
		$survey['id'] = '';
		$survey['created'] = '';
		$survey['name'] .= '_copy';
		$survey['status'] = 0;

		// check for 'name' conflict, and resolve
		$sql = "SELECT COUNT(*) FROM surveys WHERE name='". $survey['name'] ."'";
		$i=0;
		while(@mysql_result(@mysql_query($sql),0,0) > 0) {
			$sql = "SELECT COUNT(*) FROM surveys WHERE name='". $survey['name'] . ++$i ."'";
		}
		if($i)
			$survey['name'] .= $i;

		// create new survey
		array_walk($survey, 'add_squotes');
		$sql  = "INSERT INTO surveys ";
		$sql .= '('. join(',',array_keys($survey)) .') ';
		$sql .= 'VALUES ( '. join(',',array_values($survey)) .' )';
		$result = @mysql_query($sql);
		if(!$result)
			return(1);
		$new_sid = mysql_insert_id();

		// build an array for weather a question type has answer options
		// used in copying questions
		$has_answers = arr_has_answers();

		// make copies of all the questions
		$sql = "SELECT * FROM survey_questions WHERE survey_id='${sid}' ORDER by position,id";
		$result = mysql_query($sql);
		$pos=10;
		while($question = mysql_fetch_array($result,MYSQL_ASSOC)) {
			// skip deleted question in copy
			if($question['status'] & STATUS_DELETED)
				continue;
				
			$tid = $question['type_id'];
			$qid = $question['id'];

			// fix some fields first
			$question['id'] = '';
			$question['survey_id'] = $new_sid;
			$question['position'] = $pos;
			$pos += 10;

			// copy question to new survey
			array_walk($question,'add_squotes');
			$sql = "INSERT INTO survey_questions ";
			$sql .= '('. join(',',array_keys($question)) .') ';
			$sql .= 'VALUES ( '. join(',',array_values($question)) .' )';
			if(!mysql_query($sql))
				return(1);
			$new_qid = mysql_insert_id();

			// make copies of all the question choices (if exist)
			if($has_answers[$tid]) {
				$sql = "SELECT * FROM question_choices WHERE question_id='${qid}' ORDER by id";
				$result2 = mysql_query($sql);
				while($choice = mysql_fetch_array($result2,MYSQL_ASSOC)) {
					$choice['id'] = '';
					$choice['question_id'] = $new_qid;

					array_walk($choice,'add_squotes');
					$sql = "INSERT INTO question_choices ";
					$sql .= '('. join(',',array_keys($choice)) .') ';
					$sql .= 'VALUES ( '. join(',',array_values($choice)) .' )';
					if(!mysql_query($sql))
						return(1);
				}
				@mysql_free_result($result2);
			}
		}
		@mysql_free_result($result);

		return;
	}

	if(!empty($sid)) {
		copy_survey($sid);
?>
<script language="JavaScript"><!--
window.location="<?php echo($PHP_SELF . "?where=manage"); ?>"
//-->
</script>
<noscript><a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a></noscript>
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
