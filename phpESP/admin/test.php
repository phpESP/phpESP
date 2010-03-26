<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* phpESP System Information */

session_start();
// if the session fails to start
if (!isset($_SESSION)) {
   echo "This script can't work without setting the php session variable first!!!";
   exit ;
}

if (!isset($_SESSION['esp_counter']))
    $_SESSION['esp_counter'] = 0;
$_SESSION['esp_counter']++;

function _pass($str)
{
    echo '<font color="green">' . htmlspecialchars($str) . '</font>';
}

function _fail($str)
{
    echo '<font color="red">' . htmlspecialchars($str) . '</font>';
}

function check_string($have, $want)
{
    if (strcasecmp($have, $want) == 0)
        _pass($have);
    else
        _fail($have);
}

function check_bool($have, $want)
{
    $val = $have ? 'Yes' : 'No';
    if ($have == $want)
        _pass($val);
    else
        _fail($val);
}

function check_extension($ext)
{
    if (!isset($GLOBALS['php_extensions'])) {
        $GLOBALS['php_extensions'] =
                array_map('strtolower', get_loaded_extensions());
    }
    
    if (in_array(strtolower($ext), $GLOBALS['php_extensions']))
        _pass('Yes');
    else
        _fail('No');
}

function check_version()
{
    if (!function_exists('version_compare')) {
        _fail(PHP_VERSION);
        return;
    }
    if (version_compare(PHP_VERSION, '4.1.0', 'ge'))
        _pass(PHP_VERSION);
    else
        _fail(PHP_VERSION);
}

?>
<html>
<head>
<title>phpESP System Information</title>
<style type="text/css">
<!--
ul,th {
        font-family : Verdana, Arial, Helvetica, Geneva, sans-serif;
        font-size : 9px;
        font-weight : bold;
        font-variant : normal;
        font-style : normal;
}


-->
</style>
</head>
<body>
<table><tbody align="left">
  <tr><th>PHP Information</th></tr>
  <tr><td><ul>
    <li>Version: <?php check_version(); ?></li>
    <li>OS: <?php _pass(PHP_OS); ?></li>
    <li>SAPI: <?php check_string(php_sapi_name(), 'apache'); ?></li>
    <li>register_globals: <?php check_bool(ini_get('register_globals'), false); ?></li>
    <li>magic_quotes_gpc: <?php check_bool(ini_get('magic_quotes_gpc'), false); ?></li>
    <li>magic_quotes_runtime: <?php check_bool(ini_get('magic_quotes_runtime'), false); ?></li>
    <li>display_errors: <?php check_bool(ini_get('display_errors'), false); ?></li>
    <li>safe_mode: <?php check_bool(ini_get('safe_mode'), false); ?></li>
    <li>open_basedir: <?php check_string(ini_get('open_basedir'), ''); ?></li>
  </ul></td></tr>
  
  <tr><th>PHP Extensions</th></tr>
  <tr><td><ul>
    <li>dBase: <?php check_extension('dbase'); ?></li>
    <li>GD: <?php
        check_extension('gd');
        if (function_exists('gd_info')) {
            $gdinfo = gd_info();
            echo " -- ${gdinfo['GD Version']}";
        }
    ?></li>
    <li>GNU Gettext: <?php check_extension('gettext'); ?></li>
    <li>LDAP: <?php check_extension('ldap'); ?></li>
    <li>MySQL: <?php check_extension('mysql'); ?></li>
    <li>PostgreSQL: <?php check_extension('pgsql'); ?></li>
    <li>PHP Extension Dir (compiled): <?php _pass(PHP_EXTENSION_DIR); ?></li>
    <li>PHP Extension Dir (run time): <?php _pass(ini_get('extension_dir')); ?></li>
  </ul></td></tr>

  <tr><th>phpESP Settings</th></tr>
  <tr><td><ul>
    <li><b>Loading phpESP.ini.php ...</b><br />
      <?php require_once('phpESP.ini.php.default');require_once('phpESP.ini.php'); require_once('phpESP.ini.php.fixed');?></li>
    <li>Expected ESP_BASE: <?php _pass(dirname(dirname(__FILE__)) .'/'); ?></li>
    <li>Expected base_url: <?php _pass($ESPCONFIG['proto'] . $_SERVER['HTTP_HOST'] . dirname(dirname($_SERVER['REQUEST_URI'])) . '/'); ?></li>
    <li>ESP_BASE: <?php
        if ((ESP_BASE == dirname(__FILE__) . '/../') || (ESP_BASE == dirname(dirname(__FILE__)) .'/'))
            _pass(ESP_BASE);
        else
            _fail(ESP_BASE);
    ?></li>
    <li>base_url: <?php check_string($ESPCONFIG['base_url'], $ESPCONFIG['proto']. $_SERVER['HTTP_HOST'] . dirname(dirname($_SERVER['REQUEST_URI'])) . '/'); ?></li>
    <li>Version: <?php _pass($ESPCONFIG['version']); ?></li>
    <li>Debug: <?php check_bool($ESPCONFIG['DEBUG'], false); ?></li>
  </ul></td></tr>

  <tr><th>phpESP Language Settings</th></tr>
  <tr><td><ul>
    <li>GNU Gettext: <?php check_string(
            ($ESPCONFIG['gettext'] ? 'Real' : 'Emulated'), 'Real'); ?></li>
    <li>default_lang: <?php _pass($ESPCONFIG['default_lang']); ?></li>
    <li>current lang: <?php _pass($ESPCONFIG['lang']); ?></li>
    <li>available langs: <?php _pass(implode(', ', esp_getlocales())); ?><br />
      (<?php _pass(implode(', ', array_keys(esp_getlocale_map()))); ?>)
    </li>
    <li>LC_ALL: <?php check_string(setlocale(LC_ALL, 0), $ESPCONFIG['lang'].".".$ESPCONFIG['charset']); ?></li>
    <li>GNU Gettext test: <?php
        esp_setlocale('en_US');
        check_string(_('%%%% Gettext Test Failed'), 'Passed'); ?></li>
    <li>Catalog Open Test: <?php
        $ret = fopen($ESPCONFIG['locale_path'] . '/en_US/LC_MESSAGES/messages.mo', 'r');
        check_bool($ret !== false, true);
        fclose($ret);
    ?></li>
  </ul></td></tr>

  <tr><th>PHP Session Test</th></tr>
  <tr><td><ul>
    <li>session.save_path: <?php
        if (stristr(PHP_OS, 'win') && (substr(ini_get('session.save_path'), 0, 1) == '/'))
            _fail(ini_get('session.save_path'));
        else
            _pass(ini_get('session.save_path'));
    ?></li>
    <li>Counter: <?php echo $_SESSION['esp_counter']; ?></li>
  </ul></td></tr>
    
</tbody></table>
</body>
</html>
