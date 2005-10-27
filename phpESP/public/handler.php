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

	if (!defined('ESP_BASE'))
		define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

	require_once(ESP_BASE . '/admin/phpESP.ini.php');
	require_once($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);
    if (!isset($cfg['adodb_conn'])){
        esp_init_adodb();
    }
	require_once($ESPCONFIG['handler_prefix']);
	if(!defined('ESP-AUTH-OK')) {
		if (!empty($GLOBALS['errmsg']))
			echo($GLOBALS['errmsg']);
		return;
	}
    
    $_REQUEST['direct'] = '';

	if (empty($_REQUEST['referer'])) {
		$_REQUEST['referer'] = '';
        $_REQUEST['direct'] = 1;
    }

    if (isset($_REQUEST['test'])) {
        $test = $_REQUEST['test'];
    }
    else {
        $test = 0;
    }

    if (!isset($_REQUEST['sec'])) {
        $_REQUEST['sec'] = 1;
    }

    if (!isset($_REQUEST['rid'])) {
        $_REQUEST['rid'] = "";
    }

    $_REQUEST['rid'] = intval($_REQUEST['rid']);
    $test = intval($test);
    $_REQUEST['direct'] = intval($_REQUEST['direct']);
    $_REQUEST['referer'] = htmlspecialchars($_REQUEST['referer']);


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

	// else draw the survey
	$sql = "SELECT status, name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
	$result = execute_sql($sql);
    if ($result && record_count($result) > 0)
    	list ($status, $name) = fetch_row($result);
    else
        $status = 0;

	if($status & ( STATUS_DONE | STATUS_DELETED )) {
		echo(mkerror(_('Error processing survey: Survey is not active.')));
		return;
	}
	if(!($status & STATUS_ACTIVE)) {
		if(!(isset($test) && $test && ($status & STATUS_TEST))) {
			echo(mkerror(_('Error processing survey: Survey is not active.')));
			return;
		}
	}

    if ($_REQUEST['referer'] == $ESPCONFIG['autopub_url'])
        $_REQUEST['referer'] .= "?name=$name";

	$num_sections = survey_num_sections($sid);

	$msg = '';

	$action = $ESPCONFIG['proto'] . $_SERVER['HTTP_HOST'] . htmlspecialchars($_SERVER['PHP_SELF']);
	if (!empty($_SERVER['QUERY_STRING']))
		$action .= "?" . htmlspecialchars($_SERVER['QUERY_STRING']);

	if(!empty($_REQUEST['submit'])) {
		$msg = response_check_required($sid,$_REQUEST['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $_REQUEST['rid'], $_REQUEST['sec']);
			$_REQUEST['rid'] = response_insert($sid,$_REQUEST['sec'],$_REQUEST['rid']);
			response_commit($_REQUEST['rid']);
			response_send_email($sid,$_REQUEST['rid']);
			goto_thankyou($sid,$_REQUEST['referer']);
			return;
		}
	}

	if(!empty($_REQUEST['resume']) && $ESPCONFIG['auth_response'] && auth_get_option('resume')) {
        response_delete($sid, $_REQUEST['rid'], $_REQUEST['sec']);
		$_REQUEST['rid'] = response_insert($sid,$_REQUEST['sec'],$_REQUEST['rid']);
        if ($action == $ESPCONFIG['autopub_url'])
    		goto_saved("$action?name=$name");
        else
            goto_saved($action);
		return;
	}

	if(!empty($_REQUEST['next'])) {
		$msg = response_check_required($sid,$_REQUEST['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $_REQUEST['rid'], $_REQUEST['sec']);
			$_REQUEST['sec']++;
		}
	}
	
	if (!empty($_REQUEST['prev']) && $ESPCONFIG['auth_response'] && auth_get_option('navigate')) {
		if(empty($msg)) {
            if (auth_get_option('resume'))
                response_delete($sid, $_REQUEST['rid'], $_REQUEST['sec']);
			$_REQUEST['rid'] = response_insert($sid,$_REQUEST['sec'],$_REQUEST['rid']);
			$_REQUEST['sec']--;
		}
	}
    
    if ($ESPCONFIG['auth_response'] && auth_get_option('resume') && $_REQUEST['rid']>0)
        response_import_sec($sid, $_REQUEST['rid'], $_REQUEST['sec']);
	
?>
<form method="post" name="phpesp_response" action="<?php echo($action); ?>">
<input type="hidden" name="referer" value="<?php echo ($_REQUEST['referer']); ?>">
<input type="hidden" name="direct" value="<?php echo($_REQUEST['direct']); ?>">
<input type="hidden" name="userid" value="<?php echo($_REQUEST['userid']); ?>">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="rid" value="<?php echo($_REQUEST['rid']); ?>">
<input type="hidden" name="sec" value="<?php echo($_REQUEST['sec']); ?>">
<input type="hidden" name="name" value="<?php echo($name); ?>">
<?php	survey_render($sid,$_REQUEST['sec'],$msg); ?>
<?php
		if ($ESPCONFIG['auth_response']) {
			if (auth_get_option('navigate') && $_REQUEST['sec'] > 1) { ?>
	<input type="submit" name="prev" value="Previous Page">
<?php
			}
			if (auth_get_option('resume')) { ?>
	<input type="submit" name="resume" value="Save">
<?php
			}
		}
		if($_REQUEST['sec'] == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Submit Survey">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>
