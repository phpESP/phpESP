<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");

	if(empty($old_tab))
		return;

	$f_arr = array();
	$v_arr = array();

	// new survey
	if(empty($survey_id)) {
		// need to fill out at least some info on 1st tab before proceeding
		if(empty($name) || empty($title)) {
			$tab = "general";
			echo(mkwarn('Sorry, please fill out the name and title before proceeding.'));
			return;
		}

		// create a new survey in the database
		$fields =
		array('name','title','subtitle','email','thank_head','thank_body','info');
		foreach($fields as $f) {
			if(!empty($$f)) {
				array_push($f_arr,$f);
				array_push($v_arr,"'".$$f."'");
			}
		}
		$sql = "INSERT INTO surveys (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) . ")";

		$result = @mysql_query ($sql);
		if(!$result) {
			$tab = "general";
			echo(mkwarn('Sorry, name already in use. Pick a new name.'));
			echo(mkerror(mysql_errno().': '.mysql_error()));
			return;
		}

		$sql = "SELECT id FROM surveys WHERE name='${name}'";
		$result = mysql_query($sql);
		$survey_id = mysql_result($result,0,0);
		return;
	}

	// survey already started
	switch($old_tab) {
		// coming from the general tab ...
		case "general":
			$fields = array('name','title','subtitle','email','thank_head','thank_body','info');
			$sql = "SELECT name FROM surveys WHERE id='" . $survey_id ."'";
			$result = mysql_query($sql);

			// trying to change survey name
			if(mysql_result($result,0,0) != $name) {
				$sql = "SELECT COUNT(*) WHERE name='" . $name ."'";
				$result = @mysql_query($sql);
				if(@mysql_result($result,0,0) != 0) {
					$tab = "general";
					echo(mkwarn('Sorry, that name is already in use.'));
					return;
				}
			}

			// UPDATE the row in the DB with current values
			foreach($fields as $f) {
					array_push($f_arr,$f ."='" . $$f . "'");
			}
			$sql = "UPDATE surveys SET " . join(',',$f_arr) . " WHERE id='${survey_id}'";
			$result = @mysql_query($sql);
			if(!$result) {
				$tab = "general";
				echo(mkwarn('Warning, error encountered.'));
				echo(mkerror(mysql_errno().': '.mysql_error()));
				return;
			}
			return;

		// coming from the questions tab ...
		case "questions":
			// if the question box is empty ... ignore everything else
			if(empty($content))
				return;

			// constraint: question type required
			if(empty($type_id)) {
				echo(mkwarn('Sorry, you must select a type for this question.'));
				$tab = 'questions';
				$dont_clear = 1;
				return;
			}

			if(stristr($id,'new'))
				$id = '';

			// constraint: can not change between question w/ answer choices and one w/o
			$fields = array('survey_id','type_id','result_id','parent_id','required','params','content');
			$sql =  "SELECT has_answers FROM question_types WHERE id='${type_id}'";
			$result = mysql_query($sql);
			$has_answers = mysql_result($result,0,0);
			if($id != '') {
				$sql =  "SELECT question_types.has_answers FROM survey_questions LEFT JOIN
					question_types ON question_types.id = survey_questions.type_id
					WHERE survey_questions.survey_id='${survey_id}' AND
					survey_questions.id='${id}'"; 
				$result = mysql_query($sql);
				$old_has_answers = mysql_result($result,0,0);
				if($old_has_answers != $has_answers) { // trying to change between incompatible question types
					$tab = "questions";
					$sql = "SELECT type_id FROM survey_questions WHERE id='${id}'";
					$result = mysql_query($sql);
					$type_id = mysql_result($result,0,0);
					echo(mkwarn('Sorry, you cannot change between those types of question. Create a new question instead.'));
					$tab = 'questions';
					$dont_clear = 1;
					return;
				}
			}

			// constraint: check for mutually exclusive Question Type-Result Type pairs
			if(empty($result_id))
				$result_id = 1; // default to percentages
			if($type_id == 2 || $type_id == 3) {  // single line or essay
				$result_id = 4; // require list type results
			} elseif($result_id == 4) { // list
				$result_id = 1;	// only single line or essay can use a list result
			} elseif($type_id == 7 && $result_id == 2) { // rating question & ranking result
				$result_id = 1; // too confusing, so force to percentages
			} elseif($type_id == 8) { // rank question
				$ersult_id = 5;	// force to average rank
			}
			
			// UPDATE row in the DB for the current question
			$fields = array('type_id','result_id','parent_id','required','params','content');
			if($id != '') {
				foreach($fields as $f) {
					array_push($f_arr,"${f}='${$f}'");
				}
				$sql = "UPDATE survey_questions SET " . join(',',$f_arr) . " WHERE id='${id}'";

			// INSERT row in the DB for new question
			} else {
				$fields = array('survey_id','type_id','result_id','position','parent_id','required','params','content');
				if(empty($position) || $position == 0) {
					// set the position to the end
					$sql = "SELECT MAX(position) FROM survey_questions WHERE survey_id='${survey_id}'";
					$result = mysql_query($sql);
					$position = mysql_result($result,0,0) + 10;
				}
				foreach($fields as $f) {
					if(!empty($$f)) {
						array_push($f_arr,$f);
						array_push($v_arr,"'${$f}'");
					}
				}
				$sql  = "INSERT INTO survey_questions (" . join(',',$f_arr) . ") VALUES (" . join(',',$v_arr) .")";
			}
			$result = @mysql_query($sql);
			if($id == '')
				$id = mysql_insert_id();
			if(!$result) {
				echo(mkwarn('Warning, error encountered.'));
				echo(mkerror(mysql_errno().': '.mysql_error()));
				$tab = 'questions';
				$dont_clear = 1;
				return;
			}

			// UPDATE or INSERT rows for each of the question choices for this question
			if($has_answers) {
				$question_id = $id;
				$count = 0;
				for($i=1;$i<$num_choices+1;$i++) {
					$sql='';
					$choice_id = ${"choice_id_".$i};
					$choice_content = ${"choice_content_".$i};
					// each of the submitted choices
					if($choice_id=='' && $choice_content!='') {
						// insert (new)
						$sql = "INSERT INTO question_choices (question_id,content) VALUES ('${question_id}','${choice_content}')";
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
						$result = @mysql_query($sql);
						if(!$result) {
							echo(mkwarn('Warning, error encountered.'));
							echo(mkerror(mysql_errno().': '.mysql_error()));
							$tab = 'questions';
							$dont_clear = 1;
							return;
						}
					}
				}
				if(!$count) {
					echo(mkwarn('Sorry, you need at least one answer option for this question type.'));
					$tab = 'questions';
					$dont_clear = 1;
					return;
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
?>
