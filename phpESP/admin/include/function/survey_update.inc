<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

if(!defined('_FUNCTION_SURVEY_UPDATE')) {
	define('_FUNCTION_SURVEY_UPDATE',TRUE);

/* {{{ proto string survey_update(int* survey_id, string* tab, string old_tab)
   Reads current form variables from HTTP_POST_VARS.
   Returns an empty string on sucess, else returns an string
   with an error message. */
function survey_update(&$survey_id, &$tab, $old_tab) {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

	// do not need update
	if(empty($old_tab))
		return;

	$f_arr = array();
	$v_arr = array();

	// new survey
	if(empty($survey_id)) {
		// need to fill out at least some info on 1st tab before proceeding
		if(empty($HTTP_POST_VARS['name']) || empty($HTTP_POST_VARS['title'])) {
			$tab = "general";
			return(_('Sorry, please fill out the name and title before proceeding.'));
		}

		// create a new survey in the database
		$fields = array('name','title','subtitle','email','thank_head','thank_body','info');
		foreach($fields as $f) {
			if(!empty($HTTP_POST_VARS[$f])) {
				array_push($f_arr,$f);
				array_push($v_arr,"'".$HTTP_POST_VARS[$f]."'");
			}
		}
		$sql = "INSERT INTO surveys (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) . ")";

		$result = @mysql_query ($sql);
		if(!$result) {
			$tab = "general";
			return(_('Sorry, name already in use. Pick a new name.') .' [ ' .mysql_errno().': '.mysql_error().' ]');
		}

		$sql = "SELECT id FROM surveys WHERE name='".  $HTTP_POST_VARS['name'] ."'";
		$result = mysql_query($sql);
		$survey_id = mysql_result($result,0,0);
		mysql_free_result($result);
		return;
	}

	// survey already started
	switch($old_tab) {
		// coming from the general tab ...
		case "general":
			$fields = array('name','title','subtitle','email','thank_head','thank_body','info');
			$sql = "SELECT name FROM surveys WHERE id='${survey_id}'";
			$result = mysql_query($sql);
			$name = mysql_result($result,0,0);
			mysql_free_result($result);

			// trying to change survey name
			if($name != $HTTP_POST_VARS['name']) {
				$sql = "SELECT COUNT(*) WHERE name='" . $HTTP_POST_VARS['name'] ."'";
				$result = mysql_query($sql);
				$count = mysql_result($result,0,0);
				mysql_free_result($result);
				if($count != 0) {
					$tab = "general";
					return(_('Sorry, that name is already in use.'));
				}
			}

			// UPDATE the row in the DB with current values
			foreach($fields as $f) {
				array_push($f_arr,$f ."='" . $HTTP_POST_VARS[$f] . "'");
			}
			$sql = "UPDATE surveys SET " . join(',',$f_arr) . " WHERE id='${survey_id}'";
			$result = mysql_query($sql);
			if(!$result) {
				$tab = "general";
				return(_('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]');
			}
			return;

		// coming from the questions tab ...
		case "questions":
			// if the question box is empty ... ignore everything else
			if(empty($HTTP_POST_VARS['content']))
				return;

			// constraint: question type required
			if(empty($HTTP_POST_VARS['type_id'])) {
				$tab = 'questions';
				return(_('Sorry, you must select a type for this question.'));
			}

			$has_choices = esp_type_has_choices();

			$qid = $HTTP_POST_VARS['id'];
			if(stristr($qid,'new'))
				$qid = '';

			// constraint: can not change between question w/ answer choices and one w/o
			if($qid != '') {
				$sql =  "SELECT Q.type_id FROM survey_questions Q WHERE Q.survey_id='${survey_id}' AND Q.id='${qid}'"; 
				$result = mysql_query($sql);
				list($old_type_id) = mysql_fetch_row($result);
				mysql_free_result($result);
				if($has_choices[$HTTP_POST_VARS['type_id']] != $has_choices[$old_type_id]) { // trying to change between incompatible question types
					$tab = "questions";
					$HTTP_POST_VARS['type_id'] = $old_type_id;
					return(_('Sorry, you cannot change between those types of question. Create a new question instead.'));
				}
			}
			
			// constraint: check for mutually exclusive Question Type-Result Type pairs
			if(empty($HTTP_POST_VARS['result_id']))
				$HTTP_POST_VARS['result_id'] = 1; // default to percentages
			if($HTTP_POST_VARS['type_id'] == 2 || $HTTP_POST_VARS['type_id'] == 3) {  // single line or essay
				$HTTP_POST_VARS['result_id'] = 4; // require list type results
			} elseif($HTTP_POST_VARS['result_id'] == 4) { // list
				$HTTP_POST_VARS['result_id'] = 1;	// only single line or essay can use a list result
			} elseif($HTTP_POST_VARS['type_id'] == 7 && $HTTP_POST_VARS['result_id'] == 2) { // rating question & ranking result
				$HTTP_POST_VARS['result_id'] = 1; // too confusing, so force to percentages
			} elseif($HTTP_POST_VARS['type_id'] == 8) { // rank question
				$HTTP_POST_VARS['result_id'] = 5;	// force to average rank
			}
			
			// UPDATE row in the DB for the current question
			if($qid != '') {
				$fields = array('type_id','result_id','parent_id','required','params','content');
				foreach($fields as $f) {
					array_push($f_arr,$f.'='.$HTTP_POST_VARS[$f]);
				}
				$sql = "UPDATE survey_questions SET " . join(',',$f_arr) . " WHERE id='${qid}'";

			// INSERT row in the DB for new question
			} else {
				$fields = array('survey_id','type_id','result_id','position','parent_id','required','params','content');
				if(empty($HTTP_POST_VARS['position']) || $HTTP_POST_VARS['position'] == 0) {
					// set the position to the end
					$sql = "SELECT MAX(position) FROM survey_questions WHERE survey_id='${survey_id}'";
					$result = mysql_query($sql);
					$position = mysql_result($result,0,0) + 10;
					mysql_free_result($result);
				}
				foreach($fields as $f) {
					if(!empty($HTTP_POST_VARS[$f])) {
						array_push($f_arr,$f);
						array_push($v_arr,$HTTP_POST_VARS[$f]);
					}
				}
				$sql  = "INSERT INTO survey_questions (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) .")";
			}
			$result = mysql_query($sql);
			if($qid == '')
				$qid = mysql_insert_id();
			if(!$result) {
				$tab = 'questions';
				return(_('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]');
			}

			// UPDATE or INSERT rows for each of the question choices for this question
			if($has_choices[$HTTP_POST_VARS['type_id']]) {
				$count = 0;
				for($i=1;$i<$num_choices+1;$i++) {
					$sql='';
					$choice_id = ${"choice_id_".$i};
					$choice_content = ${"choice_content_".$i};
					// each of the submitted choices
					if($choice_id=='' && $choice_content!='') {
						// insert (new)
						$sql = "INSERT INTO question_choices (question_id,content) VALUES ('${qid}','${choice_content}')";
						++$count;
					} elseif($choice_id!='' && $choice_content=='') {
						// delete (old)
						$sql = "DELETE FROM question_choices WHERE id='${choice_id}'";
					} elseif($choice_id!='' && $choice_content!='') {
						// update (old)
						$sql = "UPDATE question_choices SET content='${choice_content}' WHERE id='${choice_id}'";
						++$count;
					}
					if($sql != '') {
						$result = mysql_query($sql);
						if(!$result) {
							$tab = 'questions';
							return(_('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]');
						}
					}
				}
				if(!$count) {
					$tab = 'questions';
					return(_('Sorry, you need at least one answer option for this question type.'));
				}
			}
			return;

		case "preview":
			// can not change anything here yet, so no need to update DB.
			return;

		case "order":
			// it updates the DB itself
			return;
	}
	return;
}
/* }}} */

} // end _FUNCTION_SURVEY_UPDATE
?>
