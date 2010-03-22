<?php
/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Matthew Gregg
// <greggmc at musc.edu>

	require_once("./phpESP.first.php");	
	
$_name = '';
$_title = '';
$_css = '';
    $sid = '';
    if (isset($_GET['sid'])) {
        $sid=intval($_GET['sid']);
    } elseif (isset($_POST['sid'])) {
        $sid=intval($_POST['sid']);
    } elseif (isset($_GET['name'])) {
		$_name = _addslashes($_GET['name']);
		unset($_GET['name']);
	} elseif (isset($_POST['name'])) {
       	$_name = _addslashes($_POST['name']);
    }
	$_SERVER['QUERY_STRING'] = ereg_replace('(^|&)name=[^&]*&?', '', $_SERVER['QUERY_STRING']);
	$_SERVER['QUERY_STRING'] = ereg_replace('(^|&)sid=[^&]*&?', '', $_SERVER['QUERY_STRING']);

	if (!empty($_name)) {
        	$_sql = "SELECT id,title,theme FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name = $_name ORDER BY id ASC";
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
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" 
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<?php
	if(!empty($ESPCONFIG['charset'])) {
		echo('<meta http-equiv="Content-Type" content="text/html; charset='. $ESPCONFIG['charset'] ."\" />\n");
	}
    if(!empty($ESPCONFIG['favicon'])) {
        echo("<link rel=\"shortcut icon\" href=\"" . $ESPCONFIG['favicon'] . "\" />\n");
    }
    if (!empty($_css)) {
	    echo('<link rel="stylesheet" href="'. $GLOBALS['ESPCONFIG']['css_url'].$_css ."\" type=\"text/css\" />\n");
    }
    unset($_css);
?>
<script type="text/javascript" src="<?php echo($ESPCONFIG['js_url']);?>default.js"></script>
<title><?php echo($_title); ?></title>
</head>
<body>
<?php
	unset($_name);
	unset($_title);
	include($ESPCONFIG['handler']);
?>
</body>
</html>
