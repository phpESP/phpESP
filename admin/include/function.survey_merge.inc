<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_MERGE')) {
 	define('_FUNCTION_SURVEY_MERGE',TRUE);
	
	if(!defined('DEBUG'))
		define('DEBUG',FALSE);

	// builds HTML for the results for the survey 'sid'
	// with the added limitation that the answer to
	// $questionId is an element of the array $choiceIds
	
	// if $questionId is not specified (or less than 0)
	// the standard (original) results display is created
	
	// void crossanalysis(int $surveyId, 
	//                    int $precision,
	//                    bool $showTotals, 
	//                    int $questionId,
	//                    int[] $choiceIds);
	
	function survey_merge($sids, $precision = 1, $showTotals = 1, $qid = '', $cids = '') {
		if(!is_array($sids) || count($sids) < 1) {
			$errmsg = "Error opening merging surveys. [ ID:${sids} ]";
			return($errmsg);
		}
		$sidstr = array_to_insql($sids);
		$sid = $sids[0];
		
		if(empty($precision))	$precision  = 1;
		if(empty($showTotals))	$showTotals = 1;
		
		// set up things differently for cross analysis
		$cross = !empty($qid);
		if($cross) {
			$qids = merge_expand_qids(array($qid), $sids);
			if(!empty($cids)) {
				$cids = merge_expand_cids($cids, $qids);
				$cidstr = array_to_insql($cids);
			}
		}

		// build associative array holding weather each question
		// type has answer choices or not and the table the answers are in
		$has_answers = array();
		$answer_table = array();
		$sql = 'SELECT id,has_answers,answer_table
				  FROM question_types 
				 ORDER BY id';
		if(!($result = mysql_query($sql))) {
			$errmsg = "Error phpESP system tables corrupted. [ Table(s): question_types ]";
			return($errmsg);
		}
		while($row = mysql_fetch_row($result)) {
			$has_answers[$row[0]]=$row[1];
			$answer_table[$row[0]]=$row[2];
		}
		mysql_free_result($result);

		// load survey title (and other globals)
		$sql = "SELECT * FROM surveys WHERE id=${sid}";
		if(!($result = mysql_query($sql))) {
			$errmsg = "Error opening selected survey. [ ID:${sid} R:" . mysql_num_rows($result) ."]";
			return($errmsg);
		}
		$survey = mysql_fetch_array($result, MYSQL_ASSOC);
		mysql_free_result($result);

		// load survey questions
		$sql = "SELECT * 
				  FROM survey_questions 
				 WHERE survey_id=${sid} AND 
				       status='' 
				 ORDER BY position,id";
		if(!($questions_result = mysql_query($sql))) {
			$errmsg = "Error opening selected survey. No questions found for survey. [ ID:${sid} ]";
			return($errmsg);
		}
		
		// find out more about the question we are cross analyzing on (if any)
		if($cross) {
			$sql = "SELECT type_id FROM survey_questions WHERE id=${qid}";
			$result = mysql_query($sql);
			$crossTable = $answer_table[mysql_result($result,0,0)];
			mysql_free_result($result);
			if(!in_array($crossTable, array('answers_single','answers_bool','answers_multiple'))) {
				$errmsg = "Error cross-analyzing. Question not valid type. [ Table: ${crossTable} ]";
				return($errmsg);
			}
		}

	// find total number of survey responses
	// and relevant response ID's
		$sql = "";
		if($cross) {
			if(!empty($cidstr))
				$sql = "SELECT A.response_id
						  FROM ${crossTable} A,
						       survey_responses R
						 WHERE A.response_id=R.id AND
						       R.complete='yes' AND
						       A.question_id ${qidstr} AND
				    		   A.choice_id ${cidstr}
						 ORDER BY A.response_id";
			else
				$sql = "SELECT A.response_id
						  FROM ${crossTable} A,
						       survey_responses R
						 WHERE A.response_id=R.id AND
						       R.complete='yes' AND
						       A.question_id ${qidstr} AND
				    		   A.choice_id = 0
						 ORDER BY A.response_id";
		} else {
			$sql = "SELECT R.id
					  FROM survey_responses R
					 WHERE R.survey_id ${sidstr} AND
					       R.complete='yes'
					 ORDER BY R.id";
		}
		if(!($result = mysql_query($sql))) {
			$errmsg = "Error opening selected survey. [ ID:${sid} ] [ ".mysql_error()."]";
			return($errmsg);
		}
		$total = mysql_num_rows($result);
		if($total < 1) {
			$errmsg = "Error opening selected survey. No responses found for survey. [ ID:${sid} ]";
//			return($errmsg);
			echo("<!-- \$errmsg = '$errmsg' -->\n");
		}
		
		$rid = array();
		while($row = mysql_fetch_row($result)) {
			array_push($rid, $row[0]);
		}
		mysql_free_result($result);

		// create a string suitable for inclusion in a SQL statement
		// containing the desired Response IDs
		// ex: "('304','311','317','345','365','370','403','439')"
		$ridstr = "IN (" . ereg_replace("([^,]+)","'\\1'", join(",", $rid)) .")";

		if(1) {
			echo("<!--
\$sid = $sid
\$precision = $precision
\$showTotals = $showTotals
\$qid = $qid
\$cids = $cids
\$cross = $cross
\$crossTable = $crossTable
\$cidstr = $cidstr
\$ridstr = $ridstr
-->");
		}
?>

<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<?php
		if($cross) {
			echo("<blockquote>Cross analysis on QID: ${qid}</blockquote>\n");
		}
?>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php
		$i=0; // question number counter
		while($question = mysql_fetch_array($questions_result)) {
			// process each question
			$qid = $question['id'];
			$tid = $question['type_id'];
			$table = $answer_table[$tid];

			if($tid == 99) {
				echo("<tr><td><hr></td></tr>\n");
				continue;
			}

			++$i;
			
			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr xbgcolor="<?php echo($bg); ?>">
 		<td>
			<A NAME="Q<?php echo($i); ?>"><?php echo($i); ?>.</A>
			<?php echo($question["content"]); ?>

			<blockquote>
<?php
			$counts = array();

// -------------------------------- answers_bool --------------------------------
			if($table == "answers_bool") {
				$sql = "SELECT A.choice_id, COUNT(A.response_id)
						  FROM ${table} A
						 WHERE A.question_id='${qid}' AND 
						       A.response_id ${ridstr} AND
							   A.choice_id IN ('yes','no')
						 GROUP BY A.choice_id";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				if($result = mysql_query($sql)) {
					while(list($ccontent,$count) = mysql_fetch_row($result)) {
						$counts[ucfirst($ccontent)] = $count;
					}
					mysql_free_result($result);
				}

				if(empty($question["result_id"]))
					$question["result_id"] = 1;	// default to percentages for yes/no

// -------------------------------- answers_single --------------------------------
// -------------------------------- answers_multiple --------------------------------
			} elseif($table == "answers_single" || $table == "answers_multiple") {
				$sql = "SELECT C.content, COUNT(A.response_id) AS num
						  FROM question_choices C,
						       ${table} A
						 WHERE C.question_id='${qid}' AND
						       C.content NOT LIKE '!other%' AND
							   A.question_id=C.question_id AND
							   A.choice_id=C.id AND
							   A.response_id ${ridstr}
						 GROUP BY C.id";
//						 ORDER BY C.id";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				if($result = mysql_query($sql)) {
					while(list($ccontent,$count) = mysql_fetch_row($result)) {
						$counts[$ccontent] = $count;
					}
					mysql_free_result($result);
				}
				// handle 'other...'
				$sql = "SELECT A.answer, C.content
						  FROM answers_other A, question_choices C
						 WHERE A.question_id='${qid}' AND
						       A.choice_id=C.id AND
    						   A.response_id ${ridstr}
						 ORDER BY C.id, A.answer";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				if($result = mysql_query($sql)) {
					while(list($answer,$ccontent) = mysql_fetch_row($result)) {
						$content = ereg_replace("!other=?", "", $ccontent);
						if($content != '')	$content .= ": ";
						$content .= htmlspecialchars($answer);
						$counts[$content]++;
					}
					mysql_free_result($result);
				}
				if(empty($question["result_id"]))
					$question["result_id"] = 1;	// default to percentages
// -------------------------------- answers_text --------------------------------
			} elseif($table == "answers_text") {
				$sql = "SELECT A.answer, COUNT(A.response_id) AS num
						  FROM ${table} A
						 WHERE A.question_id='${qid}' AND
    						   A.response_id ${ridstr}
						 GROUP BY A.answer";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				if($result = mysql_query($sql)) {
					while(list($text, $num) = mysql_fetch_row($result)) {
						if(!empty($text))
							$counts[htmlspecialchars($text)] = $num;
					}
					mysql_free_result($result);
				}
				$question["result_id"] = 4;	// force "list" type response for text fields
// -------------------------------- answers_rank --------------------------------
			} elseif($table == "answers_rank") {
				if($tid == 8) { //Rank
					$sql = "SELECT C.content, AVG(A.rank) AS average
							  FROM question_choices C, ${table} A
							 WHERE C.question_id='${qid}' AND
    							   A.question_id='${qid}' AND
								   A.choice_id=C.id AND
								   A.rank>0 AND
								   A.response_id ${ridstr}
							 GROUP BY C.id";
					if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
					if($result = mysql_query($sql)) {
						while(list($ccontent,$avg) = mysql_fetch_row($result)) {
							$counts[$ccontent] = $avg;
						}
						mysql_free_result($result);
					}
					$question["result_id"] = 99;	// force to rank
				} else {
					$sql = "SELECT A.rank, COUNT(A.response_id) AS num
							  FROM ${table} A
							 WHERE A.question_id='${qid}' AND
    							   A.response_id ${ridstr}
							 GROUP BY A.rank";
					if($result = mysql_query($sql)) {
						while(list($rank, $num) = mysql_fetch_row($result)) {
							if($rank == -1) { $rank = "N/A"; }
							$counts[$rank] += $num;
						}
						mysql_free_result($result);
					}
					if(empty($question["result_id"]))
						$question["result_id"] = 2;	// default to rank
				}
			}
// ---------------------------------------------------------------------------
			switch($question["result_id"]) {
				case "1":	// Percentages
					mkrespercent($counts,$total,$precision,$showTotals);
					break;
				case "2":	// Rank
					mkresrank($counts,$total,$precision,$showTotals);
					break;
				case "3":	// Count
					mkrescount($counts,$total,$precision,$showTotals);
					break;
				case "4":	// List
					mkreslist($counts,$total,$precision,$showTotals);
					break;
				case "99":	// Average
					mkresavg($counts,$total,$precision,$showTotals);
					break;
			} // end switch
?>
			</blockquote>
		</td>
	</tr>
<?php	} // end while ?>
</table>
<?php
		mysql_free_result($questions_result);
		return;
	} // end function survey_results

} // end _FUNCTION_SURVEY_MERGE
