<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if(empty($GLOBALS['ESPCONFIG']['DEBUG_RESULTS']))
	$GLOBALS['ESPCONFIG']['DEBUG_RESULTS'] = $GLOBALS['ESPCONFIG']['DEBUG'];

/* {{{ proto void survey_results_navbar(int survey_id, int response_id)
   Builds HTML for a response navigation bar to move between individual
   responses.
 */
function survey_results_navbar($sid, $curr_rid) {
	$sql = "SELECT R.id
		FROM ".$GLOBALS['ESPCONFIG']['response_table']." R
		WHERE R.survey_id='${sid}' AND
		R.complete = 'Y'
		ORDER BY R.id";
	$result = execute_sql($sql);
	$total = record_count($result);
	if ($total < 1)
		return;

	$rids = array();
	$i = 0;
	$curr_pos = -1;
	while ($row = fetch_row($result)) {
        $result->MoveNext();
		array_push($rids, $row[0]);
		if ($row[0] == $curr_rid)
			$curr_pos = $i;
		$i++;
	}
	db_close($result);

	$prev_rid = ($curr_pos > 0) ? $rids[$curr_pos - 1] : null;
	$next_rid = ($curr_pos < $total - 1) ? $rids[$curr_pos + 1] : null;
	$rows_per_page = 1;
	$pages = ceil($total / $rows_per_page);

	$url = $GLOBALS['ESPCONFIG']['ME'] . "?where=results&sid=$sid";

	$mlink = create_function('$i,$r', 'return "<a href=\"'.$url.'&rid=$r\">$i</a>";');

	$linkarr = array();

	$display_pos = 1;
	if ($prev_rid != null)
		array_push($linkarr, "<a href=\"$url&rid=$prev_rid\">"._('Previous').'</a>');
	for ($i = 0; $i < $curr_pos; $i++) {
		array_push($linkarr, "<a href=\"$url&rid=".$rids[$i]."\">$display_pos</a>");
		$display_pos++;
	}
	array_push($linkarr, "<a href=\"$url&rid=$curr_rid\"><b>$display_pos</b></a>");
	for (++$i; $i < $total; $i++) {
		$display_pos++;
		array_push($linkarr, "<a href=\"$url&rid=".$rids[$i]."\">$display_pos</a>");
	}
	if ($next_rid != null)
		array_push($linkarr, "<a href=\"$url&rid=$next_rid\">"._('Next').'</a>');

	?>
		<center>
		<h2><?php echo _('Navigate Individual Respondent Submissions'); ?></h2>
		<br />
		<?php echo implode(' | ', $linkarr); ?>
		<br />
		</center>
		<?php
}
/* }}} */

/* {{{ proto string survey_results(int survey_id, int precision, bool show_totals, int question_id, array choice_ids, int response_id)
	Builds HTML for the results for the survey. If a
	question id and choice id(s) are given, then the results
	are only calculated for respodants who chose from the
	choice ids for the given question id.
	Returns empty string on sucess, else returns an error
	string. */
function survey_results($sid, $precision = 1, $showTotals = 1, $qid = '', $cids = '', $rid = '', $guicross='') {
	$bg = '';
	if(empty($precision))	$precision  = 1;
	if($showTotals === '')	$showTotals = 1;
		
    if(is_int($cids)) {
        $cids = array($cids);
    }
    if(is_string($cids)) {
        $cids = split(" ",$cids); // turn space seperated list into array
    }
	// set up things differently for cross analysis
	$cross = !empty($qid);
	if($cross) {
		if(is_array($cids) && count($cids)>0) {
			$cidstr = array_to_insql($cids);
		} else {
			$cidstr = '';
		}
	}

	// build associative array holding whether each question
	// type has answer choices or not and the table the answers are in
	$has_choices = array();
	$response_table = array();
	$sql = 'SELECT id,has_choices,response_table
			  FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].'
			 ORDER BY id';
	if(!($result = execute_sql($sql))) {
        $errmsg = sprintf('%s [ %s: question_type ]',
                _('Error system table corrupt.'), _('Table'));
		return($errmsg);
	}
	while($row = fetch_row($result)) {
        $result->MoveNext();
		$has_choices[$row[0]]=$row[1];
		$response_table[$row[0]]=$row[2];
	}
	db_close($result);

	// load survey title (and other globals)
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id='${sid}'";
	if(!($result = execute_sql($sql,"",ADODB_FETCH_ASSOC))) {
		$errmsg = _('Error opening survey.') ." [ ID:${sid} R:" . record_count($result) ."]";
		return($errmsg);
	}
	$survey = fetch_row($result);
	db_close($result);

	// load survey questions
	$sql = "SELECT *
			  FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			 WHERE survey_id='${sid}' AND
				   deleted='N'
			 ORDER BY position,id";
	if(!($questions_result = execute_sql($sql,"",ADODB_FETCH_ASSOC))) {
		$errmsg = _('Error opening survey.') .' '. _('No questions found.') ." [ ID:${sid} ]";
		return($errmsg);
	}

	// find out more about the question we are cross analyzing on (if any)
	if($cross) {
        $sql = "SELECT type_id FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id=${qid}";
		$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
        while($content = fetch_row($result)){
            $result->MoveNext();
            $crossTable = $response_table[$content['type_id']];
            $q_content = $content['type_id'];
        }
		db_close($result);
		if(!in_array($crossTable, array('response_single','response_bool','response_multiple'))) {
			$errmsg = _('Error cross-analyzing. Question not valid type.') .' [ '. _('Table') .": ${crossTable} ]";
			return($errmsg);
		}
	}

// find total number of survey responses
// and relevant response ID's
	if (!empty($rid)) {
		$ridstr = "= '${rid}'";
        $total = 1;
	}
    else {
        $sql = "";
        if($cross) {
            if(!empty($cidstr)) {
                $sql = "SELECT A.response_id
					      FROM ".$GLOBALS['ESPCONFIG'][$crossTable.'_table']." A,
						       ".$GLOBALS['ESPCONFIG']['response_table']." R
					     WHERE A.response_id=R.id AND
						       R.complete='Y' AND
						       A.question_id='${qid}' AND
				    	       A.choice_id ${cidstr}
					     ORDER BY A.response_id";
            }
            else {
                $sql = "SELECT A.response_id
					      FROM ".$GLOBALS['ESPCONFIG'][$crossTable.'_table']." A,
						       ".$GLOBALS['ESPCONFIG']['response_table']." R
					     WHERE A.response_id=R.id AND
						       R.complete='Y' AND
						       A.question_id='${qid}' AND
				    	       A.choice_id = 0
					     ORDER BY A.response_id";
            }
        }
        else {
		    $sql = "SELECT R.id
				      FROM ".$GLOBALS['ESPCONFIG']['response_table']." R
				     WHERE R.survey_id='${sid}' AND
					       R.complete='Y'
				     ORDER BY R.id";
	    }
	    if(!($result = execute_sql($sql))) {
		    $errmsg = _('Error opening survey.') ." [ ID:${sid} ] [ ".ErrorMsg()."]";
		    return($errmsg);
	    }
	    $total = record_count($result);
	    if($total < 1) {
		    $errmsg = _('Error opening survey.') .' '. _('No responses found.') ." [ ID:${sid} ]";
			    return($errmsg);
            if ($GLOBALS['ESPCONFIG']['DEBUG'])
                echo("<!-- \$errmsg = '$errmsg' -->\n");
	    }

		$rids = array();
		while($row = fetch_row($result)) {
			$result->MoveNext();
			array_push($rids, $row[0]);
		}
		db_close($result);
		// create a string suitable for inclusion in a SQL statement
		// containing the desired Response IDs
		// ex: "('304','311','317','345','365','370','403','439')"
		$ridstr = array_to_insql($rids);
	}
    
    if (!empty($rid)) {
        // show response navigation bar
        survey_results_navbar($sid, $rid);
    }

?>
<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<?php
	if($cross) {
		echo("<blockquote>" ._('Cross analysis on QN:') ." ${q_content}</blockquote>\n");
	}
?>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php
	$i=0; // question number counter
	while($question = fetch_row($questions_result)) {
        $questions_result->MoveNext();
		// process each question
		$qid = $question['id'];
		$tid = $question['type_id'];
		$table = $response_table[$tid];
		$totals = $showTotals;

		if ($tid == 99) {
			echo("<tr><td><hr></td></tr>\n");
			continue;
		}
		if ($tid == 100) {
			echo("<tr><td>". $question['content'] ."</td></tr>\n");
			continue;
		}

		if($bg != '#eeeeee')	$bg = '#eeeeee';
		else                	$bg = '#ffffff';

?>
		<tr xbgcolor="<?php echo($bg); ?>">  
 		<td>
<?php
		if ($tid < 50) {
			if (!empty($guicross)){
				echo ('<input type="hidden" name="where" value="results" />');
				echo ('<input type="hidden" name="sid" value="'.$sid.'" />');
				echo ("\n<table width=\"90%\" border=\"0\">\n");
				echo ("<tbody>\n");
				echo ("   <tr>\n");
				echo ("      <td width=\"34\" height=\"31\" bgcolor=\"khaki\">\n");
				if ($tid==1 || $tid==4 || $tid==5 || $tid==6){
					echo ("<div align=\"center\">\n");
					echo ("   <input type=\"radio\" name=\"qid\" value=\"".$qid."\" />\n");
					echo ("</div>\n");
				}
				echo ("</td>\n");
				echo ("<td width=\"429\" bgcolor=\"#CCCCCC\">\n");
			} //end if empty($guicross)
			echo ("<A NAME=\"Q".++$i."\"><b>".$i.".</b></A>\n");
			echo ("<b>".$question['content']."</b>\n");
			if (!empty($guicross)){
				echo ("</td>\n");
				echo ("<td width=\"33\" bgcolor=\"#CC0000\">\n");
				if ($tid==1 || $tid==4 || $tid==5 || $tid==6){
					echo ("<div align=\"center\">\n");
					echo ("<input type=\"radio\" name=\"qidr\" value=\"".$qid."\" />\n");
					echo ("</div>\n");
				}
				echo ("</td>\n");
				echo ("<td width=\"32\" bgcolor=\"#0099FF\">\n");
				if ($tid==1 || $tid==4 || $tid==5 || $tid==6){
					echo ("<div align=\"center\">\n");
					echo ("<input type=\"radio\" name=\"qidc\" value=\"".$qid."\" />\n");
					echo ("</div>\n");
				}
				echo ("</td>\n");
				echo ("</tr>\n");
				echo ("</tbody>\n");
				echo ("</table>\n");
			} //end if empty($guicross)
		} //end if ($tid < 50)

$counts = array();
switch ($table) {
// -------------------------------- response_bool --------------------------------
		case 'response_bool':
			$counts = array(_('Yes') => 0, _('No') => 0);
			$sql = "SELECT A.choice_id, COUNT(A.response_id)
					  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE A.question_id='${qid}' AND
						   A.response_id ${ridstr} AND
						   A.choice_id != ''
					 GROUP BY A.choice_id";
			if ($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])
				echo("<!-- \$sql = '$sql' -->\n");
			if ($result = execute_sql($sql)) {
				while(list($ccontent,$count) = fetch_row($result)) {
					$result->MoveNext();
					$ccontent = htmlspecialchars($ccontent);
					if ($ccontent == 'Y')
						$ccontent = _('Yes');
					else
						$ccontent = _('No');
					$counts[$ccontent] = $count;
				}
				db_close($result);
			}

			if (empty($question["result_id"]))
				$question["result_id"] = 1;	// default to percentages for yes/no
			break;
// -------------------------------- response_single --------------------------------
// -------------------------------- response_multiple --------------------------------
		case 'response_multiple':
			$totals = 0;
		case 'response_single':
			$sql = "SELECT C.content FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." C
				WHERE C.question_id='${qid}' AND C.content NOT LIKE '!other%'
				ORDER BY C.id";
			if ($result = execute_sql($sql)) {
				for ($j = 0; $j < record_count($result); $j++) {
					$result->Move($j);
					$counts[htmlspecialchars($result->fields[0])] = 0;
				}
				db_close($result);
			}

			$sql = "SELECT C.content, COUNT(A.response_id) AS num
					  FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." C,
						   ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE C.question_id='${qid}' AND
						   C.content NOT LIKE '!other%' AND
						   A.question_id=C.question_id AND
						   A.choice_id=C.id AND
						   A.response_id ${ridstr}
					 GROUP BY C.id, C.content";
			if ($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])
				echo("<!-- \$sql = '$sql' -->\n");
			if ($result = execute_sql($sql)) {
				while (list($ccontent,$count) = fetch_row($result)) {
					$result->MoveNext();
					$ccontent = htmlspecialchars($ccontent);
					$counts[$ccontent] = $count;
				}
				db_close($result);
			}

			// handle 'other...'
			$sql = "SELECT A.response, C.content
					  FROM ".$GLOBALS['ESPCONFIG']['response_other_table']." A, ".$GLOBALS['ESPCONFIG']['question_choice_table']." C
					 WHERE A.question_id='${qid}' AND
						   A.choice_id=C.id AND
    					   A.response_id ${ridstr}
					 ORDER BY C.id, A.response";
			if ($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if ($result = execute_sql($sql)) {
				while(list($answer,$ccontent) = fetch_row($result)) {
					$result->MoveNext();
					$ccontent = htmlspecialchars($ccontent);
					$content = preg_replace(array('/^!other=/', '/^!other/'),
								array('', 'Other:'), $ccontent);
					$content .= ' ' . htmlspecialchars($answer);
					$counts[$content] = (isset($counts[$content]) ? $counts[$content] : 0)+1;
				}
				db_close($result);
			}
			if (empty($question["result_id"]))
				$question["result_id"] = 1;	// default to percentages
			break;
// -------------------------------- response_text --------------------------------
		case 'response_text':
			$sql = "SELECT A.response, COUNT(A.response_id) AS num
					  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE A.question_id='${qid}' AND
    					   A.response_id ${ridstr}
					 GROUP BY A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = execute_sql($sql)) {
				while(list($text, $num) = fetch_row($result)) {
					$result->MoveNext();
					if(!empty($text))
						$counts[htmlspecialchars($text)] = $num;
				}
				db_close($result);
			}
			$question["result_id"] = 4;	// force "list" type response for text fields
			break;
// -------------------------------- response_date --------------------------------
		case 'response_date':
			$sql = "SELECT DATE_FORMAT(A.response,'".$GLOBALS['ESPCONFIG']['date_format']."'), COUNT(A.response_id) AS num
					  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE A.question_id='${qid}' AND
    					   A.response_id ${ridstr}
					 GROUP BY A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = execute_sql($sql)) {
				while(list($text, $num) = fetch_row($result)) {
					$result->MoveNext();
					if(!empty($text))
						$counts[htmlspecialchars($text)] = $num;
				}
				db_close($result);
			}
			$question["result_id"] = 4;	// force "list" type response for date fields
			break;
// -------------------------------- response_rank --------------------------------
		case 'response_rank':
			if($tid == 8) { //Rank
                $sql = "SELECT C.content FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." C
                        WHERE C.question_id='${qid}' AND C.content NOT LIKE '!other%'
                        ORDER BY C.id";
                if ($result = execute_sql($sql)) {
                    for ($j = 0; $j < record_count($result); $j++) {
                        $result->Move($j);
                        $counts[htmlspecialchars($result->fields[0])] = null;
                    }
                    db_close($result);
                }
				$sql = "SELECT C.content, AVG(A.rank+1) AS average
						  FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." C, ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
						 WHERE C.question_id='${qid}' AND
    						   A.question_id='${qid}' AND
							   A.choice_id=C.id AND
							   A.rank>=0 AND
							   A.response_id ${ridstr}
						 GROUP BY C.id, C.content";
				if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
				if($result = execute_sql($sql)) {
					while(list($ccontent,$avg) = fetch_row($result)) {
						$result->MoveNext();
						$ccontent = htmlspecialchars($ccontent);
						$counts[$ccontent] = $avg;
					}
					db_close($result);
				}
				$question["result_id"] = 99;	// force to rank
			} else {
				$sql = "SELECT A.rank, COUNT(A.response_id) AS num
						  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
						 WHERE A.question_id='${qid}' AND
    						   A.response_id ${ridstr}
						 GROUP BY A.rank";
				if($result = execute_sql($sql)) {
					while(list($rank, $num) = fetch_row($result)) {
						$result->MoveNext();
						if($rank == -1) { $rank = "N/A"; }
						$counts[_($rank)] += $num;
					}
					db_close($result);
				}
				if(empty($question["result_id"]))
					$question["result_id"] = 2;	// default to rank
			}
			break;
		}
// ---------------------------------------------------------------------------
	if (!empty($guicross) && $question["result_id"] == 1){
		mkcrossformat($counts,$qid,$tid);
	} else {
		switch($question["result_id"]) {
			case "1":	// Percentages
				mkrespercent($counts,$total,$precision,$totals);
				break;
			case "2":	// Rank
				mkresrank($counts,$total,$precision,$totals);
				break;
			case "3":	// Count
				mkrescount($counts,$total,$precision,$totals);
				break;
			case "4":	// List
				mkreslist($counts,$total,$precision,$totals);
				break;
			case "99":	// Average
				mkresavg($counts,$total,$precision,$totals,$question['length']);
				break;
		} // end switch
	} //end if
?>
		</td>
	</tr>
<?php } // end while ?>
</table>
<?php
	db_close($questions_result);
	return;
}
/* }}} */

?>
