<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

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

    // clean up survey and question names
    if (isset($HTTP_POST_VARS['name'])) {
        $HTTP_POST_VARS['name'] = preg_replace("/[^A-Z0-9]+/i","_", trim($HTTP_POST_VARS['name']));
        $HTTP_POST_VARS['name'] = preg_replace("/(^_|_$)/i","", $HTTP_POST_VARS['name']);
    }
	// new survey
	if(empty($survey_id)) {
		// need to fill out at least some info on 1st tab before proceeding
		if(empty($HTTP_POST_VARS['name']) || empty($HTTP_POST_VARS['title'])
				|| empty($HTTP_POST_VARS['realm'])) {
			$tab = "general";
			$errstr = _('Sorry, please fill out the name, group, and title before proceeding.');
			return(0);
		}

		// create a new survey in the database
		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info');
		foreach($fields as $f) {
			if(isset($HTTP_POST_VARS[$f])) {
				array_push($f_arr,$f);
				array_push($v_arr, _addslashes($HTTP_POST_VARS[$f]));
			}
		}
		array_push($f_arr, 'owner');
		array_push($v_arr, _addslashes($GLOBALS['HTTP_SESSION_VARS']['acl']['username']));
        array_push($f_arr, 'changed');
        array_push($v_arr, sys_time_stamp());
		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['survey_table']." (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) . ")";
		$result = execute_sql($sql);
		if(!$result) {
			$tab = "general";
			$errstr = _('Sorry, name already in use. Pick a new name.') .' [ ' .ErrorNo().': '.ErrorMsg().' ]';
			return(0);
		}

		$sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=".  _addslashes($HTTP_POST_VARS['name']);
        $result = execute_sql($sql);
		$survey_id = $result->fields[0];
		return(1);
	}

	// survey already started
	switch($old_tab) {
		// coming from the general tab ...
		case "general":
			if(empty($HTTP_POST_VARS['name']) || empty($HTTP_POST_VARS['title'])
					|| empty($HTTP_POST_VARS['realm'])) {
				$tab = "general";
				$errstr = _('Sorry, please fill out the name, group, and title before proceeding.');
				return(0);
			}

			$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info');
			$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${survey_id}";
			$name = get_one($sql);

			// trying to change survey name
			if($name != $HTTP_POST_VARS['name']) {
				$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=" . _addslashes($HTTP_POST_VARS['name']);
				$count = get_one($sql);
				if($count != 0) {
					$tab = "general";
					$errstr = _('Sorry, that name is already in use.');
					return(0);
				}
			}

			// UPDATE the row in the DB with current values
            array_push($f_arr, 'changed='.sys_time_stamp());
			foreach($fields as $f) {
				array_push($f_arr,$f ."=" . _addslashes($HTTP_POST_VARS[$f]));
			}
			$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['survey_table']." SET " . join(', ',$f_arr) . " WHERE id=${survey_id}";
			$result = execute_sql($sql);
			if(!$result) {
				$tab = "general";
				$errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
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
				} 
                while ($str1 != $str2);
                $HTTP_POST_VARS['name'] = $str1;
                }
                $HTTP_POST_VARS['name'] = strtoupper(preg_replace("/[^A-Z0-9]+/i","_", trim($HTTP_POST_VARS['name'])));
                $HTTP_POST_VARS['name'] = preg_replace("/(^_|_$)/i","", $HTTP_POST_VARS['name']);

            // angek: start: 280903. 
            // do not allow questions in the same survey to have the same name.
            // applies to questions with a deleted flag of 'N' only.
            if($HTTP_POST_VARS['name'] != $HTTP_POST_VARS['name_dupcheck']) {
                $found=0;
                $qn_names = array();
                $sql = "SELECT name FROM question WHERE survey_id=${survey_id} AND deleted = 'N'";
                $result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
                while ($names = fetch_row($result)){
                    array_push($qn_names, $names['name']);
                    $result->MoveNext();
                }
                db_close($result);
                foreach ($qn_names as $x){
                    if ($HTTP_POST_VARS['name'] == $x){
                        $found = 1;
                    }
                }
                if ($found){
                    $dont_clear = 1;
                    $tab = 'questions';
                    $errstr=_('Sorry, this question name is already in use.');
                    return(0);
                }
            }
            // angek: end: 280903.

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
				$sql =  "SELECT Q.type_id FROM ".$GLOBALS['ESPCONFIG']['question_table']." Q WHERE Q.survey_id=${survey_id} AND Q.id=${qid}";
				$old_type_id = get_one($sql);
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
			if(empty($HTTP_POST_VARS['length']) && $HTTP_POST_VARS['type_id'] < 50) {
				$arr = array(
					0,		// 0: unused
					0,		// 1: Yes/No
					20,		// 2: Text Box  (width)
					60,		// 3: Essay Box (width)
					0,		// 4: Radio Buttons
					0,		// 5: Check Boxes (minumum)
					0,		// 6: Dropdown Box (length)
					5,		// 7: Rating (# cols)
					5,		// 8: Rate (# cols)
					0,		// 9: Date
					10		// 10: Numeric (digits)
					);
				$HTTP_POST_VARS['length'] = $arr[$HTTP_POST_VARS['type_id']];
			}

			// defaults for precision field
			if(empty($HTTP_POST_VARS['precise']) && $HTTP_POST_VARS['type_id'] < 50) {
				$arr = array(
					0,		// 0: unused
					0,		// 1: Yes/No
					0,		// 2: Text Box
					5,		// 3: Essay Box (height)
					10,		// 4: Radio Buttons
					0,		// 5: Check Boxes (maximum)
					0,		// 6: Dropdown Box
					0,		// 7: Rating (use N/A)
					0,		// 8: Rate (use N/A)
					0,		// 9: Date
					0		// 10: Numeric (decimal)
					);
				$HTTP_POST_VARS['precise'] = $arr[$HTTP_POST_VARS['type_id']];
			}

			// UPDATE row in the DB for the current question
			if($qid != '') {
				$fields = array('name','type_id','length','precise','required','content');
				foreach($fields as $f) {
					if(isset($HTTP_POST_VARS[$f]))
						array_push($f_arr,$f."="._addslashes($HTTP_POST_VARS[$f]));
				}
				$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET " . join(', ',$f_arr) . " WHERE id=${qid}";

			// INSERT row in the DB for new question
			} else {
				// set the position to the end
				$sql = "SELECT MAX(position)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id=${survey_id}";
				$HTTP_POST_VARS['position'] = get_one($sql);

				$HTTP_POST_VARS['survey_id'] = $survey_id;

				$fields = array('survey_id','name','type_id','length','precise','required','content','position');
				foreach($fields as $f) {
					if(isset($HTTP_POST_VARS[$f])) {
						array_push($f_arr, $f);
						array_push($v_arr,  _addslashes($HTTP_POST_VARS[$f]));
					}
				}
				$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." (" . join(', ',$f_arr) . ") VALUES (" . join(', ',$v_arr) .")";
			}
			$result = execute_sql($sql);
			if($qid == '')
				$qid = insert_id('question_id_seq');
			$HTTP_POST_VARS['id'] = $qid;
			if(!$result) {
				$tab = 'questions';
				$dont_clear = 1;
				$errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
				return(0);
			}

			// UPDATE or INSERT rows for each of the question choices for this question
			if($has_choices[$HTTP_POST_VARS['type_id']]) {
				$cids = array();
				$sql = "SELECT c.id FROM ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c WHERE q.id=c.question_id AND q.survey_id=${survey_id}";
				$result = execute_sql($sql);
				for ($i = 0; $i < record_count($result); ++$i) {
                    $result->Move($i);
					array_push($cids, $result->fields[0]);
				}
				db_close($result);
				$count = 0;
				for($i=1;$i<$HTTP_POST_VARS['num_choices']+1;$i++) {
					$sql='';
					$choice_id      = intval($HTTP_POST_VARS["choice_id_${i}"]);
					$choice_content = $HTTP_POST_VARS["choice_content_${i}"];
                    if ($choice_content != '') {
                        $choice_content = _addslashes($choice_content);
                    }
					// each of the submitted choices
					if($choice_id=='' && $choice_content!='') {
						// insert (new)
						$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_choice_table']." (question_id,content) VALUES (${qid},${choice_content})";
						++$count;
					} elseif($choice_id!='' && $choice_content=='') {
						// delete (old)
						$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id=${choice_id}";
					} elseif($choice_id!='' && $choice_content!='' && in_array($choice_id, $cids)) {
						// update (old)
						$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_choice_table']." SET content=${choice_content} WHERE id=${choice_id}";
						++$count;
					}
					if($sql != '') {
						$result = execute_sql($sql);
						if(!$result) {
							$tab = 'questions';
							$dont_clear = 1;
							$errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
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
            if(isset($HTTP_POST_VARS['questions_order']))
                $questions_order = $HTTP_POST_VARS['questions_order'];

            $sid = $survey_id;

            if (isset($HTTP_POST_VARS['questions_order'])) {
                $sql = "SELECT id, position FROM ".$GLOBALS['ESPCONFIG']['question_table']."
                WHERE survey_id=$sid AND deleted='N'
                ORDER BY position";
                $result = execute_sql($sql);
                $max = record_count($result);
                $order = split('\|', $questions_order);
                for($i = 0; $i < count($order); $i++) {
                    $sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET position=$i WHERE id=$order[$i] AND survey_id=$sid";
                    execute_sql($sql);
                }
            }
            if (isset($HTTP_POST_VARS['removeit'])) {
                if ($HTTP_POST_VARS['removeit'] == 1) {
                    if(isset($HTTP_POST_VARS['questions'])) {
                        $qid = intval($HTTP_POST_VARS['questions']); 
                        $sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET deleted='Y' WHERE id=$qid AND survey_id=$sid";
                        execute_sql($sql);
                    } 
                }
            }
            if(isset($HTTP_POST_VARS['addbreak'])) {
                $sql = "SELECT MAX(position)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id=$sid";
                $pos = get_one($sql);

                $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." (survey_id,type_id,position,content)
                VALUES ($sid, 99, $pos, 'break')";
                execute_sql($sql);
            }
            db_close($result);
            return(1);
        }
	return(1);
}
/* }}} */

?>
