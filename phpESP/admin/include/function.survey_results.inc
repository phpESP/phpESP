<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_RESULTS')) {
 	define('_FUNCTION_SURVEY_RESULTS',TRUE);
	
	if(!defined('DEBUG'))
		define('DEBUG',TRUE);

	// builds HTML for the results for the survey 'sid'
	// with the added limitation that the answer to
	// $questionId is an element of the array $choiceIds
	
	// if $questionId is not specified (or less than 0)
	// the standard (original) results display is created
	
	// void crossanalysis(int $surveyId, 
	//                    bool $showTotals, 
	//                    int $questionId,
	//                    int[] $choiceIds);
	
	function survey_results($sid, $show_totals = 1, $qid = -1, $cid = array()) {
		echo('<!-- $Id$ -->'."\n");

		// set up things differently for cross analysis
		$cross = 0;
		if($qid > 0) {
			$cross = 1;
				
			// create a string suitable for inclusion in a SQL statement
			// containing the desired Choice IDs
			// ex: "('123','124','125')"
			$cidstr = ereg_replace("([0-9]+)","'\\1'", "(" . join(",", $cid) . ")");
		}

		if(DEBUG)	echo("<!-- \$cidstr = $cidstr -->\n");

		// build associative array holding weather each question
		// type has answer choices or not and the table the answers are in
		$has_answers = array();
		$answer_table = array();
		$sql = 'SELECT id,has_answers,answer_table
				  FROM question_types 
				 ORDER BY id';
		$result = mysql_query($sql);
		while($row = mysql_fetch_row($result)) {
			$has_answers[$row[0]]=$row[1];
			$answer_table[$row[0]]=$row[2];
		}
		mysql_free_result($result);

		// load survey title (and other globals)
		$sql = "SELECT * FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) != 1) {
			$errmsg = "Error opening selected survey. [ ID:${sid} R:" . mysql_num_rows($result) ."]";
			return(1);
		}
		$survey = mysql_fetch_array($result);
		mysql_free_result($result);

		// load survey questions
		$sql = "SELECT * 
				  FROM survey_questions 
				 WHERE survey_id='${sid}' AND 
				       status='' 
				 ORDER BY position,id";
		$questions_result = mysql_query($sql);
		if(mysql_num_rows($questions_result) < 1) {
			$errmsg = "Error opening selected survey. No questions found for survey. [ ID:${sid} ]";
			return(1);
		}

	// find total number of survey responses
	// and relevant response ID's
		$sql = "";
		if($cross) {
			$sql = "SELECT R.id
					  FROM answers_single A,
				    	   survey_responses R
					 WHERE R.survey_id='${sid}' AND
				    	   R.complete = 'yes' AND
					       A.question_id='${qid}' AND
				    	   A.choice_id IN ${cidstr}
					 ORDER BY R.id";
		} else {
			$sql = "SELECT R.id
					  FROM survey_responses R
					 WHERE R.survey_id='${sid}' AND
					       R.complete='yes'
					 ORDER BY R.id";
		}
		$result = mysql_query($sql);
		$total = mysql_num_rows($result);
		$rid = array();
		while($row = mysql_fetch_row($result)) {
			array_push($rid, $row[0]);
		}
		mysql_free_result($result);

		// create a string suitable for inclusion in a SQL statement
		// containing the desired Response IDs
		// ex: "('304','311','317','345','365','370','403','439')"
		$ridstr = ereg_replace("([0-9]+)","'\\1'", "(" . join(",", $rid) . ")");

		
		if(DEBUG)	echo("<!-- \$ridstr = $ridstr -->\n");

		$i=0;
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
?>
	<tr>
 		<td>
			<A NAME="Q<?php echo($i); ?>"><?php echo($i); ?>.</A>
			<?php echo($question["content"]); ?>

			<blockquote>
<?php
			$counts = array();

// -------------------------------- answers_bool --------------------------------
			if($table == "answers_bool") {
				$sql = "SELECT COUNT(*)
						  FROM ${table} A
						 WHERE A.question_id='${qid}' AND 
						       A.choice_id='yes' AND
						       A.response_id IN ${ridstr}";
				$result = mysql_query($sql);
				$counts['Yes'] = mysql_result($result,0,0);
				mysql_free_result($result);

				$sql = "SELECT COUNT(*)
						  FROM ${table} A
						 WHERE A.question_id='${qid}' AND 
						       A.choice_id='no' AND
						       A.response_id IN ${ridstr}";
				$result = mysql_query($sql);
				$counts['No'] = mysql_result($result,0,0);
				mysql_free_result($result);

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
							   A.response_id IN ${ridstr}
						 GROUP BY C.id";
//						 ORDER BY C.id";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				$result = mysql_query($sql);
				while(list($ccontent,$count) = mysql_fetch_row($result)) {
					$counts[$ccontent] = $count;
				}
				mysql_free_result($result);
				// handle 'other...'
				$sql = "SELECT A.answer, C.content, COUNT(A.response_id) AS num
						  FROM answers_other A, question_choices C
						 WHERE A.question_id='${qid}' AND
						       A.choice_id=C.id AND
    						   A.response_id IN ${ridstr}
						 GROUP BY C.id, A.answer";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				if($result = mysql_query($sql)) {
					while(list($answer,$ccontent,$num) = mysql_fetch_row($result)) {
						$content = ereg_replace("!other=?", "", $ccontent) . $answer;
						$counts[$content] += $num;
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
    						   A.response_id IN $ridstr
						 GROUP BY A.answer";
				if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
				$result = mysql_query($sql);
				while(list($text, $num) = mysql_fetch_row($result)) {
					if(!empty($text))
						$counts[$text] = $num;
				}
				mysql_free_result($result);
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
								   A.response_id IN $ridstr
							 GROUP BY C.id";
					if(DEBUG)	echo("<!-- \$sql = '$sql' -->\n");
					$result = mysql_query($sql);
					while(list($ccontent,$avg) = mysql_fetch_row($result)) {
						$counts[$ccontent] = $avg;
					}
					$question["result_id"] = 99;	// force to rank
				} else {
					$sql = "SELECT A.rank, COUNT(A.response_id) AS num
							  FROM ${table} A
							 WHERE A.question_id='${qid}' AND
    							   A.response_id IN $ridstr
							 GROUP BY A.rank";
					$result = mysql_query($sql);
					while(list($rank, $num) = mysql_fetch_row($result)) {
						if($rank == -1) { $rank = "N/A"; }
						$counts[$rank] += $num;
					}
					mysql_free_result($result);
					if(empty($question["result_id"]))
						$question["result_id"] = 2;	// default to rank
				}
			}
// ---------------------------------------------------------------------------
			switch($question["result_id"]) {
				case "1":	// Percentages
					mkrespercent($counts,$total,$show_totals);
					break;
				case "2":	// Rank
					mkresrank($counts,$total,$show_totals);
					break;
				case "3":	// Count
					mkrescount($counts,$total,$show_totals);
					break;
				case "4":	// List
					mkreslist($counts,$total,$show_totals);
					break;
				case "99":	// Average
					mkresavg($counts,$total,$show_totals);
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
	} // end function survey_results

} // end _FUNCTION_SURVEY_RESULTS
