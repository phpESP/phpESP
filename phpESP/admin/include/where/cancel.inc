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

	if(empty($HTTP_SESSION_VARS['survey_id'])) {
		// no survey INSERTed yet, so just go back to management interface
		@session_unset();
		@session_destroy();
		include(esp_where('index'));
		return;
	}

	$sql = "SELECT status FROM survey WHERE	id='".$HTTP_SESSION_VARS['survey_id']."'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) < 1) {
		@session_unset();
		@session_destroy();
		include(esp_where('index'));
		return;
	}
	$status = mysql_result($result,0,0);
	$status |= STATUS_DELETED;
	$sql = "UPDATE survey SET status='${status}' WHERE id='".$HTTP_SESSION_VARS['survey_id']."'";
	$result = mysql_query($sql);
	@session_unset();
	@session_destroy();
	include(esp_where('index'));
	return;
?>
