<?php

	require('/usr/local/lib/php/contrib/phpESP/admin/phpESP.ini');
	
	$_name = '';
	$_title = '';
    $_css = '';
	if (isset($HTTP_GET_VARS['name'])) {
		$_name = XADDSLASHES($HTTP_GET_VARS['name']);
		unset($HTTP_GET_VARS['name']);
		$HTTP_SERVER_VARS['QUERY_STRING'] = 
			ereg_replace('(^|&)name=[^&]*&?', '', $HTTP_SERVER_VARS['QUERY_STRING']);
	}

	if (!empty($_name)) {
        $_sql = "SELECT id,title,theme FROM survey WHERE name = '$_name'";
        if ($_result = mysql_query($_sql)) {
            if (mysql_num_rows($_result) > 0)
                list($sid, $_title, $_css) = mysql_fetch_row($_result);
            mysql_free_result($_result);
        }
        unset($_sql);
        unset($_result);
	}

    // call the handler-prefix once $sid is set to handle
    // authentication / authorization 
	include($ESPCONFIG['handler_prefix']);

	if (empty($_name) && isset($sid) && $sid) {
        $_sql = "SELECT title,theme FROM survey WHERE id = '$sid'";
        if ($_result = mysql_query($_sql)) {
            if (mysql_num_rows($_result) > 0){
                list($_title, $_css) = mysql_fetch_row($_result);
            }
            mysql_free_result($_result);
        }
        unset($_sql);
        unset($_result);
	}

?>
<html>
<head><title><?php echo($_title); ?></title>
<?php
    if (!empty($_css)) {
	    echo('<link rel="stylesheet" href="'. $GLOBALS['ESPCONFIG']['survey_css_dir'].$_css ."\" type=\"text/css\">\n");
    }
    unset($_css);
?>
</head>
<body>
<?php
	unset($_name);
	unset($_title);
	include($ESPCONFIG['handler']);
?>
</body>
</html>
