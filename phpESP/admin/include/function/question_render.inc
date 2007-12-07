<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

/* {{{ proto bool question_render($question, int question_number)
   Builds HTML for the question */
function question_render($question,$question_number,$auto_num,$on_one_line=0) {
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
            <td width="100%" height="35" valign="top" align="left">
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

			return;
		}

		$has_choices = esp_type_has_choices();
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=$qid AND content NOT LIKE '!other%' ORDER BY id";
			$choices_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']." WHERE question_id=$qid AND content LIKE '!other%' ORDER BY id";
			$others_result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
			$others = record_count($others_result);
		} else { $choices_result = ''; }
?>
<tr>
        <td class="preQuestionBorder" >&nbsp;</td>
      </tr>
      <tr>
        <td height="62" valign=top>
                <table class="qnOuterTable" width="100%" cellspacing="1">
            <tr>
              <td width="100%" height="60" valign="top">
                 <table class="qnInnerTable" width="100%" cellPadding="10"  cellSpacing="1">
                  <tr>
                    <td class="qnInnerTd" valign="top" width=5%>
			<?php if($question['required']=='Y') { echo('<font color="#FF0000">*</font>'); } ?><A NAME="Q<?php echo($question_number); ?>"><?php if($auto_num=='Y') {echo $question_number.".";} ?></A></td>
		     <td class="qnInner" width=100% align=left>
			<?php echo($question['content']); ?>
		    </td>
<?php
		if ($on_one_line==0) {
                     echo "</tr><tr>";
		}
?>

		    <td></td>
                    <td class="qnType">
<?php
		switch($tid) {
			case '1':	// Yes/No
?>
				<table cellSpacing="0" cellPadding="0">
                        	<tbody>
                          	<tr>
                            		<td class="qnType"><?php echo(mkradio($qid,'Y')); ?></td>
                            		<td class="qnType"><?php echo(_('Yes')); ?></td>
                          	</tr>
                          	<tr>
                            		<td class="qnType"><?php echo(mkradio($qid,'N')); ?></td>
                            		<td class="qnType"><?php echo(_('No')); ?></td>
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
							<td class="qnType"><?php echo(mkradio($qid,$choice['id'])); ?></td>
							<td class="qnType"><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = fetch_row($others_result)) {
					$others_result->MoveNext();
					$cid = $other['id'];
					if (!strcmp($other['content'],"!other")) {
					   $other_text = 'Other:';
					} else {
					   $other_text = preg_replace(
							array("/^!other=/","/^!other/"),
							array('',''),
							$other['content']);
					}
?>
						<tr>
							<td class="qnType"><?php echo(mkradio($qid,"other_$cid")); ?></td>
							<td class="qnType">
<?php
                    $cid = "${qid}_${cid}";
                    echo("$other_text&nbsp;<input type=\"text\" size=\"20\" name=\"$cid\" onKeyPress=\"other_check(this.name)\"");
                	if (isset($_POST[$cid]))
                        echo(' value="'. _stripslashes(htmlspecialchars($_POST[$cid])) .'"');
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
							<td class="qnType"><?php echo(mkcheckbox($qid,$choice['id'])); ?></td>
							<td class="qnType"><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = fetch_row($others_result)) {
					$others_result->MoveNext();
					$cid = $other['id'];
					if (!strcmp($other['content'],"!other")) {
					   $other_text = 'Other:';
					} else {
					   $other_text = preg_replace(
							array("/^!other=/","/^!other/"),
							array('',''),
							$other['content']);
					}
?>
						<tr>
							<td class="qnType"><?php echo(mkcheckbox($qid,"other_$cid")); ?></td>
							<td class="qnType">
<?php
                    $cid = "${qid}_${cid}";
                    echo("$other_text&nbsp;<input type=\"text\" size=\"20\" name=\"$cid\" onKeyPress=\"other_check(this.name)\"");
                	if (isset($_POST[$cid]))
                        echo(' value="'. _stripslashes(htmlspecialchars($_POST[$cid])) .'"');
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
							<td width="60" class="qnType"><?php echo(mkradio($qid,1)); ?> <?php echo(_('1')); ?></td>
							<td width="60" class="qnType"><?php echo(mkradio($qid,2)); ?> <?php echo(_('2')); ?></td>
							<td width="60" class="qnType"><?php echo(mkradio($qid,3)); ?> <?php echo(_('3')); ?></td>
							<td width="60" class="qnType"><?php echo(mkradio($qid,4)); ?> <?php echo(_('4')); ?></td>
							<td width="60" class="qnType"><?php echo(mkradio($qid,5)); ?> <?php echo(_('5')); ?></td>
							<td width="60" class="qnType"><?php echo(mkradio($qid,'N/A')); ?> <?php echo(_('N/A')); ?></td>
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
							<td class="qnType"></td>
<?php
				$bg = '#eeeeee';
				for ($j = 0; $j < $size; $j++) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>" class="qnType"><?php echo($j+1); ?></td>
<?php
					if ($bg == '#eeeeee')   $bg = '#dddddd';
					else                    $bg = '#eeeeee';
				}
				if ($prec) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>" class="qnType"><?php echo(_('N/A')); ?></td>
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
							<td align="right" class="qnType"><?php echo($choice['content']); ?></td>
<?php
					$bg = '#eeeeee';
					for ($j = 0; $j < $size; $j++) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>" class="qnType"><?php echo(mkradio($str,$j)); ?></td>
<?php
						if ($bg == '#eeeeee')   $bg = '#dddddd';
						else                    $bg = '#eeeeee';
					}
					if ($prec) {
?>
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>" class="qnType"><?php echo(mkradio($str,'-1')); ?></td>
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
					<?php echo(mktext($qid, 10, 10)); ?> <em>(e.g. <?php echo strftime ($GLOBALS['ESPCONFIG']['date_format'],strtotime("09/21/2003")) ;?>)</em>
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
                         </td>
                  </tr>
                </table></td>
            </tr>
          </table></td>
      </tr>
<?php
	return;
}
/* }}} */
?>
