<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

if(!defined('_FUNCS_RENDER')) {
	define('_FUNCS_RENDER',TRUE);

	// builds HTML for the survey 'sid' (prefixed w/ message)
	// writes to client
	function render_survey($sid, $section = 1, $message = '') {
		global $HTTP_POST_VARS;
		@reset($HTTP_POST_VARS);

		$has_answers = arr_has_answers();

	// load survey title (and other globals)
		$sql = "SELECT * FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) != 1)
			return(1);
		$survey = mysql_fetch_array($result);
		mysql_free_result($result);

	// section:
		$num_sections = count_sections($sid);
		if($section > $num_sections)
			return(1);	// invalid section

	// load survey questions
		$sql = "SELECT * FROM survey_questions ";
		$sql .= section_range_sql($sid,$section);
		$sql .= "ORDER BY position,id";
		$questions_result = mysql_query($sql);
		if(mysql_num_rows($questions_result) < 1)
			return(1);

	// find out what question number we are on $i
		$i=0;
		for($j=1;$j<$section;$j++) {
			$sql = "SELECT COUNT(*) FROM survey_questions ";
			$sql .= section_range_sql($sid,$j);
			$i += mysql_result(mysql_query($sql),0,0);
		}
?>
<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<?php if($num_sections>1) { ?>
	<font size="-1">Page <?php echo($section); ?> of <?php echo($num_sections); ?></font>
<?php } ?>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<blockquote><?php echo($message); ?></blockquote>
<p><font size="-1">Questions marked with a <font color="#FF0000">*</font> are required.</font></p>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php

		while($question = mysql_fetch_array($questions_result)) {
			// process each question
			$qid = $question['id'];
			$tid = $question['type_id'];
			if($tid == '99')
				break;

			++$i;
			if($has_answers[$tid]) {
				$sql = "SELECT * FROM question_choices WHERE question_id='${qid}' AND content!='!other' ORDER BY id";
				$choices_result = mysql_query($sql);
				$sql = "SELECT * FROM question_choices WHERE question_id='${qid}' AND content='!other' ORDER BY id";
				$others_result = mysql_query($sql);
				$others = mysql_num_rows($others_result);
			} else { $choices_result = ''; }
?>
	<tr>
		<td valign="TOP" align="RIGHT">
<?php if($question['required']=='Y') { ?>
			<font color="#FF0000">*</font>
<?php } ?>
			<A NAME="Q<?php echo($i); ?>"><?php echo($i); ?>.</A>
		</td>
		<td valign="TOP" align="LEFT">
				<?php echo($question['content']); ?>

				<blockquote>
<?php
			switch($tid) {
				case '1':	// Yes/No
?>
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<td><?php echo(mkradio($qid,'yes')); ?></td>
							<td>Yes</td>
						</tr>
						<tr>
							<td><?php echo(mkradio($qid,'no')); ?></td>
							<td>No</td>
						</tr>
					</table>
<?php
					break;
				case '2':	// single text line
?>
					<?php echo(mktext($qid)); ?>
<?php
					break;
				case '3':	// essay
?>
					<?php echo(mktextarea($qid,4,66,'VIRTUAL')); ?>
<?php
					break;
				case '4':	// radio
?>
					<table border="0" cellspacing="0" cellpadding="0">
<?php				while($choice = mysql_fetch_array($choices_result)) {	?>
						<tr>
							<td><?php echo(mkradio($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php				}
					if($others > 0 ) { ?>
						<tr>
							<td><?php echo(mkradio($qid,'other_0')); ?></td>
							<td>Other: <?php echo(mktext("${qid}_0")); ?></td>
						</tr>
<?php				}	?>
					</table>
<?php
					break;
				case '5':	// check boxes
?>
					<table border="0" cellspacing="0" cellpadding="0">
<?php				while($choice = mysql_fetch_array($choices_result)) {	?>
						<tr>
							<td><?php echo(mkcheckbox($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php				}
					for($j=0;$j<$others;$j++) { ?>
						<tr>
							<td><?php echo(mkcheckbox($qid,"other_${j}")); ?></td>
							<td>Other: <?php echo(mktext("${qid}_${j}")); ?></td>
						</tr>
<?php				}	?>
					</table>
<?php
					break;
				case '6':	// dropdown box
					$options = array();
					while($choice = mysql_fetch_array($choices_result)) {
						$options[$choice['id']] = $choice['content'];
					}
?>
					<?php echo(mkselect($qid,$options)); ?>
<?php
					break;
				case '7':	// ranking
?>
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<td width="60"><?php echo(mkradio($qid,1)); ?> 1</td>
							<td width="60"><?php echo(mkradio($qid,2)); ?> 2</td>
							<td width="60"><?php echo(mkradio($qid,3)); ?> 3</td>
							<td width="60"><?php echo(mkradio($qid,4)); ?> 4</td>
							<td width="60"><?php echo(mkradio($qid,5)); ?> 5</td>
							<td width="60"><?php echo(mkradio($qid,'N/A')); ?> N/A</td>
						</tr>
					</table>
<?php
					break;
			}
			// end of select
?>
				</blockquote>
		</td>
	</tr>
<?php
		}
		// end of questions
?>
</table>
<?php
		return;
	}
	// end of render_survey($sid,$message);

} // end _FUNCS_RENDER

?>
