<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_SURVEY_REPORT')) {
 	define('_FUNCTION_SURVEY_REPORT',TRUE);
	
/* {{{ proto string survey_report(int survey_id, bool details, string format)
    Build a description of a survey, including all unique ids.
	Rerturns an empty string on success, else an error message. */
function survey_report($sid, $details = 0, $format = '') {
	if(empty($sid)) return;
	// build associative array holding weather each question
	// type has answer choices or not and the table the answers are in
	$has_choices = array();
	$response_table = array();
	$sql = 'SELECT id,has_choices,response_table
			  FROM question_type 
			 ORDER BY id';
	if(!($result = mysql_query($sql))) {
		$errmsg = _('Error system table corrupt.') ." [ ". _('Table') .": question_type ]";
		return($errmsg);
	}
	while($row = mysql_fetch_row($result)) {
		$has_choices[$row[0]]=$row[1];
		$response_table[$row[0]]=$row[2];
	}
	mysql_free_result($result);

	// load survey title (and other globals)
	$sql = "SELECT * FROM survey WHERE id='$sid'";
	if(!($result = mysql_query($sql))) {
		$errmsg = _('Error opening survey.') ." [ ID:${sid} R:" . mysql_num_rows($result) ."]";
		return($errmsg);
	}
	$survey = mysql_fetch_array($result, MYSQL_ASSOC);
	mysql_free_result($result);

	// load survey questions
	$sql = "SELECT * FROM question 
			 WHERE survey_id='$sid' AND deleted='N'
			 ORDER BY position";
	if(!($questions_result = mysql_query($sql))) {
		$errmsg = _('Error opening survey.') .' '. _('No questions found.') ." [ ID:${sid} ]";
		return($errmsg);
	}
?>
<h2><?php echo(_('Report for') .': '. $survey["title"] .' ['. _('ID') .': '. $survey['id'] .']'); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<table border="0" cellspacing="2" cellpadding="0" width="100%">
<tr>
	<th align="left"><?php echo(_('#')); ?></th>
	<th align="left"><?php echo(_('ID')); ?></th>
	<th align="left"><?php echo(_("Req'd")); ?></th>
	<th align="left"><?php echo(_('Public')); ?></th>
	<th align="left" colspan="2"><?php echo(_('Content')); ?></th>
</tr>
<?php
	$i = 0;
	while($question = mysql_fetch_array($questions_result, MYSQL_ASSOC)) {
		// process each question
		$qid   = &$question['id'];
		$tid   = &$question['type_id'];
		$reqd  = ($question['required'] == 'Y') ?
					_('Yes') : _('No');
		$publ  = ($question['public'] == 'Y') ?
					_('Yes') : _('No');
		$table = &$response_table[$tid];

		if($tid == 99) {
			echo("<tr><td colspan=\"6\"><hr></td></tr>\n");
			continue;
		}

		if($bg != '#ffffff')	$bg = '#ffffff';
		else					$bg = '#eeeeee';

?>
<tr bgcolor="<?php echo($bg); ?>">
 	<td align="right"><?php if ($tid < 50) echo(++$i); ?></td>
	<th align="right"><?php echo($qid); ?></th>
	<td><?php echo($reqd); ?></td>
	<td><?php echo($publ); ?></td>
	<td colspan="2"><?php echo($question["content"]); ?></td>
</tr>
<?php
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM question_choice
					 WHERE question_id = $qid
					 ORDER BY id";
 			$choices_result = mysql_query($sql);
			while($choice = mysql_fetch_array($choices_result, MYSQL_ASSOC)) {
?>
<tr bgcolor="<?php echo($bg); ?>"><th align="left"></th>
	<td></td>
	<td>&nbsp;</td>
	<td>&nbsp;</td>
	<th align="right"><?php echo($choice['id']); ?></th>
	<td><?php echo($choice['content']); ?></td>
</tr>
<?php
			}
			mysql_free_result($choices_result);
		} // end if has_choices
	} // end while
	mysql_free_result($questions_result);
?>
</table>
<?php
	return;
}
/* }}} */

} // end _FUNCTION_SURVEY_REPORT
?>
