<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if(empty($GLOBALS['ESPCONFIG']['DEBUG_MERGE']))
	$GLOBALS['ESPCONFIG']['DEBUG_MERGE'] = $GLOBALS['ESPCONFIG']['DEBUG'];

/* {{{ proto string survey_merge(array survey_ids, int precision, bool show_totals)
   Builds a HTML result page after merging the results from
   each of the survey ids.
   Return an empty string on sucess, else a string with an
   error message. */
function survey_merge($sids, $precision = 1, $showTotals = 1) {
	// sanity check arguments
	if(!is_array($sids)) {
		$errmsg = _('Invalid argument.') ." [ ${sids} ]";
		return($errmsg);
	}
	$num = count($sids);
	if($num < 1) {
		$errmsg = _('Invalid argument.') .' [ '. join(',',$sids) .' ]';
		return($errmsg);
	}

	// set defaults
	if(empty($precision))	$precision  = 1;
	if(empty($showTotals))	$showTotals = 1;

	// convert arguments to alternate forms
	$sidstr = array_to_insql($sids);

	$response_table = esp_type_response_table();

	// load survey title (and other globals) from 1st surveyId
	$sid = $sids[0];
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
	if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
	if(!($result = execute_sql($sql,"", ADODB_FETCH_ASSOC))) {
		$errmsg = _('Error opening survey.') ." [ ID:${sid} R:" . record_count($result) ."] [ ".ErrorMsg()."]";
		return($errmsg);
	}
	$survey = fetch_row($result);
	db_close($result);

	// find total number of survey responses
	$sql = "SELECT R.id
			  FROM ".$GLOBALS['ESPCONFIG']['response_table']." R
			 WHERE R.survey_id ${sidstr} AND
				   R.complete='Y'";
	if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
	if(!($result = execute_sql($sql))) {
		$errmsg = _('Error opening surveys.') ." [ ID: ${sidstr} ] [ ".ErrorMsg()."]";
		return($errmsg);
	}
	$total = record_count($result);
	if($total < 1) {
		$errmsg = _('Error opening surveys.') ." ".
			_('No responses found.') ." [ ID: ${sidstr} ] [ ". _('TOTAL') .": ${total} ]";
		db_close($result);
		return($errmsg);
	}
	// and the desired response id's
	$rids = array();
	while($row = fetch_row($result)) {
        $result->MoveNext();
		array_push($rids, $row[0]);
	}
	db_close($result);
	$ridstr = array_to_insql($rids);

	// load survey questions
	for($i=0; $i<$num; $i++) {
		$sid = $sids[$i];
		$sql = "SELECT *
				  FROM ".$GLOBALS['ESPCONFIG']['question_table']."
				 WHERE survey_id=${sid} AND
					   deleted='N'
				 ORDER BY position,id";
		if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
		$questions[$i] = execute_sql($sql,"",ADODB_FETCH_ASSOC);
		if(!$questions[$i]) {
			$errmsg = _('Error opening surveys.') .' '. _('No questions found.') . " [ ID:${sid} ] [ ".ErrorMsg()."]";
			return($errmsg);
		}
	}

?>
<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php
	$q=0; // question number counter
	while(1) {
		// get next question from each of SIDs (lock-step)
		$qids = array();
		for($i=0; $i<$num; $i++) {
			if(!($question[$i] = fetch_row($questions[$i]))) {
				for($j=0; $j<$num; $j++) {
					db_close($questions[$j]);
				}
				echo("</table>\n");
				return;
			}
            $questions[$i]->MoveNext();
			if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$i = $i; \$question[\$i]['id'] = ".$question[$i]['id']." -->\n");
			array_push($qids, $question[$i]['id']);
		}
		$qidstr = array_to_insql($qids);
		if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$qidstr = $qidstr -->\n");

		// now -- we assume the surveys are identical, and there is a
		// one-to-one relation between the questions of each
		// we take the first element of the SIDs array to be the "master"

		$tid = $question[0]['type_id'];
		$table = $response_table[$tid];
		if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$table = $table -->\n");

		if($tid == 99) {
			echo("<tr><td><hr></td></tr>\n");
			continue;
		}
		if($tid == 100) {
			echo("<tr><td>". $question[0]['content'] ."</td></tr>\n");
			continue;
		}

		++$q;
        $bg = "";
		if($bg != '#eeeeee')	$bg = '#eeeeee';
		else                	$bg = '#ffffff';
?>
	<tr xbgcolor="<?php echo($bg); ?>">
 		<td>
			<A NAME="Q<?php echo($q); ?>"><?php echo($q); ?>.</A>
			<?php echo($question[0]['content']); ?>

			<blockquote>
<?php
		$counts = array();

		switch($table) {
// -------------------------------- response_bool --------------------------------
		case 'response_bool':
			$sql = "SELECT A.choice_id, COUNT(A.response_id)
					  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE A.question_id ${qidstr} AND
						   A.response_id ${ridstr}
					 GROUP BY A.choice_id";
			if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
			$result = execute_sql($sql);
			while(list($text,$count) = fetch_row($result)) {
                $result->MoveNext();
				$counts[$text] = $count;
			}
			db_close($result);

			if(empty($question[0]['result_id']))
				$question[0]['result_id'] = 1;	// default to percentages for yes/no

			break;

// -------------------------------- response_single ----------------------------------
// -------------------------------- response_multiple --------------------------------
		case 'response_multiple':
			$showTotals = 0;
		case 'response_single':
			for($i=0; $i<$num; $i++) {
				$sid = $sids[$i];
				$qid = $question[$i]['id'];
				$sql = "SELECT id
					      FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
						 WHERE question_id=${qid} AND
							   content NOT LIKE '!other%'
						 ORDER BY id";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result = execute_sql($sql);
				$cids[$i] = array();
				while(list($cid) = fetch_row($result)) {
                    $result->MoveNext();
					array_push($cids[$i],$cid);
				}
				db_close($result);
			}
			$content = array();
			foreach($cids[0] as $cid) {
				$sql = "SELECT content FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id=${cid}";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result = get_one($sql);
				array_push($content, $result);
			}
			$cnum = count($content);
			for($j=0; $j<$cnum; $j++) {
				$mycids = array();
				for($i=0; $i<$num; $i++) {
					array_push($mycids, $cids[$i][$j]);
				}
				$mycidstr = array_to_insql($mycids);

				$sql = "SELECT COUNT(*)
						  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
						 WHERE A.choice_id ${mycidstr} AND
							   A.response_id ${ridstr}";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result=get_one($sql);
				$counts[$content[$j]] = $result;
			}

			// handle 'other...'
			for($i=0; $i<$num; $i++) {
				$sid = $sids[$i];
				$qid = $question[$i]['id'];
				$sql = "SELECT id
					      FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
						 WHERE question_id ${qidstr} AND
							   content LIKE '!other%'
						 ORDER BY id";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result = execute_sql($sql);
				$cids[$i] = array();
				while(list($cid) = fetch_row($result)) {
                    $result->MoveNext();
					array_push($cids[$i],$cid);
				}
				db_close($result);
			}
			$content = array();
			foreach($cids[0] as $cid) {
				$sql = "SELECT content FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id=${cid}";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result=get_one($sql);
				$text = ereg_replace("!other=?", "", $result);
				if(!empty($text))
					$text .= ': ';
				array_push($content, $text);
			}
			$cnum = count($content);
			for($j=0; $j<$cnum; $j++) {
				$mycids = array();
				for($i=0; $i<$num; $i++) {
					array_push($mycids, $cids[$i][$j]);
				}
				$mycidstr = array_to_insql($mycids);

				$sql = "SELECT A.response, COUNT(A.response_id)
						  FROM ".$GLOBALS['ESPCONFIG']['response_other_table']." A
						 WHERE A.choice_id ${mycidstr} AND
							   A.response_id ${ridstr}
						 GROUP BY A.response";
				if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
				$result=execute_sql($sql);
				while(list($response,$count) = fetch_row($result)) {
                    $result->MoveNext();
					if(!empty($response)) {
						$text = $content[$j] . htmlspecialchars($response);
						$counts[$text] = $count;
					}
				}
				db_close($result);
			}

			if(empty($question[0]['result_id']))
				$question[0]['result_id'] = 1;	// default to percentages

			break;
// -------------------------------- response_text --------------------------------
		case 'response_text':
			$sql = "SELECT A.response, COUNT(A.response_id)
					  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
					 WHERE A.question_id ${qidstr} AND
						   A.response_id ${ridstr}
					 GROUP BY A.response";
			if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
			$result = execute_sql($sql);
			while(list($response, $count) = fetch_row($result)) {
                $result->MoveNext();
				if(!empty($response))
					$counts[htmlspecialchars($response)] = $count;
			}
			db_close($result);

			$question[0]['result_id'] = 4;	// force "list" type response for text fields

			break;
// -------------------------------- response_rank --------------------------------
		case 'response_rank':
			if($tid == 8) { //Rank
				for($i=0; $i<$num; $i++) {
					$sid = $sids[$i];
					$qid = $question[$i]['id'];
					$sql = "SELECT id
					          FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
							 WHERE question_id ${qidstr}
							 ORDER BY id";
					if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
					$result = execute_sql($sql);
					$cids[$i] = array();
					while(list($cid) = fetch_row($result)) {
                        $result->MoveNext();
						array_push($cids[$i],$cid);
					}
					db_close($result);
				}
				$content = array();
				foreach($cids[0] as $cid) {
					$sql = "SELECT content FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id=${cid}";
					if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
					$result=get_one($sql);
					array_push($content, $result);
				}
				$cnum = count($content);
				for($j=0; $j<$cnum; $j++) {
					$mycids = array();
					for($i=0; $i<$num; $i++) {
						array_push($mycids, $cids[$i][$j]);
					}
					$mycidstr = array_to_insql($mycids);

					$sql = "SELECT AVG(A.rank)
							  FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." A
							 WHERE A.choice_id ${mycidstr} AND
								   A.response_id ${ridstr} AND
								   A.rank>0";
					if($GLOBALS['ESPCONFIG']['DEBUG_MERGE'])	echo("<!-- \$sql = ${sql} -->\n");
					$result=get_one($sql);
					$counts[$content[$j]] = $result;
				}

				$question[0]['result_id'] = 99;	// force to rank
			} else {
				// deprecated ... I am not going to write this
				// code. If you need it email me or write it yourself.
			}
			break;
		}
// ---------------------------------------------------------------------------
		switch($question[0]['result_id']) {
			case '1':	// Percentages
				mkrespercent($counts,$total,$precision,$showTotals);
				break;
			case '2':	// Rank
				mkresrank($counts,$total,$precision,$showTotals);
				break;
			case '3':	// Count
				mkrescount($counts,$total,$precision,$showTotals);
				break;
			case '4':	// List
				mkreslist($counts,$total,$precision,$showTotals);
				break;
			case '99':	// Average
				mkresavg($counts,$total,$precision,$showTotals,0);
				break;
		} // end switch
?>
			</blockquote>
		</td>
	</tr>
<?php	} // end while -- execution should never pass this point ?>
</table>
<?php
	return;
}
/* }}} */

?>
