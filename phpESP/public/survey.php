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

	if (!empty($_name)) {
        $sql = "SELECT id,title FROM survey WHERE name = '$_name'";
        if ($result = mysql_query($sql)) {
	        if (mysql_num_rows($result) > 0)
		        list($sid, $_title) = mysql_fetch_row($result);
			mysql_free_result($result);
		}
	}
?>
<html>
<head><title><?php echo($_title); ?></title></head>
<body>
<?php
	unset($_name);
	unset($_title);
	include($ESPCONFIG['handler']);
?>
</body>
</html>
