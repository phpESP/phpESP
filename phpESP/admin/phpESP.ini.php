<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

if (!defined('ESP_BASE')) define('ESP_BASE', dirname(dirname(__FILE__)) .'/');
if (isset($_SERVER))  $server =& $_SERVER;
else                  $server =& $HTTP_SERVER_VARS;

/**
 * Here are all the configuration options.
 */

// Base URL for phpESP
$ESPCONFIG['base_url'] = 'http://' . $server['HTTP_HOST'] . '/phpESP/';

// URL of the images directory (for <img src='...'> tags)
$ESPCONFIG['image_url'] = $ESPCONFIG['base_url'] . 'images/';

// URL of the automatic survey publisher
$ESPCONFIG['autopub_url'] = $ESPCONFIG['base_url'] . 'public/survey.php';

// URL of the CSS directory (for themes)
$ESPCONFIG['css_url'] = $ESPCONFIG['base_url'] . 'public/css/';

// Database connection information
$ESPCONFIG['db_host'] = 'localhost';
$ESPCONFIG['db_user'] = 'phpesp';
$ESPCONFIG['db_pass'] = 'phpesp';
$ESPCONFIG['db_name'] = 'phpesp';

// Allow phpESP to send email (BOOLEAN)
$ESPCONFIG['allow_email'] = true;

// Send human readable email, rather than machine readable (BOOLEAN)
$ESPCONFIG['human_email'] = false;

// Use authentication for designer interface (BOOLEAN)
$ESPCONFIG['auth_design'] = true;

// Use authentication for survey responders (BOOLEAN)
$ESPCONFIG['auth_response'] = true;

// Choose authentication type: { 'default', 'ldap' }
$ESPCONFIG['auth_type'] = 'default';

// LDAP connection information
// (Set these values if you choose 'ldap' as the authentication type.)
$ESPCONFIG['ldap_server'] = 'ldap.example.com';
$ESPCONFIG['ldap_port']   = '389';
$ESPCONFIG['ldap_dn']     = 'dc=example,dc=com';
$ESPCONFIG['ldap_filter'] = 'uid=';

// Group to add responders to via the sign-up page
// (Set to "null", without quotes, to disable the sign-up page.)
$ESPCONFIG['signup_realm'] = 'auto';

// Default language for designer interface
// (Must have gettext support avaiable.)
$ESPCONFIG['default_lang'] = 'en_US';

// HTML character set used by phpESP
// (Try 'Windows-1251' for Cryillic, etc.)
$ESPCONFIG['charset'] = 'ISO-8859-1';

// Default number of option lines for new questions
$ESPCONFIG['default_num_choices'] = 10;

// Colors used by phpESP
$ESPCONFIG['main_bgcolor']      = '#FFFFFF';
$ESPCONFIG['link_color']        = '#0000CC';
$ESPCONFIG['vlink_color']       = '#0000CC';
$ESPCONFIG['alink_color']       = '#0000CC';
$ESPCONFIG['table_bgcolor']     = '#0099FF';
$ESPCONFIG['active_bgcolor']    = '#FFFFFF';
$ESPCONFIG['dim_bgcolor']       = '#3399CC';
$ESPCONFIG['error_color']       = '#FF0000';
$ESPCONFIG['warn_color']        = '#FF0000';
$ESPCONFIG['reqd_color']        = '#FF0000';
$ESPCONFIG['bgalt_color1']      = '#FFFFFF';
$ESPCONFIG['bgalt_color2']      = '#EEEEEE';

/*******************************************************************
 * Most users will not need to change anything below this line.    *
 *******************************************************************/

// Enable debugging code (BOOLEAN)
$ESPCONFIG['DEBUG'] = false;

// Name of application
$ESPCONFIG['name'] = 'phpESP';

// Application version
$ESPCONFIG['version'] = '1.6 RC3';

// Extension of support files
$ESPCONFIG['extension'] = '.inc';

// Survey handler to use
$ESPCONFIG['handler']        = ESP_BASE . '/public/handler.php';
$ESPCONFIG['handler_prefix'] = ESP_BASE . '/public/handler-prefix.php';

// Valid tabs when editing surveys
$ESPCONFIG['tabs'] = array('general', 'questions', 'preview', 'order', 'finish');

// Copy of PHP_SELF for later use
$ESPCONFIG['ME'] = $server['PHP_SELF'];

// CSS stylesheet to use for designer interface
$ESPCONFIG['style_sheet'] = null;

// Status of gettext extension
$ESPCONFIG['gettext'] = extension_loaded('gettext');

// HTML page title
$ESPCONFIG['title'] = $ESPCONFIG['name'] .', v'. $ESPCONFIG['version'];

// phpESP include path
$ESPCONFIG['include_path'] = ESP_BASE . '/admin/include/';

// phpESP css path
$ESPCONFIG['css_path'] = ESP_BASE . '/public/css/';

// phpESP locale path
$ESPCONFIG['locale_path'] = ESP_BASE . '/locale/';

// Load I18N support
require_once($ESPCONFIG['include_path'] . '/lib/espi18n' . $ESPCONFIG['extension']);
esp_setlocale_ex();

// default thank you messages
$ESPCONFIG['thank_head'] = _('Thank You For Completing This Survey.');
$ESPCONFIG['thank_body'] = _('Please do not use the back button on your browser to go
back. Please click on the link below to return you to where
you launched this survey.');

if (!file_exists($ESPCONFIG['include_path']. '/funcs'. $ESPCONFIG['extension'])) {
    printf('<b>'. _('Unable to find the phpESP %s directory.
			Please check %s to ensure that all paths are set correctly.') .
			'</b>', 'include', 'phpESP.ini.php');
    exit;
}
if (!file_exists($ESPCONFIG['css_path'])) {
    printf('<b>'. _('Unable to find the phpESP %s directory.
			Please check %s to ensure that all paths are set correctly.') .
			'</b>', 'css', 'phpESP.ini.php');
    exit;
}

if (isset($GLOBALS)) {
    $GLOBALS['ESPCONFIG'] = $ESPCONFIG;
} else {
    global $ESPCONFIG;
}

require_once($ESPCONFIG['include_path'].'/funcs'.$ESPCONFIG['extension']);

?>
