<?php

# $Id$

// Written by James Flemer
// <jflemer@alum.rpi.edu>

if (isset($_SERVER))  $server =& $_SERVER;
else                  $server =& $HTTP_SERVER_VARS;

/**
 * Load the GNU Gettext module if it is not loaded.
 * If it cannot be loaded, define the NOP wrapper.
 * If the wrapper is defined, English will be the only
 * language available.
 *
 * See:
 *  http://www.php.net/manual/en/ref.gettext.php
 */
$ESPCONFIG['lang'] = $ESPCONFIG['default_lang'];

if (!function_exists('gettext')) {
    if (!ini_get('safe_mode') && ini_get('enable_dl')) {
    	$_ext = 'gettext.so';
    	if (substr(PHP_OS, 0, 3) == 'WIN') {
    		if (function_exists('version_compare') && 
    				version_compare(phpversion(), "4.2.3", ">="))
    		{
    			$_ext = 'libintl-1.dll';
    		} else {
    			$_ext = 'php_gettext.dll';
    		}
    	}
        @dl($_ext);
        unset($_ext);
    }
}

if (!function_exists('gettext')) {
	function bind_textdomain_codeset() {return null;}
	function bindtextdomain() {return null;}
	function dcgettext($d,$s,$c) {return $s;}
	function dgettext($d,$s) {return $s;}
    function gettext($s) {return($s);}
    function textdomain() {return null;}
    function _($s) {return($s);}

//	function dcngettext($d,...) {return null;}
//	function dngettext() {return null;}
//	function ngettext(...) {return null;}	
}

if (!empty($server['HTTP_ACCEPT_LANGUAGE'])) {
	$_langs = preg_replace('(;q=[0-9.]*|\*)', '', $server['HTTP_ACCEPT_LANGUAGE']);
	$_langs = preg_replace('/-([A-Z]*)/ie', "'_'.strtoupper('\\1')", $_langs);
    $_langs = split(' *, *', $_langs);
    foreach ($_langs as $_lang) {
    	if (empty($_lang) || ereg('[./\\]', $_lang))
    		continue;
        if (file_exists($ESPCONFIG['locale_path'] . "/$_lang")) {
            $ESPCONFIG['lang'] = $_lang;
            break;
        }
        $_lang = substr($_lang, 0, strpos($_lang, '_'));
        if (file_exists($ESPCONFIG['locale_path'] . "/$_lang")) {
            $ESPCONFIG['lang'] = $_lang;
            break;
        }
    }
    unset($_lang);
    unset($_langs);
}
setlocale(LC_ALL, $ESPCONFIG['lang']);
setlocale(LC_MESSAGES, $ESPCONFIG['lang']);
bindtextdomain('messages', $ESPCONFIG['locale_path']);
textdomain('messages');
@putenv('LANG=' . $ESPCONFIG['lang']);
@putenv('LANGUAGE=' . $ESPCONFIG['lang']);

?>
