<?php

# $Id$

// Written by James Flemer <jflemer@acm.jhu.edu>
//        and Romans Jasins <roma@latnet.lv>

	require("/usr/home/jflemer/phpESP/admin/phpESP.ini");
	require($PIECES."/funcs".$EXT);

	if(empty($sid)) {
		echo(mkerror('Error processing survey: Survey not specified.'));
		return;
	}
	
	if(empty($HTTP_POST_VARS['userid'])) {
		// find remote user id (takes the first non-empty of the folowing)
		//  1. a GET variable named 'userid'
		//  2. the PHP_AUTH_USER
		//  3. the REMOTE_USER set by HTTP-Authintication
		//  4. the query string
		if (!empty($HTTP_GET_VARS['userid'])) {
			$userid = $HTTP_GET_VARS['userid'];
		} elseif(!empty($PHP_AUTH_USER)) {
			$userid = $PHP_AUTH_USER;
		} elseif(!empty($REMOTE_USER)) {
			$userid = $REMOTE_USER;
		} elseif(!empty($QUERY_STRING)) {
			$userid = urldecode($QUERY_STRING);
		}
		$HTTP_POST_VARS['userid'] = $userid;
	}

	if(empty($HTTP_POST_VARS['referer']))
		$referer = $HTTP_REFERER;

	if(empty($HTTP_POST_VARS['sec'])) {
		// check for authorization on the survey
		include($PIECES."/auth".$EXT);
		if(!survey_auth($sid,$PHP_AUTH_USER,$PHP_AUTH_PW))
			return;
	}

	// show results instead of show survey
	// but do not allow getting results from URL or FORM
	if($results && empty($HTTP_GET_VARS['results']) && empty($HTTP_POST_VARS['results'])) {
		survey_results($sid);
		return;
	}

	// else draw the survey
	$sql = "SELECT status FROM surveys WHERE id='${sid}'";
	$result = mysql_query($sql);
	$status = mysql_result($result,0,0);
	if($status & ( STATUS_DONE | STATUS_DELETED )) {
		echo(mkerror('Error processing survey: Survey is not active.'));
		return;
	}
	if(!($status & STATUS_ACTIVE)) {
		if(!($test && ($status & STATUS_TEST))) {
			echo(mkerror('Error processing survey: Survey is not active.'));
			return;
		}
	}

	$num_sections = count_sections($sid);

	if(empty($HTTP_POST_VARS['sec']) || $HTTP_POST_VARS['sec'] < 1)
		$sec = 1;

	$msg = '';

	if(!empty($submit)) {
		$msg = check_required($sid,$sec);
		if(empty($msg)) {
			$rid = insert_values($sid,$sec,$rid);
			complete_response($rid);
			email_values($sid,$rid);
			goto_thankyou($sid,$referer);
			$rid = '';
			return;
		}
	}

	if(!empty($HTTP_POST_VARS['next'])) {
		$msg = check_required($sid,$sec);
		if(empty($msg)) {
			$rid = insert_values($sid,$sec,$rid);
			$sec++;
		}
	}
?>
<form method="post" action="<?php echo($PHP_SELF); ?>">
<input type="hidden" name="referer" value="<?php echo($referer); ?>">
<input type="hidden" name="userid" value="<?php echo($userid); ?>">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="rid" value="<?php echo($rid); ?>">
<input type="hidden" name="sec" value="<?php echo($sec); ?>">


<?php	render_survey($sid,$sec,$msg); ?>
<?php	if($sec == $num_sections)	{ ?>
	<input type="submit" name="submit" value="Finish">
<?php	} else { ?>
	<input type="submit" name="next" value="Next Page">
<?php	} ?>
</form>
