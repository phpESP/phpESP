<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");
	session_register("new_survey");
	session_register("curr_q");
	$survey_id = '';
	$new_survey = 1;
	$curr_q = 'new';
	$tab="general";
	if(!file_exists($PIECES."/".$where.$EXT))
		$where="index";
	include($PIECES."/tab".$EXT);

?>
