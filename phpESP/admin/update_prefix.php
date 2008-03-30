<?php

    /* $Id: manage.php 1029 2008-03-30 13:31:55Z liedekef $ */

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
            echo("<b>FATAL: Unable to open config file. Aborting.</b>");
            exit;
    }
    if(!file_exists($FIXED_CONFIG)) {
            echo("<b>FATAL: Unable to open fixed config file. Aborting.</b>");
            exit;
    }
    require_once($DEFAULT_CONFIG);
    require_once($CONFIG);
    $new_prefix=$DB_PREFIX;
    if (!isset($OLD_DB_PREFIX)) {
	    echo("<b>FATAL: Please define \$OLD_DB_PREFIX in the config file. Aborting.</b>");
            exit;
    }
    $DB_PREFIX=$OLD_DB_PREFIX;
    require_once($FIXED_CONFIG);

    /* check for an unsupported web server configuration */
    if((in_array(php_sapi_name(), $ESPCONFIG['unsupported'])) and ($ESPCONFIG['auth_design']) and ($ESPCONFIG['auth_mode'] == 'basic')) {
        echo ('<b>FATAL: Your webserver is running PHP in an unsupported mode. Aborting.</b><br/>');
        echo ('<b>Please read <a href="http://phpesp.sf.net/cvs/docs/faq.html?rev=.&content-type=text/html#iunsupported">this</a> entry in the FAQ for more information</b>');
        exit;
    }

    esp_init_adodb();

    if(get_cfg_var('register_globals')) {
        $_SESSION['acl'] = &$acl;
    }

    if($ESPCONFIG['auth_design']) {
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

    if($_SESSION['acl']['superuser'] != 'Y') {
      exit;
    }
    
    foreach ($ESPCONFIG as $name=>$value) {
        if (substr($name,-6)=="_table") {
            $newvalue=str_replace($DB_PREFIX,"",$value);
            print "<br \>Renaming $value to $new_prefix$newvalue ... ";
            $sql="RENAME TABLE $value TO $new_prefix$newvalue";
            $result = execute_sql($sql);
            if (!$result) {
                echo(mkerror(_('FAILED')));
            } else {
                echo _('DONE');
            }
        }
    }
       
    echo("<br><a href=\"manage.php\">" . _('Go back to Management Interface') . "</a>\n");

