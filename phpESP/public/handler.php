<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* When using the authentication for responses you need to include
 * part of the script *before* your template so that the
 * HTTP Auth headers can be sent when needed.
 *
 * See the handler-prefix.php file for details.
 */

	require('/www/phpesp/admin/phpESP.ini');
	require($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);
	require($ESPCONFIG['handler_prefix']);
	if(!defined('AUTHHAND-OK')) {
		if (!empty($GLOBALS['errmsg']))
			echo($GLOBALS['errmsg']);
		return;
	}

	if(empty($HTTP_POST_VARS['rid']))
		$HTTP_POST_VARS['rid'] = '';
	
	if (empty($HTTP_POST_VARS['sec']))
		$HTTP_POST_VARS['sec'] = 1;
	else
		$HTTP_POST_VARS['sec'] = intval($HTTP_POST_VARS['sec']);

	if (empty($HTTP_POST_VARS['referer']))
		$HTTP_POST_VARS['referer'] = '';

	// show results instead of show survey
	// but do not allow getting results from URL or FORM
	if(isset($results) && $results) {
		// small security issue here, anyone could pick a QID to crossanalyze
		survey_results($sid,$precision,$totals,$qid,$cids);
		return;
	}

	// else draw the survey
	$sql = "SELECT status FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
	$status = @mysql_result($result,0,0);
	if($status & ( STATUS_DONE | STATUS_DELETED )) {
		echo(mkerror(_('Error processing survey: Survey is not active.')));
		return;
	}
	if(!($status & STATUS_ACTIVE)) {
		if(!($test && ($status & STATUS_TEST))) {
			echo(mkerror(_('Error processing survey: Survey is not active.')));
			return;
		}
	}

	$num_sections = survey_num_sections($sid);

	$msg = '';

	if(!empty($HTTP_POST_VARS['submit'])) {
		$msg = response_check_required($sid,$HTTP_POST_VARS['sec']);
		if(empty($msg)) {
			$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
			response_commit($HTTP_POST_VARS['rid']);
			response_send_email($sid,$HTTP_POST_VARS['rid']);
			goto_thankyou($sid,$HTTP_POST_VARS['referer'], $HTTP_POST_VARS['userid']);
			return;
		}
	}


	if(!empty($HTTP_POST_VARS['next'])) {
		$msg = response_check_required($sid,$HTTP_POST_VARS['sec']);
		if(empty($msg)) {
			$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);

            // figure out what skips are from last page
            // see if any evaluate to true
            //
            // loop around each skip (use post var "skip_count")
            //
            $new_section = 0;          // initialize to simply "next page"
            for ($i=0; $i<$HTTP_POST_VARS['skip_count']; $i++){

                // get POST variable corrosponding to skip_i_id to find result
                //
                $question_id = $HTTP_POST_VARS["skip_".$i."_id"];
                $intended_response = $HTTP_POST_VARS["skip_".$i."_value"];
                $target_section = $HTTP_POST_VARS["skip_".$i."_target"];

                // if we just finished last question
                //
                if ($target_section == 0){
                    $sid=$sid*-1;
                    break;
                }

                // find result from previous page
                //
                $answer = $HTTP_POST_VARS[$question_id];
                $answer_is_array = false;
                if (is_array($answer)){
                    $answer_is_array = true;
                }

                if ($HTTP_POST_VARS["date_field"] != "" && $HTTP_POST_VARS["date_field"] != null){
                    if ($HTTP_POST_VARS["date_field"] == $question_id){
                        $answer = round(str2date($answer), -3);                 // have to round because db rounds it
                        $intended_response = sprintf("%f", $intended_response);
                        $intended_response = $intended_response;
                    }
                }


                // in case of !other questions:
                // 
                // use answer[]  loop through the array for each op below?

                if ($HTTP_POST_VARS[$question_id."_".$o_answer_array[1]]){
                    $answer = $o_answer_array[1];
                }
                if ($answer == 'Y') $answer = 1;        // a little touch-up work here
                if ($answer == 'N') $answer = 0;        // and here


                // if this is a direct GOTO, simply go ahead
                //
                if ($intended_response == -1){ 
                    $new_section = $target_section;
                    break;
                }

                // find the OP, substitute text (eg NOT EQUALS) with symbol (eg <>)
                //
                $op_txt = $HTTP_POST_VARS["skip_".$i."_op"];
                $position = strpos($op_txt, "_#_");
                $rating_skip = false;
                $rating_num = 0;


                if ($postition !== FALSE && $position != null){
                    $rating_skip = true;
                    $op_sub_txt = substr($op_txt, 0, $position);
                    $position += 3;
                    $rating_num = substr($op_txt, $position);
                    $op_txt = $op_sub_txt;
                } else {
                    $op_txt = $HTTP_POST_VARS["skip_".$i."_op"];
                }


                if ($op_txt == "EQUALS"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val == $intended_response) $new_section = $target_section;
                    } else if ($answer_is_array){
                        if (in_array($intended_response, $answer)) $new_section = $target_section;
                    } else {
                        if ($answer == $intended_response) $new_section = $target_section;
                    }
                    // loop here?
                }
                else if ($op_txt == "NOT_EQUAL_TO"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val != $intended_response) $new_section = $target_section;
                    } else if ($answer_is_array){
                        if (!in_array($intended_response, $answer)) $new_section = $target_section;
                    } else {
                        if ($answer != $intended_response) $new_section = $target_section;
                    }
                    // and here? etc...
                }
                else if ($op_txt == "LESS_THAN"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val < $intended_response) $new_section = $target_section;
                    } else if ($answer < $intended_response) $new_section = $target_section;
                }
                else if ($op_txt == "GREATER_THAN"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val > $intended_response) $new_section = $target_section;
                    } else if ($answer > $intended_response) $new_section = $target_section;
                }
                else if ($op_txt == "LESS_THAN_OR_EQUAL_TO"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val <= $intended_response) $new_section = $target_section;
                    } else if ($answer <= $intended_response) $new_section = $target_section;
                }
                else if ($op_txt == "GREATER_THAN_OR_EQUAL_TO"){ 
                    if ($rating_skip){
                        $question_rate_num = $question_id . "_" . $rating_num;
                        $question_rate_val = $HTTP_POST_VARS["$question_rate_num"];
                        $question_rate_val += 1;                                        // stupid original programming fix
                        if ($question_rate_val >= $intended_response) $new_section = $target_section;
                    } else if ($answer >= $intended_response) $new_section = $target_section;
                }
            }
//            echo "new section: $new_section <br>";

            // calculate page number from section id
            //
            $page_num_sql = "SELECT id FROM question WHERE type_id=99 AND deleted='N' AND survey_id=$sid ORDER BY position ASC";
            $page_num_result = mysql_query($page_num_sql);

            $page_number = 1;
            $found_page = false;
            while ($page_num_object = mysql_fetch_object($page_num_result)){
                $section_id = $page_num_object->id;
//                echo "$section_id <br>" ;
                $page_number++;
                if ($section_id == $new_section) {
                    $found_page = true;
                    break;
                }
            }
            if (!$found_page && $HTTP_POST_VARS['sec'] != 1){ 
//                echo "ERROR: couldn't determine proper page. starting over. <br>";
//                $page_number = 1;
			      $HTTP_POST_VARS['sec']++;
            }
//            echo "go to page $page_number <br>";
			$HTTP_POST_VARS['sec'] = $page_number;
		}
        else {
            // missed a required field
            // somehow dont increment question number here?
        }
	}
	
	$action = $HTTP_SERVER_VARS['PHP_SELF'];
	if (!empty($HTTP_SERVER_VARS['QUERY_STRING']))
		$action .= "?" . $HTTP_SERVER_VARS['QUERY_STRING'];
?>
<form method="post" name="phpesp_response" action="<?php echo($action); ?>">
<input type="hidden" name="referer" value="<?php echo($HTTP_POST_VARS['referer']); ?>">
<input type="hidden" name="userid" value="<?php echo($HTTP_POST_VARS['userid']); ?>">
<input type="hidden" name="sid" value="<?php if ($sid < 0) echo($sid*-1); else echo $sid; ?>">
<input type="hidden" name="rid" value="<?php echo($HTTP_POST_VARS['rid']); ?>">
<input type="hidden" name="sec" value="<?php echo($HTTP_POST_VARS['sec']); ?>">
<?php	if ($sid >= 0 ) { render_survey($sid,$HTTP_POST_VARS['sec'],$msg); ?>
<?php            
    //
    // figure out what questions are on this page
    $qlist = survey_question_list($sid,$HTTP_POST_VARS['sec'],$msg);
    $qlist_print = implode(", ", $qlist);
    //
    // figure out if any questions on this page have skips on them
    $skip_sql = "SELECT * FROM question WHERE length IN (" . $qlist_print .") AND type_id=98 AND deleted='N'";
    $skip_result = mysql_query($skip_sql);

    $skip_counter = 0;
    while ($skip_object = mysql_fetch_object($skip_result)) {
//        echo "if $skip_object->length $skip_object->name $skip_object->precise then goto section $skip_object->result_id <br>";
        echo "<input type=hidden name=skip_".$skip_counter."_id value=".$skip_object->length."> \n";
        echo "<input type=hidden name=skip_".$skip_counter."_op value=".$skip_object->name."> \n";
        echo "<input type=hidden name=skip_".$skip_counter."_value value=".$skip_object->precise."> \n";
        echo "<input type=hidden name=skip_".$skip_counter."_target value=".$skip_object->result_id."> \n";
        $skip_counter++;
    }
    echo "<input type=hidden name=skip_count value=".$skip_counter."> \n";
}
if ($sid < 0){
    echo "You have completed the survey.<br><br>";
    $sid = $sid * -1;
    $HTTP_POST_VARS['sec'] = $num_sections;
}
?>
<?php	if($HTTP_POST_VARS['sec'] == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Submit Survey">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>

<?php
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
