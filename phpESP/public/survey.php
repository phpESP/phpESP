<?php

	require('/usr/local/lib/php/contrib/phpESP/admin/phpESP.ini');
	
	$_name = '';
	$_title = '';
	if (isset($HTTP_GET_VARS['name'])) {
		$_name = XADDSLASHES($HTTP_GET_VARS['name']);
		unset($HTTP_GET_VARS['name']);
		$HTTP_SERVER_VARS['QUERY_STRING'] = 
			ereg_replace('(^|&)name=[^&]*&?', '', $HTTP_SERVER_VARS['QUERY_STRING']);
	}

	//let's also see if the survey that's about to be rendered has an associated theme
	if (!empty($_name)) {
        	$sql = "SELECT id,title,theme FROM survey WHERE name = '$_name'";
        	if ($result = mysql_query($sql)) {
	        	if (mysql_num_rows($result) > 0)
		        	list($sid, $_title, $css_file) = mysql_fetch_row($result);
			mysql_free_result($result);
		}
	}
	// need to take care of the scenario where a user may submit a survey without completing all
	// required questions. If this occurs we need to ensure that the survey will still be rendered
	// using the desired theme (if set).
	else if (isset($HTTP_SERVER_VARS['HTTP_REFERER'])) {
        	list ($survey_url, $survey_name) = split('=',$HTTP_SERVER_VARS['HTTP_REFERER']);
		$sql = "SELECT theme FROM survey WHERE name = '$survey_name'";
                if ($result = mysql_query($sql)) {
                       	if (mysql_num_rows($result) > 0){
                               	list($theme) = mysql_fetch_row($result);
                        }
                        mysql_free_result($result);
                }
	}
	include($ESPCONFIG['handler_prefix']);
?>
<html>
<head><title><?php echo($_title); ?></title>
<?php
if (isset($css_file)) {
	echo("<link rel=\"stylesheet\" href=\"".$GLOBALS['ESPCONFIG']['survey_css_dir'].$css_file."\"  type=\"text/css\">\n");
}
else if(isset($theme)) {
	 echo("<link rel=\"stylesheet\" href=\"".$GLOBALS['ESPCONFIG']['survey_css_dir'].$theme."\"  type=\"text/css\">\n");
}
?>
</head>
<body>
<?php
	unset($_name);
	unset($_title);
	unset($css_file);
	unset($theme);
	include($ESPCONFIG['handler']);
?>
</body>
</html>
