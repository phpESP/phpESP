<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	session_start();
	
	if (!defined('ESP_BASE'))
		define('ESP_BASE', dirname(__FILE__) . '/../');

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

	include(esp_where($where));

	if(file_exists($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']))
		include($ESPCONFIG['include_path']."/foot".$ESPCONFIG['extension']);

?>
</BODY>
</HTML>
<?php exit; ?>
