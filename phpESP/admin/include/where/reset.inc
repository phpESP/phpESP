<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	@session_unregister('survey_id');
	@session_destroy();
	include(esp_where('index'));
?>
