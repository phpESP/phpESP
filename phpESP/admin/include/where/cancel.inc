<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register('survey_id');
	session_register('survey_realm');
	if(ini_get('register_globals')) {
		$HTTP_SESSION_VARS['survey_id']    = &$survey_id;
		$HTTP_SESSION_VARS['survey_realm'] = &$survey_realm;
	}

	if(empty($HTTP_SESSION_VARS['survey_id'])) {
		// no survey INSERTed yet, so just go back to management interface
		$HTTP_SESSION_VARS['survey_id']    = '';
		$HTTP_SESSION_VARS['survey_realm'] = '';
		include(esp_where('index'));
		return;
	}

	$sql = "SELECT status FROM survey WHERE	id='".$HTTP_SESSION_VARS['survey_id']."'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) < 1) {
		mysql_free_result($result);
		$HTTP_SESSION_VARS['survey_id']    = '';
		$HTTP_SESSION_VARS['survey_realm'] = '';
		include(esp_where('index'));
		return;
	}
	$status = mysql_result($result,0,0);
	mysql_free_result($result);
	$status |= STATUS_DELETED;
	$sql = "UPDATE survey SET status='${status}' WHERE id='".$HTTP_SESSION_VARS['survey_id']."'";
	mysql_query($sql);
	$HTTP_SESSION_VARS['survey_id']    = '';
	$HTTP_SESSION_VARS['survey_realm'] = '';
	include(esp_where('index'));
	return;
?>
