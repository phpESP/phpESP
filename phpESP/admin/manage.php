<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_start();

 	$CONFIG = "phpESP.ini";

	if(file_exists($CONFIG))
		include($CONFIG);

	$where = strtolower($where);
?>
<!-- $Id$ -->
<HTML>
<HEAD>
	<TITLE><?php echo($cf_title); ?></TITLE>
<?php
	if(!empty($cf_style_sheet)) {
		echo('<LINK href="'. $cf_style_sheet .'" rel="stylesheet" type="text/css">'."\n");
	}
?>
</HEAD>
<BODY BGCOLOR="<?php echo($cf_main_bgcolor); ?>"
 <?php if(!empty($cf_link_color)) echo("LINK=\"${cf_link_color}\""); ?>
 <?php if(!empty($cf_vlink_color)) echo("VLINK=\"${cf_vlink_color}\""); ?>
 <?php if(!empty($cf_alink_color)) echo("ALINK=\"${cf_alink_color}\""); ?>>
<?php

	if($DEBUG) {
		echo("<blockquote>\n");
		@reset ($HTTP_GET_VARS);
		while (list ($key, $val) = @each ($HTTP_GET_VARS)) {
			echo "$key => $val<br>\n";
		}
		@reset ($HTTP_POST_VARS);
		while (list ($key, $val) = @each ($HTTP_POST_VARS)) {
			echo "$key => $val<br>\n";
		}
		echo("</blockquote>\n");
	}

	if(empty($where))
		$where = "index";
	$where = ereg_replace(' +','_',$where);
	if(!$cf_insecure) {
		if(in_array($where,$cf_private))		// Do not allow direct access to private files
			$where = "index";
		if(!ereg('^[A-Za-z0-9_]+$',$where))	// Valid chars are [A-Za-z0-9_]
			$where = "index";
		if(!file_exists($PIECES."/".$where.$EXT))
			$where = "index";
	}

	if(file_exists($PIECES."/head".$EXT))
		include($PIECES."/head".$EXT);

	if(file_exists($PIECES."/".$where.$EXT))
		include($PIECES."/".$where.$EXT);

	if(file_exists($PIECES."/foot".$EXT))
		include($PIECES."/foot".$EXT);

?>
</BODY>
</HTML>
<?php exit; ?>
