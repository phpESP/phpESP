<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if(empty($HTTP_SESSION_VARS['curr_q']))
		$HTTP_SESSION_VARS['curr_q'] = 0;
	
	$curr_q =& $HTTP_SESSION_VARS['curr_q'];
	$sid = $HTTP_SESSION_VARS['survey_id'];

	// build array of question IDs
	$sql = "SELECT id
	FROM question
	WHERE survey_id='${sid}' AND
		deleted='N' AND
		type_id != 99
	ORDER BY position";
	$result = mysql_query($sql);
	$total_num_q = mysql_num_rows($result);
	$q_ids = array();
	for($i=0;$i<$total_num_q;$i++) {
		array_push($q_ids,mysql_result($result,$i,0));
	}
	mysql_free_result($result);
	
	// update failed, stay on same question
	if(!$updated || isset($HTTP_POST_VARS['extra_choices'])) {
		$q=0;
		while($q_ids[$q] != $HTTP_POST_VARS['id'] && $q < $total_num_q) {
			$q++;
		}
		$HTTP_POST_VARS['q'] = ++$q;
		unset($q);
	} else {
		if(empty($HTTP_POST_VARS['q']))
			$HTTP_POST_VARS['q'] = $curr_q;
		if($HTTP_POST_VARS['q'] == _('New Question'))
			$HTTP_POST_VARS['q'] = 0;
		if($HTTP_POST_VARS['q']<0 || $HTTP_POST_VARS['q']>$total_num_q)
			$HTTP_POST_VARS['q'] = 1;
	}
	$curr_q = $HTTP_POST_VARS['q'];
	if($curr_q)
		$curr_q_id = $q_ids[$curr_q-1];
	else
		$curr_q_id = 0;

	$fields = array('type_id','result_id','position','parent_id','required','private','content');
	if($updated && $total_num_q > 0 && $curr_q) {
		// survey questions exist already
		// load values from DB
		$sql = "SELECT * FROM question WHERE id='${curr_q_id}' AND deleted='N' ORDER BY position";
		$result = mysql_query($sql);
		$question = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
		foreach($fields as $f) {
			$HTTP_POST_VARS[$f] = $question[$f];
			$$f = $question[$f];
		}
	} else if ($updated) {
		// adding a new question (possibly because there are no questions yet)
		$curr_q = 0;
		$curr_q_id = '';
		foreach($fields as $f) {
			$HTTP_POST_VARS[$f] = '';
			$$f = '';
		}
	} else {
		foreach($fields as $f) {
			$$f = $HTTP_POST_VARS[$f];
		}
	}
?>
<table width="100%" border="0" cellspacing="0">
	<tr>
		<td colspan="4" align="center">
			<input type="hidden" name="id" value="<?php echo($curr_q_id); ?>">
			<?php echo(_('Edit this question, or click the number of the question you would like to edit:')); ?><br>
<?php for($i=1;$i<$total_num_q+1;++$i) { ?>
			<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
			<input type="submit" name="q" value="<?php echo(_('New Question')); ?>">
		</td>
	</tr>
	<tr>
		<td colspan="4"><hr></td>
	</tr>
	<tr>
		<td valign=TOP><b><?php if(!$curr_q){echo(_('New Question'));}else{echo(_('Question')." $curr_q");} ?></b></td>
		<td valign=TOP colspan="3">
			<?php echo(mktextarea("content",4,60,"VIRTUAL")); ?>

		</td>
	</tr>
	<tr>
		<td><b><?php echo(_('Answer Required')); ?></b></td>
		<td colspan="3">
			<?php echo(mkselect("required",array("Y" => _('Yes'), "N" => _('No')))); ?>
			(<?php echo(_('Default').': '._('No')); ?>)
		</td>
	</tr>
	<tr>
		<td valign=TOP><b><?php echo(_('Type of Response')); ?></b></td>
		<td valign=TOP>
<?php
	$sql = "SELECT id,type FROM question_type WHERE id < 50 ORDER BY type";
	$result = mysql_query($sql);
	while ( list($tid, $tname) = mysql_fetch_row($result) ) {
		echo(mkradio("type_id",$tid) .' '. _($tname) ."<br>\n");
	}
	mysql_free_result($result);
?>
		</td>
		<td valign=TOP><b><?php echo(_('Result Type')); ?></b></td>
		<td valign=TOP>
<?php
	$sql = "SELECT id,type FROM result_type ORDER BY type";
	$result = mysql_query($sql);
	while ( list($tid, $tname) = mysql_fetch_array($result) ) {
		echo(mkradio("result_id",$tid) .' '. _($tname) ."<br>\n");
	}
	mysql_free_result($result);
?>
		</td>
	</tr>
</table>
<?php
	// has answer options ... so show that part of the form
	if($curr_q == 0 || 
		empty($type_id) ||
		mysql_result(mysql_query("SELECT has_choices
		FROM question_type
		WHERE id='${type_id}'"),0,0)=='Y') {
		include($GLOBALS['ESPCONFIG']['include_path']."/tab/questions_options".$GLOBALS['ESPCONFIG']['extension']);
	}
?>
<hr>
<?php for($i=1;$i<$total_num_q+1;++$i) { ?>
			<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
			<input type="submit" name="q" value="<?php echo(_('New Question')); ?>">
