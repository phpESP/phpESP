<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	@session_unregister('survey_id');
	@session_destroy();
	include(esp_where('index'));
?>
