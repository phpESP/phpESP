<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	session_start();
	
	if (!defined('ESP_BASE'))
		define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

 	$CONFIG = ESP_BASE . '/admin/phpESP.ini.php';

	if(!file_exists($CONFIG)) {
		echo("<b>FATAL: Unable to open $CONFIG. Aborting.</b>");
		exit;
	}
	if(!extension_loaded('mysql')) {
		echo('<b>FATAL: Mysql extension not loaded. Aborting.</b>');
		exit;
	}
	require_once($CONFIG);
	
	esp_init_db();
	
	session_register('acl');
	if(get_cfg_var('register_globals')) {
		$HTTP_SESSION_VARS['acl'] = &$acl;
	}
	if($ESPCONFIG['auth_design']) {
		if(!manage_auth(
				_addslashes(@$HTTP_SERVER_VARS['PHP_AUTH_USER']),
				_addslashes(@$HTTP_SERVER_VARS['PHP_AUTH_PW'])))
			exit;
	} else {
		$HTTP_SESSION_VARS['acl'] = array (
			'username'  => 'none',
			'pdesign'   => array('none'),
			'pdata'     => array('none'),
			'pstatus'   => array('none'),
			'pall'      => array('none'),
			'pgroup'    => array('none'),
			'puser'     => array('none'),
			'superuser' => 'Y',
			'disabled'  => 'N'
		);
	}
	
	$where = '';
	if(isset($HTTP_POST_VARS['where']))
		$where = $HTTP_POST_VARS['where'];
	elseif(isset($HTTP_GET_VARS['where']))
		$where = $HTTP_GET_VARS['where'];

	if ($where == 'download') {
		include(esp_where($where));
		exit;
	}
?>
<html>
<!-- $Id$ -->
<head>
	<title><?php echo($ESPCONFIG['title']); ?></title>
<?php
	if(!empty($ESPCONFIG['style_sheet'])) {
		echo("<link href=\"". $ESPCONFIG['style_sheet'] ."\" rel=\"stylesheet\" type=\"text/css\" />\n");
	}
	if(!empty($ESPCONFIG['charset'])) {
		echo('<meta http-equiv="Content-Type" content="text/html; charset='. $ESPCONFIG['charset'] ."\" />\n");
	}
?>
</head>
<body <?php
	echo('bgcolor="'. $ESPCONFIG['main_bgcolor'] .'"');
	if(!empty($ESPCONFIG['link_color']))  echo(' link="'.  $ESPCONFIG['link_color']  .'"');
	if(!empty($ESPCONFIG['vlink_color'])) echo(' vlink="'. $ESPCONFIG['vlink_color'] .'"');
	if(!empty($ESPCONFIG['alink_color'])) echo(' alink="'. $ESPCONFIG['alink_color'] .'"'); ?>>
<?php
	if($ESPCONFIG['DEBUG']) {
		include($ESPCONFIG['include_path']."/debug".$ESPCONFIG['extension']);
	}

	if(file_exists($ESPCONFIG['include_path']."/head".$ESPCONFIG['extension']))
		include($ESPCONFIG['include_path']."/head".$ESPCONFIG['extension']);

	include(esp_where($where));

	if(file_exists($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']))
		include($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']);

?>
</body>
</html>
<?php exit; ?>
