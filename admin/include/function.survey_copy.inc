<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_COPY')) {
 	define('_FUNCTION_SURVEY_COPY',TRUE);
	
	// creates an editable copy of a survey
	
	// void survey_copy(int $surveyId);
	
	function survey_copy ($sid) {
		// see if the SID exists
		$sql = "SELECT COUNT(*) FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		$i = mysql_result($result,0,0);
		mysql_free_result($result);
		if(!$i)
			return("Survey to copy does not exist. [ SID: $sid ]");
	
		// find the next available name for survey
		// of the form <name>_copyN
		$i='';
		do {
			$sql = "
SELECT COUNT(*)
  FROM surveys A, surveys B
 WHERE A.id='${sid}' AND
       B.name=CONCAT(A.name,'_copy${i}')";
	   		$result = mysql_query($sql);
			$j = mysql_result($result,0,0);
			mysql_free_result($result);
		} while($j && ++$i);
		
		// copy the main survey part
		$sql = "
INSERT INTO surveys (name,title,thanks_page,email,subtitle,info,thank_head,thank_body)
    SELECT CONCAT(S.name,'_copy${i}'),S.title,S.thanks_page,S.email,S.subtitle,S.info,S.thank_head,S.thank_body
      FROM surveys S
	 WHERE S.id='${sid}'";
	 	if(!($result = mysql_query($sql)))
			return("Error copying survey. [ ".mysql_error()." ]");
		$newsid = mysql_insert_id();

		// build an array for weather a question type has answer options
		// used in copying questions
		$has_answers = arr_has_answers();

		// make copies of all the questions *with out* choices
		// NOTE: This will break on 'parent_id' which is currently not used
		$sql = "
INSERT INTO survey_questions (survey_id,type_id,result_id,position,required,params,content)
    SELECT '${newsid}',Q.type_id,Q.result_id,Q.position,Q.required,Q.params,Q.content
      FROM survey_questions Q, question_types T
	 WHERE S.survey_id='${sid} AND
	       S.type_id=T.id AND
		   T.has_answers = 0 AND
	       NOT (S.status & '" . STATUS_DELETED ."')";
		if(!($result = mysql_query($sql)))
			return("Error copying survey. [ ".mysql_error()." ]");

		// make copies of all the questions *with* choices
		// NOTE: This will break on 'parent_id' which is currently not used
		$sql = "
SELECT Q.id
  FROM survey_questions Q, question_types T
 WHERE S.survey_id='${sid} AND
       S.type_id=T.id AND
	   T.has_answers != 0 AND
	   NOT (S.status & '" . STATUS_DELETED ."')";
		$result = mysql_query($sql);
		while(list($qid) = mysql_fetch_row($result)) {
			$sql = "
INSERT INTO survey_questions (survey_id,type_id,result_id,position,required,params,content)
    SELECT '${newsid}',Q.type_id,Q.result_id,Q.position,Q.required,Q.params,Q.content
      FROM survey_questions Q
	 WHERE Q.id='${qid}";
		 	$result2 = mysql_query($sql);
			$newqid = mysql_insert_id();

			// make copies of all the question choices (if exist)
			$sql = "
INSERT INTO question_choices (question_id,content,value)
	SELECT '${newqid}',C.content,C.value
	  FROM question_choices C
	 WHERE C.question_id='${qid}'";
			$result2 = mysql_query($sql);
		}
		mysql_free_result($result);
		return;
	} // end survey_copy(int $surveyId);

} // end _FUNCTION_SURVEY_COPY
