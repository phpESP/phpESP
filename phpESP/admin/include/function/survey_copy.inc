<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto void add_squotes(string* value, string key)
   Adds single quotes to value. */
function add_squotes (&$val, $key) {
	$val = _addslashes($val);
}
/* }}} */

/* {{{ proto bool survey_copy(int survey_id)
   Creates an editable copy of a survey. */
function survey_copy($sid) {
    $survey_fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info');
	$sql = "SELECT ". join(', ',$survey_fields)." FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
	$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
	if(record_count($result) != 1)
		return(false);
	$survey = fetch_row($result);
	db_close($result);

	// clear the sid,  change the name, and clear the status
	$survey['owner'] = $GLOBALS['HTTP_SESSION_VARS']['acl']['username'];
	$survey['name'] .= '_'. _('copy');
	$survey['status'] = 0;

	// check for 'name' conflict, and resolve
	$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=". _addslashes($survey['name']);
	$i=0;
    $result = execute_sql($sql);
	while($result->fields[0] > 0) {
        $result->MoveNext();
		$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=". _addslashes($survey['name'].++$i);
        $result = execute_sql($sql);
	}
	if($i)
		$survey['name'] .= $i;

	// create new survey
	array_walk($survey, 'add_squotes');
	$survey['changed'] = sys_time_stamp();
	$sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG']['survey_table']." ";
	$sql .= '('. join(',',array_keys($survey)) .') ';
	$sql .= 'VALUES ( '. join(',',array_values($survey)) .' )';
	$result = execute_sql($sql);
	if(!$result)
		return(false);
	$new_sid = insert_id('survey_id_seq');

	$has_choices = esp_type_has_choices();

	// make copies of all the questions
    $question_fields = array('id', 'survey_id', 'name', 'type_id', 'result_id', 'length', 'precise', 
    'position', 'content', 'required', 'deleted', 'public');
	$sql = "SELECT ". join(', ',$question_fields)." FROM ".$GLOBALS['ESPCONFIG']['question_table']."
		WHERE survey_id=${sid} AND deleted='N'
		ORDER by position, id";
	$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
	$pos=1;
    array_shift($question_fields);
	while($question = fetch_row($result)) {
        $result->MoveNext();
		$tid = $question['type_id'];
		$qid = $question['id'];
		// fix some fields first
		$question['survey_id'] = $new_sid;
		$question['position'] = $pos++;

		// copy question to new survey
        array_shift($question);
        if(!isset($question['result_id'])) {
            $question['result_id'] = 'NULL';
        }
        $question['name'] = _addslashes($question['name']);
        $question['content'] = _addslashes($question['content']);
        $question['required'] = _addslashes($question['required']);
        $question['deleted'] = _addslashes($question['deleted']);
        $question['public'] = _addslashes($question['public']);
		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." ";
		$sql .= '('. join(',', $question_fields) .') ';
		$sql .= 'VALUES ( '. join(',',array_values($question)) .' )';
		if(!execute_sql($sql))
			return(false);
		$new_qid = insert_id('question_id_seq');

		// make copies of all the question choices (if exist)
		if($has_choices[$tid]) {
            $question_choice_fields = array('question_id', 'content', 'value');
			$sql = "SELECT ". join(', ',$question_choice_fields)."
                FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
				WHERE question_id=${qid}
				ORDER BY id";
			$result2 = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			while($choice = fetch_row($result2)) {
                $result2->MoveNext();
				$choice['question_id'] = $new_qid;
                $choice['content'] = _addslashes($choice['content']);
                $choice['value'] = _addslashes($choice['value']);

				$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_choice_table']." ";
				$sql .= '('. join(',',$question_choice_fields) .') ';
				$sql .= 'VALUES ( '. join(',',array_values($choice)) .' )';
				if(!execute_sql($sql))
					return(false);
			}
			db_close($result2);
		}
	}
	db_close($result);

	return(true);
}
/* }}} */

?>
