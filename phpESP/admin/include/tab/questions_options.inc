<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

// New Interface Design by Kon Angelopoulos
// for the phpESP project.
// <angekproductions@iprimus.com.au>

	// do not need to import session vars here
	// it is done by questions.inc before this code executes

	// this type of question has answer options
	if ($updated && $curr_q) {
		$sql = "SELECT id,content,feedback,credit FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=${curr_q} ORDER BY id";
		$result = execute_sql($sql);
		$c = record_count($result);
	} else {
		if ($curr_q && isset($_POST['num_choices']))
			$c = intval($_POST['num_choices']);
		else
			$c = $GLOBALS['ESPCONFIG']['default_num_choices'];
	}
	if(isset($_POST['extra_choices']))
		$num_choices = max($c, $_POST['num_choices']) + 1;
	else
		$num_choices = $c;
?>
<table id="answerlines" cellspacing="0">
	<tr><td colspan="4"><hr /></td></tr>
	<tr>
		<td colspan="4">
			<?php printf(_('Enter the possible answers (if applicable). Enter
%s on an line by itself to create a
fill-in-the-blank answer at the end of this question.
Any blank lines will be suppressed.'), '<tt>!other</tt>'); ?>
			<?php echo '<p>' . _('To assign numeric credit for a particular
            answer, enter the associated credit beside each possible
            answer.  Only question types of Radio Buttons, Check Boxes, and
            Dropdown Box may have credit. Credit for a multiple selection is
            the sum of the individual credit.') . '</p>'; ?>
			<?php echo '<p>' . _('To provide feedback to the survey respondent when
            selecting a particular answer, enter that feedback beside the
            answer.  Only question types of Radio Buttons, Check Boxes, and
            Dropdown Box may have feedback. The feedback may include HTML, if
            desired.') . '</p>'; ?>
			<input type="hidden" id="num_choices" name="num_choices" value="<?php echo($num_choices); ?>" />
		</td>
	</tr>
    <tr>
       <th>&nbsp;</th>
       <th><?= _('Answer') ?></th>
       <th><?= _('Credit') ?></th>
       <th><?= _('Feedback') ?></th>
    </tr>
<?php
	for($i=1; $i<$num_choices+1; ++$i) {
		if ($updated && $curr_q) {
			list($choice_id, $choice_content, $choice_feedback, $choice_credit) = fetch_row($result);
			$result->MoveNext();
		} else {
			if ($curr_q) {
			   $choice_id = intval($_POST["choice_id_$i"]);
			   if(isset($_POST["choice_content_$i"]))
			      $choice_content  = _stripslashes($_POST["choice_content_$i"]);
			      $choice_credit   = _stripslashes($_POST["choice_credit_$i"]);
			      $choice_feedback = _stripslashes($_POST["choice_feedback_$i"]);
			}
		}
?>
	   <tr>
		<td class="numbered"><?php echo($i); ?>.</td>
		<td class="left">
			<input type="hidden" name="choice_id_<?php echo($i); ?>"
				value="<?php if(isset($choice_id)) echo($choice_id); ?>" />
			<input type="text" size="60" name="choice_content_<?php echo($i); ?>"
				value="<?php if(isset($choice_content)) echo(htmlspecialchars($choice_content)); ?>" />
		</td>
        <td class="left"><input type="text" name="choice_credit_<?php echo($i); ?>" value="<?php if(isset($choice_credit)) echo(htmlspecialchars($choice_credit)); ?>" size="4" /></td>
        <td class="left"><textarea name="choice_feedback_<?php echo($i); ?>"><?php if(isset($choice_feedback)) echo(htmlspecialchars($choice_feedback)); ?></textarea></td>
	   </tr>
<?php	} ?>
	<tr>
		<td>&nbsp;</td>
		<td class="left"><input type="button" name="extra_choices" onclick="addAnswerLine()" value="<?php echo(_('Add another answer line')); ?>" /> <input type="button" name="clear" value="<?php echo(_('Clear all answer lines')); ?>" onclick="clearTextInputs()" /></td>
        <td>&nbsp;</td>
	</tr>
</table>
