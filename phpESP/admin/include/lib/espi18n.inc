<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/**
 * Load the GNU Gettext module if it is not loaded.
 * If it cannot be loaded, define the NOP wrapper.
 * If the wrapper is defined, English will be the only
 * language available.
 *
 * See:
 *  http://www.php.net/manual/en/ref.gettext.php
 */

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

    $ESPCONFIG['gettext'] = false;

//    function dcngettext($d,...) {return null;}
//    function dngettext() {return null;}
//    function ngettext(...) {return null;}
}

function esp_setlocale_ex($str = null)
{
    global $ESPCONFIG;

    if (isset($_SERVER))  $server =& $_SERVER;
    else                  $server =& $HTTP_SERVER_VARS;

    $lang = null;

    if ($str == null && isset($server['HTTP_ACCEPT_LANGUAGE']))
        $str = $server['HTTP_ACCEPT_LANGUAGE'];

    if (!empty($str)) {
        $_langs = preg_replace('(;q=[0-9.]*|\*)', '', $str);
        $_langs = preg_replace('/-([A-Z]*)/ie', "'_'.strtoupper('\\1')", $_langs);
        $_langs = split(' *, *', $_langs);

        $_locales = esp_getlocales();
        $_map = esp_getlocale_map();

        foreach ($_langs as $_lang) {
            if (empty($_lang) || ereg('[./\\]', $_lang))
                continue;
            if (isset($_locales[$_lang])) {
                $lang = $_lang;
                break;
            }
            if (isset($_map[$_lang])) {
                $lang = $_map[$_lang];
                break;
            }
            if (($pos = strpos($_lang, '_')) !== false) {
                $_lang = substr($_lang, 0, $pos);
                if (isset($_locales[$_lang])) {
                    $lang = $_lang;
                    break;
                }
                if (isset($_map[$_lang])) {
                    $lang = $_map[$_lang];
                    break;
                }
            }
        }
    }
    if ($lang == null) {
        $lang = $ESPCONFIG['default_lang'];
    }

    return esp_setlocale($lang);
}

function esp_setlocale($lang)
{
    global $ESPCONFIG;

    $ESPCONFIG['lang'] = $lang;
    #$lang .= ".UTF-8";
    $lang.=".".$ESPCONFIG['charset'];
    setlocale(LC_ALL, $lang);
    if (defined('LC_MESSAGES'))
        setlocale(LC_MESSAGES, $lang);
    bindtextdomain('messages', $ESPCONFIG['locale_path']);
    bind_textdomain_codeset('messages',$ESPCONFIG['charset']);
    textdomain('messages');
    @putenv('LANG=' . $lang);
    @putenv('LANGUAGE=' . $lang);

}

function esp_getlocales()
{
    global $ESPCONFIG;

    static $locales = null;

    if ($locales == null) {
        $locales = array();
        $dir = @opendir($ESPCONFIG['locale_path']);
        if ($dir === false)
            return $locales;

        while (($file = readdir($dir)) !== false) {
            if (@is_readable($ESPCONFIG['locale_path']."/$file/LC_MESSAGES/messages.mo"))
                array_push($locales, $file);
        }
        closedir($dir);
    }

    return $locales;
}

function esp_getlocale_map()
{
    global $ESPCONFIG;

    static $map = null;

    if ($map == null) {
        $map = array();
        if (is_readable($ESPCONFIG['locale_path'] . '/map'))
        {
            // get 2-letter mappings
            $arr = file($ESPCONFIG['locale_path'] . '/map');
            foreach ($arr as $line) {
                $line = trim(preg_replace("/[ \t]*#.*/", '', $line));
                if  (empty($line))
                    continue;
                list($a, $b) = preg_split("/\s+/", $line);
                $map[$a] = $b;
            }
        }
    }

    return $map;
}

?>
