<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");
	session_register("new_survey");
	session_register("curr_q");		// curr_q is session variable
	// q is form variable

	// build array of question IDs
	$sql = "SELECT id FROM survey_questions WHERE survey_id='${survey_id}' AND status=0 AND type_id!=99 ORDER BY position";
	$result = mysql_query($sql);
	$total_num_q = mysql_num_rows($result);
	$q_ids = array();
	for($i=0;$i<$total_num_q;$i++) {
		array_push($q_ids,mysql_result($result,$i,0));
	}

	if($extra_choices && empty($q)) {
		$q=1;
		while($q_ids[$q-1] != $id && $q <= $total_num_q) {
			$q++;
		}
	}

	if(!$dont_clear) {
		if(isset($q) && !empty($q))
			$curr_q = $q;
		if(stristr($curr_q,'new'))
			$curr_q = 0;
	}

	$fields = array('type_id','result_id','parent_id','required','params','content');

	if($total_num_q > 0 && $curr_q) {
		// survey questions exist already
		// 1st sanity check the question #
		if(empty($curr_q) || $curr_q<1 || $curr_q>$total_num_q) {
			$curr_q = 1;
		}
		$curr_q_id = $q_ids[$curr_q-1];
		if(!$dont_clear) {
			// load values from DB
			$sql = "SELECT " . join(',',$fields) . " FROM survey_questions WHERE id='${curr_q_id}' AND status='' ORDER BY position";
			$result = mysql_query($sql);
			$arr = mysql_fetch_array($result);
			foreach($fields as $f) {
				$$f = $arr["$f"];
			}
			mysql_free_result($result);
		}
	} else {
		// adding a new question (possibly because there are no questions yet)
		$curr_q = "New";
		$curr_q_id = '';
		if(!$dont_clear)
			foreach($fields as $f) { $$f = ''; }
	}
?>
<table width="100%" border="0" cellspacing="0">
	<tr>
		<td colspan="4" align="center">
			<input type="hidden" name="id" value="<?php echo($curr_q_id); ?>">
			Edit this question, or click the number of the question you would
			like to edit:<br>
<?php for($i=1;$i<$total_num_q+1;++$i) { ?>
			<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
			<input type="submit" name="q" value="New Question">
		</td>
	</tr>
	<tr>
		<td colspan="4"><hr></td>
	</tr>
	<tr>
		<td valign=TOP><b>Question <?php if(!stristr("new",$curr_q)) echo($curr_q); ?></b></td>
		<td valign=TOP colspan="3">
			<?php echo(mktextarea("content",4,60,"VIRTUAL")); ?>

		</td>
	</tr>
	<tr>
		<td><b>Answer Required</b></td>
		<td colspan="3">
			<?php echo(mkselect("required",array("Y" => "Yes", "N" => "No"))); ?>
			(Default: No)
		</td>
	</tr>
	<tr>
		<td valign=TOP><b>Type of Response</b></td>
		<td valign=TOP>
<?php
	$sql = "SELECT * FROM question_types WHERE id < 50 ORDER BY name";
	$result = mysql_query($sql);
	while ( $type = mysql_fetch_array($result) ) {
		echo(mkradio("type_id",$type["id"]) . " " . $type["name"] . "<br>\n");
	}
	mysql_free_result($result);
?>
		</td>
		<td valign=TOP><b>Result Type</b></td>
		<td valign=TOP>
<?php
	$sql = "SELECT * FROM result_types ORDER BY name";
	$result = mysql_query($sql);
	while ( $type = mysql_fetch_array($result) ) {
		echo(mkradio("result_id",$type["id"]) . " " . $type["name"] . "<br>\n");
	}
	mysql_free_result($result);
?>
		</td>
	</tr>
</table>
<?php
	// has answer options ... so show that part of the form
	if($curr_q == "New" || empty($type_id) || mysql_result(mysql_query("SELECT has_answers FROM question_types WHERE id='${type_id}'"),0,0)) {
		include($PIECES."/question_options".$EXT);
	}
?>
<hr>
<?php for($i=1;$i<$total_num_q+1;++$i) { ?>
			<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
			<input type="submit" name="q" value="New Question">
