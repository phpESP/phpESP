<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

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

    /* check for an unsupported web server configuration */
    if((in_array(php_sapi_name(), $ESPCONFIG['unsupported'])) and ($ESPCONFIG['auth_design']) and ($ESPCONFIG['auth_mode'] == 'php')) {
        echo ('<b>FATAL: Your webserver is running PHP in an unsupported mode. Aborting.</b><br/>');
        echo ('<b>Please read <a href="http://phpesp.sf.net/cvs/docs/faq.html?rev=.&content-type=text/html#iunsupported">this</a> entry in the FAQ for more information</b>');
        exit;
    }
	
    esp_init_adodb();
	
	session_register('acl');
	if(get_cfg_var('register_globals')) {
		$HTTP_SESSION_VARS['acl'] = &$acl;
	}
	if($ESPCONFIG['auth_design']) {
        if ($ESPCONFIG['auth_mode'] == 'php') {
            $raw_password = @$HTTP_SERVER_VARS['PHP_AUTH_PW'];
            $username = @$HTTP_SERVER_VARS['PHP_AUTH_USER'];
        }
        elseif ($ESPCONFIG['auth_mode'] == 'form') {
            if (!isset($HTTP_POST_VARS['username'])) {
                $HTTP_POST_VARS['username'] = "";
            }
            if (!isset($HTTP_POST_VARS['password'])) {
                $HTTP_POST_VARS['password'] = "";
            }
            if (!isset($HTTP_SESSION_VARS['username'])) {
                session_register('username');
            }
            if (!isset($HTTP_SESSION_VARS['raw_password'])) {
                session_register('raw_password');
            }
                
            if ($HTTP_POST_VARS['username'] != "") {
                $username = $HTTP_POST_VARS['username'];
            }
            elseif ($HTTP_SESSION_VARS['username'] != "") {
                $username = $HTTP_SESSION_VARS['username'];
            }
            if ($HTTP_POST_VARS['password'] != "") {
                    $raw_password = $HTTP_POST_VARS['password'];
            }
            elseif ($HTTP_SESSION_VARS['raw_password'] != "") {
                    $raw_password = $HTTP_SESSION_VARS['raw_password'];
            }
        }
        $password = _addslashes($raw_password);
        if(!manage_auth($username, $password, $raw_password))
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
<head>
	<title><?php echo($ESPCONFIG['title']); ?></title>
    <script type="text/javascript" src="<?php echo($ESPCONFIG['js_url']);?>default.js"></script>
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
