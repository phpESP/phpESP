<?php
/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Matthew Gregg
// <greggmc at musc.edu>

	if (!defined('ESP_BASE'))
		define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

	$CONFIG = ESP_BASE . 'admin/phpESP.ini.php';
	if(!file_exists($CONFIG)) {
		echo("<b>FATAL: Unable to open config file Aborting.</b>");
		exit;
	}
	if(!extension_loaded('mysql')) {
		echo('<b>FATAL: Mysql extension not loaded. Aborting.</b>');
		exit;
	}
	require_once($CONFIG);	
	
    esp_init_adodb();
	
	$_name = '';
	$_title = '';
	$_css = '';
    $sid = '';
	if (isset($HTTP_GET_VARS['name'])) {
		$_name = _addslashes($HTTP_GET_VARS['name']);
		unset($HTTP_GET_VARS['name']);
		$HTTP_SERVER_VARS['QUERY_STRING'] =
			ereg_replace('(^|&)name=[^&]*&?', '', $HTTP_SERVER_VARS['QUERY_STRING']);
	}

	if (!empty($_name)) {
        	$_sql = "SELECT id,title,theme FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name = $_name";
        	if ($_result = execute_sql($_sql)) {
            		if (record_count($_result) > 0)
                		list($sid, $_title, $_css) = fetch_row($_result);
            		db_close($_result);
        		}
        	unset($_sql);
        	unset($_result);
		}

        // To make all results public uncomment the next line.
        //$results = 1;
        // See the FAQ for more instructions.

        // call the handler-prefix once $sid is set to handle
        // authentication / authorization


        if (empty($_name) && isset($sid) && $sid) {
            $_sql = "SELECT title,theme FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id = '$sid'";
            if ($_result = execute_sql($_sql)) {
                if (record_count($_result) > 0){
                    list($_title, $_css) = fetch_row($_result);
                }
                db_close($_result);
            }
            unset($_sql);
            unset($_result);
        }
        include($ESPCONFIG['handler_prefix']);

?>
<html>
<head><title><?php echo($_title); ?></title>
<?php
    if (!empty($_css)) {
	    echo('<link rel="stylesheet" href="'. $GLOBALS['ESPCONFIG']['css_url'].$_css ."\" type=\"text/css\">\n");
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
