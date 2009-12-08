<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

// string	mkwarn(char *warning);
// string	mkerror(char *error);

// string	mkradio(char *name, char *value);
// string	mkcheckbox(char *name, char *value);
// string	mktext(char *name);
// string   mkhidden(char *name, char *$value)
// string	mktextarea(char *name, int rows, int cols, char *wordwrap);
// string	mkselect(char *name, char *options[]);
// string	mkfile(char *name);

/* {{{ proto string mkwarn(string message)
   Returns HTML format for an warning message. */
function mkwarn ($msg) {
	return("<span class=\"warn\">${msg}</span>\n");
}
/* }}} */

/* {{{ proto string mkerror(string message)
   Returns HTML format for an error message. */
function mkerror ($msg) {
	return("<span class=\"error\">[ ${msg} ]</span>\n");
}
/* }}} */

/* {{{ proto string mkradio(string name, string value)
   Returns HTML format for a radio button. */
function mkradio ($_name, $value, $varr = null) {
    	if ($varr == null)
        	$varr =& $_POST;
	$str = '<input type="radio" name="' . htmlspecialchars($_name) .'" value="' . htmlspecialchars($value) .'"';
	if ( isset($varr[$_name]) && $varr[$_name] == $value )
		$str .= " checked=\"checked\"";
	$str .= ' />';
	return($str);
}
/* }}} */
	
/* {{{ proto string mkcheckbox(string name, string value)
   Returns HTML format for a check box. */
function mkcheckbox ($_name, $value, $varr = null) {
    	if ($varr == null)
        	$varr =& $_POST;
	$str = '<input type="checkbox" name="' . htmlspecialchars($_name) .'[]" value="' . htmlspecialchars($value) .'"';
	if (isset($varr[$_name]) && is_array($varr[$_name]) && in_array($value, $varr[$_name]))
		$str .= " checked=\"checked\"";
	$str .= ' />';
	return($str);
}
/* }}} */

/* {{{ proto string mktext(string name, int size)
   Returns HTML format for a text entry line. */
function mktext ($_name, $size = 20, $max = 0, $varr = null) {
        if ($varr == null)
            $varr =& $_POST;
	$size = intval($size);
	$max  = intval($max);
	$str = "size=\"$size\"";
	if ($max)	$str .= " maxlength=\"$max\"";
	if (isset($varr[$_name]))
		return('<input type="text" '. $str .' name="'. htmlspecialchars($_name) .'" value="'. _stripslashes(htmlspecialchars($varr[$_name])) .'" />');
	return('<input type="text" '. $str .' name="'. htmlspecialchars($_name) .'" />');
}
/* }}} */

/* {{{ proto string mkpass(string name)
   Returns HTML format for a password entry line. */
function mkpass ($_name) {
	return('<input type="password" name="'. htmlspecialchars($_name) .'" />');
}
/* }}} */

/* {{{ proto string mkhidden(string name)
   Returns HTML format for a hidden form field. */
function mkhidden ($_name, $varr = null) {
    	if ($varr == null)
        	$varr =& $_POST;
	if (isset($varr[$_name]))
		return('<input type="hidden" name="'. htmlspecialchars($_name) .'" value="'. _stripslashes(htmlspecialchars($varr[$_name])) .'" />');
	return('<input type="hidden" name="'. htmlspecialchars($_name) .'" />');
}
/* }}} */

/* {{{ proto string mktextarea(string name, int rows, int cols, string wrap_type)
   Returns HTML format for a text entry box. */
function mktextarea ($_name, $rows, $cols, $wrap, $varr = null) {
    	if ($varr == null)
        	$varr =& $_POST;
	$str = '<textarea name="' . htmlspecialchars($_name) .'"';
	if($rows > 0)
		$str .= ' rows="' . $rows . '"';
	if($cols > 0)
		$str .= ' cols="' . $cols . '"';
	//if($wrap != '')
	//	$str .= ' wrap="' . strtolower($wrap) . '"';
	$str .= '>';
	if (isset($varr[$_name]))
		 $str .= _stripslashes(htmlspecialchars($varr[$_name]));
	$str .= '</textarea>';
	return($str);
}
/* }}} */

/* {{{ proto string mkselect(string name, array options)
   Returns HTML format for a select box (dropdown). */
function mkselect ($_name, $options, $varr = null) {
    	if ($varr == null)
        	$varr =& $_POST;
	$str  = "<select name=\"${_name}\">\n";
	$str .= "<option></option>\n";
	while(list($cid, $content) = each($options)) {
		$checked = '';
		if (isset($varr[$_name]) && $varr[$_name] == $cid)
			$checked = " selected=\"selected\"";
		$str .= "<option value=\"${cid}\"${checked}>${content}</option>\n";
	}
	$str .= "</select>\n";
	return($str);
}
/* }}} */

/* {{{ proto string mkfile(string name)
   Returns HTML format for a file selection button. */
function mkfile ($_name) {
	return('<input type="file" name="'.htmlspecialchars($_name) .'" />');
}
/* }}} */

/* {{{ proto string mksubmit(string name, string value)
   Returns HTML format for a submit button. */
function mksubmit($_name, $_value = null, $_attrs = array ()) {
    if ($_value == null)
        $_value = _('Submit');
    if (!empty($_value))
        $_value = ' value="'.htmlspecialchars($_value).'"';
    $_extra = make_element_attrs_from_array($_attrs);

    return sprintf('<input type="submit" name="%s" %s %s />', htmlspecialchars($_name), $_value, $_extra);
}
/* }}} */

/* {{{ proto string show_login(string surveyname, string message, string css, string title)
    Returns HTML login form */
function show_login($_surveyname = null, $_message = null, $_css = null, $_title = "Survey Login") {
    $str = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">";
    $str .="<head>\n";
    if(!empty($GLOBALS['ESPCONFIG']['charset'])) {
        echo('<meta http-equiv="Content-Type" content="text/html; charset='. $GLOBALS['ESPCONFIG']['charset'] ."\" />\n");
    }
    $str .="<title>Survey Login: $_title</title>\n";
    if (empty($_css)) {
        $_css = "default.css";
    }
    $str .= '<link rel="stylesheet" href="'. $GLOBALS['ESPCONFIG']['css_url'].$_css .'" type="text/css">';
    if(!empty($GLOBALS['ESPCONFIG']['favicon'])) {
        $str .= "<link rel=\"shortcut icon\" href=\"" . $GLOBALS['ESPCONFIG']['favicon'] . "\" />\n";
    }
    $str .=  "\n</head>\n<body id=\"auth\">\n";
    
    $str .= '<div class="headerGraphic"></div>';
    $str .= '<div class="login">';
    if ($_title) {
        $str .= "<h1>$_title</h1>\n";
    }
    if ($_message) {
        $str .= mkerror($_message);
    }

    $str .= '<form name="loginform" id="loginform"  method="post">';
    $str .= "\n<fieldset>\n";
    $str .= '<div class="row">';
    $str .= '<label for="username">';
    $str .= _('Username:');
    $str .= '</label>';
    $str .= mktext("username");
    $str .= "\n".'</div><div class="row">'."\n";
    $str .= '<label for="password">';
    $str .= _('Password:');
    $str .= '</label>';
    $str .= mkpass("password");
    $str .= "\n".'</div><div class="row">'."\n";
    # if the variable $lang is set, we assume we don't need to ask for the language
    if ((!isset($_REQUEST['lang']) && !isset($_SESSION['language'])) || empty($_SESSION['language'])) {
       $str .= _('Language:');
       $_languages=array();
       foreach (esp_getlocales() as $_lang) {
	   $_languages[$_lang]=$_lang;
       }
       $str .= mkselect("lang",$_languages);
       $str .= "\n".'</div><div class="row">'."\n";
    }
    $str .= mksubmit("Login");
    if ($_surveyname != "") {
        $str .= '<input type="hidden" name="'."$_surveyname".'"';
    }
    $str .= "\n</div></fieldset>\n</form>\n";
    $str .= "</div>";
    $str .= "\n</body>\n</html>";

    return $str;
}
/* }}} */

/* {{{ proto string make_element_attrs_from_array(array attributes)
   Convert an array of key => value pairs into an HTML attributes specification key="value"
*/

function make_element_attrs_from_array($attrs) {
    $extra = '';
    foreach ($attrs as $attr => $value) {
        $extra .= sprintf(' %s="%s"', $attr, htmlspecialchars($value));
    }

    return trim($extra);
}

// }}}
?>
