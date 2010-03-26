<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

/* When using the authentication for responses you need to include
 * part of the script *before* your template so that the
 * HTTP Auth headers can be sent when needed.
 *
 * See the handler-prefix.php file for details.
 */

	// add this at the top of every php script using this one:
	// require_once("./phpESP.first.php");

	if (!defined('ESP-FIRST-INCLUDED')) {
		echo "In order to conduct surveys, please include phpESP.first.php in your php script!!!";
		exit;
	}

	require_once($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);
	require_once($ESPCONFIG['handler_prefix']);
	if(!defined('ESP-AUTH-OK')) {
		if (!empty($GLOBALS['errmsg']))
			echo($GLOBALS['errmsg']);
		return;
	}

    // did we show feedback on the last page of the survey?
    // SFID: 2771740
    if (isset($_REQUEST['feedback']) && is_scalar($feedback = $_REQUEST['feedback']) && 'finished' == $feedback) {
        // then, the next step is to finish up the survey, since the feedback "interrupted" that processing
        all_done();
    }

	// get the survey
	$sql = "SELECT status, name, public, open_date, close_date FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
	$result = execute_sql($sql);
        if ($result && record_count($result) > 0)
       	   list ($status, $name, $survey_public, $open_date, $close_date) = fetch_row($result);
        else
           $status = 0;

	// Added for cookie auth, to eliminate double submits
	// only for public surveys
	$cookiename="survey_".$sid;
	if (($GLOBALS['ESPCONFIG']['limit_double_postings']>0) &&
	     isset($_COOKIE["$cookiename"]) &&
	     $survey_public=='Y' &&
	     !($ESPCONFIG['auth_response'] && auth_get_option('resume'))) {
			echo (mkerror(_('You have already completed this survey.')));
			return;
	}
		
    
	$request_direct = 0;
	$request_referer = '';

	if (!empty($_REQUEST['referer'])) {
		$request_referer = htmlspecialchars($_REQUEST['referer']);
	} else if (isset($_SERVER['HTTP_REFERER'])) {
		$request_referer = htmlspecialchars($_SERVER['HTTP_REFERER']);
	} else {
		// referer not set? Then we must be called directly
 		$request_direct = 1;
	}

//	$num_sections = survey_num_sections($sid);
//	if (!isset($_SESSION['sec']) || empty($_SESSION['sec']) || $_SESSION['sec']>$num_sections) {
//        	$_SESSION['sec'] = 1;
//    	} else {
//        	$_SESSION['sec'] = (intval($_SESSION['sec']) > 0) ?
//                		    intval($_SESSION['sec']) : 1;
//	}

// gets wrong for resumed surveys
//	if ($_SESSION['sec'] == 1) {
//	    $_SESSION['rid'] = 0;
//	}

	if (!isset($_SESSION['rid'])) {
            $_SESSION['rid'] = 0;
	}

	$_SESSION['rid'] = intval($_SESSION['rid']);

	// show results instead of show survey
	// but do not allow getting results from URL or FORM
	if(isset($results) && $results) {
           if (!isset($precision)) {
               $precision = '';
           }
           if (!isset($totals)) {
               $totals = '';
           }
           if (!isset($qid)) {
               $qid = '';
           }
           if (!isset($cids)) {
               $cids = '';
           }
           $precision = intval($precision);
           $totals = intval($totals);
           $qid = intval($qid);
           $cids = intval($cids);
	   // small security issue here, anyone could pick a QID to crossanalyze
	   survey_results($sid,$precision,$totals,$qid,$cids);
	   return;
	}

    // may this survey be accessed?
    esp_require_once('/lib/espsurvey');
    if (survey_status_is_edit($status) || survey_status_is_done($status) || survey_status_is_deleted($status)) {
        $isActive = false;
    } else if (survey_status_is_test($status)) {
        if (isset($_REQUEST['test']) && $_REQUEST['test']) {
            $isActive = true;
        } else {
            $isActive = false;
        }
    } else if (STATUS_OPEN !== survey_open($open_date, $close_date)) {
        $isActive = false;
    } else {
        $isActive = true;
    }
    if (! $isActive) {
		echo(mkerror(_('Error processing survey: Survey is not active.')));
        return;
    }

   	if ($request_referer == $ESPCONFIG['autopub_url'])
       	    $request_referer .= "?sid=$sid";

	// let's build the correct return/submit/resume link
	$action = $ESPCONFIG['proto'] . $_SERVER['HTTP_HOST'] . htmlspecialchars($_SERVER['PHP_SELF']);
	$query_string="";
	// we need to remove "sec=xx" from the query string, otherwise
	// the resume link will contain this also and the user will always
	// return to the same filled in section
	if (isset($_SERVER['QUERY_STRING']) && !empty($_SERVER['QUERY_STRING'])) {
	    $query_string=$_SERVER['QUERY_STRING'];
	}
	$query_string=preg_replace ('/sec=\d+/s','',$query_string);
	$query_string=preg_replace ('/\?$|\&$/s','',$query_string);
	$query_string=preg_replace ('/\?\&/s','\?',$query_string);
	
	if (!empty($query_string))
	    $action .= "?" . htmlspecialchars($query_string);

	$msg = '';
	if(!empty($_REQUEST['submit'])) {
	    $msg .= response_check_answers($sid,$_SESSION['rid'],$_SESSION['sec']);

	    # we only check the captcha if no all required 
        if (empty($msg) && $ESPCONFIG['use_captcha']) {
            require_once(ESP_BASE.'public/captcha_check.php');
            $msg .= response_check_captcha("captcha_check",1);
        }   

        if (empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume')) {
                // submitting a previously saved survey
                esp_require_once('/lib/espsurveystat');
                survey_stat_decrement(SURVEY_STAT_SUSPENDED, $sid);

                // delete the previous responses
                response_delete($sid, $_SESSION['rid'], $_SESSION['sec']);
            }

            $_SESSION['rid'] = response_insert($sid,$_SESSION['sec'],$_SESSION['rid']);

            // paint the feedback
            // NOTE: This function may exit
            paint_feedback_end_of_survey($sid, $_SESSION['rid'], $_SESSION['sec']);

            // no feedback, so jump to thank you
            all_done();
        }
	}

    // if we encounter the variable resume in the $_REQUEST
    // the user wants to come back later
	if(!empty($_REQUEST['resume']) && $ESPCONFIG['auth_response'] && auth_get_option('resume')) {
        response_delete($sid, $_SESSION['rid'], $_SESSION['sec']);
		$_SESSION['rid'] = response_insert($sid,$_SESSION['sec'],$_SESSION['rid']);
        if ($action == $ESPCONFIG['autopub_url'])
    		goto_saved($sid, "$action?sid=$sid");
        else
            goto_saved($sid, $action);
		return;
	}

	if(!empty($_REQUEST['next'])) {
		$msg = response_check_answers($sid,$_SESSION['rid'],$_SESSION['sec']);
		if(empty($msg)) {
            // record the response
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $_SESSION['rid'], $_SESSION['sec']);
            $_SESSION['rid'] = response_insert($sid,$_SESSION['sec'],$_SESSION['rid']);

            // show the feedback
            // NOTE: This method exits
            paint_feedback_end_of_section($sid, $_SESSION['rid'], $_SESSION['sec']);
		}
	}
	
	if (!empty($_REQUEST['prev']) && $ESPCONFIG['auth_response'] && auth_get_option('navigate')) {
		if(empty($msg)) {
            		if (auth_get_option('resume'))
               			response_delete($sid, $_SESSION['rid'], $_SESSION['sec']);
			$_SESSION['rid'] = response_insert($sid,$_SESSION['sec'],$_SESSION['rid']);
			$_SESSION['sec']--;
		}
	}

    // record start statistics
    // ... increment the attempt
    // ... assume the user will abandon the survey
    // ... NOTE: this will be remedied if there is a save or a submit
    esp_require_once('/lib/espsurveystat');
    survey_stat_increment(SURVEY_STAT_ATTEMPTED, $sid);
    survey_stat_increment(SURVEY_STAT_ABANDONED, $sid);

    // if resuming a previous survey
    if ($ESPCONFIG['auth_response'] && auth_get_option('resume') && $_SESSION['rid']>0) {
        response_import_sec($sid, $_SESSION['rid'], $_SESSION['sec']);
        survey_stat_decrement(SURVEY_STAT_SUSPENDED, $sid);
    }
	
?>
<?php
    paint_submission_form_open();
    survey_render($sid,$_SESSION['sec'],$_SESSION['rid'],$msg);
    echo '<fieldset>';
    if ($ESPCONFIG['auth_response']) {
        if (auth_get_option('navigate') && $_SESSION['sec'] > 1) {
            echo(mksubmit("prev", _('Previous Page')));
        }
        if (auth_get_option('resume')) {
            echo(mksubmit("resume", _('Save')));
        }
    }
    if($_SESSION['sec'] == $num_sections) {
        if ($ESPCONFIG['use_captcha']) {
            print '<table><tr><td><img src="'.$ESPCONFIG['base_url'].'public/captcha.php"></td>';
            print '<td>';
            echo _("Please fill in the code displayed here.");
            print '<br><input type="text" name="captcha_check"></td></tr></table>';
        }
        echo(mksubmit("submit", _('Submit Survey')));
    } else {
        echo(mksubmit("next", _('Next Page')));
    }
    echo '</fieldset>';
    paint_submission_form_close();
?>
<?php
/**
Functions to display feedback, if any, on the selected responses.
See SFID 2771740
See SFID 2771716
*/
function paint_feedback_end_of_survey($sid, $rid, $sec) {
    // paint the feedback
    // NOTE: if there is any, this function exits
    paint_feedback($sid, $rid, $sec, array ('feedback' => 'finished'));
}

function paint_feedback_end_of_section($sid, $rid, $sec) {
    // increment the section
    $_SESSION['sec']++;

    // paint my feedback
    paint_feedback($sid, $rid, $sec);
}

function paint_feedback($sid, $rid, $sec, $additional = array ()) {
    // get all the feedback so far, if any
    $hasFeedback = get_feedback($feedback, $totalCredit, $sid, $rid, $sec);

    // if there are is feedback, paint it and exit
    if ($hasFeedback) {
        // open the form
        paint_submission_form_open($additional);

        // paint feedback to each response
        echo '<table>';
        array_walk($feedback, 'paint_feedback_row');
        echo '</table>';

        // paint total credit
        if (! is_null($totalCredit)) {
            echo _('Total credit:') . ' ' . $totalCredit;
        }

        // paint the next button
        // NOTE: don't call it "next", because the logic above has specific expectations about
        // NOTE: the cases when that button is pressed
        echo '<fieldset>';
        echo mksubmit("go", _('Next Page'));
        echo '</fieldset>';

        // close the form
        paint_submission_form_close();

        // all done
        exit;
    }
}

/**
Returns (via PBR first parameter) the responses for the given survey ($sid), identified by the given response ID ($rid),
in the indicated section ($sec).  The format of $responses is:
array (
   // 'question number' => array ('Question?', array ('Choice1', 'Feedback1'), ..., array ('ChoiceN', 'FeedbackN'));
);
*/
function get_feedback(&$responses, &$totalCredit, $sid, $rid, $sec) {
    // get the questions for each section (by ID)
    // NOTE: we're given section 1-base, but this method has section 0-base
    $sec -= 1;
    $section_questions = survey_get_section_questions($sid);
    if (! array_key_exists($sec, $section_questions)) {
        // invalid section, can't possibly be any feedback
        return;
    }

    // figure out on what question number we start
    $qnum = 0;
    for ($i = 0; $i <= $sec; $i++) {
        $qnum += count($section_questions[$i]);
    }

    // get the responses for just this section
    $allResponses = response_select($sid, $rid, 'content', $section_questions[$sec]);

    // convert any rank or !other to multi-response format
    foreach ($allResponses as $qid => $feedback) {
        // if this is an other
        if (false !== strpos($qid, '_')) {
            // pull it out
            unset($allResponses[$qid]);

            /* TODO: If we ever support rank and !other as types with feedback or credit
            // store it as multi
            list ($qid,$sub) = explode('_', $qid);
            $allResponses[$qid][] = $feedback;
            */
        }
    }

    // initialize values
    $totalCredit = null;
    $hasFeedback = false;

    // put them into our desired format
    foreach ($allResponses as $qid => $feedback) {
        // figure out if this is a multi-response
        $hasMulti = (is_array($feedback[0]) ? true : false);

        // initialize the structure with the question name
        $responses[$qnum] = array ($hasMulti ? $feedback[0][0] : $feedback[0]);

        // if this is a multiple response, add them each in
        if ($hasMulti) {
            foreach ($feedback as $response) {
                if (defined($response[3]) && !empty($response[3])) {
                    $hasFeedback = true;
                } else {
	    	    $response[3]="";
	        }
                if (defined($response[4]) && !empty($response[4])) {
                    $hasFeedback = true;
                    if (is_numeric($response[4])) {
                        $totalCredit += $response[4];
                    }
                } else {
		    $response[4]="";
	        }
                $responses[$qnum][] = array ($response[1], $response[3], $response[4]);
            }

        // otherwise, add this one in
        } else {
            if (defined($feedback[3]) && !empty($feedback[3])) {
                $hasFeedback = true;
            } else {
		$feedback[3]="";
	    }
            if (defined($feedback[4]) && !empty($feedback[4])) {
                $hasFeedback = true;
                if (is_numeric($feedback[4])) {
                    $totalCredit += $feedback[4];
                }
            } else {
		$feedback[4]="";
	    }
            $responses[$qnum][] = array ($feedback[1], $feedback[3], $feedback[4]);
        }

        // increment the question number
        $qnum++;
    }

    return $hasFeedback;
}

function paint_feedback_row($response, $number) {
    // break apart the feedback data structure
    $question = array_shift($response);

    // initialize variables we'll use in our output
    $label1 = _('Your choice:');
    $label2 = _('Feedback:');
    $label3 = _('Credit:');

    // output the question
    echo <<<EOHTML
<tr>
  <td style='padding-top: .5em; text-align: right;'>{$number}.</td>
  <td style='padding-top: .5em;'>{$question}</td>
</tr>
EOHTML;

    // output each choice and feedback
    foreach ($response as $info) {
        // get the choice, feedback, and credit
        list ($choice, $feedback, $credit) = $info;

        // output the choice
        echo <<<EOHTML
<tr>
  <td style='text-align: right;'>{$label1}</td>
  <td>{$choice}</td>
</tr>
EOHTML;

        // if there's feedback, output it
        if (! empty($feedback)) {
            echo <<<EOHTML
<tr>
  <td style='text-align: right;'>{$label2}</td>
  <td>{$feedback}</td>
</tr>
EOHTML;
        }

        // if there's credit, output it
        if (! empty($credit)) {
            echo <<<EOHTML
<tr>
  <td style='text-align: right;'>{$label3}</td>
  <td>{$credit}</td>
</tr>
EOHTML;
        }
    }
}

/**
Refactored methods.
*/
function paint_submission_form_open($additional = array ()) {
    global $action, $sid, $name, $request_referer, $request_direct;
    echo <<<EOHTML
<form method="post" id="phpesp_response" action="$action">
<fieldset class="hidden">
<input type="hidden" name="referer" value="{$request_referer}" />
<input type="hidden" name="direct"  value="{$request_direct}" />
<input type="hidden" name="sid"     value="{$sid}" />
<input type="hidden" name="rid"     value="{$_SESSION['rid']}" />
<input type="hidden" name="sec"     value="{$_SESSION['sec']}" />
EOHTML;

    foreach ($additional as $field => $value) {
        echo "<input type='hidden' name='$field' value='$value' />";
    }

    echo <<<EOHTML
</fieldset>
EOHTML;
}

function paint_submission_form_close() {
    echo <<<EOHTML
</form>
EOHTML;
}

function all_done() {
    global $sid;

    // commit the response and send an email
    response_commit($_SESSION['rid']);
    response_send_email($sid,$_SESSION['rid']);

    // initialize the state variables
    $_SESSION['rid']="";
    $_SESSION['sec']="";

    // go to the thank you
    goto_thankyou($sid, $_REQUEST['referer']);
    exit;
}


?>
