<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register('survey_id');
	if(get_cfg_var('register_globals')) {
		$HTTP_SESSION_VARS['survey_id'] = &$survey_id;
	}

	unset($HTTP_SESSION_VARS['new_survey']);
	unset($HTTP_SESSION_VARS['last_tab']);
	unset($HTTP_SESSION_VARS['curr_q']);
	session_unregister('new_survey');
	session_unregister('last_tab');
	session_unregister('curr_q');

	if(empty($HTTP_SESSION_VARS['survey_id'])) {
		// no survey INSERTed yet, so just go back to management interface
		unset($HTTP_SESSION_VARS['survey_id']);
		session_unregister('survey_id');
		include(esp_where('index'));
		return;
	}

	$sql = "SELECT status FROM survey WHERE	id='".$HTTP_SESSION_VARS['survey_id']."'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) < 1) {
		unset($HTTP_SESSION_VARS['survey_id']);
		session_unregister('survey_id');
		include(esp_where('index'));
		return;
	}
	$status = mysql_result($result,0,0);
	$status |= STATUS_DELETED;
	$sql = "UPDATE survey SET status='${status}' WHERE id='".$HTTP_SESSION_VARS['survey_id']."'";
	$result = mysql_query($sql);
	unset($HTTP_SESSION_VARS['survey_id']);
	session_unregister('survey_id');
	include(esp_where('index'));
	return;
?>
