<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_LIB_ESPRESPONSE')) {
 	define('_LIB_ESPRESPONSE',TRUE);

/* {{{ proto string response_check_required(int survey_id, int section)
   Reads current form variables from HTTP_POST_VARS.
   Returns an empty string if all required fields are
   completed, else returns a message string indicating which
   questions need to be completed. */
function response_check_required($sid, $section) {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

	$sql = "
SELECT id,type_id,content
FROM survey_questions
". survey_select_section_sql($sid,$section) ." AND
   required='y'
ORDER BY position";

	$result = mysql_query($sql);
	if(mysql_num_rows($result) < 1) {
		// no required fields! so no need to continue
		mysql_free_result($result);
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
		$message = _('You are missing the following required questions:') ."<br>\n";
		while(list($qid,$content)=each($missing)) {
			if(DEBUG)
				$message .= "<!-- ${qid} -->";
			$message .= "${content}<br>\n";
		}
		return($message);
	}
	return('');
}
/* }}} */

/* {{{ proto int response_insert(int survey_id, int section, int response_id)
   Reads current form variables from HTTP_POST_VARS.
   Returns the ID for the response. */
function response_insert($sid,$section,$rid) {
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
". survey_select_section_sql($sid,$section) ." AND
   Q.survey_id='${sid}' AND
   Q.status=0 AND
   Q.type_id=T.id";

	$q_result = mysql_query($sql);
	while(list($qid, $tid, $table) = mysql_fetch_row($q_result)) {
		$val = $HTTP_POST_VARS[$qid];
		switch($table) {
			case 'answers_bool':
				$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
				$result = mysql_query($sql);
				break;
			case 'answers_text':
				// only insert if non-empty content
				if(ereg("[^ \t\n]",$val)) {
					$sql  = "INSERT INTO ${table} ( response_id,question_id,answer ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'answers_single':
				if(empty($val)) {
					$sql = "SELECT id FROM question_choices WHERE question_id='${qid}' AND content LIKE '!other%' ORDER BY id";
					$c_result = mysql_query($sql);
					while(list($cid) = mysql_fetch_row($c_result)) {
						$other = $HTTP_POST_VARS["${qid}_${cid}"];
						if(ereg("[^ \t\n]",$other)) {
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
					if(ereg("[^ \t\n]",$other)) {
						$sql = "INSERT INTO answers_other ( response_id,question_id,choice_id,answer ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
						$result = mysql_query($sql);
						$val=$cid;
					}
				}
				$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
				$result = mysql_query($sql);
				break;
			case 'answers_multiple':
				$sql = "SELECT id FROM question_choices WHERE question_id='${qid}' AND content LIKE '!other%'";
				$c_result = mysql_query($sql);
				while(list($cid) = mysql_fetch_row($c_result)) {
					$other = $HTTP_POST_VARS["${qid}_${cid}"];
					if(ereg("[^ \t\n]",$other)) {
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
					mysql_free_result($cid_result);
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
/* }}} */

/* {{{ proto bool response_commit(int response_id)
   Returns true on sucess. */
function response_commit($rid) {
	$sql = "UPDATE survey_responses SET complete='yes' WHERE id='${rid}'";
	if(mysql_query($sql))
		return(TRUE);
	return(FALSE);
}
/* }}} */

/* {{{ proto array response_select(int survey_id, int response_id)
   Returns the values from the specific response in a sorted
   associative array indexed by question_id. */
function response_select($sid, $rid) {
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
/* }}} */

/* {{{ proto bool response_send_email(int survey_id, int response_id)
   Reads current form variables from HTTP_POST_VARS.
   Returns true if mail sent. */
function response_send_email($sid, $rid) {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

	$sql = "SELECT name,email FROM surveys WHERE id='${sid}'";
	$result = mysql_query($sql);
	list($name, $email) = mysql_fetch_row($result);
	mysql_free_result($result);

	if(empty($email))
		return(FALSE);

	$sql = "SELECT id FROM survey_questions WHERE survey_id='${sid}' AND status=0 ORDER BY position,id";
	$result = mysql_query($sql);

	$subject = _('Response from survey:') ." $name [$rid]";
	$body  = "surveys.id = ${sid}\n";
	$body .= "surveys.name = ${name}\n";
	$body .= "survey_responses.id = $rid\n";
	$body .= "survey_responses.username = ". $HTTP_POST_VARS['userid'] ."\n";

	$answers = response_select($sid,$rid);

	$headers = "From: \"phpESP ". addslashes($VERSION) ."\" <phpesp\@${SERVER_NAME}>\n";
	$headers .= "X-Sender: <phpesp\@${SERVER_NAME}>\n"; 
	$headers .= "X-Mailer: phpESP\n"; // mailer
	$headers .= "Return-Path: <${SERVER_ADMIN}\@${SERVER_NAME}>\n";  // Return path for errors

	while(list($qid,$var) = each($answers)) {
		if(is_array($var)) {
			$body .= $qid ." = ". join(',',$var) . "\n";
		} else {
			$body .= $qid ." = ". $var ."\n";
		}
	}
	return(mail($email,$subject,$body,$headers));
}
/* }}} */

} // end _LIB_ESPRESPONSE
?>
