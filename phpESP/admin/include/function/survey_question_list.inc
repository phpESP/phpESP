<?php

# $Id $
# Blatently ripped off of survey_render by stevem@survey.ucsb.edu
#


if(!defined('_FUNCTION_SURVEY_QUESTION_LIST')) {
	define('_FUNCTION_SURVEY_QUESTION_LIST',TRUE);

/* {{{ proto bool survey_question_list(int survey_id, int section)
   Reads current form variables from HTTP_POST_VARS.
   Returns the list of question IDs in current section.
   For use in determining skips */
function survey_question_list($sid, $section = 1) {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

    $return_me = array();

	if(empty($section))
		$section = 1;

	$sections = survey_get_sections($sid);
	$num_sections = count($sections);
	if($section-- > $num_sections)
		return(false);	// invalid section
	
// load survey questions
	$sec_sql = array_to_insql($sections[$section]);
	$sql = "SELECT * FROM question WHERE id $sec_sql AND type_id <> 98 AND deleted='N' ORDER BY position,id"; 
	$questions_result = mysql_query($sql);
	if(mysql_num_rows($questions_result) < 1)
		return(false);

// find out what question number we are on $i
	$i=0;
	for($j=0;$j<$section;$j++){
		$i += count($sections[$j]);
    }
	while($question = mysql_fetch_array($questions_result,MYSQL_ASSOC)) {
		// process each question
        // add question id to returned array
        $qid  = &$question['id'];
        $tid  = &$question['type_id'];
        $size = &$question['length'];
        $prec = &$question['precise'];

        if ($tid == 99) break;
//        echo $qid . "\n";
        array_push($return_me, $qid);
		++$i;
	}
	// end of questions
	return $return_me;
}



/* }}} */
/* {{{ proto bool survey_titled_question_list(int survey_id, int section)
   Reads current form variables from HTTP_POST_VARS.
   Returns the list of question IDs in current section.
   Finds only questions where show_title='Y'
   For use in determining skips */
function survey_titled_question_list($sid, $section = 1) {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

    $return_me = array();

	if(empty($section))
		$section = 1;

	$sections = survey_get_sections($sid);
	$num_sections = count($sections);
	if($section-- > $num_sections)
		return(false);	// invalid section
	
// load survey questions
	$sec_sql = array_to_insql($sections[$section]);
	$sql = "SELECT * FROM question WHERE id $sec_sql AND type_id <> 98 AND show_title='Y' AND deleted='N' ORDER BY position,id"; 
	$questions_result = mysql_query($sql);
	if(mysql_num_rows($questions_result) < 1)
		return(false);

// find out what question number we are on $i
	$i=0;
	for($j=0;$j<$section;$j++){
		$i += count($sections[$j]);
    }
	while($question = mysql_fetch_array($questions_result,MYSQL_ASSOC)) {
		// process each question
        // add question id to returned array
        $qid  = &$question['id'];
        $tid  = &$question['type_id'];
        $size = &$question['length'];
        $prec = &$question['precise'];

        if ($tid == 99) 
            break;

        array_push($return_me, $qid);
		++$i;
	}
	// end of questions
	return $return_me;
}
/* }}} */

} // end _FUNCTION_QUESTION_LIST
