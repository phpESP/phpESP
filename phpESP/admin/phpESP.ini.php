<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

if (isset($GLOBALS)) {
    $GLOBALS['ESPCONFIG'] = array();
    $ESPCONFIG =& $_GLOBALS['ESPCONFIG'];
} else {
    global $ESPCONFIG;
    $ESPCONFIG = array();
}

if (isset($_SERVER))  $server =& $_SERVER;
else                  $server =& $HTTP_SERVER_VARS;

/**
 * Here are all the configuration options.
 */

// Name of application
$ESPCONFIG['name'] = 'phpESP';

// Main install path ON DISK
// (where you unpacked the tar)
$ESPCONFIG['prefix'] = '/usr/local/lib/php/contrib';

// Base URL for phpESP
$ESPCONFIG['base_url'] = 'http://' . $server['HTTP_HOST'] . '/phpESP/';

// Path to include files ON DISK
// (normally <prefix>/phpESP/admin/include)
$ESPCONFIG['include_path'] = $ESPCONFIG['prefix'] . '/phpESP/admin/include/';

// extention of include files
$ESPCONFIG['extension'] = '.inc';

// path to images ON THE WEB (for <img src='...'> tags)
$ESPCONFIG['image_path'] = $ESPCONFIG['base_url'] . 'images/';

// path to survey handler ON DISK
// (normally <install root>/phpESP/public/handler.php)
$ESPCONFIG['handler']        = $ESPCONFIG['prefix'] . '/phpESP/public/handler.php';
$ESPCONFIG['handler_prefix'] = $ESPCONFIG['prefix'] . '/phpESP/public/handler-prefix.php';

// path to survey auto_handler ON THE WEB
$ESPCONFIG['auto_handler'] = $ESPCONFIG['base_url'] . 'public/survey.php';

//CSS Directory for Surveys
//this is used for linking of style sheet (theme) to survey
$ESPCONFIG['survey_css_dir'] = $ESPCONFIG['base_url']  . 'public/css/';

//this is used internally to indicate to the general.inc file the path to the style sheets
$ESPCONFIG['survey_css'] = $ESPCONFIG['prefix'] . '/phpESP/public/css/';

// database connection info
$ESPCONFIG['db_host'] = 'localhost';
$ESPCONFIG['db_user'] = 'phpesp';
$ESPCONFIG['db_pass'] = 'phpesp';
$ESPCONFIG['db_name'] = 'phpesp';

// set to FALSE to globally disable sending email
$ESPCONFIG['allow_email'] = TRUE;

// set to TRUE to generate human readable email
// (rather than machine readable)
$ESPCONFIG['human_read_mail'] = FALSE;

// HTML Character Set (try: 'Windows-1251' for Cryillic etc)
$ESPCONFIG['cf_charset'] = 'ISO-8859-1';

// application version
$ESPCONFIG['version'] = '1.6 beta';

// use authentication on designer interface
$ESPCONFIG['auth_design'] = TRUE;

// use authentication on survey taking
$ESPCONFIG['auth_response'] = TRUE;

// set authentication type [ default, ldap ]
$ESPCONFIG['auth_type'] = 'default';

// ldap connection info (used just for authentication)
// set auth_type to ldap
$ESPCONFIG['ldap_server'] = 'ldap.example.com';
$ESPCONFIG['ldap_port']   = '389';
$ESPCONFIG['ldap_dn']     = 'dc=example,dc=com';
$ESPCONFIG['ldap_filter'] = 'uid=';

// set group to use for respondent signup page
// set to null to disable
$ESPCONFIG['signup_realm'] = 'auto';

// tabs for editing surveys
$ESPCONFIG['tabs'] = array('general', 'questions', 'preview', 'order', 'finish');

// default number of option lines for new questions
$ESPCONFIG['default_num_choices'] = 10;

// some colors
$ESPCONFIG['main_bgcolor']      = '#FFFFFF';
$ESPCONFIG['link_color']        = '#0000CC';
$ESPCONFIG['vlink_color']       = '#0000CC';
$ESPCONFIG['alink_color']       = '#0000CC';
$ESPCONFIG['table_bgcolor']     = '#0099FF';
//$ESPCONFIG['table_bgcolor']     = '#CC99FF';
//$ESPCONFIG['sub1_bgcolor']      = '#3399CC';
//$ESPCONFIG['active_bgcolor']    = '#339999';
$ESPCONFIG['active_bgcolor']    = '#FFFFFF';
$ESPCONFIG['dim_bgcolor']       = '#3399CC';
$ESPCONFIG['error_color']       = '#FFFF66';
$ESPCONFIG['warn_color']        = '#FFFF66';
$ESPCONFIG['reqd_color']        = '#FF0000';
$ESPCONFIG['bgalt_color1']      = '#FFFFFF';
$ESPCONFIG['bgalt_color2']      = '#EEEEEE';

// set to true to turn on various debugging options
$ESPCONFIG['DEBUG'] = FALSE;

// alias for PHP_SELF incase it needs to be locally
// overwritten
$ESPCONFIG['ME'] = $server['PHP_SELF'];

// CSS stylesheet to use
//$ESPCONFIG['style_sheet'] = 'phpesp.css';
$ESPCONFIG['style_sheet'] = '';

// default language
$ESPCONFIG['default_lang'] = 'en';

// locale path
$ESPCONFIG['locale_path'] = $ESPCONFIG['prefix'] . '/phpESP/locale';

// status of gettext extension
$ESPCONFIG['gettext'] = extension_loaded('gettext');

$ESPCONFIG['title'] = $ESPCONFIG['name'] .', v'. $ESPCONFIG['version'];

/**
 * Load the GNU Gettext module if it is not loaded.
 * If it cannot be loaded, define the NOP wrapper.
 * If the wrapper is defined, English will be the only
 * language available.
 */
$ESPCONFIG['lang'] = $ESPCONFIG['default_lang'];
if (!function_exists('gettext')) {
    if (!ini_get('safe_mode') && ini_get('enable_dl')) {
        @dl( (substr(PHP_OS, 0, 3) == 'WIN') ? 'php_gettext.dll' : 'gettext.so');
    }
}
if (!function_exists('gettext')) {
    function _($s) {return($s);}
    function bindtextdomain($s) {}
    function gettext($s) {return($s);}
    function textdomain($s) {}
}
if (!empty($server['HTTP_ACCEPT_LANGUAGE'])) {
    $_langs = split(' *, *', $server['HTTP_ACCEPT_LANGUAGE']);
    foreach ($_langs as $_lang) {
        $_lang = quotemeta($_lang);
        if (file_exists($ESPCONFIG['locale_path'] . "/$_lang")) {
            $ESPCONFIG['lang'] = $_lang;
            break;
        }
        $_lang = substr($_lang, 0, strpos($_lang, '-'));
        if (file_exists($ESPCONFIG['locale_path'] . "/$_lang")) {
            $ESPCONFIG['lang'] = $_lang;
            break;
        }
    }
    unset($_lang);
    unset($_langs);
}
setlocale(LC_ALL, $ESPCONFIG['lang']);
bindtextdomain('messages', $ESPCONFIG['locale_path']);
textdomain('messages');

// default thank you messages
$ESPCONFIG['thank_head'] = _('Thank You For Completing This Survey.');
$ESPCONFIG['thank_body'] = _('Please do not use the back button on your browser to go
back. Please click on the link below to return you to where
you launched this survey.');

if (!defined('STATUS_ACTIVE')) {
    define('STATUS_ACTIVE',  0x01);
    define('STATUS_DONE',    0x02);
    define('STATUS_DELETED', 0x04);
    define('STATUS_TEST',    0x08);
}

if (!file_exists($ESPCONFIG['include_path'].'/funcs'.$ESPCONFIG['extension'])) {
    echo('<b>'. _('I can not find the phpESP includes directory.
			Please check your phpESP.ini.php file to ensure that all paths are set correctly.') .
			'</b>');
    exit;
}
if (!file_exists($ESPCONFIG['survey_css'])) {
	echo('<b>'. _('I can not find the phpESP css directory.
			Please check your phpESP.ini.php file to ensure that the
			&quot;survey_css&quot; path is set correctly.') . '</b>');
}

require_once($ESPCONFIG['include_path'].'/funcs'.$ESPCONFIG['extension']);

$ESPCONFIG['db_conn'] = @mysql_connect(
        $ESPCONFIG['db_host'], $ESPCONFIG['db_user'], $ESPCONFIG['db_pass']);
if ($ESPCONFIG['db_conn'] !== false) {
    if (mysql_select_db($ESPCONFIG['db_name'], $ESPCONFIG['db_conn']) === false) {
        mysql_close($ESPCONFIG['db_conn']);
        $ESPCONFIG['db_conn'] = false;
    }
}
if ($ESPCONFIG['db_conn'] === false) {
    header('HTTP/1.0 503 '. _('Service Unavailable'));
    echo('<html><head><title>HTTP 503 '. _('Service Unavailable') .'</title></head>');
    echo('<body><h1>HTTP 503 '. _('Service Unavailable') .'</h1>');
    echo(mkerror(_('Connection to database failed. Please check configuration.')));
    if ($ESPCONFIG['DEBUG'])
        echo("<br>\n". mkerror(mysql_errno().": ".mysql_error()));
    echo('</body></html>');
    exit;
}

?>
