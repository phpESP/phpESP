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

	require('/usr/local/lib/php/contrib/phpESP/admin/phpESP.ini');
	require($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);
	require($ESPCONFIG['handler_prefix']);
	if(!defined('AUTHHAND-OK')) {
		if (!empty($GLOBALS['errmsg']))
			echo($GLOBALS['errmsg']);
		return;
	}

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
	$sql = "SELECT status, name FROM survey WHERE id='${sid}'";
	$result = mysql_query($sql);
    if ($result && mysql_num_rows($result) > 0)
    	list ($status, $name) = mysql_fetch_row($result);
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

	$num_sections = survey_num_sections($sid);

	$msg = '';

	$action = 'http://' . $HTTP_SERVER_VARS['HTTP_HOST'] . $HTTP_SERVER_VARS['PHP_SELF'];
	if (!empty($HTTP_SERVER_VARS['QUERY_STRING']))
		$action .= "?" . $HTTP_SERVER_VARS['QUERY_STRING'];

	if(!empty($HTTP_POST_VARS['submit'])) {
		$msg = response_check_required($sid,$HTTP_POST_VARS['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $HTTP_POST_VARS['rid'], $HTTP_POST_VARS['sec']);
			$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
			response_commit($HTTP_POST_VARS['rid']);
			response_send_email($sid,$HTTP_POST_VARS['rid']);
			goto_thankyou($sid,$HTTP_POST_VARS['referer']);
			return;
		}
	}

	if(!empty($HTTP_POST_VARS['resume']) && $ESPCONFIG['auth_response'] && auth_get_option('resume')) {
        response_delete($sid, $HTTP_POST_VARS['rid'], $HTTP_POST_VARS['sec']);
		$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
        if ($action == $ESPCONFIG['auto_handler'])
    		goto_saved("$action?name=$name");
        else
            goto_saved($action);
		return;
	}

	if(!empty($HTTP_POST_VARS['next'])) {
		$msg = response_check_required($sid,$HTTP_POST_VARS['sec']);
		if(empty($msg)) {
            if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
                response_delete($sid, $HTTP_POST_VARS['rid'], $HTTP_POST_VARS['sec']);
			$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
			$HTTP_POST_VARS['sec']++;
		}
	}
	
	if (!empty($HTTP_POST_VARS['prev']) && $ESPCONFIG['auth_response'] && auth_get_option('navigate')) {
		if(empty($msg)) {
            if (auth_get_option('resume'))
                response_delete($sid, $HTTP_POST_VARS['rid'], $HTTP_POST_VARS['sec']);
			$HTTP_POST_VARS['rid'] = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
			$HTTP_POST_VARS['sec']--;
		}
	}
    
    if ($ESPCONFIG['auth_response'] && auth_get_option('resume'))
        response_import_sec($sid, $HTTP_POST_VARS['rid'], $HTTP_POST_VARS['sec']);
	
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
<input type="hidden" name="referer" value="<?php echo($HTTP_POST_VARS['referer']); ?>">
<input type="hidden" name="userid" value="<?php echo($HTTP_POST_VARS['userid']); ?>">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="rid" value="<?php echo($HTTP_POST_VARS['rid']); ?>">
<input type="hidden" name="sec" value="<?php echo($HTTP_POST_VARS['sec']); ?>">
<?php	survey_render($sid,$HTTP_POST_VARS['sec'],$msg); ?>
<?php
		if ($ESPCONFIG['auth_response']) {
			if (auth_get_option('navigate') && $HTTP_POST_VARS['sec'] > 1) { ?>
	<input type="submit" name="prev" value="Previous Page">
<?php
			}
			if (auth_get_option('resume')) { ?>
	<input type="submit" name="resume" value="Save">
<?php
			}
		}
		if($HTTP_POST_VARS['sec'] == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Submit Survey">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>
