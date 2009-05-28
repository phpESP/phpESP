<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

/* {{{ proto bool survey_update(int* survey_id, string* tab, string old_tab)
   Reads current form variables from _POST.
   Returns an true on sucess, else returns false and
   sets global $errstr with an error message. */
function survey_update(&$survey_id, &$tab, $old_tab) {

	global $errstr;

	// do not need update
	if(empty($old_tab))
		return(1);

	$f_arr = array();
	$v_arr = array();

	// clean up survey and question names
	if (isset($_POST['name'])) {
		$_POST['name'] = preg_replace("/[^A-Z0-9]+/i","_", trim($_POST['name']));
		$_POST['name'] = preg_replace("/(^_|_$)/i","", $_POST['name']);
	}
	// new survey
	if(empty($survey_id)) {
		// need to fill out at least some info on 1st tab before proceeding
		if(empty($_POST['name']) || empty($_POST['title']) || empty($_POST['realm'])) {
			$tab = "general";
			$errstr = _('Sorry, please fill out the name, group, and title before proceeding.');
			return(0);
		}

		// create a new survey in the database
		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info','auto_num','open_date','close_date');
		foreach($fields as $f) {
			if(isset($_POST[$f])) {
				array_push($f_arr,$f);
				array_push($v_arr, _addslashes($_POST[$f]));
			}
		}
		array_push($f_arr, 'owner');
		array_push($v_arr, _addslashes($_SESSION['acl']['username']));
		array_push($f_arr, 'changed');
		array_push($v_arr, sys_time_stamp());
		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['survey_table']." (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) . ")";
		$result = execute_sql($sql);
		if(!$result) {
			$tab = "general";
			$errstr = _('Sorry, name already in use. Pick a new name.') .' [ ' .ErrorNo().': '.ErrorMsg().' ]';
			return(0);
		}

		$sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=".  _addslashes($_POST['name']);
		$result = execute_sql($sql);
		$survey_id = $result->fields[0];
		return(1);
	}

	// survey already started
	switch($old_tab) {
		// coming from the general tab ...
		case "general":
			if(empty($_POST['name']) || empty($_POST['title']) || empty($_POST['realm'])) {
				$tab = "general";
				$errstr = _('Sorry, please fill out the name, group, and title before proceeding.');
				return(0);
			}

		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info','auto_num','open_date','close_date');
		$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${survey_id}";
		$name = get_one($sql);

		// trying to change survey name
		if($name != $_POST['name']) {
			$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=" . _addslashes($_POST['name']);
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
			array_push($f_arr,$f ."=" . _addslashes($_POST[$f]));
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
			if(empty($_POST['content']) && empty($_POST['name']))
				return(1);

		if(empty($_POST['content'])) {
			$tab = 'questions';
			$dont_clear = 1;
			$errstr = _('Please enter text for this question.');
			return(0);
		}

		// constraint: fieldname must be not empty
		//   generate it from the content if empty
		//   validate/repair fieldname
		if(empty($_POST['name'])) {
			$str1 = $_POST['content'];
			do {
				$str2 = $str1;
				$str1 = eregi_replace(
						"(^| )(what|which|why|how|who|where|how|is|are|were|the|a|it|of|do|you|your|please|enter)[ ?]",
						" ", $str2);
			} 
			while ($str1 != $str2);
			$_POST['name'] = $str1;
		}
		$_POST['name'] = strtoupper(preg_replace("/[^A-Z0-9]+/i","_", trim($_POST['name'])));
		$_POST['name'] = preg_replace("/(^_|_$)/i","", $_POST['name']);

		// constraint: question type required
		if(empty($_POST['type_id'])) {
			$tab = 'questions';
			$dont_clear = 1;
			$errstr= _('Sorry, you must select a type for this question.');
			return(0);
		}

		$_POST['type_id'] = intval($_POST['type_id']);
		// constraint: qid must be int or empty
		if($_POST['id'] == _('New Field'))
			$qid = '';
		$qid = intval($_POST['id']);	# curr_q_id

			// constraint: can not change between question w/ answer choices and one w/o
			$has_choices = esp_type_has_choices();
		if(!empty($qid)) {
			$sql =  "SELECT Q.type_id FROM ".$GLOBALS['ESPCONFIG']['question_table']." Q WHERE Q.survey_id=${survey_id} AND Q.id=${qid}";
			$old_type_id = get_one($sql);
			if($has_choices[$_POST['type_id']] != $has_choices[$old_type_id]) { // trying to change between incompatible question types
				$tab = "questions";
				$_POST['type_id'] = $old_type_id;
				$dont_clear = 1;
				$errstr = _('Sorry, you cannot change between those types of question. Create a new question instead.');
				return(0);
			}
		}

		// constraint: length must be int
		$_POST['length']  = intval($_POST['length']) or 0;

		// constraint: precise must be int
		$_POST['precise'] = intval($_POST['precise']) or 0;

		// defaults for length field
		if(empty($_POST['length']) && $_POST['type_id'] < 50) {
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
			$_POST['length'] = $arr[$_POST['type_id']];
		}

		// defaults for precision field
		if(empty($_POST['precise']) && $_POST['type_id'] < 50) {
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
			$_POST['precise'] = $arr[$_POST['type_id']];
		}


		// UPDATE row in the DB for the current question
		if($qid != '') {
			// conditional questions can't be required
			//$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['condition_table']." WHERE q1_id='$qid'";
			//$count = get_one($sql);
			//if($count != 0) {
			//   $_POST['required']='N';
			//}

			$fields = array('name','type_id','length','precise','required','content','ans_uniq');
			foreach($fields as $f) {
				if(isset($_POST[$f]))
					array_push($f_arr,$f."="._addslashes($_POST[$f]));
			}
			$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET " . join(', ',$f_arr) . " WHERE id=${qid}";

			// INSERT row in the DB for new question
		} else {
			// set the position to the end
			$sql = "SELECT MAX(position)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id=${survey_id}";
			$_POST['position'] = get_one($sql);

			$_POST['survey_id'] = $survey_id;

			$fields = array('survey_id','name','type_id','length','precise','required','content','position');
			foreach($fields as $f) {
				if(isset($_POST[$f])) {
					array_push($f_arr, $f);
					array_push($v_arr,  _addslashes($_POST[$f]));
				}
			}
			$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." (" . join(', ',$f_arr) . ") VALUES (" . join(', ',$v_arr) .")";
		}
		$result = execute_sql($sql);
		if($qid == '')
			$qid = insert_id('question_id_seq');
		$_POST['id'] = $qid;
		if(!$result) {
			$tab = 'questions';
			$dont_clear = 1;
			$errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
			return(0);
		}

		// UPDATE or INSERT rows for each of the question choices for this question
		if($has_choices[$_POST['type_id']]) {
			$cids = array();
			$sql = "SELECT c.id FROM ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c WHERE q.id=c.question_id AND q.survey_id=${survey_id}";
			$result = execute_sql($sql);
			for ($i = 0; $i < record_count($result); ++$i) {
				$result->Move($i);
				array_push($cids, $result->fields[0]);
			}
			db_close($result);
			$count = 0;
			for($i=1;$i<$_POST['num_choices']+1;$i++) {
				$sql='';
				$choice_id       = intval($_POST["choice_id_${i}"]);
				$choice_content  = $_POST["choice_content_${i}"];
				$choice_feedback = $_POST["choice_feedback_${i}"];
				$choice_credit   = $_POST["choice_credit_${i}"];
				//constraint: for rate questions (type_id=8)
				// we don't allow "!other" as a choice
				if ($_POST['type_id']==8 && $choice_content=='!other') {
						$errstr = _('For rate questions, "!other" is not a possible answer');
						return(0);
				}

				// for safety, we add slashes
				if ($choice_content != '') {
					$choice_content = _addslashes($choice_content);
				}
                $choice_feedback = _addslashes($choice_feedback);
                $choice_credit   = _addslashes($choice_credit);

				// each of the submitted choices
				if($choice_id=='' && $choice_content!='') {
					// insert (new)
					$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_choice_table']." (question_id,content,feedback,credit) VALUES (${qid},${choice_content},${choice_feedback},${choice_credit})";
					++$count;
				} elseif($choice_id!='' && $choice_content=='') {
					// delete (old)
					$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id=${choice_id}";
				} elseif($choice_id!='' && ($choice_content!='' || $choice_feedback!='' || $choice_credit!='') && in_array($choice_id, $cids)) {
					// update (old)
					$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_choice_table']." SET content=${choice_content},feedback=${choice_feedback},credit=${choice_credit} WHERE id=${choice_id}";
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
			if(!$count && !isset($_POST['extra_choices'])) {
				$tab = 'questions';
				$dont_clear = 1;
				$errstr = _('Sorry, you need at least one answer option for this question type.') .
					' [ '. _('ID') .': '. $_POST['type_id'] .' ]';
				return(0);
			}
		}
		return(1);

		case "preview":
			// can not change anything here yet, so no need to update DB.
			return(1);

		case "order":
			if(isset($_POST['questions_order']))
				$questions_order = $_POST['questions_order'];

		$sid = $survey_id;

		if (isset($_POST['questions_order'])) {
			$sql = "SELECT id, position FROM ".$GLOBALS['ESPCONFIG']['question_table']."
				WHERE survey_id=$sid AND deleted='N'
				ORDER BY position";
			$result = execute_sql($sql);
			$max = record_count($result);
			$order = split('\|', $questions_order);
			for($i = 0; $i < count($order); $i++) {
				$ord = _addslashes($order[$i]);
				$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET position=$i WHERE id=$ord AND survey_id=$sid";
				execute_sql($sql);
			}
		}
		if (isset($_POST['removeit'])) {
			if ($_POST['removeit'] == 1) {
				if(isset($_POST['questions'])) {
					$qid = intval($_POST['questions']); 
					$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET deleted='Y' WHERE id=$qid AND survey_id=$sid";
					execute_sql($sql);
				} 
			}
		}
		if(isset($_POST['addbreak'])) {
			$sql = "SELECT MAX(position)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id=$sid";
			$pos = get_one($sql);

			$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." (survey_id,name,type_id,position,content)
				VALUES ($sid, 'break', 99, $pos, 'break')";
			execute_sql($sql);
		}
		db_close($result);
		return(1);

		case "conditions":
		$sid = $survey_id;
		//if(isset($_POST['addcondition']) && ($_POST['addcondition']=='go')) {
		if(isset($_POST['addcondition'])) {
			$id1=$_POST['q1_id'];
			$id2=$_POST['q2_id'];
			if ($id1==$id2) {
				$errstr = _('Please select two different questions.');
				return(0);
			}
			$cond=$_POST['cond'];
			$cond_val=_addslashes($_POST['cond_value']);
			$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['condition_table']." (survey_id,q1_id,q2_id,cond,cond_value)
				VALUES ($sid, $id1, $id2, $cond, $cond_val)";
			$result=execute_sql($sql);
			if (!$result)
			   $errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
			# Conditional questions can't be required questions as well
			//$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET required='N' WHERE id=$id1";
			//$result=execute_sql($sql);
			//if (!$result)
			//   $errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
		}
		elseif(isset($_POST['deletecondition'])) {
			$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['condition_table']." WHERE id=".key($_POST['deletecondition']);
			$result=execute_sql($sql);
			if (!$result)
			   $errstr = _('Warning, error encountered.') .' [ '.ErrorNo().': '.ErrorMsg().' ]';
		}
		return(1);
	}
	return(1);
}
/* }}} */

?>
