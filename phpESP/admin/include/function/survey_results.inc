<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if(!defined('_FUNCTION_SURVEY_RESULTS')) {
 	define('_FUNCTION_SURVEY_RESULTS',TRUE);

require ($ESPCONFIG['include_path']."where/browse_responses.inc");

if(empty($GLOBALS['ESPCONFIG']['DEBUG_RESULTS']))
	$GLOBALS['ESPCONFIG']['DEBUG_RESULTS'] = $GLOBALS['ESPCONFIG']['DEBUG'];

/* {{{ proto string survey_results(int survey_id, int preceision, bool show_totals, int question_id, array choice_ids, int response_id)
	Builds HTML for the results for the survey. If a
	question id and choice id(s) are given, then the results
	are only calculated for respodants who chose from the
	choice ids for the given question id.
	Returns empty string on sucess, else returns an error
	string. */
function survey_results($sid, $precision = 1, $showTotals = 1, $qid = '', $cids = '', $rid = '') {
	$bg = '';
	if(empty($precision))	$precision  = 1;
	if($showTotals === '')	$showTotals = 1;

	if(is_int($cids))
		$cids = array($cids);
	if(is_string($cids))
		$cids = split(" ",$cids); // turn space seperated list into array

	// set up things differently for cross analysis
	$cross = !empty($qid);
	if($cross) {
		if(is_array($cids) && count($cids)>0) {
			$cidstr = array_to_insql($cids);
		} else {
			$cidstr = '';
		}
	}

	// build associative array holding weather each question
	// type has answer choices or not and the table the answers are in
	$has_choices = array();
	$response_table = array();
	$sql = 'SELECT id,has_choices,response_table
			  FROM question_type
			 ORDER BY id';
	if(!($result = mysql_query($sql))) {
		$errmsg = _('Error system table corrupt.') . ' [ '. _('Table') .': question_type ]';
		return($errmsg);
	}
	while($row = mysql_fetch_row($result)) {
		$has_choices[$row[0]]=$row[1];
		$response_table[$row[0]]=$row[2];
	}
	mysql_free_result($result);

	// load survey title (and other globals)
	$sql = "SELECT * FROM survey WHERE id='${sid}'";
	if(!($result = mysql_query($sql))) {
		$errmsg = _('Error opening survey.') ." [ ID:${sid} R:" . mysql_num_rows($result) ."]";
		return($errmsg);
	}
	$survey = mysql_fetch_array($result, MYSQL_ASSOC);
	mysql_free_result($result);

	// load survey questions
	$sql = "SELECT *
			  FROM question
			 WHERE survey_id='${sid}' AND
				   deleted='N'
			 ORDER BY position,id";
	if(!($questions_result = mysql_query($sql))) {
		$errmsg = _('Error opening survey.') .' '. _('No questions found.') ." [ ID:${sid} ]";
		return($errmsg);
	}

	// find out more about the question we are cross analyzing on (if any)
	if($cross) {
		$sql = "SELECT type_id FROM question WHERE id=${qid}";
		$result = mysql_query($sql);
		$crossTable = $response_table[mysql_result($result,0,0)];
		mysql_free_result($result);
		if(!in_array($crossTable, array('response_single','response_bool','response_multiple'))) {
			$errmsg = _('Error cross-analyzing. Question not valid type.') .' [ '. _('Table') .": ${crossTable} ]";
			return($errmsg);
		}
	}

// find total number of survey responses
// and relevant response ID's
	if (!empty($rid)) {
		navigate_responses($sid,$rid);
		$ridstr = "= '${rid}'";
        $total = 1;
	} else {
	    $sql = "";
	    if($cross) {
		    if(!empty($cidstr))
			    $sql = "SELECT A.response_id
					      FROM ${crossTable} A,
						       response R
					     WHERE A.response_id=R.id AND
						       R.complete='Y' AND
						       A.question_id='${qid}' AND
				    	       A.choice_id ${cidstr}
					     ORDER BY A.response_id";
		    else
			    $sql = "SELECT A.response_id
					      FROM ${crossTable} A,
						       response R
					     WHERE A.response_id=R.id AND
						       R.complete='Y' AND
						       A.question_id='${qid}' AND
				    	       A.choice_id = 0
					     ORDER BY A.response_id";
	    } else {
		    $sql = "SELECT R.id
				      FROM response R
				     WHERE R.survey_id='${sid}' AND
					       R.complete='Y'
				     ORDER BY R.id";
	    }
	    if(!($result = mysql_query($sql))) {
		    $errmsg = _('Error opening survey.') ." [ ID:${sid} ] [ ".mysql_error()."]";
		    return($errmsg);
	    }
	    $total = mysql_num_rows($result);
	    if($total < 1) {
		    $errmsg = _('Error opening survey.') .' '. _('No responses found.') ." [ ID:${sid} ]";
			    return($errmsg);
            if ($GLOBALS['ESPCONFIG']['DEBUG'])
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
	}
?>
<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<?php
	if($cross) {
		echo("<blockquote>" ._('Cross analysis on QID:') ." ${qid}</blockquote>\n");
	}
?>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php
	$i=0; // question number counter
	while($question = mysql_fetch_array($questions_result)) {
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
			<?php if ($tid < 50) { ?>
			<A NAME="Q<?php echo(++$i); ?>"><?php echo($i); ?>.</A>
			<?php } ?>
			<?php echo($question["content"]); ?>

			<blockquote>
<?php
		$counts = array();
		switch ($table) {
// -------------------------------- response_bool --------------------------------
		case 'response_bool':
			$sql = "SELECT A.choice_id, COUNT(A.response_id)
					  FROM ${table} A
					 WHERE A.question_id='${qid}' AND
						   A.response_id ${ridstr} AND
						   A.choice_id != ''
					 GROUP BY A.choice_id";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = mysql_query($sql)) {
				while(list($ccontent,$count) = mysql_fetch_row($result)) {
					if ($ccontent == 'Y')
						$ccontent = _('Yes');
					else
						$ccontent = _('No');
					$counts[$ccontent] = $count;
				}
				mysql_free_result($result);
			}

			if(empty($question["result_id"]))
				$question["result_id"] = 1;	// default to percentages for yes/no
			break;
// -------------------------------- response_single --------------------------------
// -------------------------------- response_multiple --------------------------------
		case 'response_multiple':
			$totals = 0;
		case 'response_single':
                $question_content=array();
                $counts1=array();
                $sql = "SELECT content FROM question_choice WHERE question_id='${qid}' ORDER BY id";
                if($result = mysql_query($sql)){
	               	while (list($row) = mysql_fetch_row($result)){
        	               	array_push($question_content , $row);
                        }
                        mysql_free_result($result);
                        unset($sql);
                }

			$sql = "SELECT C.content, COUNT(A.response_id) AS num
					  FROM question_choice C,
						   ${table} A
					 WHERE C.question_id='${qid}' AND
						   C.content NOT LIKE '!other%' AND
						   A.question_id=C.question_id AND
						   A.choice_id=C.id AND
						   A.response_id ${ridstr}
					 GROUP BY C.id";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = mysql_query($sql)) {
				while(list($ccontent,$count) = mysql_fetch_row($result)) {
					$counts1[$ccontent] = $count;
				}
				mysql_free_result($result);
			}
			//lets show all the question choices in the results page regardless of an
			//associated count value.
                        foreach($question_content as $qn){
	 	        	if (isset($counts1[$qn]))
                	        	$counts[$qn] = $counts1[$qn];
                                else
                                        $counts[$qn] = '0';
                        }

			// handle 'other...'
			$sql = "SELECT A.response, C.content
					  FROM response_other A, question_choice C
					 WHERE A.question_id='${qid}' AND
						   A.choice_id=C.id AND
    					   A.response_id ${ridstr}
					 ORDER BY C.id, A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = mysql_query($sql)) {
				while(list($answer,$ccontent) = mysql_fetch_row($result)) {
					$content = ereg_replace("!other=?", "", $ccontent);
					if($content != '')	$content .= ": ";
					$content .= htmlspecialchars($answer);
					$counts[$content] = (isset($counts[$content]) ? $counts[$content] : 0)+1;
				}
				mysql_free_result($result);
			}
			if(empty($question["result_id"]))
				$question["result_id"] = 1;	// default to percentages
			break;
// -------------------------------- response_text --------------------------------
		case 'response_text':
			$sql = "SELECT A.response, COUNT(A.response_id) AS num
					  FROM ${table} A
					 WHERE A.question_id='${qid}' AND
    					   A.response_id ${ridstr}
					 GROUP BY A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = mysql_query($sql)) {
				while(list($text, $num) = mysql_fetch_row($result)) {
					if(!empty($text))
						$counts[htmlspecialchars($text)] = $num;
				}
				mysql_free_result($result);
			}
			$question["result_id"] = 4;	// force "list" type response for text fields
			break;
// -------------------------------- response_date --------------------------------
		case 'response_date':
			$sql = "SELECT A.response, COUNT(A.response_id) AS num
					  FROM ${table} A
					 WHERE A.question_id='${qid}' AND
    					   A.response_id ${ridstr}
					 GROUP BY A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
			if($result = mysql_query($sql)) {
				while(list($text, $num) = mysql_fetch_row($result)) {
					if(!empty($text))
						$counts[htmlspecialchars($text)] = $num;
				}
				mysql_free_result($result);
			}
			$question["result_id"] = 4;	// force "list" type response for date fields
			break;
// -------------------------------- response_rank --------------------------------
		case 'response_rank':
			if($tid == 8) { //Rank
				$sql = "SELECT C.content, AVG(A.rank+1) AS average
						  FROM question_choice C, ${table} A
						 WHERE C.question_id='${qid}' AND
    						   A.question_id='${qid}' AND
							   A.choice_id=C.id AND
							   A.rank>=0 AND
							   A.response_id ${ridstr}
						 GROUP BY C.id";
				if($GLOBALS['ESPCONFIG']['DEBUG_RESULTS'])	echo("<!-- \$sql = '$sql' -->\n");
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
						$counts[_($rank)] += $num;
					}
					mysql_free_result($result);
				}
				if(empty($question["result_id"]))
					$question["result_id"] = 2;	// default to rank
			}
			break;
		}
// ---------------------------------------------------------------------------
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
?>
			</blockquote>
		</td>
	</tr>
<?php } // end while ?>
</table>
<?php
	mysql_free_result($questions_result);
	return;
}
/* }}} */

} // end _FUNCTION_SURVEY_RESULTS
?>
