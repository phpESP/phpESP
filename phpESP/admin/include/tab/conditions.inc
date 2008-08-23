<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

?>

<input type="hidden" name="conditions" value="" />

<?php
$sid = $_SESSION['survey_id'];

$conditions[0]=_('equals');
$conditions[1]=_('is different from');
$conditions[2]=_('smaller than');
$conditions[3]=_('bigger than');

$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['condition_table']." WHERE survey_id=$sid ORDER BY id";
$result = execute_sql($sql);
if (record_count($result)) {
   echo(_('Current conditions defined:'));
   print "<br /><table>";

   $print_or=0;
   while( list($id, $tmp_sid, $qid1, $qid2, $cond, $cond_val) = fetch_row($result)) {
	   if ($print_or==1) {
	      print "<tr><td>"._('OR')."</td></tr>";
	   } else {
	      $print_or=1;
	   }
	   print "<tr><td>";
	   echo(_('Only show question '));
	   $sql2 = "SELECT content FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id=$qid1"; 
	   $result2=execute_sql($sql2);
	   list($content)=fetch_row($result2);
	   $content=strip_tags($content);
	   $dots = "";
	   if (strlen($content) > 30) $dots = "...";
	   print "'".substr($content,0, 30).$dots."' ";
	   echo(_('if answer to question '));
	   $sql2 = "SELECT content FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id=$qid2"; 
	   $result2=execute_sql($sql2);
	   list($content)=fetch_row($result2);
	   $content=strip_tags($content);
	   $dots = "";
	   if (strlen($content) > 30) $dots = "...";
	   print "'".substr($content,0, 30).$dots."' ";
	   echo $conditions[$cond]." '".$cond_val."'";
	   $result->MoveNext();
	   print " <input type=\"submit\" name=\"deletecondition[$id]\" size=5 value=\""._('Delete')."\" />";
	   print "</td></tr>";
   }
   print "</table><hr>";
}

$sql = "SELECT id,type_id,position,content FROM ".$GLOBALS['ESPCONFIG']['question_table']."
WHERE survey_id=$sid AND deleted='N'
ORDER BY position";
$result = execute_sql($sql);
$max = record_count($result);
?>
<div align=left>
<?php
   echo(_('All conditions are "OR" conditions, so if one conditions is fullfilled, the question is shown.<br />
A question that has a condition must be an optional question. So, if you define a condition on a question, the "Required" parameter is set to "No".<br />
A question that has a condition must be on the next page than the question/value that is used to compare it with. Use the "Order tab" to insert a section break where wanted (this is <b>NOT</b> done automatically).<br />
A Yes/No question can only be compared using the values "Yes or "No".'));
?>
</div>
<hr>
<?php
   echo(_('Add conditions to a question.'));
?>
<hr>
<?php
if ($max > 20) {
	$max = 20;
}
?>

<?php echo(_('Only show question ')); ?>
<select name="q1_id" id="q1_id" size="<?php echo($max)?>">
<?php
$i = 1;
while( list($qid, $tid, $pos, $content) = fetch_row($result)) {
	$result->MoveNext();
	$dots = "";
	$content = strip_tags($content);
	if($tid != 99) {
		if (strlen($content) > 30) {
			$dots = "...";
		}
		echo('<option value="'.$qid.'">'.$i++.'. '.substr($content,0, 30).$dots.'</option>');
	}
	else {
		//++$sec;
		echo('<option value="'.$qid.'"><strong>'._('----- Section Break -----').'</strong></option>\n');
	}
}
?>
</select>

<?php echo(_('if answer to question ')); ?>
<select name="q2_id" id="q2_id" size="<?php echo($max)?>">
<?php
$i = 1;
$result->MoveFirst();
while( list($qid, $tid, $pos, $content) = fetch_row($result)) {
	$result->MoveNext();
	$dots = "";
	$content = strip_tags($content);
	if($tid != 99) {
		if (strlen($content) > 30) {
			$dots = "...";
		}
		echo('<option value="'.$qid.'">'.$i++.'. '.substr($content,0, 30).$dots.'</option>');
	}
	else {
		//++$sec;
		echo('<option value="'.$qid.'"><strong>'._('----- Section Break -----').'</strong></option>\n');
	}
}

if ($result) {
   db_close($result);
}

?>
</select>

<select name="cond" id="cond">
<option value="0"><?php echo $conditions[0]; ?></option>
<option value="1"><?php echo $conditions[1]; ?></option>
<option value="2"><?php echo $conditions[2]; ?></option>
<option value="3"><?php echo $conditions[3]; ?></option>
</select>
<input type="text" name="cond_value" id="cond_value" size=5 value="NONE">
<input type="submit" name="addcondition" size=5 value="<?php echo(_('Go')); ?>" />
<hr />
<br /><br />
