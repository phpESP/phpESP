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
    

	if (empty($_POST['referer'])) {
		$_POST['referer'] = '';
        $_POST['direct'] = 1;
    }

    if (isset($_GET['test'])) {
        $test = $_GET['test'];
    }

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

    if ($_POST['referer'] == $ESPCONFIG['autopub_url'])
        $_POST['referer'] .= "?name=$name";

	$num_sections = survey_num_sections($sid);

	$msg = '';

	$action = $ESPCONFIG['proto'] . $_SERVER['HTTP_HOST'] . $_SERVER['PHP_SELF'];
	if (!empty($_SERVER['QUERY_STRING']))
		$action .= "?" . $_SERVER['QUERY_STRING'];

	if(!empty($_POST['submit'])) {
		$msg = response_check_required($sid,$_POST['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $_POST['rid'], $_POST['sec']);
			$_POST['rid'] = response_insert($sid,$_POST['sec'],$_POST['rid']);
			response_commit($_POST['rid']);
			response_send_email($sid,$_POST['rid']);
			goto_thankyou($sid,$_POST['referer']);
			return;
		}
	}

	if(!empty($_POST['resume']) && $ESPCONFIG['auth_response'] && auth_get_option('resume')) {
        response_delete($sid, $_POST['rid'], $_POST['sec']);
		$_POST['rid'] = response_insert($sid,$_POST['sec'],$_POST['rid']);
        if ($action == $ESPCONFIG['autopub_url'])
    		goto_saved("$action?name=$name");
        else
            goto_saved($action);
		return;
	}

	if(!empty($_POST['next'])) {
		$msg = response_check_required($sid,$_POST['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $_POST['rid'], $_POST['sec']);
			$_POST['rid'] = response_insert($sid,$_POST['sec'],$_POST['rid']);
			$_POST['sec']++;
		}
	}
	
	if (!empty($_POST['prev']) && $ESPCONFIG['auth_response'] && auth_get_option('navigate')) {
		if(empty($msg)) {
            if (auth_get_option('resume'))
                response_delete($sid, $_POST['rid'], $_POST['sec']);
			$_POST['rid'] = response_insert($sid,$_POST['sec'],$_POST['rid']);
			$_POST['sec']--;
		}
	}
    
    if ($ESPCONFIG['auth_response'] && auth_get_option('resume') && $_POST['rid']>0)
        response_import_sec($sid, $_POST['rid'], $_POST['sec']);
	
?>
<script language="JavaScript">
<!-- // Begin <?php // This should really go into <head> tag ?>

function other_check(name)
{
  other = name.split("_");
  var f = document.phpesp_response;
  for (var i=0; i<=f.elements.length; i++) {
    if (f.elements[i].value == "other_"+other[1]) {
      f.elements[i].checked=true;
      break;
    }
  }
}
// End -->
</script>
<form method="post" name="phpesp_response" action="<?php echo($action); ?>">
<input type="hidden" name="referer" value="<?php echo htmlspecialchars($_POST['referer']); ?>">
<input type="hidden" name="direct" value="<?php echo htmlspecialchars($_POST['direct']); ?>">
<input type="hidden" name="userid" value="<?php echo($_POST['userid']); ?>">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="rid" value="<?php echo($_POST['rid']); ?>">
<input type="hidden" name="sec" value="<?php echo($_POST['sec']); ?>">
<input type="hidden" name="name" value="<?php echo($name); ?>">
<?php	survey_render($sid,$_POST['sec'],$msg); ?>
<?php
		if ($ESPCONFIG['auth_response']) {
			if (auth_get_option('navigate') && $_POST['sec'] > 1) { ?>
	<input type="submit" name="prev" value="Previous Page">
<?php
			}
			if (auth_get_option('resume')) { ?>
	<input type="submit" name="resume" value="Save">
<?php
			}
		}
		if($_POST['sec'] == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Submit Survey">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>
