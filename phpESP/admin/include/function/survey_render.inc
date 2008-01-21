<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

/* {{{ proto bool survey_render(int survey_id, int section, int resonse_id, string message)
   Reads current form variables from $_POST.
   Builds HTML for the survey (prefixed w/ message). */
function survey_render($sid, $section = 1, $rid=0, $message = '') {
	//global $_POST;
	@reset($_POST);

	if(empty($section))
		$section = 1;

// load survey title (and other globals)
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
	$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
	if(record_count($result) != 1)
		return(false);
	$survey = fetch_row($result);
	db_close($result);

	$sections = survey_get_sections($sid);
	$num_sections = count($sections);
	if($section-- > $num_sections)
		return(false);	// invalid section

	$section_questions = survey_get_section_questions($sid);

// load survey questions
	$sec_sql = array_to_insql($sections[$section]);
	$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id $sec_sql ORDER BY position,id";
	$questions_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
	if(record_count($questions_result) < 1)
		return(false);

// check to see if there are required questions
	$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE id $sec_sql AND required='Y'";
	$has_required = get_one($sql);

// find out what question number we are on $i
	$i=0;
	for($j=0;$j<$section;$j++)
		$i += count($section_questions[$j]);
?>
<table class="headerGraphic">
<tr>
	<td class="image"></td>
</tr>
</table>
<h2 class="surveyTitle"><?php echo($survey["title"]); ?></h2>
<h3 class="surveySubtitle"><?php echo($survey["subtitle"]); ?></h3>
<?php if($num_sections>1) { ?>
	<font size="-1" class="surveyPage"><?php printf(_('Page %d of %d'), $section+1, $num_sections); ?></font>
<?php } ?>
<blockquote class="addInfo"><?php if ($section==0) echo($survey["info"]); ?></blockquote>
<blockquote class="message"><?php echo($message); ?></blockquote>
<?php if($has_required) { ?>
    <p class="reqQuestion"><font size="-1">
    <?php printf(_('Questions marked with a %s are required.'), '<font color="#FF0000">*</font>'); ?></font></p>
<?php } ?>
<table class="mainTable">
<?php
	$questions_shown=0;
	while($question = fetch_row($questions_result)) {
		$questions_result->MoveNext();
		$i++;
		// process each question
		$qid  = &$question['id'];
		$tid  = &$question['type_id'];
		if ($tid == 100) {
			// this type should not increase the question number
			$i--;
		}
		#if (!question_conditioncheck($sid,$qid,$_POST['rid'])) {
		if (question_conditioncheck($sid,$qid,$rid)) {
		   $questions_shown=1;
		   question_render($question,$i,$survey['auto_num']);
		} elseif (isset($_POST['tab']) && ($_POST['tab']=="preview")) {
		   // also show in preview
		   $questions_shown=1;
		   question_render($question,$i,$survey['auto_num']);
		}
	}
	if ($questions_shown==0) {
        print "<p class=\"message\">";
		print(_('Sorry, no questions for you to answer in this section.'));
		print "</p><br /><br />";
	}
?>
</table>
<?php if($num_sections>1) { ?>
	<font size="-1" class="surveyPage"><?php printf(_('Page %d of %d'), $section+1, $num_sections); ?></font><br />
<?php } ?>
<?php
	return;
}
/* }}} */

?>
