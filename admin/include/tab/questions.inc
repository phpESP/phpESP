<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if(empty($HTTP_SESSION_VARS['curr_q']))
		$HTTP_SESSION_VARS['curr_q'] = 0;
    if(empty($HTTP_POST_VARS['id']))
        $HTTP_POST_VARS['id'] = 0;
	
	$curr_q =& $HTTP_SESSION_VARS['curr_q'];
	$sid =& $HTTP_SESSION_VARS['survey_id'];
   	$id = intval($HTTP_POST_VARS['id']);

	// build array of question IDs
	$sql = "SELECT id FROM question
	WHERE survey_id='$sid' AND deleted='N' AND type_id != 99
	ORDER BY position";
	$result = mysql_query($sql);
	$total_num_q = mysql_num_rows($result);
	$q_ids = array();
	while(list($qi) = mysql_fetch_row($result))
		array_push($q_ids, $qi);
	mysql_free_result($result);
	
	// update failed, stay on same question
	if(!$updated || isset($HTTP_POST_VARS['extra_choices'])) {
		for ( $q = 0; $q < $total_num_q && $q_ids[$q] != $id ; $q++);
		$HTTP_POST_VARS['q'] = ++$q;
		unset($q);
	} else {
		if(empty($HTTP_POST_VARS['q']))
			$HTTP_POST_VARS['q'] = $curr_q;
		if($HTTP_POST_VARS['q'] == _('New Field'))
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
		$sql = "SELECT * FROM question WHERE id='${curr_q_id}' AND deleted='N' ORDER BY position";
		$result = mysql_query($sql);
		$question = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
		foreach($fields as $f) {
			$HTTP_POST_VARS[$f] =& $question[$f];
			$$f =& $question[$f];
		}
	} else if ($updated) {
		// adding a new question (possibly because there are no questions yet)
		$curr_q = 0;
		$curr_q_id = '';
		if(!empty($HTTP_POST_VARS['type_id']))
			$type_id =& $HTTP_POST_VARS['type_id'];
		if(!empty($HTTP_POST_VARS['required']))
			$required =& $HTTP_POST_VARS['required'];
		foreach(array('name','length','precise','content','position') as $f) {
			$HTTP_POST_VARS[$f] = '';
			$$f =& $HTTP_POST_VARS[$f];
		}
	} else {
		foreach($fields as $f) {
            if(!empty($HTTP_POST_VARS[$f])) 
			    $$f = DESLASH($HTTP_POST_VARS[$f]);
            else
                $$f = '';
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
// comment -->
</script>
<center>
	<input type="hidden" name="id" value="<?php echo($curr_q_id); ?>">
	<p>
	<?php echo(_('Edit this field, or click the number of the field you would like to edit:')); ?>
	</p>
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Field')); ?>">
	<hr>
	<b><?php
		if(!$curr_q) { 
			echo(_('New Field'));
		} else {
			echo(_('Field')." $curr_q");
		} ?></b>
	<table border="0">
		<tr>
			<th>&nbsp;</th>
			<th><?php echo(_('Field Name')); ?></th>
			<th><?php echo(_('Type')); ?></th>
			<th><?php echo(_('Length')); ?></th>
			<th><?php echo(_('Precision')); ?></th>
			<th><?php echo(_('Required?')); ?></th>
		</tr><tr>
			<td>&nbsp;</td>
			<td align="center"><?php echo(mktext('name',12)); ?></td>
			<td align="center"><?php
				if($updated && empty($GLOBALS['type_id'])) $GLOBALS['type_id'] = 2;
				$sql = 'SELECT id, type FROM question_type WHERE id != 99';
				$result = mysql_query($sql);
				$arr = array();
				while(list($key, $val) = mysql_fetch_row($result)) {
					$arr["$key"] = _($val);
				}
				echo(mkselect('type_id',$arr));
			?></td>
			<td align="center"><?php
				if(empty($GLOBALS['length'])) $GLOBALS['length'] = 0; 
				echo(mktext("length",6));
			?></td>
			<td align="center"><?php
				if(empty($GLOBALS['precise'])) $GLOBALS['precise'] = 0; 
				echo(mktext("precise",6));
			?></td>
			<td align="center"><?php
				if(empty($GLOBALS['required'])) $GLOBALS['required'] = 'N';
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
	if($curr_q == 0 || 
			empty($type_id) ||
			mysql_result(mysql_query("SELECT has_choices
					FROM question_type
					WHERE id='${type_id}'"),0,0)=='Y') {
		include($GLOBALS['ESPCONFIG']['include_path']."/tab/questions_options".$GLOBALS['ESPCONFIG']['extension']);
	}
?>
	<hr>
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>">
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Field')); ?>">
