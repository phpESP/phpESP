<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if(!defined('_LIB_ESPMERGE')) {
 	define('_LIB_ESPMERGE',TRUE);

/* {{{ proto array merge_expand_qids(int target_qid, array surveys)
   Returns an array of question ids spanning the given
   survey ids. */
function merge_expand_qids($targetq, $surveys) {
	$sql = "SELECT survey_id FROM question WHERE id ". array_to_insql($targetq);
	$result = mysql_query($sql);
	$targets = mysql_result($result, 0, 0);
	mysql_free_result($result);

	$sidstr = array_to_insql($surveys);
	$sql = "
SELECT Q.survey_id, Q.id
FROM question Q
WHERE Q.survey_id ${$sidstr} AND deleted='N'
ORDER BY Q.survey_id, Q.position, Q.id";
	$result = mysql_query($sql);
	foreach($surveys as $sid) {
		$i[$sid] = 0;
	}
	while(list($sid, $qid) = mysql_fetch_row($result)) {
		$$sid[$i[$sid]++] = $qid;
	}
	mysql_free_result($result);
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
	$sql = "SELECT question_id FROM question_choice WHERE id ". array_to_insql($targetc);
	$result = mysql_query($sql);
	$targetq = mysql_result($result, 0, 0);
	mysql_free_result($result);

	array_push($questions,$targetq);
	$qidstr = array_to_insql($questions);
	$sql = "
SELECT C.question_id, C.id
FROM question_choice C
WHERE C.question_id ${$qidarr}
ORDER BY C.question_id, C.id";
	$result = mysql_query($sql);
	foreach($questions as $qid) {
		$i[$qid] = 0;
	}
	while(list($qid, $cid) = mysql_fetch_row($result)) {
		$$qid[$i[$qid]++] = $cid;
	}
	mysql_free_result($result);
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
	
} // end _LIB_ESPMERGE
?>
