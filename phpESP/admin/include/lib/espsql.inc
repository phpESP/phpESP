<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto string array_to_insql(array elements)
   Returns a string of a SQL fragment to klugde around
   mysql's lack of nested SELECTS. */
function array_to_insql($array) {
	if (count($array))
		return("IN (".ereg_replace("([^,]+)","'\\1'",join(",",$array)).")");
	return 'IS NULL';
}
/* }}} */

/* {{{ proto int survey_num_sections(int survey_id)
   Returns the number of sections in the survey. */
function survey_num_sections($sid) {
	$sql = "
SELECT COUNT(*) + 1
FROM question
WHERE survey_id='${sid}' AND
  type_id='99' AND
  deleted='N'";
	$result = mysql_query($sql);
	$count = mysql_result($result,0,0);
	mysql_free_result($result);
	return($count);
}
/* }}} */

/* {{{ proto array survey_get_sections(int survey_id)
	Returns 2D-array with question id's of sections. */
function survey_get_sections($sid, $field = 'id') {
	if (empty($field))
		$field = 'id';

	$sql = "SELECT $field, type_id FROM question
			WHERE survey_id = $sid AND deleted = 'N'
			ORDER BY position";
	if (!($result = mysql_query($sql)))
		return array();
	$ret = array();
	$sec = array();
	while (list($key, $type) = mysql_fetch_row($result)) {
		if ($type != 99) {
			array_push($sec, $key);
		} else {
			array_push($ret, $sec);
			$sec = array();
		}
	}
	array_push($ret, $sec);
	mysql_free_result($result);
	return $ret;
}
/* }}} */

/* {{{ proto array survey_get_section_questions(int survey_id)
	Returns 2D-array with question id's of sections, excludes
    the section text question type. */
function survey_get_section_questions($sid, $field = 'id') {
	if (empty($field))
		$field = 'id';

	$sql = "SELECT $field, type_id FROM question
			WHERE survey_id = $sid AND deleted = 'N'
			ORDER BY position";
	if (!($result = mysql_query($sql)))
		return array();
	$ret = array();
	$sec = array();
	while (list($key, $type) = mysql_fetch_row($result)) {
        if ($type != 100) {
	    	if ($type != 99) {
		    	array_push($sec, $key);
	    	} else {
	    		array_push($ret, $sec);
	    		$sec = array();
	    	}
        }
	}
	array_push($ret, $sec);
	mysql_free_result($result);
	return $ret;
}
/* }}} */


/* {{{ proto string survey_select_section_sql(int survey_id, int section)
   Returns a string of a SQL fragment to limit questions to
   specified section. */
function survey_select_section_sql($sid, $section, $table = '') {
	if(!empty($table))
		$table .= '.';
	$sql = "
SELECT position
FROM question
WHERE survey_id='${sid}' AND
   type_id='99' AND
   deleted='N'
ORDER BY position,id";
	$result = mysql_query($sql);
	$num_sections = mysql_num_rows($result) + 1;

	if($section > $num_sections)
		return('');	// invalid section

	$ret = array("${table}survey_id='${sid}'", "${table}deleted='N'");
	if($section>1 && $num_sections>1)
		array_push($ret, "${table}position>" . mysql_result($result,$section-2,0));
	if($section<$num_sections && $num_sections>1)
		array_push($ret, "${table}position<" . mysql_result($result,$section-1,0));

	mysql_free_result($result);
	return('WHERE ' . join(' AND ',$ret) . ' ');
}
/* }}} */

/* {{{ proto array esp_type_has_choices()
   Returns an associative array of bools indicating if each
   question type has answer choices. */
function esp_type_has_choices() {
	$has_choices = array();
	$sql = 'SELECT id, has_choices FROM question_type ORDER BY id';
	$result = mysql_query($sql);
	while(list($tid,$answ) = mysql_fetch_row($result)) {
		if($answ == 'Y')
			$has_choices[$tid]=1;
		else
			$has_choices[$tid]=0;
	}
	mysql_free_result($result);
	return($has_choices);
}
/* }}} */

/* {{{ proto array esp_type_response_table()
   Returns an associative array of bools indicating the
   table the responses are stored in. */
function esp_type_response_table() {
	$sql = 'SELECT id, response_table FROM question_type ORDER BY id';
	$result = mysql_query($sql);
	$response_table = array();
	while(list($tid,$answ) = mysql_fetch_row($result)) {
		$response_table[$tid]=$answ;
	}
	mysql_free_result($result);
	return($response_table);
}
/* }}} */

?>
