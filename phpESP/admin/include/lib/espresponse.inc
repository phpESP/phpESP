<?php

    /* $Id$ */

    /* vim: set tabstop=4 shiftwidth=4 expandtab: */

    // Written by James Flemer
    // <jflemer@alum.rpi.edu>
    /* {{{ proto string response_check_answers(int survey_id, int section)
    Reads current form variables from HTTP_POST_VARS.
    Returns an empty string if all required fields are
    completed, else returns a message string indicating which
    questions need to be completed. It also checks numerical fields for only
    numerical values */
    function response_check_answers($sid, $rid, $section) {
        //angek: may need to keep for backwards compatability
        //global $_POST;
        $sql = "SELECT id,type_id,content,required,ans_uniq FROM ".$GLOBALS['ESPCONFIG']['question_table']."
        ". survey_select_section_sql($sid,$section) ." AND
        deleted='N' AND type_id < 98
        ORDER BY position";

        $result = execute_sql($sql);
        if(record_count($result) < 1) {
            // no rows returned, so no need to continue
            db_close($result);
            return('');
        }

        $missing = array();	// array of missing questions
	$not_numeric = array(); // array of numerical questions that are non-numeric
	$unique_answers = array(); // array of questions that must have a unique answer (only for Textbox or numerical types)

        // we loop through all the questions and check for
        // missing and non-numerical issues, but only if the question
        // was shown in the first place (it might have had a condition on it)
        while(list($qid,$tid,$content,$required,$ans_uniq) = fetch_row($result)) {

            // if the question has no condition:
            //      $question_was_shown=true
            // if the question has a condition:
            //      $question_was_shown=true if condition is fulfilled
            //      false otherwise
            $question_was_shown=question_conditioncheck($sid,$qid,$rid);

            if ($question_was_shown) {
		if ($ans_uniq=='Y' && ($tid==2 || $tid==3 || $tid==10)) {
		    // if a answer to a question must be unique, only for
		    // text boxes, essay or numerical questions
		    $tmp_response=_addslashes($_POST[$qid]);
                    $sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['response_text_table']." WHERE question_id=$qid && response="._addslashes($_POST[$qid]);
		    $count_result = get_one($sql);
		    if ($count_result) {
			$unique_answers[$qid] = $content;
		    }
		}
                if($required=='Y' && $tid == 8) { // Rank
                    $sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=$qid";
                    $cid_result = execute_sql($sql);
                    while(list($cid) = fetch_row($cid_result)) {
                        if(!isset($_POST["${qid}_${cid}"])) {
                            $missing[$qid] = $content;
                            break;
                        }
                        $cid_result->MoveNext();
                    }
                    db_close($cid_result);
                }
                if ($required=='Y' && $tid == 10 && $_POST[$qid] === '0') { // Numeric
		    # continue to the next entry, do movenext() otherwise we
		    # stay here until eternity
            	    $result->MoveNext();
                    continue;
                } elseif ($tid == 10 && !empty($_POST[$qid]) && !is_numeric($_POST[$qid])) {
                    $not_numeric[$qid] = $content;
                }
                if (!isset($_POST[$qid])) {
                    $_POST[$qid] = '';
                }
                if($required=='Y' && $tid!=8 && empty($_POST[$qid]) && $_POST[$qid] != '0') {
                    $missing[$qid] = $content;
                }
            }
            $result->MoveNext();
        }
        db_close($result);

        $message = '';
        if(count($missing) > 0) {
            // missing required variables
            $message .= _('You are missing the following required questions:') ."<br>\n";
            while(list($qid,$content)=each($missing)) {
                if($GLOBALS['ESPCONFIG']['DEBUG'])
                   $message .= "<!-- ${qid} -->";
                $message .= "==> ${content}<br>\n";
            }
        }
        if(count($not_numeric) > 0) {
            // missing required variables
            $message .= _('Please only use numbers for the following questions:') ."<br>\n";
            while(list($qid,$content)=each($not_numeric)) {
                if($GLOBALS['ESPCONFIG']['DEBUG'])
                   $message .= "<!-- ${qid} -->";
                $message .= "==> ${content}<br>\n";
            }
        }
        if(count($unique_answers) > 0) {
            // double responses where unique one is needed
            $message .= _('Please use a different answer for the following questions:') ."<br>\n";
            while(list($qid,$content)=each($unique_answers)) {
                if($GLOBALS['ESPCONFIG']['DEBUG'])
                   $message .= "<!-- ${qid} -->";
                $message .= "==> ${content}<br>\n";
            }
        }

        return($message);
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
            $sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['response_'.$tbl.'_table']." WHERE response_id = $rid $qids";
            $res = execute_sql($sql);
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
            $sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['response_'.$tbl.'_table']." WHERE question_id $qids";
            $res = execute_sql($sql);
        }
        /* ensure responses from testing status are also deleted */
        $sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['response_table']." WHERE survey_id=${sid}";
        execute_sql($sql);
    }
    /* }}} */

    /* {{{ proto int response_insert(int survey_id, int section, int response_id)
    Reads current form variables from HTTP_POST_VARS.
    Returns the ID for the response. */
    function response_insert($sid,$section,$rid) {
        $sql = "SELECT name, email, public FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
        $result = execute_sql($sql);
        list($name, $email, $survey_public) = fetch_row($result);

        if ($survey_public=="N") {
            if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic') {
                $userid = isset($_SERVER['PHP_AUTH_USER']) ?  $_SERVER['PHP_AUTH_USER'] : '';
            }
            elseif ($GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
                if (isset($_SESSION['espuser'])) {
                    $userid = $_SESSION['espuser'];
                }
            }
        } else {
            $userid = "anonymous";
        }
        $ip="";
        if (isset($_SERVER['REMOTE_ADDR'])) $ip=$_SERVER['REMOTE_ADDR'];
        if (isset($_SERVER['HTTP_CLIENT_IP'])) $ip="$ip (".$_SERVER['HTTP_CLIENT_IP'].")";
        elseif (isset($_SERVER['HTTP_X_FORWARDED_FOR'])) $ip="$ip (".$_SERVER['HTTP_X_FORWARDED_FOR'].")";

        $userid = _addslashes($userid);

        if(empty($rid)) {
            // create a uniqe id for this response
            $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['response_table']." (survey_id,submitted,username,ip) VALUES ( ${sid},".sys_time_stamp().",${userid},'${ip}')";
            $result = execute_sql($sql);
    	    if (!$result) {
		$msg = '<font color="red">'. _('Request failed, please contact the administrator.') .'</font>';
		if ($GLOBALS['ESPCONFIG']['DEBUG'])
		   $msg .= mkerror(ErrorNo() . ': ' . ErrorMsg());
		print $msg;
		exit;
	    }

            $rid = insert_id("response_id_seq");
        }
        $sql = "
        SELECT Q.id, Q.type_id, Q.length, Q.precise, T.response_table
        FROM ".$GLOBALS['ESPCONFIG']['question_table']." Q, ".$GLOBALS['ESPCONFIG']['question_type_table']." T
        ". survey_select_section_sql($sid,$section,'Q') ." AND
        Q.survey_id=${sid} AND
        Q.deleted='N' AND Q.type_id < 50 AND
        Q.type_id=T.id";
        $q_result = execute_sql($sql);
        while(list($qid, $tid, $length, $prec, $table) = fetch_row($q_result)) {
            $q_result->MoveNext();
            if (isset($_POST[$qid])) {
                if (is_array($_POST[$qid]))
                $bareval = array_map('_stripslashes', $_POST[$qid]);
                else
                $bareval = _stripslashes($_POST[$qid]);
            }
            else
            $bareval = '';

            if (is_string($bareval))
            $val = _addslashes($bareval);
            else
            $val = '';

	    // don't allow >1 responses for the same response ID and the same question ID
	    $sql  = "SELECT COUNT(*) from ".$GLOBALS['ESPCONFIG'][$table.'_table']." WHERE response_id=${rid} AND question_id=${qid}";
	    $count = get_one($sql);
            if ($count > 0) {
		return $rid;
	    }


            switch($table) {
                case 'response_bool':
                    $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,choice_id ) VALUES ( ${rid},${qid},${val} )";
                    if (!empty($val))
                    $result = execute_sql($sql);
                    break;
                case 'response_text':
                    // only insert if non-empty content
                    if($tid == 10) { // numeric
			#$bareval = ereg_replace("[^0-9.\-]*(-?[0-9]*\.?[0-9]*).*", '\1', $bareval);
			# replace "," by "."
			if (ereg (",",$bareval)) {
				$bareval = ereg_replace(",",".",$bareval);
			}
			# if float, check length and precision
			if (ereg ("\.",$bareval)) {
				$bareval = ereg_replace("[^0-9.\-]*(-?[0-9]{1,$length}\.[0-9]{1,$prec}).*", '\1', $bareval);
			} else {
				# else check only length
				$bareval = ereg_replace("[^0-9.\-]*(-?[0-9]{1,$length}).*", '\1', $bareval);
			}
                    }
                    if(ereg("[^ \t\n]",$bareval)) {
                        $val = _addslashes($bareval);
                        $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,response ) VALUES ( ${rid},${qid},${val} )";
                        $result = execute_sql($sql);
                    }
                    break;
                case 'response_date':
                    // only insert if non-empty content
                    //if(($bareval = ereg_replace("[^0-9]*([0-9]+)/([0-9]+)/([0-9]+).*", '\3-\1-\2', $bareval))) {
                    if(function_exists("strptime")) {
			if ($bareval=strptime($bareval,$GLOBALS['ESPCONFIG']['date_format'])) {
                           $bareval['tm_year']+=1900;
                           $bareval['tm_mon']+=1;
                           $val = $bareval['tm_year']."-".
                               $bareval['tm_mon']."-".
                               $bareval['tm_mday'];
                           $val = _addslashes($val);
                           $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,response ) VALUES ( ${rid},${qid},${val} )";
                           $result = execute_sql($sql);
			}
		    } elseif ($val = ereg_replace("[^0-9]*([0-9]+)/([0-9]+)/([0-9]+).*", '\3-\2-\1', $bareval)) {
                        $val = _addslashes($val);
                        $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,response ) VALUES ( ${rid},${qid},${val} )";
                        $result = execute_sql($sql);
                    }
                    break;
                case 'response_single':
                    if(empty($bareval)) {
                        $sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=${qid} AND content LIKE '!other%' ORDER BY id";
                        $c_result = execute_sql($sql);
                        while(!$c_result) {
                            $c_result->MoveNext();
                            list($cid) = fetch_row($c_result);
                            if (!isset($_POST["${qid}_${cid}"])) {
                                continue;
                            }
                            $other = _addslashes($_POST["${qid}_${cid}"]);
                            if(ereg("[^ \t\n]",$other)) {
                                $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['response_other_table']." ( response_id,question_id,choice_id,response ) VALUES ( ${rid},${qid},${cid},${other} )";
                                $result = execute_sql($sql);
                                $val = $cid;
                                break;
                            }
                        }
                        db_close($c_result);
                    }
                    if(ereg("other_([0-9]+)", $bareval, $regs)) {
                        $cid=$regs[1];
                        if (!isset($_POST["${qid}_${cid}"]))
                        break; // out of the case
                        $other = _addslashes($_POST["${qid}_${cid}"]);
                        if(ereg("[^ \t\n]",$other)) {
                            $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['response_other_table']." ( response_id,question_id,choice_id,response ) VALUES ( ${rid},${qid},${cid},${other} )";
                            $result = execute_sql($sql);
                            $val=$cid;
                        }
                    }
                    $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,choice_id ) VALUES ( ${rid},${qid},${val} )";
                    $result = execute_sql($sql);
                    break;
                case 'response_multiple':
                    $sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=${qid} AND content LIKE '!other%'";
                    $c_result = execute_sql($sql);
                    while(!$c_result->EOF) {
                        list($cid) = fetch_row($c_result);
                        $c_result->MoveNext();
                        if (!isset($_POST["${qid}_${cid}"]) || empty($_POST["${qid}_${cid}"])) {
                            continue;
                        }
                        if (!isset($_POST[$qid]))
                        $_POST[$qid] = array($cid);
                        else
                        array_push($_POST[$qid], $cid);
                        $other = _addslashes($_POST["${qid}_${cid}"]);
                        if(ereg("[^ \t\n]",$other)) {
                            $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['response_other_table']." ( response_id,question_id,choice_id,response ) VALUES ( ${rid},${qid},${cid},${other} )";
                            $result = execute_sql($sql);
                        }
                    }
                    db_close($c_result);
                    #if(!isset($_POST[$qid]) || count($_POST[$qid]) < 1)
                    if(!isset($_POST[$qid]) || !is_array($_POST[$qid]))
			break;
                    foreach($_POST[$qid] as $cid) {
                        $cid = _addslashes($cid);
                        if(ereg("other_[0-9]+", $cid))
                        continue;
                        $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,choice_id ) VALUES ( ${rid},${qid},${cid} )";
                        $result = execute_sql($sql);
                    }
                    break;
                case 'response_rank':
                    if($tid == 8) { // Rank
                    $sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=${qid}";
                    $cid_result = execute_sql($sql);
                    while(!$cid_result->EOF) {
                        list($cid) = fetch_row($cid_result);
                        $cid_result->MoveNext();
                        if (!isset($_POST["${qid}_${cid}"]))
                        continue;
                        $val = $_POST["${qid}_${cid}"];
                        if(strtolower($val) == "n/a")
                        $rank = -1;
                        else
                        $rank = intval($val);
                        $rank = _addslashes($rank);
                        $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,choice_id,rank ) ";
                        $sql .= "VALUES ( ${rid},${qid},${cid},${rank} )";
                        $result=execute_sql($sql);
                    }
                    db_close($cid_result);
                    break;
                    }
                    if(strtolower($bareval) == "n/a")
                    $rank = -1;
                    else
                    $rank = intval($bareval);
                    $sql  = "INSERT INTO ".$GLOBALS['ESPCONFIG'][$table.'_table']." ( response_id,question_id,rank ) VALUES ( ${rid},${qid},${rank} )";
                    $result = execute_sql($sql);
                    break;
            }
        }
        db_close($q_result);
        return($rid);
    }
    /* }}} */

    /* {{{ proto bool response_commit(int response_id)
    Marks a response as completed.  Returns true on success.
    */
    function response_commit($rid) {
	$ip="";
	if (isset($_SERVER['REMOTE_ADDR'])) $ip=$_SERVER['REMOTE_ADDR'];
	if (isset($_SERVER['HTTP_CLIENT_IP'])) $ip="$ip (".$_SERVER['HTTP_CLIENT_IP'].")";
	elseif (isset($_SERVER['HTTP_X_FORWARDED_FOR'])) $ip="$ip (".$_SERVER['HTTP_X_FORWARDED_FOR'].")";

        $sql = "UPDATE ".$GLOBALS['ESPCONFIG']['response_table']." SET complete='Y', submitted =".sys_time_stamp().",ip='$ip' WHERE id=${rid}";
        if(execute_sql($sql))
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
    the first elements.  The last three elements are the response content,
    the content ID, the feedback (if appropriate), and credit (if
    appropriate) respectively.  (For multiple answer questions, the value is an
    array of these arrays.)

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
        FROM ".$GLOBALS['ESPCONFIG']['response_bool_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q
        WHERE a.response_id=${rid} AND a.question_id=q.id $qids";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            $val = array_pop($row);
            $values[$qid] = $row;

            $ivalue = '';
	        if ($val == 'Y') {
		       $ivalue = _('Yes');
	        } elseif ($val == 'N') {
		       $ivalue =  _('No');
	        }
	        array_push($values["$qid"], $ivalue, $val);
            $result->MoveNext();
        }
        db_close($result);

        // --------------------- response_single ---------------------
        $sql = "SELECT q.id $col,c.content,c.id,c.feedback,c.credit
        FROM ".$GLOBALS['ESPCONFIG']['response_single_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c
        WHERE a.response_id=${rid} AND a.question_id=q.id AND a.choice_id=c.id $qids";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            $c = count($row);
            $val = $row[$c - 4];
            if (ereg('^!other', $val))
               $row[$c - 3] = 'other_' . $row[$c - 3];
            else
               settype($row[$c - 3], 'integer');
            $values[$qid] = $row;
            $result->MoveNext();
        }
        db_close($result);

        // --------------------- response_multiple ---------------------
        $sql = "SELECT q.id $col,c.content,c.id,c.feedback,c.credit
        FROM ".$GLOBALS['ESPCONFIG']['response_multiple_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c
        WHERE a.response_id=${rid} AND a.question_id=q.id AND a.choice_id=c.id $qids
        ORDER BY a.question_id,c.id";
        $result = execute_sql($sql);
        $arr = array();
        $tmp = null;
        while(!$result->EOF) {
            $row = fetch_row($result);

            $qid = array_shift($row);
            $c = count($row);
            $val = $row[$c - 4];
            if (ereg('^!other', $val))
               $row[$c - 3] = 'other_' . $row[$c - 3];
            else
               settype($row[$c - 3], 'integer');
            if($tmp == $qid) {
                $arr[] = $row;
                $result->MoveNext();
                continue;
            }
            if($tmp != null)
            $values["$tmp"]=$arr;
            $tmp = $qid;
            $arr = array($row);
            $result->MoveNext();
        }
        if($tmp != null)
        $values["$tmp"]=$arr;
        db_close($result);
        unset($arr);
        unset($tmp);
        unset($row);

        // --------------------- response_other ---------------------
        $sql = "SELECT q.id,c.id $col,a.response
        FROM ".$GLOBALS['ESPCONFIG']['response_other_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c
        WHERE a.response_id=${rid} AND a.question_id=q.id AND a.choice_id=c.id $qids
        ORDER BY a.question_id,c.id";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            $cid = array_shift($row);
            array_push($row, $row[count($row) - 1]);
            $values["${qid}_${cid}"] = $row;
            $result->MoveNext();
        }
        db_close($result);

        // --------------------- response_rank ---------------------
        $sql = "SELECT (". $GLOBALS['ESPCONFIG']['adodb_conn']->Concat("q.id","'_'","c.id") .") AS id $col,c.content,a.rank
        FROM ".$GLOBALS['ESPCONFIG']['response_rank_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c
        WHERE a.response_id=${rid} AND a.question_id=q.id AND a.choice_id=c.id $qids
        ORDER BY a.question_id,c.id";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            settype($row[count($row) - 1], 'integer');
            if (isset($row[3])) {
                if($row[3]>=0) {
                    $row[3]++;
                }
            }
            $values[$qid] = $row;
            $result->MoveNext();
        }
        db_close($result);

        // --------------------- response_text ---------------------
        $sql = "SELECT q.id $col,a.response
        FROM ".$GLOBALS['ESPCONFIG']['response_text_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q
        WHERE a.response_id=${rid} AND a.question_id=q.id $qids";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            $values["$qid"]=$row;
            $val = array_pop($values["$qid"]);
            array_push($values["$qid"], $val, $val);
            $result->MoveNext();
        }
        db_close($result);

        // --------------------- response_date ---------------------
        $sql = "SELECT q.id $col,a.response
        FROM ".$GLOBALS['ESPCONFIG']['response_date_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q
        WHERE a.response_id=${rid} AND a.question_id=q.id $qids";
        $result = execute_sql($sql);
        while(!$result->EOF) {
            $row = fetch_row($result);
            $qid = array_shift($row);
            $values["$qid"]=$row;
            $val = array_pop($values["$qid"]);
            array_push($values["$qid"], $val, $val);
            $result->MoveNext();
        }
        db_close($result);

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
                if ($arr[3] < 0) {
                    $arr[3] = 'n/a';
                }
//                else {
//                    $arr[3]++;
//                }
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
        if (!empty($tmpk) || count($tmpv)) {
            array_push($hmn, array($tmpk, $tmpv));
            $tmpk = null;
            $tmpv = array();
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
                    $cpq[] = array($qid, $arr[2]+1, array($arr[1]));
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
            $index = $qid;
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
                $str1 = trim(strtoupper(eregi_replace(
                    "[^A-Z0-9]+", " ", $str1)));
                    $str1 = ereg_replace(' +','_',$str1);
                    $arr[1] = strtoupper($arr[1]);
                    $arr[1] .= "_$str1";
                    $nam[$index] = array($arr[1]=>$arr[3]);
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
            $nam[$index] = array($key=>$val);
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

        $sql = "SELECT name, email, public FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
        $result = execute_sql($sql);
        list($name, $email, $survey_public) = fetch_row($result);
        db_close($result);

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
        if ($survey_public=="N") {
            $userid = "";
            if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic' &&
		        isset($_SERVER['PHP_AUTH_USER'])) {
                $userid = $_SERVER['PHP_AUTH_USER'];
            } elseif ($GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
                $userid = $_SESSION['espuser'];
            }

	        $user['user.username'] = $userid;
            $sql = "SELECT fname,lname,email FROM ".$GLOBALS['ESPCONFIG']['respondent_table']." WHERE username = " . _addslashes($userid);
            $result = execute_sql($sql);
            if ($result && record_count($result)) {
		        list ($user_fname, $user_lname, $user_email) = fetch_row($result);
                db_close($result);
                if (!empty($user_fname)) $user["user.firstname"] = $user_fname;
                if (!empty($user_lname)) $user["user.lastname"] = $user_lname;
                if (!empty($user_email)) $user["user.email"] = $user_email;
            }
        } else {
            $user['user.username'] = "anonymous";
        }

        $subject = _('Response from survey:') ." $name [$rid]";
        $body = '';
        reset($user);
        while (list($k, $v) = each($user))
        $body .= $k . $qsep . $v . $end;

        if (isset($GLOBALS['ESPCONFIG']['email_from_name']) &&
            isset($GLOBALS['ESPCONFIG']['email_from_address']) && 
            isset($GLOBALS['ESPCONFIG']['email_return_path'])) {
            $headers = "From: ".addslashes($GLOBALS['ESPCONFIG']['email_from_name']).
                        "<".addslashes($GLOBALS['ESPCONFIG']['email_from_address']).">\n";
            $headers .= "Return-Path: <". addslashes($GLOBALS['ESPCONFIG']['email_return_path']).">\n";
        } else {
           $headers = "From: \"phpESP ".
           addslashes($GLOBALS['ESPCONFIG']['version']) .
           "\" <phpesp@". $_SERVER['SERVER_NAME'] .">\n";
           $headers .= "Return-Path: <". $_SERVER['SERVER_ADMIN'] ."@".
                       $_SERVER['SERVER_NAME'] . ">\n";
           $headers .= "X-Sender: <phpesp@". $_SERVER['SERVER_NAME'] .">\n";
        }
        $headers .= "X-Mailer: phpESP\n";

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

        // support for multiple emails separated by ;
        if(strpos($email, ';') !== false) {
                $retVal = 1;
                $emailArray = split(";", $email);
                foreach($emailArray as $email_address) {
                        if (!mail($email_address,$subject,$body,$headers)) {
                            $retVal = 0;
                        }
                }
                return $retVal;
        } else {
                return(mail($email,$subject,$body,$headers));
        }
    }
    /* }}} */

    /* {{{ proto int response_select_max_pos(int survey_id, int response_id)
    Returns the position of the last answered question in a response.
    */
    function response_select_max_pos($sid, $rid) {
        $max = 0;

        foreach (array('bool', 'single', 'multiple', 'rank', 'text', 'other', 'date') as $tbl) {
            $sql = "SELECT MAX(q.position) FROM ".$GLOBALS['ESPCONFIG']['response_'.$tbl.'_table']." a, ".$GLOBALS['ESPCONFIG']['question_table']." q
            WHERE a.response_id = $rid AND
            q.id = a.question_id AND
            q.survey_id = $sid AND
            q.deleted = 'N'";
            #$res = execute_sql($sql);
            #if ($num = get_one($res) > $max)
            $num = get_one($sql);
            if ($num > $max)
               $max = $num;
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
        $sql = "SELECT COUNT(*)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." q
        WHERE q.survey_id = $sid AND
        q.type_id = 99 AND
        q.position < $pos AND
        q.deleted = 'N'";
        #$res = execute_sql($sql);
        #$max = get_one($res);
        $max = get_one($sql);
        return $max;
    }
    /* }}} */

    /* {{{ proto void response_import_sec(int survey_id, int response_id, int section, &array destination)
    Populates the destination array with the answers from a given
    section of a given response.
    */
    function response_import_sec($sid, $rid, $sec, $varr = null) {
        if ($varr == null)
        $varr =& $_POST;

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
