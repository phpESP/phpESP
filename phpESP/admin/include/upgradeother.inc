<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

	if(isset($runnow)) {
		$sql = "SELECT NOW()+0, COUNT(id), MAX(choice_id) FROM answers_other";
		$result = mysql_query($sql);
		list($o_now, $o_count, $o_maxcid) = mysql_fetch_row($result);
		mysql_free_result($result);

		$sql = "SELECT MAX(id) FROM question_choices";
		$result = mysql_query($sql);
		list($c_maxid) = mysql_fetch_row($result);
		mysql_free_result($result);
	}

	if(!isset($submit) || $o_now<1 || $o_maxcid<1 || $c_maxid<1) { ?>
<input type="hidden" name="where" value="upgradeother">
<table border=0>
<tr><td colspan=3>
	<p>If you are here, it means that you have been running the
	development version of <tt>phpESP</tt> right? If you haven't
	been, then you have no need to go to this page.</p>
	<p>Now hopefully you have *just* upgraded, or CVS up'ed. 
	If you ran the two following SQL queries before you upgraded
	(as recommended) then just enter the results in the form
	below and click on <tt>Go</tt>. Otherwise click on the 
	<tt>Run Now</tt> button below to run them now.</p>
	
	<p><tt>SELECT NOW()+0, COUNT(id), MAX(choice_id) FROM answers_other</tt><br>
	   <tt>SELECT MAX(id) FROM question_choices</tt></p>
	
	<center><input type="submit" name="runnow" value="Run Now"></center>
</td></tr>
<tr><td colspan=3><hr></td></tr>
<tr><th colspan=3 align="left">First Query</th></tr>
<tr><th>NOW()+0</th><th>COUNT(id)</th><th>MAX(choice_id)</th></tr>
<tr><td><input type="text" name="o_now" size="20" value="<?php echo($o_now); ?>"></td>
    <td><input type="text" name="o_count" size="20" value="<?php echo($o_count); ?>"></td>
    <td><input type="text" name="o_maxcid" size="20" value="<?php echo($o_maxcid); ?>"></td></tr>
<tr><th colspan=3 align="left">Second Query</th></tr>
<tr><th>MAX(choice_id)</th><th></th><th></th></tr>
<tr><td><input type="text" name="c_maxid" size="20" value="<?php echo($c_maxid); ?>"></td><td></td><td></td></tr>
<tr><th colspan=3><input type="submit" name="submit" value="Go"></th></tr>
<tr><td colspan=3><hr></td></tr>
</table>
<?php
		return;
	}
	
	
	$min = min($o_maxcid, $c_maxid);

// --------------------------- SAFE ---------------------------
	$safe_aids = array();
	$safe_qids = array();
	$sql = "
SELECT A.id
  FROM answers_other A, survey_responses R
 WHERE A.response_id=R.id AND
       R.submitted < $o_now AND
	   A.choice_id <= $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($safe_aids, $row[0]);
	}
	mysql_free_result($result);
	$sql = "
SELECT DISTINCT A.question_id
  FROM answers_other A, survey_responses R
 WHERE A.response_id=R.id AND
       R.submitted < $o_now AND
	   A.choice_id <= $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($safe_qids, $row[0]);
	}
	mysql_free_result($result);
	
// --------------------------- MAYBE ---------------------------
	$maybe_aids = array();
	$maybe_qids = array();
	$sql = "
SELECT A.id
  FROM answers_other A, survey_responses R
 WHERE A.response_id=R.id AND
       R.submitted >= $o_now AND
	   A.choice_id <= $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($maybe_aids, $row[0]);
	}
	mysql_free_result($result);
	$sql = "
SELECT DISTINCT A.question_id
  FROM answers_other A, survey_responses R
 WHERE A.response_id=R.id AND
       R.submitted >= $o_now AND
	   A.choice_id <= $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($maybe_qids, $row[0]);
	}
	mysql_free_result($result);
	
// --------------------------- BAD ---------------------------
	$bad_aids = array();
	$bad_qids = array();
	$sql = "
SELECT A.id
  FROM answers_other A
 WHERE A.choice_id > $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($bad_aids, $row[0]);
	}
	mysql_free_result($result);
	$sql = "
SELECT DISTINCT A.question_id
  FROM answers_other A
 WHERE A.choice_id > $min";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		array_push($bad_qids, $row[0]);
	}
	mysql_free_result($result);

// ---------------------------------------------------------
	$final = array();
	foreach($safe_qids as $qid) {
		$min = 0;
		$map = array();
		
		$sql = "
SELECT MIN(C.id)
  FROM question_choices C
 WHERE C.content LIKE '!other%' AND
       C.question_id = $qid";
		$result = mysql_query($sql);
		$min = mysql_result($result,0,0);
		mysql_free_result($result);
		
		$sql = "
SELECT C.id-$min AS 'from', C.id AS 'to'
  FROM question_choices C
 WHERE C.content LIKE '!other%' AND
       C.question_id = $qid";
		$result = mysql_query($sql);
		while($row = mysql_fetch_row($result)) {
			$old = $row[0];
			$new = $row[1];
			$map[$old]=$new;
			array_push($final,
"UPDATE answers_other SET choice_id='$new' WHERE question_id=$qid AND choice_id='$old';");
		}
		mysql_free_result($result);
	}
// ---------------------------------------------------------
?>
<table border=0>
<tr><th align="left">Safe AID's</th></tr>
<tr><td><?php echo(join(", ",$safe_aids)); ?>&nbsp;</td></tr>
<tr><th align="left">Maybe Safe AID's</th></tr>
<tr><td><?php echo(join(", ",$maybe_aids)); ?>&nbsp;</td></tr>
<tr><th align="left">Bad AID's</th></tr>
<tr><td><?php echo(join(", ",$bad_aids)); ?>&nbsp;</td></tr>
<tr><td><hr></td></tr>
<tr><th align="left">Safe QID's</th></tr>
<tr><td><?php echo(join(", ",$safe_qids)); ?>&nbsp;</td></tr>
<tr><th align="left">Maybe Safe QID's</th></tr>
<tr><td><?php echo(join(", ",$maybe_qids)); ?>&nbsp;</td></tr>
<tr><th align="left">Bad QID's</th></tr>
<tr><td><?php echo(join(", ",$bad_qids)); ?>&nbsp;</td></tr>
<tr><td><hr></td></tr>
<tr><th align="left">SQL</th></tr>
<tr><td><pre><?php echo(join("\n",$final)); ?></pre></td></tr>
</table>
