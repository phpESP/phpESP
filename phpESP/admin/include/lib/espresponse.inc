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

	$sql = "
SELECT id,type_id,content
FROM question
". survey_select_section_sql($sid,$section) ." AND
   required='Y' AND deleted='N'
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
			$sql = "SELECT id FROM question_choice WHERE question_id='$qid'";
			$cid_result = mysql_query($sql);
			while(list($cid) = mysql_fetch_row($cid_result)) {
				if(!isset($HTTP_POST_VARS["${qid}_${cid}"])) {
					$missing[$qid] = $content;
					break;
				}
			}
			mysql_free_result($cid_result);
			continue;
		}
		if ($tid == 10 && $HTTP_POST_VARS[$qid] === '0') // Numeric
			continue;
		if(empty($HTTP_POST_VARS[$qid]))
			$missing[$qid] = $content;
	}
	mysql_free_result($result);
	
	if(count($missing) > 0) {
		// missing required variables
		$message = _('You are missing the following required questions:') ."<br>\n";
		while(list($qid,$content)=each($missing)) {
			if($GLOBALS['ESPCONFIG']['DEBUG'])
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

	$userid = $GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER'];
	if (empty($userid))
		$userid = $HTTP_POST_VARS['userid'];

	if(empty($rid)) {
		// create a uniqe id for this response
		$sql = "INSERT INTO response (survey_id,username) VALUES ( '${sid}','${userid}' )";
		$result = mysql_query($sql);
		$rid = mysql_insert_id();
	}

	$sql  = "
SELECT Q.id, Q.type_id, T.response_table
FROM question Q, question_type T
". survey_select_section_sql($sid,$section,'Q') ." AND
   Q.survey_id='${sid}' AND
   Q.deleted='N' AND Q.type_id < 50 AND
   Q.type_id=T.id";

	$q_result = mysql_query($sql);
	while(list($qid, $tid, $table) = mysql_fetch_row($q_result)) {
		if (is_string($HTTP_POST_VARS[$qid]))
			$val = XADDSLASHES($HTTP_POST_VARS[$qid]);
		switch($table) {
			case 'response_bool':
				$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
				$result = mysql_query($sql);
				break;
			case 'response_text':
				// only insert if non-empty content
				if($tid == 10) { // numeric
					$val = ereg_replace("[^0-9.\-]*(-?[0-9]*\.?[0-9]*).*", '\1', $HTTP_POST_VARS[$qid]);
				}

				if(ereg("[^ \t\n]",$val)) {
					$sql  = "INSERT INTO ${table} ( response_id,question_id,response ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'response_date':
				// only insert if non-empty content
				if(($val = ereg_replace("[^0-9]*([0-9]+)/([0-9]+)/([0-9]+).*", '\3-\1-\2', $val))) {
					$sql  = "INSERT INTO ${table} ( response_id,question_id,response ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'response_single':
				if(empty($val)) {
					$sql = "SELECT id FROM question_choice WHERE question_id='${qid}' AND content LIKE '!other%' ORDER BY id";
					$c_result = mysql_query($sql);
					while(list($cid) = mysql_fetch_row($c_result)) {
						$other = XADDSLASHES($HTTP_POST_VARS["${qid}_${cid}"]);
						if(ereg("[^ \t\n]",$other)) {
							$sql = "INSERT INTO response_other ( response_id,question_id,choice_id,response ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
							$result = mysql_query($sql);
							$val = $cid;
							break;
						}
					}
					mysql_free_result($c_result);
				}
				if(ereg("other_([0-9]+)", $val, $regs)) {
					$cid=$regs[1];
					$other = XADDSLASHES($HTTP_POST_VARS["${qid}_${cid}"]);
					if(ereg("[^ \t\n]",$other)) {
						$sql = "INSERT INTO response_other ( response_id,question_id,choice_id,response ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
						$result = mysql_query($sql);
						$val=$cid;
					}
				}
				$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
				$result = mysql_query($sql);
				break;
			case 'response_multiple':
				$sql = "SELECT id FROM question_choice WHERE question_id='${qid}' AND content LIKE '!other%'";
				$c_result = mysql_query($sql);
				while(list($cid) = mysql_fetch_row($c_result)) {
					$other = XADDSLASHES($HTTP_POST_VARS["${qid}_${cid}"]);
					if(ereg("[^ \t\n]",$other)) {
						$sql = "INSERT INTO response_other ( response_id,question_id,choice_id,response ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
						$result = mysql_query($sql);
						$val[] = $cid;
					}
				}
				mysql_free_result($c_result);
				if(count($HTTP_POST_VARS[$qid]) < 1)
					break;
				foreach($HTTP_POST_VARS[$qid] as $cid) {
					$cid = XADDSLASHES($cid);
					if(ereg("other_[0-9]+", $cid))
						continue;
					$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${cid}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'response_rank':
				if($tid == 8) { // Rank
					$sql = "SELECT id FROM question_choice WHERE question_id='${qid}'";
					$cid_result = mysql_query($sql);
					while(list($cid) = mysql_fetch_row($cid_result)) {
						$val = XADDSLASHES($HTTP_POST_VARS[$qid ."_". $cid]);
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
	$sql = "UPDATE response SET complete='Y' WHERE id='${rid}'";
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

	// --------------------- response_bool ---------------------
	$sql = "SELECT question_id,choice_id FROM response_bool WHERE response_id='${rid}'";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_single ---------------------
	$sql = "SELECT question_id,choice_id FROM response_single WHERE response_id='${rid}'";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_multiple ---------------------
	$sql = "SELECT question_id,choice_id FROM response_multiple WHERE response_id='${rid}' ORDER BY question_id";
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

	// --------------------- response_rank ---------------------
	$sql = "SELECT question_id,choice_id,rank FROM response_rank WHERE response_id='${rid}'";
	$result = mysql_query($sql);
	while(list($qid,$cid,$rank) = mysql_fetch_row($result)) {
		if($cid)
			$values["${qid}_${cid}"]=$rank;
		else
			$values["$qid"]=$rank;
	}
	mysql_free_result($result);

	// --------------------- response_text ---------------------
	$sql = "SELECT question_id,response FROM response_text WHERE response_id='${rid}'";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_other ---------------------
	$sql = "SELECT question_id,choice_id,response FROM response_other WHERE response_id='${rid}' ORDER BY question_id";
	$result = mysql_query($sql);
	while(list($qid,$cid,$answer) = mysql_fetch_row($result)) {
		$values["${qid}_${cid}"]=$answer;
	}
	mysql_free_result($result);

	// --------------------- response_date ---------------------
	$sql = "SELECT a.question_id,a.response
	FROM response_date a
	WHERE a.response_id='${rid}'";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- return ---------------------
	ksort($values);
	return($values);
}
/* }}} */

/* {{{ proto array response_select_named(int survey_id, int response_id)
   Returns the values from the specific response in a sorted
   associative array indexed by question_id. */
function response_select_named($sid, $rid) {
	$values = array();

	// --------------------- response_bool ---------------------
	$sql = "SELECT q.name,a.choice_id
	FROM response_bool a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_single ---------------------
	$sql = "SELECT q.name,c.content 
	FROM response_single a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_multiple ---------------------
	$sql = "SELECT q.name,c.content 
	FROM response_multiple a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id
	ORDER BY a.question_id";
	$result = mysql_query($sql);
	$arr = array();
	$tmp = '';
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		if (ereg('^!other', $cid)) continue;
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

	// --------------------- response_other ---------------------
	$sql = "SELECT q.name,c.content,a.response
	FROM response_other a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id
	ORDER BY a.question_id";
	$result = mysql_query($sql);
	while(list($qid,$cid,$answer) = mysql_fetch_row($result)) {
		$cid = ereg_replace("!other=?", '', $cid);
		if (!empty($cid))
			$cid .= ': ';
		if (isset($values[$qid]))
			array_push($values[$qid], "$cid$answer");
		else
			$values[$qid] = "$cid$answer";
	}
	mysql_free_result($result);

	// --------------------- response_rank ---------------------
	$sql = "SELECT q.name,c.content,a.rank
	FROM response_rank a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id";
	$result = mysql_query($sql);
	while(list($qid,$cid,$rank) = mysql_fetch_row($result)) {
		if($cid) {
				$str1 = "${qid}_${cid}";
				do {
					$str2 = $str1;
					$str1 = eregi_replace(
					"(^| )(what|which|why|how|who|where|how|is|are|were|the|a|it|of|do|you|your|please|enter)[ ?]", 
					" ", $str2);
				} while ($str1 != $str2);
				$qid = $str1;
				$qid = strtoupper(eregi_replace(
					"[^A-Z0-9]+", "_", trim($qid)));
				$qid = ereg_replace('_$',"",$qid);
		}
		$values[$qid] = $rank;
	}
	mysql_free_result($result);

	// --------------------- response_text ---------------------
	$sql = "SELECT q.name,a.response
	FROM response_text a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
	}
	mysql_free_result($result);

	// --------------------- response_date ---------------------
	$sql = "SELECT q.name,a.response
	FROM response_date a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id";
	$result = mysql_query($sql);
	while(list($qid,$cid) = mysql_fetch_row($result)) {
		$values["$qid"]=$cid;
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
	if (!$GLOBALS['ESPCONFIG']['allow_email']) {
		return TRUE;
	}

	$sql = "SELECT name, email FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
	list($name, $email) = mysql_fetch_row($result);
	mysql_free_result($result);

	if(empty($email))
		return(FALSE);

	$sql = "SELECT id FROM question WHERE survey_id='${sid}' AND deleted='N' ORDER BY position,id";
	$result = mysql_query($sql);

	$userid = $GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER'];
	if(empty($userid))
		$userid = $GLOBALS['HTTP_POST_VARS']['userid'];

	$subject = _('Response from survey:') ." $name [$rid]";
	$body  = "survey.id = ${sid}\n";
	$body .= "survey.name = ${name}\n";
	$body .= "response.id = ${rid}\n";
	$body .= "response.username = ${userid}\n";

	$answers = response_select($sid,$rid);

	$headers = "From: \"phpESP ".
		addslashes($GLOBALS['ESPCONFIG']['VERSION']) .
		"\" <phpesp@". $GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] .">\n";
	$headers .= "X-Sender: <phpesp@". $GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] .">\n"; 
	$headers .= "X-Mailer: phpESP\n";
	$headers .= "Return-Path: <". $GLOBALS['HTTP_SERVER_VARS']['SERVER_ADMIN'] ."@".
		$GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] . ">\n";

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
