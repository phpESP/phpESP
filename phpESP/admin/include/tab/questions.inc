<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	if(empty($_SESSION['curr_q']))
		$_SESSION['curr_q'] = 0;
    if(empty($_POST['id']))
        $_POST['id'] = 0;

	$curr_q =& $_SESSION['curr_q'];
	$sid =& $_SESSION['survey_id'];
   	$id = intval($_POST['id']);

    if (isset($_POST['type_id']))
        $_POST['type_id'] = intval($_POST['type_id']) ?
                $_POST['type_id'] : '';
    else
        $_POST['type_id'] = '';

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
	if(!$updated || isset($_POST['extra_choices'])) {
		for ( $q = 0; $q < $total_num_q && $q_ids[$q] != $id ; $q++);
		$_POST['q'] = ++$q;
		unset($q);
	} else {
		if(empty($_POST['q']))
			$_POST['q'] = $curr_q;
		if($_POST['q'] == _('New Question'))
			$_POST['q'] = 0;
		if($_POST['q']<0 || $_POST['q']>$total_num_q)
			$_POST['q'] = 1;
	}
	$curr_q = $_POST['q'];
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
			$_POST[$f] =& $question[$f];
		}
	} else if ($updated) {
		// adding a new question (possibly because there are no questions yet)
		$curr_q = 0;
		$curr_q_id = '';
		foreach(array('name','length','precise','content','position') as $f) {
			$_POST[$f] = '';
		}
	} else {
		foreach($fields as $f) {
            if(!empty($_POST[$f]))
			    $_POST[$f] = _stripslashes($_POST[$f]);
            else
                $_POST[$f] = '';
		}
	}
?>
<script type="text/javascript">
<!-- // comment
function clearTextInputs() {
	var i = 1;
    while (document.forms[1].elements["choice_content_" + i]) {
        document.forms[1].elements["choice_content_" + i].value = "";
        i++;
    }
}

function addAnswerLine() {
    var el = document.getElementById('answerlines');
    var numchoice = document.getElementById('num_choices').value;
    numchoice++;
    var tablerow = el.insertRow(numchoice+1);
    var tablecell = tablerow.insertCell(-1);
    tablecell.innerHTML = numchoice+".";
    tablecell.className = "numbered";
    tablecell = tablerow.insertCell(-1);
    var text = "<input type=\"hidden\" name=\"choice_id_"+numchoice+"\" value=\"\" />\n";
    text = text+"<input type=\"text\" size=\"60\" name=\"choice_content_"+numchoice+"\" value=\"\" />\n";
    tablecell.innerHTML = text;
    tablecell.className = "left";
    document.getElementById('num_choices').value = numchoice;
}
    
    
function validate() {
    return true;
}
// comment -->
</script>
	<input type="hidden" name="id" value="<?php echo($curr_q_id); ?>" />
	<p>
	<?php echo(_('Edit this question, or click the number of the question you would like to edit:')); ?>
	</p>
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>" />
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Question')); ?>" />
	<hr /> 
	<strong><?php
		if(!$curr_q) {
			echo(_('New Question'));
		} else {
			echo(_('Question')." $curr_q");
		} ?></strong>
	<table>
		<tr>
			<th>&nbsp;</th>
			<th><?php echo(_('Question Name')); ?></th>
			<th><?php echo(_('Type')); ?></th>
			<th><?php echo(_('Length')); ?></th>
			<th><?php echo(_('Precision')); ?></th>
			<th><?php echo(_('Required')); ?>?</th>
		</tr><tr>
			<td>&nbsp;</td>
			<td><?php echo(mktext('name',12)); ?></td>
			<td><?php
				if($updated && empty($_POST['type_id'])) $_POST['type_id'] = 2;
				$sql = 'SELECT id, type FROM '.$GLOBALS['ESPCONFIG']['question_type_table'].' WHERE id != 99';
				$result = execute_sql($sql);
				$arr = array();
				while(list($key, $val) = fetch_row($result)) {
                    $result->MoveNext();
					$arr["$key"] = _($val);
				}
				echo(mkselect('type_id',$arr));
			?></td>
			<td><?php
				if(empty($_POST['length'])) $_POST['length'] = 0;
				echo(mktext("length",6));
			?></td>
			<td><?php
				if(empty($_POST['precise'])) $_POST['precise'] = 0;
				echo(mktext("precise",6));
			?></td>
			<td><?php
				if(empty($_POST['required'])) $_POST['required'] = 'N';
				echo(mkselect("required",array(
					"Y" => _('Yes') . '               ',
					"N" => _('No')
				))); ?></td>
		</tr>
		<tr>
			<th>Text</th>
			<td colspan="5"><?php
				echo(mktextarea("content",4,60,"VIRTUAL"));
			?></td>
		</tr>
	</table>
<?php
	// has answer options ... so show that part of the form
    $sql = "SELECT has_choices
					FROM ".$GLOBALS['ESPCONFIG']['question_type_table']."
        WHERE id=" . $_POST['type_id'];
    $choices = get_one($sql);
	if($curr_q == 0 ||
            empty($_POST['type_id']) || $choices =='Y') {
		include($GLOBALS['ESPCONFIG']['include_path']."/tab/questions_options".$GLOBALS['ESPCONFIG']['extension']);
	}
?>
	<hr />
<?php for($i=1; $i<$total_num_q+1; ++$i) { ?>
	<input type="submit" name="q" value="<?php echo($i); ?>" />
<?php } ?>
	<input type="submit" name="q" value="<?php echo(_('New Question')); ?>" />
