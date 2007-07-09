<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto string survey_report(int survey_id, bool details, string format)
    Build a description of a survey, including all unique ids.
	Rerturns an empty string on success, else an error message. */
function survey_report($sid, $details = 0, $format = '') {
	if(empty($sid)) return;
	$bg = '';
	// build associative array holding weather each question
	// type has answer choices or not and the table the answers are in
	$has_choices = array();
	$response_table = array();
	$sql = 'SELECT id,has_choices,response_table
			  FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].'
			 ORDER BY id';
	if(!($result = execute_sql($sql))) {
		$errmsg = _('Error system table corrupt.') ." [ ". _('Table') .": question_type ]";
		return($errmsg);
	}
	while($row = fetch_row($result)) {
        $result->MoveNext();
		$has_choices[$row[0]]=$row[1];
		$response_table[$row[0]]=$row[2];
	}
	db_close($result);

	// load survey title (and other globals)
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=$sid";
	if(!($result = execute_sql($sql,"",ADODB_FETCH_ASSOC))) {
		$errmsg = _('Error opening survey.') ." [ ID:${sid} R:" . record_count($result) ."]";
		return($errmsg);
	}
	$survey = fetch_row($result);
	db_close($result);

	// load survey questions
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			 WHERE survey_id=$sid AND deleted='N'
			 ORDER BY position";
	if(!($questions_result = execute_sql($sql,"",ADODB_FETCH_ASSOC))) {
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
	<th align="center" colspan="2"><?php echo(_('Content')); ?></th>
</tr>
<?php
	$i = 0;
	while($question = fetch_row($questions_result)) {
        $questions_result->MoveNext();
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
 	<td align="left"><?php if ($tid < 50) echo(++$i); ?></td>
	<th align="left"><?php echo($qid); ?></th>
	<td align="left"><?php echo($reqd); ?></td>
	<td align="left"><?php echo($publ); ?></td>
	<td colspan="2"><?php echo($question["content"]); ?></td>
</tr>
<?php
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
					 WHERE question_id = $qid
					 ORDER BY id";
 			$choices_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			while($choice = fetch_row($choices_result)) {
                $choices_result->MoveNext();
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
			db_close($choices_result);
		} // end if has_choices
	} // end while
	db_close($questions_result);
?>
</table>
<?php
	return;
}
/* }}} */

?>
