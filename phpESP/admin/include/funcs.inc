<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* NOTE: In order for these to work you must have:
 *    register_globals = On
 * in php.ini
 */

// int		count_sections(int sid);
// char*	section_range_sql(int sid, int section);
// bool[]	arr_has_answers(void);
// !exit	goto_thankyou(int sid, char* referer);
// char*	check_required(int sid, int section);
// char*->char*	retrieve_values(int sid, int section);
// int		insert_values(int sid, int section, int rid);
// bool		complete_response(int rid);
// char*	email_values(int sid, int rid);
// void		clear_results(int sid);

if(!defined('_FUNCS')) {
 	define('_FUNCS',TRUE);

	// return the number of sections in survey $sid
	function count_sections($sid) {
		$sql = "
SELECT COUNT(*)
  FROM survey_questions
 WHERE survey_id='${sid}' AND
       type_id='99' AND
	   status=0";
		return(mysql_result(mysql_query($sql),0,0) + 1);
	}

	// return a string w/ SQL fragment to limit to this $section
	function section_range_sql($sid,$section) {
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

		return('WHERE ' . join(' AND ',$ret) . ' ');
	}

	// build associative array holding weather each question
	// type has answer choices or not
	function arr_has_answers() {
		$has_answers = array();
		$sql = 'SELECT id,has_answers FROM question_types ORDER BY id';
		$result = mysql_query($sql);
		while(list($tid,$answ) = mysql_fetch_row($result)) {
			$has_answers[$tid]=$answ;
		}
		mysql_free_result($result);
		return($has_answers);
	}

	// redirect to thank you page for survey ID 'sid'
	// exits PHP!
	function goto_thankyou($sid,$referer) {
		global $cf_thank_body;
		global $cf_thank_head;
		$sql = "SELECT thanks_page,thank_head,thank_body FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		list($thank_url,$thank_head,$thank_body) = mysql_fetch_row($result);
		if((empty($thank_head)  && empty($thank_body)) && !empty($thank_url)) {
?>
<script language="JavaScript">
<!--
window.location="<?php echo($thank_url); ?>"
//-->
</script>
<noscript><h2>Thank You for completing this survey.</h2>
Please click <a href="<?php echo($thank_url); ?>">here</a> to continue.
</noscript>
<?php
			exit;
		}

		if(empty($thank_body) && empty($thank_head)) {
			$thank_body = $cf_thank_body;
			$thank_head = $cf_thank_head;
		}
?>
<h2><?php echo($thank_head); ?></h2>
<blockquote><?php echo($thank_body); ?></blockquote>
<a href="<?php echo($referer); ?>">Return</a>
<?php
		return;
	}

	// check for required questions for survey ID 'sid'
	// return message with missed questions
	function check_required($sid,$section) {
		global $HTTP_POST_VARS;
		@reset($HTTP_POST_VARS);

		$sql = "SELECT id,type_id,content FROM survey_questions ";
		$sql .= section_range_sql($sid,$section);
		$sql .= "AND required='y' ORDER BY position";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) < 1) {
			// no required fields! so no need to continue
			return('');
		}
		$missing = array();	// array of missing questions
		while(list($qid,$tid,$content) = mysql_fetch_row($result)) {
			if($tid == 8) { // Rank
				$sql = "SELECT id FROM question_choices WHERE question_id='${qid}'";
				$cid_result = mysql_query($sql);
				while(list($cid) = mysql_fetch_row($cid_result)) {
					if(empty($HTTP_POST_VARS["${qid}_${cid}"])) {
						$missing[$qid] = $content;
						break;
					}
				}
				mysql_free_result($cid_result);
				continue;
			}
			if(empty($HTTP_POST_VARS[$qid]))
					$missing[$qid] = $content;
		}
		mysql_free_result($result);
		if(count($missing) > 0) {
			// missing required variables
			$message = "You are missing the following required questions:<br>\n";
			while(list($qid,$content)=each($missing)) {
				$message .= "<!-- ${qid}  -->${content}<br>\n";
			}
			return($message);
		}
		return('');
	}

	// load the values from a specific response $rid, and return
	// them in an array (like HTTP_POST_VARS)
	function retrieve_values($sid, $rid) {
		$values = array();

		// --------------------- answers_bool ---------------------
		$sql = "SELECT question_id,choice_id FROM answers_bool WHERE response_id='${rid}'";
		$result = mysql_query($sql);
		while(list($qid,$cid) = mysql_fetch_row($result)) {
			$values["$qid"]=$cid;
		}
		mysql_free_result($result);

		// --------------------- answers_single ---------------------
		$sql = "SELECT question_id,choice_id FROM answers_single WHERE response_id='${rid}'";
		$result = mysql_query($sql);
		while(list($qid,$cid) = mysql_fetch_row($result)) {
			$values["$qid"]=$cid;
		}
		mysql_free_result($result);

		// --------------------- answers_multiple ---------------------
		$sql = "SELECT question_id,choice_id FROM answers_multiple WHERE response_id='${rid}' ORDER BY question_id";
		$result = mysql_query($sql);
		$arr = array();
		$tmp = '';
		while(list($qid,$cid) = mysql_fetch_row($result)) {
			if($tmp == $qid) {
				array_push($arr, $cid);
				continue;
			}
			if(!empty($tmp))
				$values["$tmp"]=$arr;
			$tmp = $qid;
			$arr = array($cid);
		}
		if(!empty($tmp))
			$values["$tmp"]=$arr;
		mysql_free_result($result);

		// --------------------- answers_rank ---------------------
		$sql = "SELECT question_id,choice_id,rank FROM answers_rank WHERE response_id='${rid}'";
		$result = mysql_query($sql);
		while(list($qid,$cid,$rank) = mysql_fetch_row($result)) {
			if($cid)
				$values["${qid}_${cid}"]=$rank;
			else
				$values["$qid"]=$rank;
		}
		mysql_free_result($result);

		// --------------------- answers_text ---------------------
		$sql = "SELECT question_id,answer FROM answers_text WHERE response_id='${rid}'";
		$result = mysql_query($sql);
		while(list($qid,$cid) = mysql_fetch_row($result)) {
			$values["$qid"]=$cid;
		}
		mysql_free_result($result);

		// --------------------- answers_other ---------------------
		$sql = "SELECT question_id,choice_id,answer FROM answers_other WHERE response_id='${rid}' ORDER BY question_id";
		$result = mysql_query($sql);
		while(list($qid,$cid,$answer) = mysql_fetch_row($result)) {
			$values["${qid}_${cid}"]=$answer;
		}
		mysql_free_result($result);

		// --------------------- return ---------------------
		ksort($values);
		return($values);
	}

	// insert survey response into the database (for survey ID 'sid')
	function insert_values($sid,$section,$rid) {
		global $HTTP_POST_VARS;
		@reset($HTTP_POST_VARS);

		$userid = $HTTP_POST_VARS['userid'];

		if(empty($rid)) {
			// create a uniqe id for this response
			$sql = "INSERT INTO survey_responses (survey_id,username) VALUES ( '${sid}','${userid}' )";
			$result = mysql_query($sql);
			$rid = mysql_insert_id();
		}

		$sql  = "
SELECT Q.id, Q.type_id, T.answer_table
  FROM survey_questions Q, question_types T
". section_range_sql($sid,$section) ." AND
       Q.survey_id='${sid}' AND
       Q.status=0 AND
	   Q.type_id=T.id";

		$q_result = mysql_query($sql);
		while(list($qid, $tid, $table) = mysql_fetch_row($q_result)) {
			$val = $HTTP_POST_VARS["$qid"];
			switch($table) {
				case 'answers_bool':
					$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
					break;
				case 'answers_text':
					$sql  = "INSERT INTO ${table} ( response_id,question_id,answer ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
					break;
				case 'answers_single':
					if(empty($val)) {
						$sql = "SELECT id FROM question_choices WHERE question_id='${qid}' AND content LIKE '!other%' ORDER BY id";
						$c_result = mysql_query($sql);
						while(list($cid) = mysql_fetch_row($c_result)) {
							$other = $HTTP_POST_VARS["${qid}_${cid}"];
							if(ereg("[^ \t]",$other)) {
								$sql = "INSERT INTO answers_other ( response_id,question_id,choice_id,answer ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
								$result = mysql_query($sql);
								$val = $cid;
								break;
							}
						}
						mysql_free_result($c_result);
					}
					if(ereg("other_([0-9]+)", $val, $regs)) {
						$cid=$regs[1];
						$other = $HTTP_POST_VARS["${qid}_${cid}"];
						$sql = "INSERT INTO answers_other ( response_id,question_id,choice_id,answer ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
						$result = mysql_query($sql);
						$val=$cid;
					}
					$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
					break;
				case 'answers_multiple':
					$sql = "SELECT id FROM question_choices WHERE question_id='${qid}' AND content LIKE '!other%'";
					$c_result = mysql_query($sql);
					while(list($cid) = mysql_fetch_row($c_result)) {
						$other = $HTTP_POST_VARS["${qid}_${cid}"];
						if(ereg("[^ \t]",$other)) {
							$sql = "INSERT INTO answers_other ( response_id,question_id,choice_id,answer ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
							$result = mysql_query($sql);
							$val[] = $cid;
						}
					}
					mysql_free_result($c_result);
					if(empty($val))
						break;
					foreach($val as $cid) {
						if(ereg("other_[0-9]+", $cid))
							continue;
						$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${cid}' )";
						$result = mysql_query($sql);
					}
					break;
				case 'answers_rank':
					if($tid == 8) { // Rank
						$sql = "SELECT id FROM question_choices WHERE question_id='${qid}'";
						$cid_result = mysql_query($sql);
						while(list($cid) = mysql_fetch_row($cid_result)) {
							$val = $HTTP_POST_VARS[$qid ."_". $cid];
							if(strtolower($val) == "n/a")
								$rank = -1;
							else
								$rank = $val;
							$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id,rank ) ";
							$sql .= "VALUES ( '${rid}','${qid}','${cid}','${rank}' )";
							mysql_query($sql);
						}
						break;
					}
					if(strtolower($val) == "n/a")
						$rank = -1;
					else
						$rank = $val;
					$sql  = "INSERT INTO ${table} ( response_id,question_id,rank ) VALUES ( '${rid}','${qid}','${rank}' )";
					$result = mysql_query($sql);
					break;
			}
		}
		mysql_free_result($q_result);
		return($rid);
	}

	// function to update a $rid as complete
	function complete_response($rid) {
		$sql = "UPDATE survey_responses SET complete='yes' WHERE id='${rid}'";
		return(!mysql_query($sql));
	}

	// email survey response (for survey ID 'sid')
	function email_values($sid, $rid) {
		global $DEBUG;
		global $HTTP_POST_VARS;
		@reset($HTTP_POST_VARS);

		$sql = "SELECT name,email FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		list($name, $email) = mysql_fetch_row($result);
		mysql_free_result($result);

		if(empty($email))
			return;

		$sql = "SELECT id FROM survey_questions WHERE survey_id='${sid}' AND status=0 ORDER BY position,id";
		$result = mysql_query($sql);

		$subject = "Response from survey: ${name} [${rid}]";
		$body  = "surveys.id = ${sid}\n";
		$body .= "surveys.name = ${name}\n";
		$body .= "survey_responses.id = $rid\n";
		$body .= "survey_responses.username = ". $HTTP_POST_VARS['userid'] ."\n";

		$answers = retrieve_values($sid,$rid);

		while(list($qid,$var) = each($answers)) {
			if(is_array($var)) {
				$body .= $qid ." = ". join(',',$var) . "\n";
			} else {
				$body .= $qid ." = ". $var ."\n";
			}
		}
		return(mail($email,$subject,$body));
	}

	// clear the results of a survey (non-reversable)
	function clear_results($sid) {
		$sql = "SELECT id FROM survey_questions WHERE survey_id='${sid}'";
		$result = mysql_query($sql);
		$qids = array();
		while(list($qid) = mysql_fetch_row($result)) {
			array_push($qids, $qid);
		}
		mysql_free_result($result);
		$qidstr = "question_id IN " . ereg_replace("([0-9]+)","'\\1'", "(" . join(",", $qids) . ")");
	
		$tables = array('answers_bool','answers_multiple','answers_other','answers_rank','answers_single','answers_text');
		foreach($tables as $table) {
			$sql = "DELETE FROM ${table} WHERE ${qidstr}";
			$result = mysql_query($sql);
		}
		$sql = "DELETE FROM survey_responses WHERE survey_id=${sid}";
		mysql_query($sql);

		return;
	}

	require($PIECES."/funcs_html".$EXT);
	require($PIECES."/funcs_render".$EXT);
	require($PIECES."/function.survey_results".$EXT);
	require($PIECES."/function.survey_report".$EXT);
	require($PIECES."/function.survey_copy".$EXT);
	require($PIECES."/function.survey_purge".$EXT);

} // end if !defined(_FUNCS)

?>
