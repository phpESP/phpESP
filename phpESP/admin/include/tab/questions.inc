<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	if(empty($HTTP_SESSION_VARS['curr_q']))
		$HTTP_SESSION_VARS['curr_q'] = 0;
    if(empty($HTTP_POST_VARS['id']))
        $HTTP_POST_VARS['id'] = 0;

	$curr_q =& $HTTP_SESSION_VARS['curr_q'];
	$sid =& $HTTP_SESSION_VARS['survey_id'];
   	$id = intval($HTTP_POST_VARS['id']);

    if (isset($HTTP_POST_VARS['type_id']))
        $HTTP_POST_VARS['type_id'] = intval($HTTP_POST_VARS['type_id']) ?
                $HTTP_POST_VARS['type_id'] : '';
    else
        $HTTP_POST_VARS['type_id'] = '';

	// build array of question IDs
	$sql = "SELECT id FROM ".$GLOBALS['ESPCONFIG']['question_table']."
	WHERE survey_id=$sid AND deleted='N' AND type_id != 99
	ORDER BY position";
	$result = execute_sql($sql);
	$total_num_q = record_count($result);
	$q_ids = array();
	while(list($qi) = fetch_row($result)) {
        $result->MoveNext();
		array_push($q_ids, $qi);
    }
	db_close($result);

	// update failed, stay on same question
	if(!$updated || isset($HTTP_POST_VARS['extra_choices'])) {
		for ( $q = 0; $q < $total_num_q && $q_ids[$q] != $id ; $q++);
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
	if($curr_q && isset($q_ids[$curr_q-1]))
		$curr_q_id = $q_ids[$curr_q-1];
	else
		$curr_q_id = 0;

	$fields = array('name','type_id','length','precise','required','content','position');
	if($updated && $total_num_q > 0 && $curr_q) {
		// survey questions exist already
		// load values from DB
		$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id=${curr_q_id} AND deleted='N' ORDER BY position";
		$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
		$question = fetch_row($result);
		db_close($result);
		foreach($fields as $f) {
			$HTTP_POST_VARS[$f] =& $question[$f];
		}
	} else if ($updated) {
		// adding a new question (possibly because there are no questions yet)
		$curr_q = 0;
		$curr_q_id = '';
		foreach(array('name','length','precise','content','position') as $f) {
			$HTTP_POST_VARS[$f] = '';
		}
	} else {
		foreach($fields as $f) {
            if(!empty($HTTP_POST_VARS[$f]))
			    $HTTP_POST_VARS[$f] = _stripslashes($HTTP_POST_VARS[$f]);
            else
                $HTTP_POST_VARS[$f] = '';
		}
	}
?>
<script language="javascript">
<!-- // comment
function clearTextInputs() {
	var i = 1;
	while (document.phpesp.elements["choice_content_" + i]) {
		document.phpesp.elements["choice_content_" + i].value = "";
		i++;

        }
}
function validate() {
    return true;
}
// comment -->
</script>
<center>
	<input type="hidden" name="id" value="<?php echo($curr_q_id); ?>">
	<p>
	<?php echo(_('Edit this question, or click the number of the question you would like to edit:')); ?>
	</p>
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Question')); ?>">
	<hr>
	<b><?php
		if(!$curr_q) {
			echo(_('New Question'));
		} else {
			echo(_('Field')." $curr_q");
		} ?></b>
	<table border="0">
		<tr>
			<th>&nbsp;</th>
			<th><?php echo(_('Question Name')); ?></th>
			<th><?php echo(_('Type')); ?></th>
			<th><?php echo(_('Length')); ?></th>
			<th><?php echo(_('Precision')); ?></th>
			<th><?php echo(_('Required')); ?>?</th>
		</tr><tr>
			<td>&nbsp;</td>
			<td align="center"><?php echo(mktext('name',12)); ?></td>
			<td align="center"><?php
				if($updated && empty($HTTP_POST_VARS['type_id'])) $HTTP_POST_VARS['type_id'] = 2;
				$sql = 'SELECT id, type FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].' WHERE id != 99';
				$result = execute_sql($sql);
				$arr = array();
				while(list($key, $val) = fetch_row($result)) {
                    $result->MoveNext();
					$arr["$key"] = _($val);
				}
				echo(mkselect('type_id',$arr));
			?></td>
			<td align="center"><?php
				if(empty($HTTP_POST_VARS['length'])) $HTTP_POST_VARS['length'] = 0;
				echo(mktext("length",6));
			?></td>
			<td align="center"><?php
				if(empty($HTTP_POST_VARS['precise'])) $HTTP_POST_VARS['precise'] = 0;
				echo(mktext("precise",6));
			?></td>
			<td align="center"><?php
				if(empty($HTTP_POST_VARS['required'])) $HTTP_POST_VARS['required'] = 'N';
				echo(mkselect("required",array(
					"Y" => _('Yes') . '               ',
					"N" => _('No')
				))); ?></td>
		</tr>
		<tr>
			<th align="left" valign="top">Text</th>
			<td colspan="5" valign="top"><?php
				echo(mktextarea("content",4,60,"VIRTUAL"));
			?></td>
		</tr>
	</table>
<?php
	// has answer options ... so show that part of the form
    $sql = "SELECT has_choices
					FROM ".$GLOBALS['ESPCONFIG']['question_type_table']."
        WHERE id=" . $HTTP_POST_VARS['type_id'];
    $choices = get_one($sql);
	if($curr_q == 0 ||
            empty($HTTP_POST_VARS['type_id']) || $choices =='Y') {
		include($GLOBALS['ESPCONFIG']['include_path']."/tab/questions_options".$GLOBALS['ESPCONFIG']['extension']);
	}
?>
	<hr>
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Question')); ?>">
