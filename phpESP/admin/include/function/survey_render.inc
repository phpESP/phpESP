<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

if(!defined('_FUNCTION_SURVEY_RENDER')) {
	define('_FUNCTION_SURVEY_RENDER',TRUE);

/* {{{ proto bool survey_render(int survey_id, int section, string message)
   Reads current form variables from HTTP_POST_VARS.
   Builds HTML for the survey (prefixed w/ message). */
function render_survey($sid, $section = 1, $message = '') {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

	$has_choices = esp_type_has_choices();

// load survey title (and other globals)
	$sql = "SELECT * FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) != 1)
		return(false);
	$survey = mysql_fetch_array($result,MYSQL_ASSOC);
	mysql_free_result($result);

// section:
	$num_sections = survey_num_sections($sid);
	if($section > $num_sections)
		return(false);	// invalid section

// load survey questions
	$sql = "SELECT * FROM question ";
	$sql .= survey_select_section_sql($sid,$section);
	$sql .= "ORDER BY position,id";
	$questions_result = mysql_query($sql);
	if(mysql_num_rows($questions_result) < 1)
		return(false);

// check to see if there are required questions
	$sql = "SELECT COUNT(*) FROM question ";
	$sql .= survey_select_section_sql($sid,$section);
	$sql .= "AND required='Y'";
	$has_required = mysql_result(mysql_query($sql),0,0);		

// find out what question number we are on $i
	$i=0;
	for($j=1;$j<$section;$j++) {
		$sql = "SELECT COUNT(*) FROM question ";
		$sql .= survey_select_section_sql($sid,$j);
		$i += mysql_result(mysql_query($sql),0,0);
	}
?>
<h2><?php echo($survey["title"]); ?></h2>
<h3><?php echo($survey["subtitle"]); ?></h3>
<?php if($num_sections>1) { ?>
	<font size="-1"><?php echo(_('Page')); ?> <?php echo($section); ?> <?php echo(_('of')); ?> <?php echo($num_sections); ?></font>
<?php } ?>
<blockquote><?php echo($survey["info"]); ?></blockquote>
<blockquote><?php echo($message); ?></blockquote>
<?php if($has_required) { ?>
	<p><font size="-1"><?php echo(_('Questions marked with a')); ?> 
	<font color="#FF0000">*</font> <?php echo(_('are required.')); ?></font></p>
<?php } ?>
<table border="0" cellspacing="0" cellpadding="0" width="100%">
<?php
	while($question = mysql_fetch_array($questions_result,MYSQL_ASSOC)) {
		// process each question
		$qid = $question['id'];
		$tid = $question['type_id'];
		if($tid == '99')
			break;

		++$i;
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM question_choice WHERE question_id='${qid}' AND content NOT LIKE '!other%' ORDER BY id";
			$choices_result = mysql_query($sql);
			$sql = "SELECT * FROM question_choice WHERE question_id='${qid}' AND content LIKE '!other%' ORDER BY id";
			$others_result = mysql_query($sql);
			$others = mysql_num_rows($others_result);
		} else { $choices_result = ''; }
?>
	<tr>
		<td valign="TOP" align="RIGHT">
			<?php if($question['required']=='Y') { echo('<font color="#FF0000">*</font>'); } ?><A NAME="Q<?php echo($i); ?>"><?php echo($i); ?>.</A>
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
							<td><?php echo(mkradio($qid,'Y')); ?></td>
							<td><?php echo(_('Yes')); ?></td>
						</tr>
						<tr>
							<td><?php echo(mkradio($qid,'N')); ?></td>
							<td><?php echo(_('No')); ?></td>
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
<?php			while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {	?>
						<tr>
							<td><?php echo(mkradio($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = mysql_fetch_array($others_result,MYSQL_ASSOC)) {	
					$cid = $other['id'];
					$other_text = preg_replace(array("/\!other=(.*)/","/\!other/"),array("\\1 ","Other: "),$other['content']);
?>
						<tr>
							<td><?php echo(mkradio($qid,"other_${cid}")); ?></td>
							<td><?php echo($other_text . mktext("${qid}_${cid}")); ?></td>
						</tr>
<?php
					$j++;
				}
?>
					</table>
<?php
				break;
			case '5':	// check boxes
?>
					<table border="0" cellspacing="0" cellpadding="0">
<?php			while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {	?>
						<tr>
							<td><?php echo(mkcheckbox($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = mysql_fetch_array($others_result,MYSQL_ASSOC)) {	
					$cid = $other['id'];
					$other_text = preg_replace(array("/\!other=(.*)/","/\!other/"),array("\\1 ","Other: "),$other['content']);
?>
						<tr>
							<td><?php echo(mkcheckbox($qid,"other_${cid}")); ?></td>
							<td><?php echo($other_text . mktext("${qid}_${cid}")); ?></td>
						</tr>
<?php
					$j++;
				}
?>
					</table>
<?php
				break;
			case '6':	// dropdown box
				$options = array();
				while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {
					$options[$choice['id']] = $choice['content'];
				}
?>
					<?php echo(mkselect($qid,$options)); ?>
<?php
				break;
			case '7':	// rating
?>
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<td width="60"><?php echo(mkradio($qid,1)); ?> <?php echo(_('1')); ?></td>
							<td width="60"><?php echo(mkradio($qid,2)); ?> <?php echo(_('2')); ?></td>
							<td width="60"><?php echo(mkradio($qid,3)); ?> <?php echo(_('3')); ?></td>
							<td width="60"><?php echo(mkradio($qid,4)); ?> <?php echo(_('4')); ?></td>
							<td width="60"><?php echo(mkradio($qid,5)); ?> <?php echo(_('5')); ?></td>
							<td width="60"><?php echo(mkradio($qid,'N/A')); ?> <?php echo(_('N/A')); ?></td>
						</tr>
					</table>
<?php
				break;
			case '8':	// ranking
?>
					<table border="0" cellspacing="1" cellpadding="0">
						<tr>
							<td></td>
							<td width="40" align="center" bgcolor="#eeeeee">1</td>
							<td width="40" align="center" bgcolor="#dddddd">2</td>
							<td width="40" align="center" bgcolor="#eeeeee">3</td>
							<td width="40" align="center" bgcolor="#dddddd">4</td>
							<td width="40" align="center" bgcolor="#eeeeee">5</td>
							<td width="40" align="center" bgcolor="#dddddd">N/A</td>
						</tr>
<?php
				while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {
					$cid = $choice['id'];
					$str = $qid . "_" . $cid;
?>
						<tr>
							<td align="right"><?php echo($choice['content']); ?></td>
							<td width="40" align="center" bgcolor="#eeeeee"><?php echo(mkradio($str,1)); ?></td>
							<td width="40" align="center" bgcolor="#dddddd"><?php echo(mkradio($str,2)); ?></td>
							<td width="40" align="center" bgcolor="#eeeeee"><?php echo(mkradio($str,3)); ?></td>
							<td width="40" align="center" bgcolor="#dddddd"><?php echo(mkradio($str,4)); ?></td>
							<td width="40" align="center" bgcolor="#eeeeee"><?php echo(mkradio($str,5)); ?></td>
							<td width="40" align="center" bgcolor="#dddddd"><?php echo(mkradio($str,'N/A')); ?></td>
						</tr>
<?php			} ?>
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
<?php if($num_sections>1) { ?>
	<font size="-1"><?php echo(_('Page')); ?> <?php echo($section); ?> <?php echo(_('of')); ?> <?php echo($num_sections); ?></font><br>
<?php } ?>
<?php
	return;
}
/* }}} */

} // end _FUNCTION_SURVEY_RENDER
?>
