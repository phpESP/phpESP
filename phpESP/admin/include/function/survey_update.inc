<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

if(!defined('_FUNCTION_SURVEY_UPDATE')) {
	define('_FUNCTION_SURVEY_UPDATE',TRUE);

/* {{{ proto bool survey_update(int* survey_id, string* tab, string old_tab)
   Reads current form variables from HTTP_POST_VARS.
   Returns an true on sucess, else returns false and 
   sets global $errstr with an error message. */
function survey_update(&$survey_id, &$tab, $old_tab) {
	global $HTTP_POST_VARS, $HTTP_SERVER_VARS, $errstr;

	// do not need update
	if(empty($old_tab))
		return(1);

	$f_arr = array();
	$v_arr = array();

	// new survey
	if(empty($survey_id)) {
		$HTTP_POST_VARS['name'] = eregi_replace(
			"[^A-Z0-9]+", "_", trim($HTTP_POST_VARS['name']) );
		$HTTP_POST_VARS['name'] = ereg_replace('_$',"",$HTTP_POST_VARS['name']);

		// need to fill out at least some info on 1st tab before proceeding
		if(empty($HTTP_POST_VARS['name']) || empty($HTTP_POST_VARS['title'])) {
			$tab = "general";
			$errstr = _('Sorry, please fill out the name and title before proceeding.');
			return(0);
		}

		// create a new survey in the database
		$fields = array('name','realm','title','subtitle','email','thank_head','thank_body','info');
		foreach($fields as $f) {
			if(isset($HTTP_POST_VARS[$f])) {
				array_push($f_arr,$f);
				array_push($v_arr,"'".XADDSLASHES($HTTP_POST_VARS[$f])."'");
			}
		}
		array_push($f_arr,'owner');
		array_push($v_arr,"'".$GLOBALS['HTTP_SESSION_VARS']['acl']['username']."'");
		$sql = "INSERT INTO survey (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) . ")";

		$result = @mysql_query ($sql);
		if(!$result) {
			$tab = "general";
			$errstr = _('Sorry, name already in use. Pick a new name.') .' [ ' .mysql_errno().': '.mysql_error().' ]';
			return(0);
		}

		$sql = "SELECT id FROM survey WHERE name='".  XADDSLASHES($HTTP_POST_VARS['name']) ."'";
		$result = mysql_query($sql);
		$survey_id = mysql_result($result,0,0);
		mysql_free_result($result);
		return(1);
	}

	// survey already started
	switch($old_tab) {
		// coming from the general tab ...
		case "general":
			$HTTP_POST_VARS['name'] = eregi_replace(
				"[^A-Z0-9]+", "_", trim($HTTP_POST_VARS['name']) );
			$HTTP_POST_VARS['name'] = ereg_replace('_$',"",$HTTP_POST_VARS['name']);

			if(empty($HTTP_POST_VARS['name']) || empty($HTTP_POST_VARS['title'])
					|| empty($HTTP_POST_VARS['realm'])) {
				$tab = "general";
				$errstr = _('Sorry, please fill out the name, group, and title before proceeding.');
				return(0);
			}

			$fields = array('name','realm','title','subtitle','email','thank_head','thank_body','info');
			$sql = "SELECT name FROM survey WHERE id='${survey_id}'";
			$result = mysql_query($sql);
			$name = mysql_result($result,0,0);
			mysql_free_result($result);

			// trying to change survey name
			if($name != $HTTP_POST_VARS['name']) {
				$sql = "SELECT COUNT(*) FROM survey WHERE name='" . XADDSLASHES($HTTP_POST_VARS['name']) ."'";
				$result = mysql_query($sql);
				$count = mysql_result($result,0,0);
				mysql_free_result($result);
				if($count != 0) {
					$tab = "general";
					$errstr = _('Sorry, that name is already in use.');
					return(0);
				}
			}

			// UPDATE the row in the DB with current values
			foreach($fields as $f) {
				array_push($f_arr,$f ."='" . XADDSLASHES($HTTP_POST_VARS[$f]) . "'");
			}
			$sql = "UPDATE survey SET " . join(', ',$f_arr) . " WHERE id='${survey_id}'";
			$result = mysql_query($sql);
			if(!$result) {
				$tab = "general";
				$errstr = _('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]';
				return(0);
			}
			return(1);

		// coming from the questions tab ...
		case "questions":
			// if the question box is empty ... ignore everything else
			if(empty($HTTP_POST_VARS['content']) && empty($HTTP_POST_VARS['name']))
				return(1);
			
			if(empty($HTTP_POST_VARS['content'])) {
				$tab = 'questions';
				$dont_clear = 1;
				$errstr = _('Please enter text for this question.');
				return(0);
			}

			// constraint: fieldname must be not empty
			//   generate it from the content if empty
			//   validate/repair fieldname
			if(empty($HTTP_POST_VARS['name'])) {
				$str1 = $HTTP_POST_VARS['content'];
				do {
					$str2 = $str1;
					$str1 = eregi_replace(
					"(^| )(what|which|why|how|who|where|how|is|are|were|the|a|it|of|do|you|your|please|enter)[ ?]", 
					" ", $str2);
				} while ($str1 != $str2);
				$HTTP_POST_VARS['name'] = $str1;
			}
			$HTTP_POST_VARS['name'] = strtoupper(substr( eregi_replace(
				"[^A-Z0-9]+", "_", trim($HTTP_POST_VARS['name'])), 0, 10));
			$HTTP_POST_VARS['name'] = ereg_replace('_$',"",$HTTP_POST_VARS['name']);

			// constraint: question type required
			if(empty($HTTP_POST_VARS['type_id'])) {
				$tab = 'questions';
				$dont_clear = 1;
				$errstr= _('Sorry, you must select a type for this question.');
				return(0);
			}

			// constraint: qid must be int or empty
			if($HTTP_POST_VARS['id'] == _('New Field'))
				$qid = '';
			$qid = intval($HTTP_POST_VARS['id']);	# curr_q_id

			// constraint: can not change between question w/ answer choices and one w/o
			$has_choices = esp_type_has_choices();
			if(!empty($qid)) {
				$sql =  "SELECT Q.type_id FROM question Q WHERE Q.survey_id='${survey_id}' AND Q.id='${qid}'"; 
				$result = mysql_query($sql);
				$old_type_id = mysql_result($result,0,0);
				mysql_free_result($result);
				if($has_choices[$HTTP_POST_VARS['type_id']] != $has_choices[$old_type_id]) { // trying to change between incompatible question types
					$tab = "questions";
					$HTTP_POST_VARS['type_id'] = $old_type_id;
					$dont_clear = 1;
					$errstr = _('Sorry, you cannot change between those types of question. Create a new question instead.');
					return(0);
				}
			}
			
			// constraint: length must be int
			$HTTP_POST_VARS['length']  = intval($HTTP_POST_VARS['length']) or 0;
			
			// constraint: precise must be int
			$HTTP_POST_VARS['precise'] = intval($HTTP_POST_VARS['precise']) or 0;
			
			// defaults for length field
			if(empty($HTTP_POST_VARS['length'])) {
				$arr = array(
					0,		// 0: unused
					0,		// 1: Yes/No
					20,		// 2: Text Box  (width)
					5,		// 3: Essay Box (height)
					10,		// 4: Radio Buttons
					10,		// 5: Check Boxes
					0,		// 6: Dropdown Box
					0,		// 7: Rating
					0,		// 8: Rate
					0,		// 9: Date
					10		// 10: Numeric
					); 
				$HTTP_POST_VARS['length'] = $arr[$HTTP_POST_VARS['type_id']];
			}

			// UPDATE row in the DB for the current question
			if($qid != '') {
				$fields = array('name','type_id','length','precise','required','content');
				foreach($fields as $f) {
					if(isset($HTTP_POST_VARS[$f]))
						array_push($f_arr,$f."='".XADDSLASHES($HTTP_POST_VARS[$f])."'");
				}
				$sql = "UPDATE question SET " . join(', ',$f_arr) . " WHERE id='${qid}'";

			// INSERT row in the DB for new question
			} else {
				// set the position to the end
				$sql = "SELECT MAX(position)+1 FROM question WHERE survey_id='${survey_id}'";
				$result = mysql_query($sql);
				$HTTP_POST_VARS['position'] = mysql_result($result,0,0);
				mysql_free_result($result);

				$HTTP_POST_VARS['survey_id'] = $survey_id;

				$fields = array('survey_id','name','type_id','length','precise','required','content','position');
				foreach($fields as $f) {
					if(isset($HTTP_POST_VARS[$f])) {
						array_push($f_arr, $f);
						array_push($v_arr, "'". XADDSLASHES($HTTP_POST_VARS[$f]) ."'");
					}
				}
				$sql = "INSERT INTO question (" . join(', ',$f_arr) . ") VALUES (" . join(', ',$v_arr) .")";
			}
			$result = mysql_query($sql);
			if($qid == '')
				$qid = mysql_insert_id();
			$HTTP_POST_VARS['id'] = $qid;
			if(!$result) {
				$tab = 'questions';
				$dont_clear = 1;
				$errstr = _('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]';
				return(0);
			}

			// UPDATE or INSERT rows for each of the question choices for this question
			if($has_choices[$HTTP_POST_VARS['type_id']]) {
				$count = 0;
				for($i=1;$i<$HTTP_POST_VARS['num_choices']+1;$i++) {
					$sql='';
					$choice_id      = XADDSLASHES($HTTP_POST_VARS["choice_id_${i}"]);
					$choice_content = XADDSLASHES($HTTP_POST_VARS["choice_content_${i}"]);
					// each of the submitted choices
					if($choice_id=='' && $choice_content!='') {
						// insert (new)
						$sql = "INSERT INTO question_choice (question_id,content) VALUES ('${qid}','${choice_content}')";
						++$count;
					} elseif($choice_id!='' && $choice_content=='') {
						// delete (old)
						$sql = "DELETE FROM question_choice WHERE id='${choice_id}'";
					} elseif($choice_id!='' && $choice_content!='') {
						// update (old)
						$sql = "UPDATE question_choice SET content='${choice_content}' WHERE id='${choice_id}'";
						++$count;
					}
					if($sql != '') {
						$result = mysql_query($sql);
						if(!$result) {
							$tab = 'questions';
							$dont_clear = 1;
							$errstr = _('Warning, error encountered.') .' [ '.mysql_errno().': '.mysql_error().' ]';
							return(0);
						}
					}
				}
				if(!$count && !isset($HTTP_POST_VARS['extra_choices'])) {
					$tab = 'questions';
					$dont_clear = 1;
					$errstr = _('Sorry, you need at least one answer option for this question type.') .
						' [ '. _('ID') .': '. $HTTP_POST_VARS['type_id'] .' ]';
					return(0);
				}
			}
			return(1);

		case "preview":
			// can not change anything here yet, so no need to update DB.
			return(1);

		case "order":
			// it updates the DB itself
			return(1);
	}
	return(1);
}
/* }}} */

} // end _FUNCTION_SURVEY_UPDATE
?>
