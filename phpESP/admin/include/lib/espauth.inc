<?php

# $Id$

//  phpESP Authentication Library Chooser
//  By: James Flemer <jflemer@acm.jhu.edu>

	$auth_type = $GLOBALS['ESPCONFIG']['auth_type'];
	if (empty($auth_type))
		$auth_type = 'default';

	if (!file_exists($GLOBALS['ESPCONFIG']['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension'])) {
		echo("<b>Unable to set up authentication for type $auth_type. Aborting.</b>");
		exit;
	}

	require($ESPCONFIG['include_path']."/lib/espauth-$auth_type".$ESPCONFIG['extension']);

?>