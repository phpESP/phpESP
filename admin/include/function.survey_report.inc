<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_REPORT')) {
 	define('_FUNCTION_SURVEY_REPORT',TRUE);
	
	if(!defined('DEBUG'))
		define('DEBUG',TRUE);

	// builds HTML for the results for the survey 'sid'
	// with the added limitation that the answer to
	// $questionId is an element of the array $choiceIds
	
	// if $questionId is not specified (or less than 0)
	// the standard (original) results display is created
	
	// void crossanalysis(int $surveyId, 
	//                    bool $showTotals, 
	//                    int $questionId,
	//                    int[] $choiceIds);
	
	function survey_report($sid, $details = 0, $format = '') {
		echo('<!-- $Id$ -->'."\n");

		// build associative array holding weather each question
		// type has answer choices or not and the table the answers are in
		$has_answers = array();
		$answer_table = array();
		$sql = 'SELECT id,has_answers,answer_table
				  FROM question_types 
				 ORDER BY id';
		if(!($result = mysql_query($sql))) {
			$errmsg = "Error phpESP system tables corrupted. [ Table(s): question_types ]";
			return($errmsg);
		}
		while($row = mysql_fetch_row($result)) {
			$has_answers[$row[0]]=$row[1];
			$answer_table[$row[0]]=$row[2];
		}
		mysql_free_result($result);

		// load survey title (and other globals)
		$sql = "SELECT * FROM surveys WHERE id='${sid}'";
		if(!($result = mysql_query($sql))) {
			$errmsg = "Error opening selected survey. [ ID:${sid} R:" . mysql_num_rows($result) ."]";
			return($errmsg);
		}
		$survey = mysql_fetch_array($result, MYSQL_ASSOC);
		mysql_free_result($result);

		// load survey questions
		$sql = "SELECT * 
				  FROM survey_questions 
				 WHERE survey_id='${sid}' AND 
				       status='' 
				 ORDER BY position,id";
		if(!($questions_result = mysql_query($sql))) {
			$errmsg = "Error opening selected survey. No questions found for survey. [ ID:${sid} ]";
			return($errmsg);
		}
?>

<h2><?php echo("Report for: ". $survey["title"] . " [ID: ". $survey['id'] ."]"); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<tr><th align="left">ID</th><th align="left">Question #</th><th align="left" colspan=2>Content</th></tr>
<?php
		while($question = mysql_fetch_array($questions_result, MYSQL_ASSOC)) {
			// process each question
			$qid = $question['id'];
			$tid = $question['type_id'];
			$table = $answer_table[$tid];

			if($tid == 99) {
				echo("<tr><td><hr></td></tr>\n");
				continue;
			}

			++$i;
			
			if($bg != "#ffffff")
				$bg = "#ffffff";
			else
				$bg = "#eeeeee";

?>
	<tr bgcolor="<?php echo($bg); ?>"><th align="left"><?php echo($qid); ?></th>
 		<td><?php echo($i); ?></td>
		<td colspan=2><?php echo($question["content"]); ?></td></tr>
<?php
			if($has_answers[$tid]) {
				$sql = "SELECT * FROM question_choices
						 WHERE question_id = ${qid}
						 ORDER BY id";
 				$choices_result = mysql_query($sql);
				while($choice = mysql_fetch_array($choices_result, MYSQL_ASSOC)) {
?>
	<tr bgcolor="<?php echo($bg); ?>"><th align="left"></th>
		<td></td>
		<th align="left"><?php echo($choice['id']); ?></th>
		<td><?php echo($choice['content']); ?></td></tr>
<?php
				}
				mysql_free_result($choices_result);
			} // end if has_answers
		} // end while
		mysql_free_result($questions_result);
?>
</table>
<?php
		return;
	} // end function survey_results

} // end _FUNCTION_SURVEY_RESULTS
