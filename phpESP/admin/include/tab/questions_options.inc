<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// do not need to import session vars here
	// it is done by questions.inc before this code executes

	// this type of question has answer options
	if ($updated && $curr_q) {
		$sql = "SELECT id,content FROM question_choice WHERE question_id='${curr_q_id}' ORDER BY id";
		$result = mysql_query($sql);
		$c = mysql_num_rows($result);
	} else {
		if (isset($HTTP_POST_VARS['num_choices']))
			$c = intval($HTTP_POST_VARS['num_choices']);
		else
			$c = $GLOBALS['ESPCONFIG']['default_num_choices'];
	}
	if(isset($HTTP_POST_VARS['extra_choices']))
		$num_choices = max($c, $HTTP_POST_VARS['num_choices']) + 1;
	else
		$num_choices = $c;
?>
<table border="0" cellspacing="0" width="100%">
	<tr><td colspan="2"><hr></td></tr>
	<tr>
		<td valign=TOP colspan="2">
			<?php echo(_('Enter the possible answers (if applicable). Enter
<tt>!other</tt> on an line by itself to create a
fill-in-the-blank answer at the end of this question.
Any blank lines will be suppressed.')); ?>
			<input type="hidden" name="num_choices" value="<?php echo($num_choices); ?>">
		</td>
	</tr>
<?php
		for($i=1; $i<$num_choices+1; ++$i) {
			if ($updated && $curr_q) {
				list($choice_id, $choice_content) = mysql_fetch_row($result);
			} elseif ($curr_q) {
					$choice_id      = intval($HTTP_POST_VARS["choice_id_$i"]);
					$choice_content = $HTTP_POST_VARS["choice_content_$i"];
			} else {
					$choice_id = $choice_content = '';
			}
?>
	<tr>
		<td valign=middle align="right"><b><?php echo($i); ?>.</b></td>
		<td valign=middle align="left">
			<input type="hidden" name="choice_id_<?php echo($i); ?>"
				value="<?php echo($choice_id); ?>">
			<input type="text" size="60" name="choice_content_<?php echo($i); ?>"
				value="<?php echo(htmlspecialchars($choice_content)); ?>">
		</td>
	</tr>
<?php	} ?>
	<tr>
		<td>&nbsp;</td>
		<td><input type="submit" name="extra_choices" value="<?php echo(_('Add another answer line')); ?>"></td>
	</tr>
</table>