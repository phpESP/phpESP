<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

if (!defined('ESP_BASE')) define('ESP_BASE', dirname(dirname(__FILE__)) .'/');
if (isset($_SERVER))  $server =& $_SERVER;
else                  $server =& $HTTP_SERVER_VARS;

/**
 * Here are all the configuration options.
 */

// use http or https?
$ESPCONFIG['proto'] = 'http://';

// Base URL for phpESP
$ESPCONFIG['base_url'] = $ESPCONFIG['proto'] . $server['HTTP_HOST'] . '/phpESP/';

// URL of the images directory (for <img src='...'> tags)
$ESPCONFIG['image_url'] = $ESPCONFIG['base_url'] . 'images/';

// URL of the automatic survey publisher
$ESPCONFIG['autopub_url'] = $ESPCONFIG['base_url'] . 'public/survey.php';

// URL of the CSS directory (for themes)
$ESPCONFIG['css_url'] = $ESPCONFIG['base_url'] . 'public/css/';

//URL for management javascript
$ESPCONFIG['js_url'] = $ESPCONFIG['base_url'] . 'js/';

// Database connection information
$ESPCONFIG['db_host'] = 'localhost';
$ESPCONFIG['db_user'] = 'phpesp';
$ESPCONFIG['db_pass'] = 'phpesp';
$ESPCONFIG['db_name'] = 'phpesp';

// ADODB 
$ESPCONFIG['adodb_path'] = '/usr/share/adodb/';
$ESPCONFIG['adodb_database_type'] = 'mysql';
$ESPCONFIG['adodb_dbpersist'] = 'true';
$ESPCONFIG['adodb_pathto_db'] = "/var/www/phpESP/scripts/db/esp.dbm";

// Allow phpESP to send email (BOOLEAN)
$ESPCONFIG['allow_email'] = true;

// Send human readable email, rather than machine readable (BOOLEAN)
$ESPCONFIG['human_email'] = true;

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
$ESPCONFIG['version'] = '1.7-dev';

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

// Database Table Names:
$DB_PREFIX = "";	// If your database uses a prefix, set it here.
$ESPCONFIG['access_table']              = $DB_PREFIX."access";
$ESPCONFIG['designer_table']            = $DB_PREFIX."designer";
$ESPCONFIG['question_table']            = $DB_PREFIX."question";
$ESPCONFIG['question_choice_table']     = $DB_PREFIX."question_choice";
$ESPCONFIG['question_type_table']       = $DB_PREFIX."question_type";
$ESPCONFIG['realm_table']               = $DB_PREFIX."realm";
$ESPCONFIG['respondent_table']          = $DB_PREFIX."respondent";
$ESPCONFIG['response_table']            = $DB_PREFIX."response";
$ESPCONFIG['response_bool_table']       = $DB_PREFIX."response_bool";
$ESPCONFIG['response_date_table']       = $DB_PREFIX."response_date";
$ESPCONFIG['response_multiple_table']   = $DB_PREFIX."response_multiple";
$ESPCONFIG['response_other_table']      = $DB_PREFIX."response_other";
$ESPCONFIG['response_rank_table']       = $DB_PREFIX."response_rank";
$ESPCONFIG['response_single_table']     = $DB_PREFIX."response_single";
$ESPCONFIG['response_text_table']       = $DB_PREFIX."response_text";
$ESPCONFIG['survey_table']              = $DB_PREFIX."survey";

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
