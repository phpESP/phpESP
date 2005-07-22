<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

/* {{{ proto bool survey_render(int survey_id, int section, string message)
   Reads current form variables from HTTP_POST_VARS.
   Builds HTML for the survey (prefixed w/ message). */
function survey_render($sid, $section = 1, $message = '') {
	global $HTTP_POST_VARS;
	@reset($HTTP_POST_VARS);

	if(empty($section))
		$section = 1;
	$has_choices = esp_type_has_choices();

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
<blockquote class="addInfo"><?php echo($survey["info"]); ?></blockquote>
<blockquote class="message"><?php echo($message); ?></blockquote>
<?php if($has_required) { ?>
    <p class="reqQuestion"><font size="-1">
    <?php printf(_('Questions marked with a %s are required.'), '<font color="#FF0000">*</font>'); ?></font></p>
<?php } ?>
<table class="mainTable">
<?php
	while($question = fetch_row($questions_result)) {
        $questions_result->MoveNext();
		// process each question
		$qid  = &$question['id'];
		$tid  = &$question['type_id'];
		$size = &$question['length'];
		$prec = &$question['precise'];

		if ($tid == 100) {

?>
<tr>
    <td class="preQuestionBorder">&nbsp;</td>
</tr>
<tr>
    <td height="35" vAlign="top">
        <table class="qnOuterTable" width="100%" border="0" cellpadding="0" cellspacing="1">
        <tr>
            <td width="100%" height="35" valign="top">
                <table class="qnInnerTable" width="100%" border="0" cellpadding="10" cellspacing="1">
                <tr>
        	        <td class="qnInner" width="100%" height="35"><?php echo $question['content']; ?></td>
                </tr>
                </table>
            </td>
        </tr>
        </table>
    </td>
</tr>
<?php

			continue;
		}

		++$i;
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=$qid AND content NOT LIKE '!other%' ORDER BY id";
			$choices_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=$qid AND content LIKE '!other%' ORDER BY id";
			$others_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			$others = record_count($others_result);
		} else { $choices_result = ''; }
?>
<tbody>
<tr>
        <td class="preQuestionBorder" >&nbsp;</td>
      </tr>
      <tr>
        <td height="62" vAlign=top>
                <table class="qnOuterTable" width="100%" cellspacing="1">
            <tr>
              <td width="100%" height="60" valign="top">
                 <table class="qnInnerTable" width="100%" cellPadding="10"  cellSpacing="1">
                  <tr>
                    <td class="qnInnerTd" rowspan="2" vAlign="top">
			<?php if($question['required']=='Y') { echo('<font color="#FF0000">*</font>'); } ?><A NAME="Q<?php echo($i); ?>"><?php echo($i); ?>.</A></td>
		    <td class="qnInner">
			<?php echo($question['content']); ?>
		    </td>
                  </tr>
                  <tr>
                    <td class="qnType">
                      <blockquote>
<?php
		switch($tid) {
			case '1':	// Yes/No
?>
				<table cellSpacing="0" cellPadding="0">
                        	<tbody>
                          	<tr>
                            		<td><?php echo(mkradio($qid,'Y')); ?></td>
                            		<td><?php echo(_('Yes')); ?></td>
                          	</tr>
                          	<tr>
                            		<td><?php echo(mkradio($qid,'N')); ?></td>
                            		<td><?php echo(_('No')); ?></td>
                          	</tr>
                        	</tbody>
                      		</table>
<?php
				break;
			case '2':	// single text line
?>
					<?php echo(mktext($qid, $size, $prec)); ?>
<?php
				break;
			case '3':	// essay
?>
					<?php echo(mktextarea($qid, $prec, $size, 'VIRTUAL')); ?>
<?php
				break;
			case '4':	// radio
?>
					<table border="0" cellspacing="0" cellpadding="0">
					<tbody>
<?php			while($choice = fetch_row($choices_result)) {	
                    $choices_result->MoveNext();
                    ?>
						<tr>
							<td><?php echo(mkradio($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = fetch_row($others_result)) {
                    $others_result->MoveNext();
					$cid = $other['id'];
					$other_text = preg_replace(
							array("/^!other=/","/^!other/"),
							array('','Other:'),
							$other['content']);
?>
						<tr>
							<td><?php echo(mkradio($qid,"other_$cid")); ?></td>
							<td>
<?php
                    $cid = "${qid}_${cid}";
                    echo("$other_text&nbsp;<input type=\"text\" size=\"20\" name=\"$cid\" onKeyPress=\"other_check(this.name)\"");
                	if (isset($GLOBALS['HTTP_POST_VARS'][$cid]))
                        echo(' value="'. _stripslashes(htmlspecialchars($GLOBALS['HTTP_POST_VARS'][$cid])) .'"');
                    echo(" />");
?>
                            </td>
						</tr>
<?php
					$j++;
				}
?>
					</tbody>
					</table>
<?php
				break;
			case '5':	// check boxes
?>
					<table border="0" cellspacing="0" cellpadding="0">
					<tbody>
<?php			while($choice = fetch_row($choices_result)) {
                    $choices_result->MoveNext();
                    ?>
						<tr>
							<td><?php echo(mkcheckbox($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = fetch_row($others_result)) {
                    $others_result->MoveNext();
					$cid = $other['id'];
					$other_text = preg_replace(
							array("/^!other=/","/^!other/"),
							array('','Other:'),
							$other['content']);
?>
						<tr>
							<td><?php echo(mkcheckbox($qid,"other_$cid")); ?></td>
							<td>
<?php
                    $cid = "${qid}_${cid}";
                    echo("$other_text&nbsp;<input type=\"text\" size=\"20\" name=\"$cid\" onKeyPress=\"other_check(this.name)\"");
                	if (isset($GLOBALS['HTTP_POST_VARS'][$cid]))
                        echo(' value="'. _stripslashes(htmlspecialchars($GLOBALS['HTTP_POST_VARS'][$cid])) .'"');
                    echo(" />");
?>
                            </td>

						</tr>
<?php
					$j++;
				}
?>
					</tbody>
					</table>
<?php
				break;
			case '6':	// dropdown box
				$options = array();
?>
<?php				while($choice = fetch_row($choices_result)) {
                        $choices_result->MoveNext();
					$options[$choice['id']] = $choice['content'];
				}
?>
					<?php echo(mkselect($qid,$options)); ?>
<?php
				break;
			case '7':	// rating
?>
					<table border="0" cellspacing="0" cellpadding="0">
					<tbody>
						<tr>
							<td width="60"><?php echo(mkradio($qid,1)); ?> <?php echo(_('1')); ?></td>
							<td width="60"><?php echo(mkradio($qid,2)); ?> <?php echo(_('2')); ?></td>
							<td width="60"><?php echo(mkradio($qid,3)); ?> <?php echo(_('3')); ?></td>
							<td width="60"><?php echo(mkradio($qid,4)); ?> <?php echo(_('4')); ?></td>
							<td width="60"><?php echo(mkradio($qid,5)); ?> <?php echo(_('5')); ?></td>
							<td width="60"><?php echo(mkradio($qid,'N/A')); ?> <?php echo(_('N/A')); ?></td>
						</tr>
					</tbody>
					</table>
<?php
				break;
			case '8':	// ranking
?>
					<table border="0" cellspacing="1" cellpadding="0">
						<tbody>
						<tr>
							<td></td>
<?php
				$bg = '#eeeeee';
				for ($j = 0; $j < $size; $j++) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>"><?php echo($j+1); ?></td>
<?php
					if ($bg == '#eeeeee')   $bg = '#dddddd';
					else                    $bg = '#eeeeee';
				}
				if ($prec) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>"><?php echo(_('N/A')); ?></td>
<?php
				}
?>
						</tr>
<?php
				while($choice = fetch_row($choices_result)) {
                    $choices_result->MoveNext();
					$cid = $choice['id'];
					$str = "${qid}_$cid";
?>
						<tr>
							<td align="right"><?php echo($choice['content']); ?></td>
<?php
					$bg = '#eeeeee';
					for ($j = 0; $j < $size; $j++) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>"><?php echo(mkradio($str,$j)); ?></td>
<?php
						if ($bg == '#eeeeee')   $bg = '#dddddd';
						else                    $bg = '#eeeeee';
					}
					if ($prec) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>"><?php echo(mkradio($str,'-1')); ?></td>
<?php
					}
?>
						</tr>
<?php			} ?>
					</tbody>
					</table>
<?php
				break;
			case '9':	// date
?>
					<?php echo(mktext($qid, 10, 10)); ?> <em>(e.g. 4/21/2002)</em>
<?php
				break;
			case '10':	// numeric
					$size++; // for sign
					if($prec)
						$size += 1 + $prec;
?>
					<?php echo(mktext($qid, $size, $size)); ?>
<?php
				break;
		}
		// end of select
?>
		</blockquote>
                         </td>
                  </tr>
                </table></td>
            </tr>
          </table></td>
      </tr></tbody>
<?php
	}
	// end of questions
?>
</table>
<?php if($num_sections>1) { ?>
	<font size="-1" class="surveyPage"><?php echo(_('Page')); ?> <?php echo($section+1); ?> <?php echo(_('of')); ?> <?php echo($num_sections); ?></font><br>
<?php } ?>
<?php
	return;
}
/* }}} */

?>
