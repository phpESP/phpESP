<?php

    /* $Id$ */

    /* vim: set tabstop=4 shiftwidth=4 expandtab: */

    // Written by James Flemer
    // For eGrad2000.com
    // <jflemer@alum.rpi.edu>

    if (!defined('ESP_BASE'))
    define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

    $CONFIG = ESP_BASE . 'admin/phpESP.ini.php';
    $DEFAULT_CONFIG = $CONFIG.'.default';
    $FIXED_CONFIG = $CONFIG.'.fixed';
    if(!file_exists($DEFAULT_CONFIG)) {
        echo("<b>FATAL: Unable to open default config file. Aborting.</b>");
        exit;
    }
    if(!file_exists($CONFIG)) {
        echo("<b>FATAL: Unable to open config file admin/phpESP.ini.php. Aborting.</b><br />");
        echo("See admin/phpESP.ini.php.default for all options. A small example:<br />");
        print nl2br(htmlspecialchars(file_get_contents("phpESP.ini.php.example")));
        exit;
    }
    if(!file_exists($FIXED_CONFIG)) {
        echo("<b>FATAL: Unable to open fixed config file. Aborting.</b>");
        exit;
    }
    require_once($DEFAULT_CONFIG);
    require_once($CONFIG);
    // let's check for values that should not exist yet ...
    // if they already exist, the user is still using an old too-complete
    // config file
    if (isset($ESPCONFIG['question_table'] )) {
        echo("<b>FATAL: Fixed values found in main config $CONFIG.<br />Please make sure $CONFIG contains only the differences from $FIXED_CONFIG, nothing else. Aborting.</b>");
        exit;
    }
    require_once($FIXED_CONFIG);

    /* check if the basic config files haven't changed */
    check_checksum($DEFAULT_CONFIG);
    #check_checksum($FIXED_CONFIG);

    /* check for an unsupported web server configuration */
    if((in_array(php_sapi_name(), $ESPCONFIG['unsupported'])) and ($ESPCONFIG['auth_design']) and ($ESPCONFIG['auth_mode'] == 'basic')) {
        echo ('<b>FATAL: Your webserver is running PHP in an unsupported mode. Aborting.</b><br/>');
        echo ('<b>Please read <a href="http://phpesp.sf.net/cvs/docs/faq.html?rev=.&content-type=text/html#iunsupported">this</a> entry in the FAQ for more information</b>');
        exit;
    }

    /* now let's check if we have the extension needed to connect to the db */
    if(!extension_loaded($ESPCONFIG['adodb_database_type'])) {
        echo('<b>FATAL: Mysql extension not loaded. Aborting.</b>');
        exit;
    }
    
    esp_init_adodb();

    if(get_cfg_var('register_globals')) {
        $_SESSION['acl'] = &$acl;
    }

    $db_version = get_dbversion();
    $where = '';
    if (version_compare($db_version,"0.0.0","eq")) {
        $where="install";
    } elseif($ESPCONFIG['auth_design']) {
        if ($ESPCONFIG['auth_mode'] == 'basic') {
            $raw_password = @$_SERVER['PHP_AUTH_PW'];
            $username = @$_SERVER['PHP_AUTH_USER'];
        }
        elseif ($ESPCONFIG['auth_mode'] == 'form') {
            if (isset($_POST['Login'])) {
                if (!isset($_POST['username'])) {
                    $username = "";
                }
                if ($_POST['username'] != "") {
                    $_SESSION['username'] = $_POST['username'];
                }
                if (!isset($_POST['password'])) {
                    $password = "";
                }
                if ($_POST['password'] != "") {
                    $_SESSION['raw_password'] = $_POST['password'];
                }
            }
            if (isset($_SESSION['username'])) {
                $username = $_SESSION['username'];
            }
            else {
                $username = "";
            }
            if (isset($_SESSION['raw_password'])) {
                $raw_password = $_SESSION['raw_password'];
            }
            else {
                $raw_password = "";
            }
        }
        $password = _addslashes($raw_password);
        if(!manage_auth($username, $password, $raw_password))
        exit;
    } else {
        $_SESSION['acl'] = array (
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

    if(empty($where) && isset($_REQUEST['where']))
       $where = $_REQUEST['where'];

    if ($where == 'download') {
        include(esp_where($where));
        exit;
    }
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
	if(!empty($ESPCONFIG['style_sheet'])) {
		echo("<link href=\"". $ESPCONFIG['style_sheet'] ."\" rel=\"stylesheet\" type=\"text/css\" />\n");
	}
?>
    <title><?php echo($ESPCONFIG['title']); ?></title>
    <script type="text/javascript" src="<?php echo($ESPCONFIG['js_url']);?>default.js"></script>
    <script type="text/javascript">
      var activateConfirmMsg="<?php echo(_('Warning! Once activated, this survey can no longer be edited.  Any further changes must be done on a copy.')); ?>";
      var cancelConfirmMsg="<?php echo(_('Warning! This survey has not been saved.  Canceling now will remove any changes.')); ?>";
      var mergeMsg="<h2><?php echo(_('You must select at least two surveys before you can merge')); ?></h2>";
    </script>
</head>
<body>
 <?php
	/* Moved to Stylesheet
	*
	*echo('bgcolor="'. $ESPCONFIG['main_bgcolor'] .'"');
	*if(!empty($ESPCONFIG['link_color']))  echo(' link="'.  $ESPCONFIG['link_color']  .'"');
	*if(!empty($ESPCONFIG['vlink_color'])) echo(' vlink="'. $ESPCONFIG['vlink_color'] .'"');
	*if(!empty($ESPCONFIG['alink_color'])) echo(' alink="'. $ESPCONFIG['alink_color'] .'"'); 
	*/

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
