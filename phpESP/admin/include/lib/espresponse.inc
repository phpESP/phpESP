<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

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

/* {{{ proto void response_delete(int survey_id, int section, int response_id)
   Deletes values for the response. */
function response_delete($sid, $rid, $sec = null) {
	if (empty($rid))
		return;

	if ($sec != null) {
		if ($sec < 1)
			return;

		/* get question_id's in this section */
		$qids = survey_get_sections($sid);
		if (!isset($qids[$sec - 1]))
			return;
		$qids = 'AND question_id '. array_to_insql($qids[$sec - 1]);
	} else {
		/* delete all */
		$qids = '';
	}

	/* delete values */
	foreach (array('bool', 'single', 'multiple', 'rank', 'text', 'other', 'date') as $tbl) {
		$sql = "DELETE FROM response_$tbl WHERE response_id = '$rid' $qids";
		$res = mysql_query($sql);
	}
}
/* }}} */

/* {{{ proto void response_delete_all(int survey_id)
   Deletes all responses from survey. */
function response_delete_all($sid) {
	$sec = survey_get_sections($sid);
	$qids = array();
	foreach ($sec as $s)
		$qids = array_merge($qids, $s);
	$qids = array_to_insql($qids);

	/* delete values */
	foreach (array('bool', 'single', 'multiple', 'rank', 'text', 'other', 'date') as $tbl) {
		$sql = "DELETE FROM response_$tbl WHERE question_id $qids";
		$res = mysql_query($sql);
	}
        /* ensure responses from testing status are also deleted */
        $sql = "DELETE FROM response WHERE survey_id=${sid}";
        mysql_query($sql);
}
/* }}} */

/* {{{ proto int response_insert(int survey_id, int section, int response_id)
   Reads current form variables from HTTP_POST_VARS.
   Returns the ID for the response. */
function response_insert($sid,$section,$rid) {
	global $HTTP_POST_VARS;

	$userid = isset($GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER']) ?
            $GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER'] : '';
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
        $bareval = isset($HTTP_POST_VARS[$qid]) ? _stripslashes($HTTP_POST_VARS[$qid]) : '';
		if (is_string($bareval))
			$val = addslashes($bareval);
        else
            $val = '';
		switch($table) {
			case 'response_bool':
				$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id ) VALUES ( '${rid}','${qid}','${val}' )";
                if (!empty($val))
    				$result = mysql_query($sql);
				break;
			case 'response_text':
				// only insert if non-empty content
				if($tid == 10) { // numeric
					$bareval = ereg_replace("[^0-9.\-]*(-?[0-9]*\.?[0-9]*).*", '\1', $bareval);
				}

				if(ereg("[^ \t\n]",$bareval)) {
                    $val = addslashes($bareval);
					$sql  = "INSERT INTO ${table} ( response_id,question_id,response ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'response_date':
				// only insert if non-empty content
				if(($bareval = ereg_replace("[^0-9]*([0-9]+)/([0-9]+)/([0-9]+).*", '\3-\1-\2', $bareval))) {
                    $val = addslashes($bareval);
					$sql  = "INSERT INTO ${table} ( response_id,question_id,response ) VALUES ( '${rid}','${qid}','${val}' )";
					$result = mysql_query($sql);
				}
				break;
			case 'response_single':
				if(empty($bareval)) {
					$sql = "SELECT id FROM question_choice WHERE question_id='${qid}' AND content LIKE '!other%' ORDER BY id";
					$c_result = mysql_query($sql);
					while(list($cid) = mysql_fetch_row($c_result)) {
                        if (!isset($HTTP_POST_VARS["${qid}_${cid}"]))
                            continue;
						$other = _addslashes($HTTP_POST_VARS["${qid}_${cid}"]);
						if(ereg("[^ \t\n]",$other)) {
							$sql = "INSERT INTO response_other ( response_id,question_id,choice_id,response ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
							$result = mysql_query($sql);
							$val = $cid;
							break;
						}
					}
					mysql_free_result($c_result);
				}
				if(ereg("other_([0-9]+)", $bareval, $regs)) {
					$cid=$regs[1];
                    if (!isset($HTTP_POST_VARS["${qid}_${cid}"]))
                        break; // out of the case
					$other = _addslashes($HTTP_POST_VARS["${qid}_${cid}"]);
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
                    if (!isset($HTTP_POST_VARS["${qid}_${cid}"]) || empty($HTTP_POST_VARS["${qid}_${cid}"]))
                        continue;
                    if (!isset($HTTP_POST_VARS[$qid]))
                        $HTTP_POST_VARS[$qid] = array($cid);
                    else
                        array_push($HTTP_POST_VARS[$qid], $cid);
					$other = _addslashes($HTTP_POST_VARS["${qid}_${cid}"]);
					if(ereg("[^ \t\n]",$other)) {
						$sql = "INSERT INTO response_other ( response_id,question_id,choice_id,response ) VALUES ( '${rid}','${qid}','${cid}','${other}' )";
						$result = mysql_query($sql);
					}
				}
				mysql_free_result($c_result);
				if(!isset($HTTP_POST_VARS[$qid]) || count($HTTP_POST_VARS[$qid]) < 1)
					break;
				foreach($HTTP_POST_VARS[$qid] as $cid) {
					$cid = _addslashes($cid);
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
                        if (!isset($HTTP_POST_VARS["${qid}_${cid}"]))
                            continue;
						$val = _addslashes($HTTP_POST_VARS["${qid}_${cid}"]);
						if(strtolower($val) == "n/a")
							$rank = -1;
						else
							$rank = intval($val);
						$sql  = "INSERT INTO ${table} ( response_id,question_id,choice_id,rank ) ";
						$sql .= "VALUES ( '${rid}','${qid}','${cid}','${rank}' )";
						mysql_query($sql);
					}
					mysql_free_result($cid_result);
					break;
				}
				if(strtolower($bareval) == "n/a")
					$rank = -1;
				else
					$rank = intval($bareval);
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
   Marks a response as completed.  Returns true on success.
 */
function response_commit($rid) {
	$sql = "UPDATE response SET complete='Y', submitted = NULL WHERE id='${rid}'";
	if(mysql_query($sql))
		return(true);
	return(false);
}
/* }}} */

/* {{{ proto int response_key_cmp(mixed left, mixed right)
   Key comparator for response keys (to handle numeric, and strings
   composed of QID_CID).
 */
function response_key_cmp($l, $r) {
    $lx = explode('_', $l);
    $rx = explode('_', $r);
    $lc = intval($lx[0]);
    $rc = intval($rx[0]);
    if ($lc == $rc) {
        if (count($lx) > 1 && count($rx) > 1) {
            $lc = intval($lx[1]);
            $rc = intval($rx[1]);
        } else if (count($lx) > 1) {
            $lc++;
        } else if (count($rx) > 1) {
            $rc++;
        }
    }
    if ($lc == $rc)
        return 0;
    return ($lc > $rc) ? 1 : -1;
}
/* }}} */

/* {{{ proto array response_select(int survey_id, int response_id, array columns, array question_ids)
   Returns the values from the specific response in a sorted
   (associative) array indexed by question_id.
   
   The key in the associative array is the Question ID, or in the case
   of "!other", or rank questions the Question ID concatenated with the
   Choice ID ("${qid}_${cid").  The value for all types is an array. 
   This array contains any columns requested by the parameter $col as
   the first elements.  The last two elements are the response content,
   and the content ID respectively.  (For multiple answer questions,
   the value is an array of these arrays.)
   
   For example, for a boolean type question (ID #42) and a check box
   type (ID #44), you might get something like:

      $response_select = array(
        '42' => array(_('No'), 'N'),
        '44' => array(
                  array('Blue', 102),
                  array('Green', 104)
                )
        );

   When in doubt, consult var_dump() as to the format of the returned
   array.
 */
function response_select($sid, $rid, $col = null, $qids = null) {
	$values = array();

    if ($col == null) {
        $col = '';
    }
    if (!is_array($col) && !empty($col)) {
        $col = explode(',', preg_replace("/\s/",'', $col));
    }
    if (is_array($col) && count($col) > 0) {
        $col = ',' . implode(',', array_map(create_function('$a','return "q.$a";'), $col));
    }

    if ($qids == null) {
        $qids = '';
    } elseif (is_array($qids)) {
        $qids = 'AND a.question_id ' . array_to_insql($qids);
    } elseif (intval($qids) > 0) {
        $qids = 'AND a.question_id = ' . intval($qids);
    } else {
        $qids = '';
    }

	// --------------------- response_bool ---------------------
	$sql = "SELECT q.id $col,a.choice_id
	FROM response_bool a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id $qids";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
        $val = array_pop($row);
		$values[$qid] = $row;
        array_push($values["$qid"], ($val == 'Y') ? _('Yes') : _('No'), $val);
	}
	mysql_free_result($result);

	// --------------------- response_single ---------------------
	$sql = "SELECT q.id $col,c.content,c.id
	FROM response_single a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id $qids";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
        $c = count($row);
        $val = $row[$c - 2];
		if (ereg('^!other', $val))
            $row[$c - 1] = 'other_' . $row[$c - 1];
        else
            settype($row[$c - 1], "int");
		$values[$qid] = $row;
	}
	mysql_free_result($result);

	// --------------------- response_multiple ---------------------
	$sql = "SELECT q.id $col,c.content,c.id
	FROM response_multiple a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id $qids
	ORDER BY a.question_id,c.id";
	$result = mysql_query($sql);
	$arr = array();
	$tmp = null;
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
        $c = count($row);
        $val = $row[$c - 2];
		if (ereg('^!other', $val))
            $row[$c - 1] = 'other_' . $row[$c - 1];
        else
            settype($row[$c - 1], "int");
		if($tmp == $qid) {
			$arr[] = $row;
			continue;
		}
		if($tmp != null)
			$values["$tmp"]=$arr;
		$tmp = $qid;
		$arr = array($row);
	}
	if($tmp != null)
		$values["$tmp"]=$arr;
	mysql_free_result($result);
    unset($arr);
    unset($tmp);
    unset($row);

	// --------------------- response_other ---------------------
	$sql = "SELECT q.id,c.id $col,a.response
	FROM response_other a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id $qids
        ORDER BY a.question_id,c.id";
        $result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
        $cid = array_shift($row);
        array_push($row, $row[count($row) - 1]);
		$values["${qid}_${cid}"] = $row;
        }
        mysql_free_result($result);

        // --------------------- response_rank ---------------------
	$sql = "SELECT CONCAT(q.id,'_',c.id) AS id $col,c.content,a.rank
        FROM response_rank a, question q, question_choice c
	WHERE a.response_id='${rid}' AND a.question_id=q.id AND a.choice_id=c.id $qids
    ORDER BY a.question_id,c.id";
        $result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
        settype($row[count($row) - 1], "int");
		$values[$qid] = $row;
        }
        mysql_free_result($result);

        // --------------------- response_text ---------------------
	$sql = "SELECT q.id $col,a.response
        FROM response_text a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id $qids";
        $result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
		$values["$qid"]=$row;
        $val = array_pop($values["$qid"]);
        array_push($values["$qid"], $val, $val);
        }
        mysql_free_result($result);

        // --------------------- response_date ---------------------
	$sql = "SELECT q.id $col,a.response
        FROM response_date a, question q
	WHERE a.response_id='${rid}' AND a.question_id=q.id $qids";
        $result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
        $qid = array_shift($row);
		$values["$qid"]=$row;
        $val = array_pop($values["$qid"]);
        array_push($values["$qid"], $val, $val);
        }
        mysql_free_result($result);

        // --------------------- return ---------------------
        uksort($values, 'response_key_cmp');
        return($values);
}
/* }}} */

/* {{{ proto array response_select_human(int survey_id, int response_id, array question_ids)
   A wrapper around response_select(), that returns an array of
   key/value pairs more suitable for humans.
 */
function response_select_human($sid, $rid, $qids = null) {
    $res = response_select($sid, $rid, 'type_id,content', $qids);
    $hmn = array();
    reset($res);
    $tmpk = null;
    $tmpv = array();
    while(list($qid, $arr) = each($res)) {
        $key = null;
        $val = null;
        if (strstr($qid, '_')) {
            // rank or other
            list($qid, $sub) = explode('_', $qid);
            if ($arr[0] != 8)
                continue; // other

            // rank
            $key = $arr[1];
            if ($arr[3] < 0)
                $arr[3] = 'n/a';
            else
                $arr[3]++;
            $val = $arr[2] . ' = ' . $arr[3];
            if ($tmpk != $key) {
                if (!empty($tmpk) || count($tmpv))
                    array_push($hmn, array($tmpk, $tmpv));
                $tmpk = null;
                $tmpv = array();
            }
            $tmpk = $key;
            array_push($tmpv, $val);
            continue;
        }
        if (!empty($tmpk) || count($tmpv))
            array_push($hmn, array($tmpk, $tmpv));
        if (is_array($arr[0])) {
            // mutiple
            $key = $arr[0][1];
            $val = array();
            foreach ($arr as $subarr) {
                if (ereg("^!other", $subarr[2])) {
                    $tmpv = preg_replace(array("/^!other=/","/^!other/"),
                            array('', 'Other'), $subarr[2]);
                    $tmp = preg_replace("/^other/", $qid, $subarr[3]);
                    if (isset($res[$tmp]))
                        $tmpv .= ': '. $res[$tmp][2];
                    array_push($val, $tmpv);
                } else {
                    array_push($val, $subarr[2]);
                }
            }
        } else {
            $key = $arr[1];
            if (ereg("^!other", $arr[2])) {
                $val = preg_replace(array("/^!other=/","/^!other/"),
                        array('', 'Other'), $arr[2]);
                $tmp = preg_replace("/^other/", $qid, $arr[3]);
                if (isset($res[$tmp]))
                    $val .= ': '. $res[$tmp][2];
            } else {
                $val = $arr[2];
            }
        }
        $tmpk = null;
        $tmpv = array();
        $hmn[] = array($key, $val);
    }
    return $hmn;
}
/* }}} */

/* {{{ proto array response_select_compact(int survey_id, int response_id, array question_ids)
   A wrapper around response_select(), that returns an array of
   key/value pairs more suitable for computer parsing.
 */
function response_select_compact($sid, $rid, $qids = null) {
    $res = response_select($sid, $rid, 'type_id', $qids);
    $cpq = array();
    reset($res);
    while(list($qid, $arr) = each($res)) {
        if (strstr($qid, '_')) {
            // rank or other
            if ($arr[0] == 8) {
                // rank
                $cpq[] = array($qid, $arr[2], array($arr[1]));
            } else {
                // other
                $cpq[] = array($qid, $arr[1]);
            }
        } elseif (is_array($arr[0])) {
            // multiple
	        $cpq[] = array($qid,
                    array_map(create_function('$a', 'return $a[2];'), $arr),
                    array_map(create_function('$b', 'return $b[1];'), $arr));
        } else {
            if ($arr[0] == 4 || $arr[0] == 6)
    	        $cpq[] = array($qid, $arr[2], array($arr[1]));
            else
    	        $cpq[] = array($qid, $arr[2]);
        }
    }
    return $cpq;
}
/* }}} */

/* {{{ proto array response_select_name(int survey_id, int response_id, array question_ids)
   A wrapper around response_select(), that returns an array of
   key/value pairs using the field name as the key.
 */
function response_select_name($sid, $rid, $qids = null) {
    $res = response_select($sid, $rid, 'type_id,name', $qids);
    $nam = array();
    reset($res);
    while(list($qid, $arr) = each($res)) {
        $key = null;
        $val = null;
        if (strstr($qid, '_')) {
            // rank or other
            list($qid, $sub) = explode('_', $qid);
            if ($arr[0] != 8)
                continue; // other

            // rank
            $str1 = $arr[2];
            do {
                $str2 = $str1;
                $str1 = eregi_replace(
                    "(^| )(what|which|why|how|who|where|how|is|are|were|the|a|it|of|do|you|your|please|enter)[ ?]",
                    " ", $str2);
            } while ($str1 != $str2);
            $str1 = strtoupper(eregi_replace(
                "[^A-Z0-9]+", "_", trim($str1)));
            $str1 = ereg_replace('_$',"",$str1);
            $arr[1] .= "_$str1";
            $nam[$arr[1]] = $arr[3];
            continue;
        }
        if (is_array($arr[0])) {
            // mutiple
            $key = $arr[0][1];
            $val = array();
            foreach ($arr as $subarr) {
                if (ereg("^!other", $subarr[2])) {
                    $tmpv = preg_replace(array("/^!other=/","/^!other/"),
                            array('', 'Other'), $subarr[2]);
                    $tmp = preg_replace("/^other/", $qid, $subarr[3]);
                    if (isset($res[$tmp]))
                        $tmpv .= ': '. $res[$tmp][2];
                    array_push($val, $tmpv);
                } else {
                    array_push($val, $subarr[2]);
                }
            }
        } else {
            $key = $arr[1];
            if (ereg("^!other", $arr[2])) {
                $val = preg_replace(array("/^!other=/","/^!other/"),
                        array('', 'Other'), $arr[2]);
                $tmp = preg_replace("/^other/", $qid, $arr[3]);
                if (isset($res[$tmp]))
                    $val .= ': '. $res[$tmp][2];
            } else {
                $val = $arr[2];
            }
        }
        $nam[$key] = $val;
    }
    return $nam;
}
/* }}} */

/* {{{ proto bool response_send_email(int survey_id, int response_id)
   Retrieves the response from the database and sends an email with the
   values.  Returns true if mail sent.
 */
function response_send_email($sid, $rid) {
	if (!$GLOBALS['ESPCONFIG']['allow_email']) {
		return true;
	}

	$sql = "SELECT name, email FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
	list($name, $email) = mysql_fetch_row($result);
	mysql_free_result($result);

	if(empty($email))
		return(false);

	//lets check to see if user wants human readable email
	if ($GLOBALS['ESPCONFIG']['human_email']) {
		$answers = response_select_human($sid, $rid);
        $qsep = "\n\t";
        $isep = "\n\t";
        $end = "\n\n";
	} else {
		$answers = response_select_compact($sid, $rid);
        $qsep = ' : ';
        $isep = ',';
        $end = "\n";
    }

    $user = array(
            'survey.id' => $sid,
            'survey.name' => $name,
            'survey.response' => $rid,
        );
	if (isset($GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER'])) {
	$userid = $GLOBALS['HTTP_SERVER_VARS']['PHP_AUTH_USER'];
        $sql = "SELECT * FROM respondent WHERE username = '" . addslashes($userid) . "'";
        $result = mysql_query($sql);
        if ($result && mysql_num_rows($result)) {
            $tmp = mysql_fetch_array($result, MYSQL_ASSOC);
            mysql_free_result($result);
            reset($tmp);
            $skip = array(
                    'password',
                    'auth',
                    'disabled',
                    'changed',
                    'expiration',
                );
            while (list($k, $v) = each($tmp)) {
                if (!in_array($k, $skip) && isset($v) && !empty($v))
                    $user["user.$k"] = $v;
            }
            unset($tmp);
        }
    } else {
		$user['user.username'] = $GLOBALS['HTTP_POST_VARS']['userid'];
    }

	$subject = _('Response from survey:') ." $name [$rid]";
    $body = '';
    reset($user);
    while (list($k, $v) = each($user))
    	$body .= $k . $qsep . $v . $end;

	$headers = "From: \"phpESP ".
		addslashes($GLOBALS['ESPCONFIG']['version']) .
		"\" <phpesp@". $GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] .">\n";
	$headers .= "X-Sender: <phpesp@". $GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] .">\n";
	$headers .= "X-Mailer: phpESP\n";
	$headers .= "Return-Path: <". $GLOBALS['HTTP_SERVER_VARS']['SERVER_ADMIN'] ."@".
		$GLOBALS['HTTP_SERVER_VARS']['SERVER_NAME'] . ">\n";

    reset($answers);
	while($arr = array_shift($answers)) {
        unset($x);
        if (count($arr) > 2)
            list($k, $v, $x) = $arr;
        else
            list($k, $v) = $arr;
        if (is_array($v))
            $v = implode($isep, $v);
        if (isset($x)) {
            if (is_array($x))
                $v .= ' (' . implode($isep, $x) . ')';
            else
                $v .= ' = ' . $x;
		}
    	$body .= $k . $qsep . $v . $end;
	}

	return(mail($email,$subject,$body,$headers));
}
/* }}} */

/* {{{ proto int response_select_max_pos(int survey_id, int response_id)
   Returns the position of the last answered question in a response.
 */
function response_select_max_pos($sid, $rid) {
	$max = 0;
	
	foreach (array('bool', 'single', 'multiple', 'rank', 'text', 'other', 'date') as $tbl) {
		$sql = "SELECT MAX(q.position) FROM response_$tbl a, question q
				WHERE a.response_id = '$rid' AND
					q.id = a.question_id AND
					q.survey_id = '$sid' AND
                    q.deleted = 'N'";
		$res = mysql_query($sql);
		if (($num = mysql_result($res, 0, 0)) > $max)
			$max = $num;
		mysql_free_result($res);
	}
        return $max;
}
/* }}} */

/* {{{ proto int response_select_max_pos(int survey_id, int response_id)
   Returns the number of the section in which questions have been
   answered in a response.
 */
function response_select_max_sec($sid, $rid) {
	$pos = response_select_max_pos($sid, $rid);
	$sql = "SELECT COUNT(*)+1 FROM question q
			WHERE q.survey_id = '$sid' AND
				q.type_id = 99 AND
				q.position < '$pos' AND
                q.deleted = 'N'";
	$res = mysql_query($sql);
	$max = mysql_result($res, 0, 0);
	mysql_free_result($res);
	return $max;
}
/* }}} */

/* {{{ proto void response_import_sec(int survey_id, int response_id, int section, &array destination)
   Populates the destination array with the answers from a given
   section of a given response.
 */
function response_import_sec($sid, $rid, $sec, $varr = null) {
    if ($varr == null)
        $varr =& $GLOBALS['HTTP_POST_VARS'];
    
    $ids = survey_get_sections($sid);
    if ($sec < 1 || !isset($ids[$sec - 1]))
        return;
    $vals = response_select($sid, $rid, 'content', $ids[$sec - 1]);

    reset($vals);
    foreach ($vals as $id => $arr) {
        if (isset($arr[0]) && is_array($arr[0])) {
            // multiple
            $varr[$id] = array_map('array_pop', $arr);
        } else {
            $varr[$id] = array_pop($arr);
        }
    }    
}
/* }}} */

?>
