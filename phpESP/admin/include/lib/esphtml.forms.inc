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
	return("<font color=\"". $GLOBALS['ESPCONFIG']['warn_color'] ."\" size=\"+1\">${msg}</font>\n");
}
/* }}} */

/* {{{ proto string mkerror(string message)
   Returns HTML format for an error message. */
function mkerror ($msg) {
	return("<font color=\"". $GLOBALS['ESPCONFIG']['error_color'] ."\" size=\"+1\">[ ${msg} ]</font>\n");
}
/* }}} */

	
/* {{{ proto string mkradio(string name, string value)
   Returns HTML format for a radio button. */
function mkradio ($_name, $value, $varr = null) {
    if ($varr == null)
        $varr =& $GLOBALS['HTTP_POST_VARS'];
	$str = '<input type="radio" name="' . htmlspecialchars($_name) .'" value="' . htmlspecialchars($value) .'"';
	if ( isset($varr[$_name]) && $varr[$_name] == $value )
		$str .= ' checked';
	$str .= ' />';
	return($str);
}
/* }}} */
	
/* {{{ proto string mkcheckbox(string name, string value)
   Returns HTML format for a check box. */
function mkcheckbox ($_name, $value, $varr = null) {
    if ($varr == null)
        $varr =& $GLOBALS['HTTP_POST_VARS'];
	$str = '<input type="checkbox" name="' . htmlspecialchars($_name) .'[]" value="' . htmlspecialchars($value) .'"';
	if (isset($varr[$_name]) && is_array($varr[$_name]) && in_array($value, $varr[$_name]))
		$str .= ' checked';
	$str .= ' />';
	return($str);
}
/* }}} */

/* {{{ proto string mktext(string name, int size)
   Returns HTML format for a text entry line. */
function mktext ($_name, $size = 20, $max = 0, $varr = null) {
    if ($varr == null)
        $varr =& $GLOBALS['HTTP_POST_VARS'];
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
        $varr =& $GLOBALS['HTTP_POST_VARS'];
	if (isset($varr[$_name]))
		return('<input type="hidden" name="'. htmlspecialchars($_name) .'" value="'. _stripslashes(htmlspecialchars($varr[$_name])) .'" />');
	return('<input type="hidden" name="'. htmlspecialchars($_name) .'" />');
}
/* }}} */

/* {{{ proto string mktextarea(string name, int rows, int cols, string wrap_type)
   Returns HTML format for a text entry box. */
function mktextarea ($_name, $rows, $cols, $wrap, $varr = null) {
    if ($varr == null)
        $varr =& $GLOBALS['HTTP_POST_VARS'];
	$str = '<textarea name="' . htmlspecialchars($_name) .'"';
	if($rows > 0)
		$str .= ' rows="' . $rows . '"';
	if($cols > 0)
		$str .= ' cols="' . $cols . '"';
	if($wrap != '')
		$str .= ' wrap="' . strtolower($wrap) . '"';
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
        $varr =& $GLOBALS['HTTP_POST_VARS'];
	$str  = "<select name=\"${_name}\">\n";
	$str .= "<option></option>\n";
	while(list($cid, $content) = each($options)) {
		$checked = '';
		if (isset($varr[$_name]) && $varr[$_name] == $cid)
			$checked = ' selected';
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
function mksubmit($_name, $_value = null) {
    if ($_value == null)
        $_value = _('Submit');
    if (!empty($_value))
        $_value = ' value="'.htmlspecialchars($_value).'"';
    return '<input type="submit" name="'.htmlspecialchars($_name).'"'.$_value.' />';
}
/* }}} */

function show_login($_surveyname = null, $_message = null, $_css = null, $_title = "Survey Login") {
    $str = "<html>\n<head>\n<title>Survey Login: $_title</title>\n";
    if (empty($_css)) {
        $_css = "default.css";
    }
    $str .= '<link rel="stylesheet" href="'. $GLOBALS['ESPCONFIG']['css_url'].$_css .'" type="text/css">';
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
    $str .= "\n<fieldset><legend>Login</legend>\n";
    $str .= '<div class="row">';
    $str .= '<label for="username">';
    $str .= "Username:";
    $str .= '</label>';
    $str .= mktext("username");
    $str .= "\n".'</div><div class="row">'."\n";
    $str .= '<label for="password">';
    $str .= "Password:";
    $str .= '</label>';
    $str .= mkpass("password");
    $str .= "\n".'</div><div class="row">'."\n";
    $str .= mksubmit("Login");
    if ($_surveyname != "") {
        $str .= mkhidden("name", $_surveyname);
    }
    $str .= "\n</div></fieldset>\n</form>\n";
    $str .= "</div>";
    $str .= "\n</body>\n</html>";

    return $str;
}
?>
