<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_start();

 	$CONFIG = "phpESP.ini";

	if(!file_exists($CONFIG)) {
		echo('<b>'. _('Unable to open INI file. Aborting.'). '</b>');
		exit;
	}
	include($CONFIG);
?>
<HTML>
<!-- $Id$ -->
<HEAD>
	<TITLE><?php echo($ESPCONFIG['title']); ?></TITLE>
<?php
	if(!empty($ESPCONFIG['style_sheet'])) {
		echo("<LINK href=\"". $ESPCONFIG['style_sheet'] ."\" rel=\"stylesheet\" type=\"text/css\">\n");
	}
	if(!empty($ESPCONFIG['charset'])) {
		echo('<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset='. $ESPCONFIG['charset'] ."\">\n");
	}
?>
</HEAD>
<BODY <?php
	echo('BGCOLOR="'. $ESPCONFIG['main_bgcolor'] .'"');
	if(!empty($ESPCONFIG['link_color']))  echo(' LINK="'.  $ESPCONFIG['link_color']  .'"');
	if(!empty($ESPCONFIG['vlink_color'])) echo(' VLINK="'. $ESPCONFIG['vlink_color'] .'"');
	if(!empty($ESPCONFIG['alink_color'])) echo(' ALINK="'. $ESPCONFIG['alink_color'] .'"'); ?>>
<?php
	if($ESPCONFIG['DEBUG']) {
		include($ESPCONFIG['include_path']."/debug".$ESPCONFIG['extension']);
	}

	if(file_exists($ESPCONFIG['include_path']."/head".$ESPCONFIG['extension']))
		include($ESPCONFIG['include_path']."/head".$ESPCONFIG['extension']);

	if(!empty($HTTP_POST_VARS['where']))
		include(esp_where($HTTP_POST_VARS['where']));
	else
		include(esp_where($HTTP_GET_VARS['where']));

	if(file_exists($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']))
		include($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']);

?>
</BODY>
</HTML>
<?php exit; ?>
