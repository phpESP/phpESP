<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

#	global $HTTP_SERVER_VARS, $HTTP_POST_VARS, $HTTP_GET_VARS;
	require($HTTP_SERVER_VARS['DOCUMENT_ROOT']. "/phpESP/admin/phpESP.ini");
	require($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);

	if(isset($HTTP_GET_VARS['sid'])) {
		echo(mkerror(_('Error processing survey: Security violation.')));
		return;
	}
	
	if(isset($HTTP_GET_VARS['results']) || isset($HTTP_POST_VARS['results'])) {
		echo(mkerror(_('Error processing survey: Security violation.')));
		return;
	}
	
	$sid = intval($sid);
	if(empty($sid))
		$sid = intval($HTTP_POST_VARS['sid']);

	if(empty($sid)) {
		echo(mkerror(_('Error processing survey: Survey not specified.')));
		return;
	}

	if(empty($HTTP_POST_VARS['userid'])) {
		// find remote user id (takes the first non-empty of the folowing)
		//  1. a GET variable named 'userid'
		//  2. the REMOTE_USER set by HTTP-Authintication
		//  3. the query string
		if (!empty($HTTP_GET_VARS['userid'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_GET_VARS['userid'];
		} elseif(!empty($HTTP_SERVER_VARS['REMOTE_USER'])) {
			$HTTP_POST_VARS['userid'] = $HTTP_SERVER_VARS['REMOTE_USER'];
		} elseif(!empty($HTTP_SERVER_VARS['QUERY_STRING'])) {
			$HTTP_POST_VARS['userid'] = urldecode($HTTP_SERVER_VARS['QUERY_STRING']);
		} else {
			$HTTP_POST_VARS['userid'] = 'unknown';
		}
	}

	if(empty($HTTP_POST_VARS['referer']))
		$HTTP_POST_VARS['referer'] = $HTTP_SERVER_VARS['HTTP_REFERER'];

	if(empty($HTTP_POST_VARS['sec']) || $HTTP_POST_VARS['sec'] < 1)
		$HTTP_POST_VARS['sec'] = 1;

	if(empty($HTTP_POST_VARS['rid']))
		$HTTP_POST_VARS['rid'] = '';

	// show results instead of show survey
	// but do not allow getting results from URL or FORM
	if($results) {
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
			goto_thankyou($sid,$HTTP_POST_VARS['referer']);
			return;
		}
	}

	if(!empty($HTTP_POST_VARS['next'])) {
		$msg = response_check_required($sid,$HTTP_POST_VARS['sec']);
		if(empty($msg)) {
			$rid = response_insert($sid,$HTTP_POST_VARS['sec'],$HTTP_POST_VARS['rid']);
			$HTTP_POST_VARS['sec']++;
		}
	}
?>
<form method="post" action="<?php echo($HTTP_SERVER_VARS['PHP_SELF']); ?>">
<input type="hidden" name="referer" value="<?php echo($HTTP_POST_VARS['referer']); ?>">
<input type="hidden" name="userid" value="<?php echo($HTTP_POST_VARS['userid']); ?>">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="rid" value="<?php echo($HTTP_POST_VARS['rid']); ?>">
<input type="hidden" name="sec" value="<?php echo($HTTP_POST_VARS['sec']); ?>">
<?php	render_survey($sid,$HTTP_POST_VARS['sec'],$msg); ?>
<?php	if($HTTP_POST_VARS['sec'] == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Submit Survey">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>
