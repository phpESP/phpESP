<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

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

	$sql = "SELECT status FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE	id=".$HTTP_SESSION_VARS['survey_id'];
	$result = execute_sql($sql);
	if(count($result) < 1) {
		db_close($result);
		$HTTP_SESSION_VARS['survey_id']    = '';
		$HTTP_SESSION_VARS['survey_realm'] = '';
		include(esp_where('index'));
		return;
	}
	$status = $result->fields[0];
	db_close($result);
	$status |= STATUS_DELETED;
	$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['survey_table']." SET status=${status} WHERE id=".$HTTP_SESSION_VARS['survey_id'];
	execute_sql($sql);
	$HTTP_SESSION_VARS['survey_id']    = '';
	$HTTP_SESSION_VARS['survey_realm'] = '';
	include(esp_where('index'));
	return;
?>
