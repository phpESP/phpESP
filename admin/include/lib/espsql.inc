<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto string array_to_insql(array elements)
   Returns a string of a SQL fragment to klugde around
   mysql's lack of nested SELECTS. */
	function array_to_insql($array) {
		if (count($array))
			return("IN (".ereg_replace("([^,]+)","\\1",join(",",array_map("_addslashes", $array))).")");
		return 'IS NULL';
	}
/* }}} */

/* {{{ proto int survey_num_sections(int survey_id)
   Returns the number of sections in the survey. */
function survey_num_sections($sid) {
	$sql = "
		SELECT COUNT(*) + 1
		FROM ".$GLOBALS['ESPCONFIG']['question_table']."
		WHERE survey_id=${sid} AND
		type_id='99' AND
		deleted='N'";
	$result = execute_sql($sql);
	$count = $result->fields[0];
	db_close($result);
	return($count);
}
/* }}} */

/* {{{ proto array survey_get_sections(int survey_id)
   Returns 2D-array with question id's of sections. */
	function survey_get_sections($sid, $field = 'id') {
		if (empty($field))
			$field = 'id';

		$sql = "SELECT $field, type_id FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			WHERE survey_id = $sid AND deleted = 'N'
			ORDER BY position";
		if (!($result = execute_sql($sql)))
			return array();
		$ret = array();
		$sec = array();
		while (list($key, $type) = fetch_row($result)) {
			$result->MoveNext();
			if ($type != 99) {
				array_push($sec, $key);
			} else {
				array_push($ret, $sec);
				$sec = array();
			}
		}
		array_push($ret, $sec);
		db_close($result);
		return $ret;
	}
/* }}} */

/* {{{ proto array survey_get_section_questions(int survey_id)
   Returns 2D-array with question id's of sections, excludes
   the section text question type. */
	function survey_get_section_questions($sid, $field = 'id') {
		if (empty($field))
			$field = 'id';

		$sql = "SELECT $field, type_id FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			WHERE survey_id = $sid AND deleted = 'N'
			ORDER BY position";
		if (!($result = execute_sql($sql)))
			return array();
		$ret = array();
		$sec = array();
		while (list($key, $type) = fetch_row($result)) {
			$result->MoveNext();
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
		db_close($result);
		return $ret;
	}
/* }}} */


/* {{{ proto string survey_select_section_sql(int survey_id, int section)
   Returns a string of a SQL fragment to limit questions to
   specified section. */
	function survey_select_section_sql($sid, $section, $table = '') {
		if(!empty($table))
			$table .= '.';
		$sql = "SELECT position FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			WHERE survey_id=${sid} AND
			type_id='99' AND
			deleted='N'
			ORDER BY position,id";
		$result = execute_sql($sql);
		$num_sections = record_count($result) + 1;

		if($section > $num_sections)
			return('');	// invalid section

		$ret = array("${table}survey_id='${sid}'", "${table}deleted='N'");
		if($section>1 && $num_sections>1) {
			$result->Move($section-2);
			array_push($ret, "${table}position>" . $result->fields[0]);
		}
		if($section<$num_sections && $num_sections>1) {
			$result->Move($section-1);
			array_push($ret, "${table}position<" . $result->fields[0]);
		}
		db_close($result);
		return('WHERE ' . join(' AND ',$ret) . ' ');
	}
/* }}} */

/* {{{ proto array esp_type_has_choices()
   Returns an associative array of bools indicating if each
   question type has answer choices. */
function esp_type_has_choices() {
	$has_choices = array();
	$sql = 'SELECT id, has_choices FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].' ORDER BY id';
	$result = execute_sql($sql);
	while(list($tid,$answ) = fetch_row($result)) {
		$result->MoveNext();
		if($answ == 'Y')
			$has_choices[$tid]=1;
		else
			$has_choices[$tid]=0;
	}
	db_close($result);
	return($has_choices);
}
/* }}} */

/* {{{ proto array esp_type_response_table()
   Returns an associative array of bools indicating the
   table the responses are stored in. */
function esp_type_response_table() {
	$sql = 'SELECT id, response_table FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].' ORDER BY id';
	$result = execute_sql($sql);
	$response_table = array();
	while(list($tid,$answ) = fetch_row($result)) {
		$result->MoveNext();
		$response_table[$tid]=$answ;
	}
	db_close($result);
	return($response_table);
}
/* }}} */

?>
