<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// this type of question has answer options
	$sql="SELECT id,content FROM question_choices WHERE question_id='${curr_q_id}' ORDER BY id";
	$result=mysql_query($sql);
	$num_choices = mysql_num_rows($result);
	if(isset($extra_choices))
		$num_choices++;
	if($curr_q == "New")
		$num_choices=$cf_default_num_choices;
?>
<table border="0" cellspacing="0" width="100%">
	<tr><td colspan="2"><hr></td></tr>
	<tr>
		<td valign=TOP colspan="2">
			Enter the possible answers (if applicable). Enter
			<tt>!other</tt> on an line by itself to create a
			fill-in-the-blank answer at the end of this question.
			Any blank lines will be suppressed.
			<input type="hidden" name="num_choices" value="<?php echo($num_choices); ?>">
		</td>
	</tr>
<?php
		for($i=1;$i<$num_choices+1;++$i) {
			if($curr_q == "New") { $choice_id=''; $choice_content=''; }
			else { list($choice_id,$choice_content) = mysql_fetch_row($result); }
			if($dont_clear) { $choice_id=''; $choice_content=${"choice_content_$i"}; }
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
		<td><input type="submit" name="extra_choices" value="Add another answer line"></td>
	</tr>
</table>
