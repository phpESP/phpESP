<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto array merge_expand_qids(int target_qid, array surveys)
   Returns an array of question ids spanning the given
   survey ids. */
function merge_expand_qids($targetq, $surveys) {
	$sql = "SELECT survey_id FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id ". array_to_insql($targetq);
	$targets = get_one($sql);

	$sidstr = array_to_insql($surveys);
	$sql = "
SELECT Q.survey_id, Q.id
FROM ".$GLOBALS['ESPCONFIG']['question_table']." Q
WHERE Q.survey_id ${$sidstr} AND deleted='N'
ORDER BY Q.survey_id, Q.position, Q.id";
	$result = execute_sql($sql);
	foreach($surveys as $sid) {
		$i[$sid] = 0;
	}
	while(list($sid, $qid) = fetch_row($result)) {
		$$sid[$i[$sid]++] = $qid;
	}
	db_close($result);
	foreach($surveys as $sid) {
		$num = $i[$sid];
	}
	$qids = array();
	for($j=0;$j<$num;$j++) {
		if(in_array($$targets[$j],$targetq)) {
			foreach($surveys as $sid) {
				array_push($qids, $$sid[$j]);
			}
		}
	}
	return($qids);
}
/* }}} */

/* {{{ proto array merge_expand_cids(int target_cid, array questions)
   Returns an array of choice ids spanning the given
   question ids. */
function merge_expand_cids($targetc, $questions) {
	$sql = "SELECT question_id FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE id ". array_to_insql($targetc);
	$result = execute_sql($sql);
	$targetq = $result->fields[0];
	db_close($result);

	array_push($questions,$targetq);
	$qidstr = array_to_insql($questions);
	$sql = "
SELECT C.question_id, C.id
FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." C
WHERE C.question_id ${$qidarr}
ORDER BY C.question_id, C.id";
	$result = execute_sql($sql);
	foreach($questions as $qid) {
		$i[$qid] = 0;
	}
	while(list($qid, $cid) = fetch_row($result)) {
		$$qid[$i[$qid]++] = $cid;
	}
	db_close($result);
	foreach($questions as $qid) {
		$num = $i[$qid];
	}
	$cids = array();
	for($j=0;$j<$num;$j++) {
		if(in_array($$targetq[$j],$targetc)) {
			foreach($questions as $qid) {
				array_push($cids, $$qid[$j]);
			}
		}
	}
	return($cids);
}
/* }}} */
	
?>
