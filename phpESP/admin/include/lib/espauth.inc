<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

//  phpESP Authentication Library Chooser
//  By: James Flemer <jflemer@alum.rpi.edu>

	$auth_type = $GLOBALS['ESPCONFIG']['auth_type'];
	if (empty($auth_type))
		$auth_type = 'default';

	if (!file_exists($GLOBALS['ESPCONFIG']['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension'])) {
		echo("<b>FATAL: Unable to set up authentication for type $auth_type. Aborting.</b>");
		exit;
	}

	require($ESPCONFIG['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension']);

?>
