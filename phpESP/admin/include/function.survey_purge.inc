<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_PURGE')) {
 	define('_FUNCTION_SURVEY_PURGE',TRUE);
	
	// purge all traces of a survey from the database
	
	// void survey_purge(int $surveyId);
	
	function survey_purge($sid) {
		// make a list of question IDs
		$sql = "SELECT id FROM survey_questions WHERE survey_id='${sid}'";
		$result = mysql_query($sql);
		$qids = array();
		while(list($qid) = mysql_fetch_row($result)) {
			array_push($qids, $qid);
		}
		mysql_free_result($result);
		$qidstr = "question_id IN " . ereg_replace("([0-9]+)","'\\1'", "(" . join(",", $qids) . ")");
	
		// work from the bottom up...
		// start with the survey results
		$tables = array('answers_bool','answers_multiple','answers_other','answers_rank','answers_single','answers_text');
		foreach($tables as $table) {
			$sql = "DELETE FROM ${table} WHERE ${qidstr}";
			$result = mysql_query($sql);
		}

		// then responses
		$sql = "DELETE FROM survey_responses WHERE survey_id=${sid}";
		mysql_query($sql);

		// then question choices
		$sql = "DELETE FROM question_choices WHERE ${qidstr}";
		mysql_query($sql);

		// then questions
		$sql = "DELETE FROM survey_questions WHERE survey_id=$sid";
		mysql_query($sql);
		
		// and finally the survey
		$sql = "DELETE FROM surveys WHERE id=$sid";
		mysql_query($sql);
		
		return;
	} // end void survey_purge($surveyId);

} // end _FUNCTION_SURVEY_PURGE
