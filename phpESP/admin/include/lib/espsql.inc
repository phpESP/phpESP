<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_LIB_ESPSQL')) {
	define('_LIB_ESPSQL', TRUE);

/* {{{ proto string array_to_insql(array elements)
   Returns a string of a SQL fragment to klugde around
   mysql's lack of nested SELECTS. */
function array_to_insql($array) {
	return("IN (".ereg_replace("([^,]+)","'\\1'",join(",",$array)).")");
}
/* }}} */

/* {{{ proto int survey_num_sections(int survey_id)
   Returns the number of sections in the survey. */
function survey_num_sections($sid) {
	$sql = "
SELECT COUNT(*)
FROM survey_questions
WHERE survey_id='${sid}' AND
  type_id='99' AND
  status=0";
	$result = mysql_query($sql);
	$count = mysql_result($result,0,0) + 1;
	mysql_free_result($result);
	return($count);
}
/* }}} */

/* {{{ proto string survey_select_section_sql(int survey_id, int section)
   Returns a string of a SQL fragment to limit questions to
   specified section. */
function survey_select_section_sql($sid, $section) {
	$sql = "
SELECT position
FROM survey_questions
WHERE survey_id='${sid}' AND
   type_id='99' AND
   status=0
ORDER BY position,id";
	$result = mysql_query($sql);
	$num_sections = mysql_num_rows($result) + 1;

	if($section > $num_sections)
		return('');	// invalid section

	$ret = array("survey_id='${sid}'", 'status=0');
	if($section>1 && $num_sections>1)
		array_push($ret, 'position>' . mysql_result($result,$section-2,0));
	if($section<$num_sections && $num_sections>1)
		array_push($ret, 'position<' . mysql_result($result,$section-1,0));

	mysql_free_result($result);
	return('WHERE ' . join(' AND ',$ret) . ' ');
}
/* }}} */

/* {{{ proto array esp_type_has_choices()
   Returns an associative array of bools indicating if each
   question type has answer choices. */
function esp_type_has_choices() {
	$has_choices = array();
	$sql = 'SELECT id, has_choices FROM question_types ORDER BY id';
	$result = mysql_query($sql);
	while(list($tid,$answ) = mysql_fetch_row($result)) {
		$has_choices[$tid]=$answ;
	}
	mysql_free_result($result);
	return($has_choices);
}
/* }}} */

} // end _LIB_ESPSQL
?>
