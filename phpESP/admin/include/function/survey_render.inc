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

	if(empty($section))
		$section = 1;

	$has_choices = esp_type_has_choices();

// load survey title (and other globals)
	$sql = "SELECT * FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) != 1)
		return(false);
	$survey = mysql_fetch_array($result,MYSQL_ASSOC);
	mysql_free_result($result);

	$sections = survey_get_sections($sid);
	$num_sections = count($sections);
	if($section-- > $num_sections)
		return(false);	// invalid section
	
// load survey questions
	$sec_sql = array_to_insql($sections[$section]);
	$sql = "SELECT * FROM question WHERE id $sec_sql AND type_id <> 98 ORDER BY position,id"; 
	$questions_result = mysql_query($sql);
	if(mysql_num_rows($questions_result) < 1)
		return(false);

    $this_sec = $section + 1;

	$these_questions = survey_question_list($sid, $this_sec);
	$titled_questions = survey_titled_question_list($sid, $this_sec);
    if ($this_sec != 1) {
    	$num_questions = count($these_questions);
    	$num_titled_questions = count($titled_questions);

        if (empty($these_questions)) $num_questions=0;
        if (empty($titled_questions)) $num_titled_questions=0;

        $num_questions -= $num_titled_questions;            // dont count questions w/ titles
        $prev_num_questions = $HTTP_POST_VARS["sofar"];
        $num_questions += $prev_num_questions;
    } else {
    	$num_titled_questions = count($titled_questions);
        if (empty($titled_questions)) $num_titled_questions=0;
    	$num_questions = count($these_questions);
        $num_questions -= $num_titled_questions;            // dont count questions w/ titles
    }


// check to see if there are required questions
	$sql = "SELECT COUNT(*) FROM question WHERE id $sec_sql AND required='Y'";
	$has_required = mysql_result(mysql_query($sql),0,0);		

// find out what question number we are on $i
	$i=0;
	for($j=0;$j<$section;$j++){
//		$i += count($sections[$j]);
        $i = $prev_num_questions;
    }
?>

<SCRIPT LANGUAGE="JavaScript">

<!-- This script and many more are available free online at -->
<!-- The JavaScript Source!! http://javascript.internet.com -->
<!-- Original:  Torsten Frey (tf@tfrey.de) -->
<!-- Web Site:  http://www.tfrey.de -->

<!-- Begin
function check_date(field){
var checkstr = "0123456789";
var DateField = field;
var Datevalue = "";
var DateTemp = "";
var seperator = "/";
var day;
var month;
var year;
var leap = 0;
var err = 0;
var i;
   err = 0;
   DateValue = DateField.value;
   /* Delete all chars except 0..9 */
   for (i = 0; i < DateValue.length; i++) {
      if (checkstr.indexOf(DateValue.substr(i,1)) >= 0) {
         DateTemp = DateTemp + DateValue.substr(i,1);
      }
   }
   DateValue = DateTemp;
   /* Always change date to 8 digits - string*/
   /* if year is entered as 2-digit / always assume 20xx */
   if (DateValue.length == 6) {
      DateValue = DateValue.substr(0,4) + '20' + DateValue.substr(4,2); }
   if (DateValue.length != 8) {
      err = 19;}
   /* year is wrong if year = 0000 */
   year = DateValue.substr(4,4);
   if (year == 0) {
      err = 20;
   }
   /* Validation of month*/
   month = DateValue.substr(0,2);
   if ((month < 1) || (month > 12)) {
      err = 21;
   }
   /* Validation of day*/
   day = DateValue.substr(2,2);
   if (day < 1) {
     err = 22;
   }
   /* Validation leap-year / february / day */
   if ((year % 4 == 0) || (year % 100 == 0) || (year % 400 == 0)) {
      leap = 1;
   }
   if ((month == 2) && (leap == 1) && (day > 29)) {
      err = 23;
   }
   if ((month == 2) && (leap != 1) && (day > 28)) {
      err = 24;
   }
   /* Validation of other months */
   if ((day > 31) && ((month == "01") || (month == "03") || (month == "05") || (month == "07") || (month == "08") || (month == "10") || (month == "12"))) {
      err = 25;
   }
   if ((day > 30) && ((month == "04") || (month == "06") || (month == "09") || (month == "11"))) {
      err = 26;
   }
   /* if 00 ist entered, no error, deleting the entry */
   if ((day == 0) && (month == 0) && (year == 00)) {
      err = 0; day = ""; month = ""; year = ""; seperator = "";
   }
   /* if no error, write the completed date to Input-Field (e.g. 13.12.2001) */
   if (err == 0) {
      DateField.value = month + seperator + day + seperator + year;
   }
   /* Error-message if err != 0 */
   else {
      alert("Date is incorrect! \n(be sure to use a 0 in front of single digits) ");
      DateField.select();
      DateField.focus();
   }
}
//  End -->
</script>

<SCRIPT LANGUAGE="JavaScript">
<!-- Original:  Mikhail Esteves (miks80@yahoo.com) -->
<!-- Web Site:  http://www.freebox.com/jackol -->

<!-- This script and many more are available free online at -->
<!-- The JavaScript Source!! http://javascript.internet.com -->

<!-- Begin
var mikExp = /[$\\@\\\#%\^\&\*\(\)\[\]\+\_\{\}\`\~\=\|]/;
function dodacheck(val) {
var strPass = val.value;
var strLength = strPass.length;
var lchar = val.value.charAt((strLength) - 1);

//if(lchar.search(mikExp) != -1) {
var number = parseFloat(lchar);
if (isNaN(number) && lchar!="."){
    var tst = val.value.substring(0, (strLength) - 1);
    val.value = tst;
   }
}
function doanothercheck(form) {
if(form.value.length < 1) {
alert("Please enter something.");
return false;
}
if(form.value.search(mikExp) == -1) {
alert("Correct Input");
return false;
}
else {
alert("Sorry, but the following characters\n\r\n\r@ $ % ^ & * # ( ) [ ] \\ { + } ` ~ =  | \n\r\n\rare not allowed!\n");
form.select();
form.focus();
return false;
}
alert("Correct Input");
return false;
}
//  End -->
</script>




    
<table class="headerGraphic">
<input type=hidden name=sofar value=<?php echo $num_questions; ?> >
<tr>
	<td class="image"></td>
</tr>
</table>
<h2 class="surveyTitle"><?php echo($survey["title"]); ?></h2>
<h3 class="surveySubtitle"><?php echo($survey["subtitle"]); ?></h3>
<?php if($num_sections>1) { ?>
<!--	<font size="-1"><?php echo(_('Page')); ?> <?php echo($section+1); ?> <?php echo(_('of')); ?> <?php echo($num_sections); ?></font> -->
<?php } ?>
<blockquote class="addInfo"><?php echo($survey["info"]); ?></blockquote>
<blockquote class="message"><?php echo($message); ?></blockquote>
<?php if($has_required) { ?>
	<p class="reqQuestion"><font size="-1"><?php echo(_('Questions marked with a')); ?> 
	<font color="#FF0000">*</font> <?php echo(_('are required.')); ?></font></p>
<?php } ?>
<table class="mainTable">
<?php
	while($question = mysql_fetch_array($questions_result,MYSQL_ASSOC)) {
		// process each question
		$qid  = &$question['id'];
		$name = &$question['name'];
		$tid  = &$question['type_id'];
		$size = &$question['length'];
		$prec = &$question['precise'];
		$titl = &$question['show_title'];

        $pipe_num = strstr($question['content'], '$_');
        $pipe_num = explode(" ", $pipe_num);
        $pipe_str = $pipe_num[0];
        $pipe_num = substr($pipe_str, 2);

        $pipe_exists = false;
        $pipe_is_array = false;

        // get the type of the piped question
        if ($pipe_num != "" && $pipe_num != null){
            $pipe_exists = true;
            $pipe_type_sql = "SELECT type_id FROM question WHERE id=$pipe_num";
            $pipe_type_res = mysql_query($pipe_type_sql);
            $pipe_type_obj = mysql_fetch_object($pipe_type_res);
            $pipe_tid = $pipe_type_obj->type_id;
        }

        if ($pipe_exists) {
            if ($HTTP_POST_VARS["$pipe_num"] != NULL && $HTTP_POST_VARS["$pipe_num"] != ""){
                // piped question in previous page
                // just get value from post vars
                $replacement = $HTTP_POST_VARS["$pipe_num"];
            } else {
                // piped question a while ago, no longer
                // in post vars, need to query db
                $rid = $HTTP_POST_VARS["rid"];
                if ($pipe_tid == 10 || $pipe_tid == 2 || $pipe_tid == 3){
                    $pipe_query_sql = "SELECT response FROM response_text WHERE response_id=$rid AND question_id=$pipe_num";
                } else if ($pipe_tid == 9) {
                    $pipe_query_sql = "SELECT response FROM response_date WHERE response_id=$rid AND question_id=$pipe_num";
                } else if ($pipe_tid == 1) {
                    $pipe_query_sql = "SELECT choice_id as response FROM response_bool WHERE response_id=$rid AND question_id=$pipe_num";
                } else if ($pipe_tid == 4 || $pipe_tid == 6) { 
                    $pipe_query_sql = "SELECT choice_id as response FROM response_single WHERE response_id=$rid AND question_id=$pipe_num";
                } else if ($pipe_tid == 5) {
                    $pipe_is_array = true;
                    $pipe_query_sql = "SELECT choice_id as response FROM response_multiple WHERE response_id=$rid AND question_id=$pipe_num";
                }
                if ($pipe_is_array){
                    $replacement = Array();
                    $pipe_query_result = mysql_query($pipe_query_sql);
                    while ($pipe_query_object = mysql_fetch_object($pipe_query_result)){
                        $answer = $pipe_query_object->response;
                        array_push($replacement, $answer);
                    }
                } else {
                    $pipe_query_result = mysql_query($pipe_query_sql);
                    @$pipe_query_object = mysql_fetch_object($pipe_query_result);
                    $replacement = $pipe_query_object->response;
                }
            }
    
            // in case of checkboxes, make the array into a string
            if ($pipe_tid == 5){
                $replacement = implode(", ", $replacement);
            }
    
    
            // cannot do piping for some types of questions
            if ($pipe_tid == 8){
                $replacement = "[this question type cannot be used in a pipe]";
            }
            // rewrite the yes/no questions better
            if ($pipe_tid == 1 && $replacement=="Y") $replacement="Yes";
            if ($pipe_tid == 1 && $replacement=="N") $replacement="No";

            // rewrite the radio button questions
            if ($pipe_tid == 4 || $pipe_tid == 6) {
                $radio_choice_sql = "SELECT content FROM question_choice WHERE id=$replacement";
                $radio_choice_res = mysql_query($radio_choice_sql);
                $radio_choice_obj = mysql_fetch_object($radio_choice_res);
    
                $replacement = $radio_choice_obj->content;
            }
            // rewrite the checkbox questions
            if ($pipe_tid == 5) {
                $radio_choice_sql = "SELECT content FROM question_choice WHERE id IN ($replacement)";
                $radio_choice_res = mysql_query($radio_choice_sql);
                $replacement = "[ ";
                while ($radio_choice_obj = mysql_fetch_object($radio_choice_res)){
                    $replacement .= "'" . $radio_choice_obj->content . "'";
                    $replacement .= " ";
                }
                $replacement .= "]";
            }



            $question['content'] = str_replace($pipe_str, $replacement, $question['content']);
        }
        
		
		if ($tid == 100) {
			echo("<tr>\n");
        		echo("	<td class=\"preQuestionBorder\">&nbsp;</td>\n");
      			echo("</tr>\n");
      			echo("<tr>\n");
        		echo("<td height=\"35\" vAlign=\"top\">\n");
                	echo("<table class=\"qnOuterTable\" width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"1\">\n");
            		echo("<tr>\n");
              		echo("	<td width=\"100%\" height=\"35\" valign=\"top\">\n");
                        echo("<table class=\"qnInnerTable\" width=\"100%\" border=\"0\" cellpadding=\"10\" cellspacing=\"1\">\n");
                  	echo("<tr>\n");
                    	echo("	<td class=\"qnInner\" width=\"100%\" height=\"35\">".$question['content'] ."</td></tr>\n");
                	echo("</table></td>\n");
            		echo("</tr>\n");
          		echo("</table></td>\n");
      			echo("</tr>\n");
			continue;
		}

        if ($titl=='N')
    		++$i;
		if($has_choices[$tid]) {
			$sql = "SELECT * FROM question_choice WHERE question_id='$qid' AND content NOT LIKE '!other%' ORDER BY id";
			$choices_result = mysql_query($sql);
			$sql = "SELECT * FROM question_choice WHERE question_id='$qid' AND content LIKE '!other%' ORDER BY id";
			$others_result = mysql_query($sql);
			$others = mysql_num_rows($others_result);
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
			<?php if($question['required']=='Y') { echo('<font color="#FF0000">*</font>'); } ?><A NAME="Q<?php echo($i); ?>">
                <?php if ($titl=='Y') echo $name; else echo($i); ?>.</A></td>
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
<?php			while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {	?>
						<tr>
							<td><?php echo(mkradio($qid,$choice['id'])); ?></td>
							<td><?php echo($choice['content']); ?></td>
						</tr>
<?php			}
				$j=0;
				while($other = mysql_fetch_array($others_result,MYSQL_ASSOC)) {	
					$cid = $other['id'];
					$other_text = preg_replace(
							array("/\!other=(.*)/","/\!other/"),
							array("\\1 ","Other: "),
							$other['content']);
?>
						<tr>
							<td><?php echo(mkradio($qid,"other_$cid")); ?></td>
							<td><?php echo($other_text . mktext("${qid}_$cid")); ?></td>
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
							<td><?php echo(mkcheckbox($qid,"other_$cid")); ?></td>
							<td><?php echo($other_text . mktext("${qid}_$cid")); ?></td>
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
<?php				while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {
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
				while($choice = mysql_fetch_array($choices_result,MYSQL_ASSOC)) {
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
							<td width="40" align="center" bgcolor="<?php echo($bg); ?>"><?php echo(mkradio($str,'N/A')); ?></td>
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
					<?php echo(mkdate($qid, 10, 10)); ?> <em>(e.g. 04/21/2002)</em>
                    <input type=hidden name=date_field value=<?php echo $qid;?> >
<?php
				break;
			case '10':	// numeric
					$size++; // for sign
					if($prec)
						$size += 1 + $prec;
?>
					<?php echo(mknum($qid, $size, $size)); ?>
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
<!--	<font size="-1"><?php echo(_('Page')); ?> <?php echo($section+1); ?> <?php echo(_('of')); ?> <?php echo($num_sections); ?></font><br> -->
<?php } ?>
<?php
	return;
}
/* }}} */
    //mysql_subquery
    //copyright 1999 avi bryant, all rights reserved
    //feel free to use this and modify it as you see fit
    function sub($result)
    {
        if($result == 0)
        return "()";
        else
        {
        $row = mysql_fetch_row($result);
        $strResult = "( $row[0]";
            while($row = mysql_fetch_row($result))
                $strResult = "$strResult , $row[0]";
        $strResult = "$strResult )";
            return $strResult;
        }
    }
    function mysql_subquery($query)
    {
        $substart = strpos($query, "(");
        if($substart != false)
        {
            $subend = strrpos($query, ")");
            $before = substr($query, 0, $substart);
            $after = substr($query, $subend+1, -1);
            $subquery = substr($query, $substart+1, $subend-$substart-1);

            $subresult = mysql_subquery($subquery);
            $subtext = sub($subresult);
            return mysql_query($before . $subtext . $after);
        }
        else return mysql_query($query);
    }

} // end _FUNCTION_SURVEY_RENDER
?>
