<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$url = $ESPCONFIG['ME'] .'?where=tab&tab=order';

	$src = intval($HTTP_POST_VARS['src']);
	$dst = intval($HTTP_POST_VARS['dst']);
	$sid = $HTTP_SESSION_VARS['survey_id'];

	if(isset($HTTP_POST_VARS['show_title'])) {
        $title_array = $HTTP_POST_VARS['show_title'];
        $title_string = implode(", ", $title_array);
        $sql = "UPDATE question SET show_title='Y' WHERE id IN (" . $title_string . ")";
        $sql2 = "UPDATE question SET show_title='N' WHERE id NOT IN (" . $title_string . ")";
        mysql_query($sql);
        mysql_query($sql2);
    } else {    // none are selected
        $sql = "UPDATE question SET show_title='N' WHERE survey_id='$sid'";
	    mysql_query($sql);
    }


	if(isset($HTTP_POST_VARS['clear_title'])) {
        $sql = "UPDATE question SET show_title='N' WHERE survey_id='$sid'";
	    mysql_query($sql);
    }

	if(isset($HTTP_POST_VARS['addbreak'])) {
		$sql = "SELECT MAX(position)+1 FROM question WHERE survey_id='$sid'";
		$result = mysql_query($sql);
		$pos = mysql_result($result, 0, 0);
		mysql_free_result($result);

		$sql = "INSERT INTO question (survey_id,type_id,position,content)
				VALUES ('$sid', 99, '$pos', 'break')";
		mysql_query($sql);
	} elseif (isset($HTTP_POST_VARS['addskip']) 
                && isset ($HTTP_POST_VARS['if_q'])
                && (
                    isset ($HTTP_POST_VARS['resp']) ||
                    isset ($HTTP_POST_VARS['free_answer'])
                   )
                && isset ($HTTP_POST_VARS['op'])
                && isset ($HTTP_POST_VARS['goto_s'])) {

		$sql = "SELECT MAX(position)+1 FROM question WHERE survey_id='$sid'";
		$result = mysql_query($sql);
		$pos = mysql_result($result, 0, 0);
		mysql_free_result($result);

        if (isset($HTTP_POST_VARS['rate_q']) && 
                $HTTP_POST_VARS['rate_q'] != "") $op = $op . "_#_" . $HTTP_POST_VARS['rate_q'];

        if (isset($HTTP_POST_VARS['free_answer']) && 
                $HTTP_POST_VARS['free_answer'] != "") $resp = $HTTP_POST_VARS['free_answer'];

        if ($HTTP_POST_VARS['use_date'] == "Y"){
            $date_string = $HTTP_POST_VARS['free_answer'];
            $resp = str2date($date_string);
        }

        $sql = "INSERT INTO question (survey_id,type_id,position,content,result_id,length,precise,name)
                VALUES ('$sid', 98, '$pos', 'skip', $goto_s, $if_q, $resp, '$op')";
        mysql_query($sql);
	} elseif ($src > 0) {
		--$src;
		--$dst;
		
		$sql = "SELECT id,position FROM question
				WHERE survey_id='$sid' AND deleted='N'
				ORDER BY position";
		$result = mysql_query($sql);

		$max = mysql_num_rows($result);

		if ($src < $max) {
			$qid = mysql_result($result, $src, 0);

			if ($dst < 0) {
				// remove
				$sql = "UPDATE question SET deleted='Y' WHERE id='$qid' AND survey_id='$sid'";
				mysql_query($sql);
			} elseif (0 <= $dst && $dst < $max && $src != $dst) {
				// move
				if ($max - 1 > $dst) {
					if ($src < $dst)
						// move down
						++$dst;
					$dst = mysql_result($result, $dst, 1);
					$sql = "UPDATE question SET position=position+1
							WHERE survey_id='$sid' AND position >= '$dst'";
					mysql_query($sql);
				} else {
					$dst = mysql_result($result, $max-1, 1) + 1;
				}
				$sql = "UPDATE question SET position='$dst' WHERE id='$qid' AND survey_id='$sid'";
				mysql_query($sql);
			}
		}
		mysql_free_result($result);
	}

//	$sql = "SELECT id,type_id,position,content FROM question
	$sql = "SELECT id,type_id,position,content,result_id, length, precise, name, show_title FROM question
			WHERE survey_id='$sid' AND deleted='N' 
			ORDER BY position";
	$result = mysql_query($sql);
	$max = mysql_num_rows($result);
	$sec = $num = 0;
?>
<?php echo(_('Change the order that questions are
presented by choosing the desired position from the
list. <br>
You can also elect to have the title of the question shown instead of
the question number.')); ?>
<hr>
<script language="javascript">
<!-- // comment
  function swap(src, dst) {
    document.phpesp.src.value=src;
    document.phpesp.dst.value=dst+1;
    document.phpesp.submit();
  }
  function remove(src) {
    document.phpesp.src.value=src;
    document.phpesp.dst.value=-1;
    document.phpesp.submit();
  }
  function editq(num) {
    if (document.phpesp.elements[1].name == 'tab') {
        document.phpesp.elements[1].value='Questions';
    }
    else {
      for (i = 1; i <= document.phpesp.elements.length; i++) {
	if (document.phpesp.elements[i].name == 'tab') {
	  document.phpesp.elements[i].value='Questions';
	  break;
	}
      }
    }  
    document.phpesp.q.value=num;
    document.phpesp.submit();
	return false;
  }
// comment -->
</script>
<input type="hidden" name="tab" value="">
<input type="hidden" name="src" value="">
<input type="hidden" name="dst" value="">
<input type="hidden" name="q"   value="">
<table>
    <tr>
        <th>show title?&nbsp;&nbsp;</th>
        <th>position</th>
        <th>[title] question</th>
        <th colspan=2>actions</th>
    </tr>
<?php	
$date_exists = false;

//	while( list($qid, $tid, $pos, $content) = mysql_fetch_row($result)) {
	while( list($qid, $tid, $pos, $content, $next_sec, $target_qid, $target_cid, $name, $show_title) = mysql_fetch_row($result)) {
        if ($tid == 9) $date_exists = true;
		$num++;
		$select = '';
		for ( $i = 0; $i < $max; ) {
			$i++;
			if( $i == $num )
				$select .= "<option value=\"$i\" selected=\"selected\">$i</option>\n";
			else
				$select .= "<option value=\"$i\">$i</option>\n";
		}

        if ($show_title=='Y'){
            $title_select = "checked";
        } else {
            $title_select = "";
        }

        $name_num = explode("_#_", $name);
        $name = $name_num[0];
        $rate_num = $name_num[1];

        if ($rate_num != ""){
            $rate_name_sql = "SELECT content FROM question_choice WHERE id=$rate_num";
            $rate_name_result = mysql_query($rate_name_sql);
            $rate_name = mysql_fetch_row($rate_name_result);
        }

?>
	<tr>
        <?php if ($tid != 99 && $tid !=98) { ?>
        <td align=center>
          <input type=checkbox onClick="javascript:this.form.submit();" <?php echo $title_select; ?> name=show_title[] value=<?php echo($qid); ?>>
        </td>
        <?php } else { ?>
        <td align=center>
            &nbsp;
        </td>
        <?php } ?>
		<td>
		<select name="pos<?php echo($num); ?>" size="1" onchange="javascript:swap(<?php echo($num); ?>, this.form.pos<?php echo($num); ?>.selectedIndex);">
<?php echo($select); ?>
		</select>
		</td>
		<td>
<?php
			if($tid != 99 && $tid !=98) {
				echo("[".$name."] ".$content);
            } else {
				++$sec;
                if ($tid == 98) {
                    // sql to decide type of target question, etc...
                    // probably find text versions of question_choice, too (eg. 167=="Honda")
                    //
                    $target_sql = "SELECT name FROM question WHERE id=".$target_qid;
                    $target_result = mysql_query($target_sql);
                    $target_object = mysql_fetch_object($target_result);

                    $target_name = $target_object->name;

                    $qtype_sql = "SELECT type FROM question_type WHERE id=(" .
                        "SELECT type_id FROM question WHERE id=".$target_qid.")";       // more readable than just type_id
                    $qtype_result = mysql_subquery($qtype_sql);
                    $qtype_object = mysql_fetch_object($qtype_result);
    
                    $target_tid = $qtype_object->type;

                    if ($name=="EQUALS") $opr = "=";
                    if ($name=="NOT_EQUAL_TO") $opr = "!=";
                    if ($name=="LESS_THAN") $opr = "<";
                    if ($name=="GREATER_THAN") $opr = ">";
                    if ($name=="LESS_THAN_OR_EQUAL_TO") $opr = "<=";
                    if ($name=="GREATER_THAN_OR_EQUAL_TO") $opr = ">=";

                    if ($next_sec == 0) $next_sec = "END";

                    if ($target_cid=="-1"){ 
                        $target_cid = "ANYTHING";
                        if ($rate_name[0] != "") $rate_name[0] = ".".$rate_name[0];
				        echo('<b>'._("----- Skip to [$next_sec] if ($target_name$rate_name[0]) $opr \"$target_cid\" -----").'</b>');
                    } else if ($target_tid == "Yes/No") { // its a yes/no question
                        if ($target_cid == 0) $answer="No"; else $answer="Yes";
				        echo('<b>'._("----- Skip to [$next_sec] if ($target_name) $opr \"$answer\" -----").'</b>');
                    } else if ($target_tid == "Date") { // its a date
                        $timestamp = sprintf("%f", $target_cid);
                        $answer = date("m/d/y", $timestamp);
				        echo('<b>'._("----- Skip to [$next_sec] if ($target_name) $opr \"$answer\" -----").'</b>');
                    } else if ($target_tid == "Dropdown Box" || $target_tid == "Radio Buttons") { // its a dropdown
                        $type_sql = "SELECT content FROM question_choice WHERE id=".$target_cid;
                        $type_result = mysql_query($type_sql);
                        $type_object = mysql_fetch_object($type_result);
                        $choice = $type_object->content;
				        echo('<b>'._("----- Skip to [$next_sec] if ($target_name) $opr \"$target_cid\" -----").'</b>');
                    } else {
                        if ($target_cid==-1) $target_cid = "ANYTHING";
                        if ($rate_name[0] != "") $rate_name[0] = ".".$rate_name[0];
    				    echo('<b>'._("----- Skip to [$next_sec] if ($target_name$rate_name[0]) $opr \"$target_cid\" -----").'</b>');
                    }
                } else {
			        echo('<b>'._("[$qid] ----- Section Break -----").'</b>');
                }
			}
?>
		</td>
		<td><input type="button" value="<?php echo(_('Remove')); ?>" onclick="javascript:remove(<?php echo($num); ?>);"></td>
		<td>
            <?php if($tid != 99 && $tid != 98) { ?>
                <input type="button" value="<?php echo(_('Edit')); ?>" 
                    onclick="javascript:editq(<?php echo($num-$sec); ?>);">
            <?php } ?>
       </td>
	</tr>
<?php
		}
		mysql_free_result($result);
?>
</table>
<br>
<input type="submit" name="addbreak" value="<?php echo(_('Add Section Break')); ?>">
<hr>

<?
    $question_array_sql = "SELECT id, name FROM question WHERE type_id <> 99 AND type_id <> 98 
            AND type_id <> 2 AND type_ID <> 3 AND survey_id='$sid' AND deleted='N'
			ORDER BY position";
    $question_array_result = mysql_query($question_array_sql);
    $num_questions = mysql_num_rows($question_array_result);
    if ($num_questions > 0) {
        $question_array = array();
        while(list($key, $val) = mysql_fetch_row($question_array_result)) {
            $question_array["$key"] = _($val);
        }
    
        $response_array_sql = "SELECT id, content FROM question_choice WHERE question_id IN 
            (SELECT id, content FROM question WHERE type_id <> 99 AND type_id <> 98 
             AND survey_id='$sid' AND deleted='N' ORDER BY position) 
            ORDER BY question_id ";
        $response_array_result = mysql_subquery($response_array_sql);
        $response_array = array();
        while(list($key, $val) = mysql_fetch_row($response_array_result)) {
            $response_array["$key"] = _($val);
        }
        $response_array["-1"] = "[ANYTHING]";
        $response_array["0"] = "[No]";
        $response_array["1"] = "[Yes]";
    
        $section_array_sql = "SELECT id, id FROM question WHERE type_id=99 AND 
    			survey_id='$sid' AND deleted='N'
    			ORDER BY position";
        $section_array_result = mysql_query($section_array_sql);
        $section_array = array();
        while(list($key, $val) = mysql_fetch_row($section_array_result)) {
            $section_array["$key"] = _($val);
        }
        $section_array[-1] = "[END]";

        $rate_q_sql = "SELECT id, content FROM question_choice WHERE question_id = 
                        ( SELECT id FROM question WHERE type_id=8 AND survey_id='$sid' 
                          AND deleted='N' ORDER BY position
                        ) ORDER BY id ";
        $rate_q_result = mysql_subquery($rate_q_sql);
        $rate_q_array = array();
        if ($rate_q_result) {
            while(list($key, $val) = mysql_fetch_row($rate_q_result)) {
                $rate_q_array["$key"] = _($val);
            }
        }
    
        $operations_array = array();
        $operations_array["EQUALS"] = "EQUALS";
        $operations_array["NOT_EQUAL_TO"] = "NOT EQUAL TO";
        $operations_array["LESS_THAN"] = "LESS THAN";
        $operations_array["GREATER_THAN"] = "GREATER THAN";
        $operations_array["LESS_THAN_OR_EQUAL_TO"] = "LESS THAN OR EQUAL TO";
        $operations_array["GREATER_THAN_OR_EQUAL_TO"] = "GREATER THAN OR EQUAL TO";

      ?>


        <table border=0>
          <tr>
            <td align=right><b>if</b></td>
            <td><?php echo(mkselect("if_q",$question_array)); ?></td><td><?php if ($rate_q_result) { echo( "<b>. </b>" . mkselect("rate_q",$rate_q_array)); ?> <font size=-1>(&lt;-- for use with rate questions)</font> <?php } ?></td>
          </tr>
          <tr>
            <td>&nbsp;</td>
            <td colspan=2 align=left><?php echo(mkselect("op",$operations_array)); ?>
            </td>
          </tr>
          <tr>
            <td> &nbsp; </td>
            <td><?php echo(mkselect("resp",$response_array)); ?></td>
            <td>OR &nbsp;<input align=left name=free_answer size=9 type=text></td>
          </tr>
          <tr>
            <td> &nbsp; </td>
            <td><font size=-1>(predefined)</font></td>
            <td><font size=-1>(free answer)</font></td>
            <td><font size=-1><?php if($date_exists) { ?>(<input type=checkbox name=use_date value=Y> check to use a date in form mm/dd/yyyy)<?php } ?></font></td>
          </tr>
          <tr>
            <td align=right><b>goto section</b></td>
            <td><?php echo(mkselect("goto_s",$section_array)); ?></td>
          </tr>
          <tr>
            <td colspan=2 align=center>
                <input type="submit" name="addskip" value="<?php echo(_('Add a Skip')); ?>"> 
            </td>
        </table>


<?php

    }       // end of "if ($num_questions > 0)" loop

?>


<?php

    //**************************************
    // Name: MySQL SubQuery
    // Description:Performs simple nested SELECT statements with MySQL (for example, of the form SELECT * FROM x WHERE y IN (SELECT y FROM z WHERE ...) ) Subqueries must be enclosed in parenthesis.by Avi Bryant
    // By: PHP Code Exchange
    //
    //
    // Inputs:None
    //
    // Returns:None
    //
    //Assumes:None
    //
    //Side Effects:None
    //**************************************
    
    //mysql_subquery
    //copyright 1999 avi bryant, all rights reserved
    //feel free to use this and modify it as you see fit
//
/*
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
*/


// a bastardized mkselect from ../lib/esphtml.forms.inc
//
function mk_js_array ($options) {
    $str  = "";
    $str .= "\t new Array(\n";
    $i = 0;
    while(list($cid, $content) = each($options)) {
        if (i!=0) $str .= ", ";
        $str .= "\t\t new Array(\"$content\",$cid ) \n";
    }
    $str .= ")\n";
    return($str);
}

// support date format as : 23 1 2003, 23-01-2003, 23/01/2003, 
// return timestamp
function str2date($in){

$t = split("/",$in);
if (count($t)!=3) $t = split("-",$in);
if (count($t)!=3) $t = split(" ",$in);

if (count($t)!=3) return -1;

if (!is_numeric($t[0])) return -1;
if (!is_numeric($t[1])) return -2;
if (!is_numeric($t[2])) return -3;

if ($t[2]<1902 || $t[2]>2037) return -3;

return mktime (0,0,0, $t[0], $t[1], $t[2]);
}



?>
