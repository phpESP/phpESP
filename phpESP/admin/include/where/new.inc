<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/*
	session_register('survey_id');
	session_register('curr_q');
	session_register('new_survey');		// flag to distinguish new vs edit
	if(get_cfg_var('register_globals')) {
		$HTTP_SESSION_VARS['survey_id']  = &$survey_id;
		$HTTP_SESSION_VARS['curr_q']     = &$curr_q;
		$HTTP_SESSION_VARS['new_survey'] = &$new_survey;
	}
	$HTTP_SESSION_VARS['survey_id']  = '';
	$HTTP_SESSION_VARS['curr_q']     = 0;
	$HTTP_SESSION_VARS['new_survey'] = TRUE;

	$HTTP_POST_VARS['tab'] = 'general';
*/
	include(esp_where('tab'));
?>
