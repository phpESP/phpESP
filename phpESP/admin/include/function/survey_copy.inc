<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_COPY')) {
 	define('_FUNCTION_SURVEY_COPY',TRUE);
	
/* {{{ proto void add_squotes(string* value, string key)
   Adds single quotes to value. */
function add_squotes (&$val, $key) {
	$val = "'" . addcslashes($val,"\\'") . "'";
}
/* }}} */

/* {{{ proto void survey_copy(int survey_id)
   Creates an editable copy of a survey. */
function survey_copy($sid) {
	$sql = "SELECT * FROM surveys WHERE id='${sid}'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) != 1)
		return;
	$survey = mysql_fetch_array($result,MYSQL_ASSOC);
	mysql_free_result($result);

	// clear the sid, clear the creation date, change the name, and clear the status
	$survey['id'] = '';
	$survey['created'] = '';
	$survey['name'] .= '_copy';
	$survey['status'] = 0;

	// check for 'name' conflict, and resolve
	$sql = "SELECT COUNT(*) FROM surveys WHERE name='". $survey['name'] ."'";
	$i=0;
	while(mysql_result(mysql_query($sql),0,0) > 0) {
		$sql = "SELECT COUNT(*) FROM surveys WHERE name='". $survey['name'] . ++$i ."'";
	}
	if($i)
		$survey['name'] .= $i;

	// create new survey
	array_walk($survey, 'add_squotes');
	$sql  = "INSERT INTO surveys ";
	$sql .= '('. join(',',array_keys($survey)) .') ';
	$sql .= 'VALUES ( '. join(',',array_values($survey)) .' )';
	$result = mysql_query($sql);
	if(!$result)
		return(1);
	$new_sid = mysql_insert_id();

	// build an array for weather a question type has answer options
	// used in copying questions
	$has_answers = esp_type_has_choices();

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
			mysql_free_result($result2);
		}
	}
	mysql_free_result($result);

	return;
}
/* }}} */

} // end _FUNCTION_SURVEY_COPY
?>
