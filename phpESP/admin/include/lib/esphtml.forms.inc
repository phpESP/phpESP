<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

// string	mkwarn(char *warning);
// string	mkerror(char *error);

// string	mkradio(char *name, char *value);
// string	mkcheckbox(char *name, char *value);
// string	mktext(char *name);
// string	mktextarea(char *name, int rows, int cols, char *wordwrap);
// string	mkselect(char *name, char *options[]);

if(!defined('_LIB_ESPHTML_FORMS')) {
	define('_LIB_ESPHTML_FORMS',TRUE);

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
function mkradio ($_name, $value) {
	// needs register_globals = on
	global $$_name;
	$str = '<input type="radio" name="' . htmlspecialchars($_name) .'" value="' . htmlspecialchars($value) .'"';
	if ( isset($$_name) && $$_name == $value )
		$str .= ' checked';
	$str .= '>';
	return($str);
}
/* }}} */
	
/* {{{ proto string mkcheckbox(string name, string value)
   Returns HTML format for a check box. */
function mkcheckbox ($_name, $value) {
	// needs register_globals = on
	global $$_name;
	$str = '<input type="checkbox" name="' . htmlspecialchars($_name) .'[]" value="' . htmlspecialchars($value) .'"';
	if ( isset($$_name) && @in_array($value,$$_name ) )
		$str .= ' checked';
	$str .= '>';
	return($str);
}
/* }}} */

/* {{{ proto string mktext(string name, int size)
   Returns HTML format for a text entry line. */
function mktext ($_name, $size = 20, $max = 0) {
	// needs register_globals = on
	global $$_name;
	$size = intval($size);
	$max  = intval($max);
	$str = "size=\"$size\"";
	if ($max)	$str .= " maxlength=\"$max\"";
	if (isset($$_name))
		return('<input type="text" '. $str .' name="'. htmlspecialchars($_name) .'" value="'. htmlspecialchars($$_name) .'">');
	return('<input type="text" '. $str .' name="'. htmlspecialchars($_name) .'">');
}
/* }}} */

/* {{{ proto string mkdate(string name, int size)
   Returns HTML format for a date entry line. */
function mkdate ($_name, $size = 20, $max = 10) {
	// needs register_globals = on
	global $$_name;
	$size = intval($size);
	$max  = intval($max);
	$str = "size=\"$size\"";
	if ($max)	$str .= " maxlength=\"$max\"";
	if (isset($$_name))
		return('<input onblur="check_date(this)" type="text" '. $str .' name="'. htmlspecialchars($_name) .'" value="'. htmlspecialchars($$_name) .'">');
	return('<input onblur="check_date(this)" type="text" '. $str .' name="'. htmlspecialchars($_name) .'">');
}
/* }}} */

/* {{{ proto string mkdate(string name, int size)
   Returns HTML format for a numeric entry line. */
function mknum ($_name, $size, $max) {
	// needs register_globals = on
	global $$_name;
	$size = intval($size);
	$max  = intval($max);
	$str = "size=\"$size\"";
	if ($max)	$str .= " maxlength=\"$max\"";
	if (isset($$_name))
		return('<input onKeyUp="javascript:dodacheck(this);" onKeyDown="javascript:dodacheck(this);" type="text" '. $str .' name="'. htmlspecialchars($_name) .'" value="'. htmlspecialchars($$_name) .'">');
	return('<input onKeyUp="javascript:dodacheck(this);" onKeyDown="javascript:dodacheck(this);" type="text" '. $str .' name="'. htmlspecialchars($_name) .'">');
}
/* }}} */

/* {{{ proto string mkpass(string name)
   Returns HTML format for a password entry line. */
function mkpass ($_name) {
	return('<input type="password" name="'. htmlspecialchars($_name) .'">');
}
/* }}} */

/* {{{ proto string mkhidden(string name)
   Returns HTML format for a hidden form field. */
function mkhidden ($_name) {
	global $$_name;
	if (isset($$_name))
		return('<input type="hidden" name="'. htmlspecialchars($_name) .'" value="'. htmlspecialchars($$_name) .'">');
	return('<input type="hidden" name="'. htmlspecialchars($_name) .'">');
}
/* }}} */

/* {{{ proto string mktextarea(string name, int rows, int cols, string wrap_type)
   Returns HTML format for a text entry box. */
function mktextarea ($_name,$rows,$cols,$wrap) {
	// needs register_globals = on
	global $$_name;
	$str = '<textarea name="' . htmlspecialchars($_name) .'"';
	if($rows > 0)
		$str .= ' rows="' . $rows . '"';
	if($cols > 0)
		$str .= ' cols="' . $cols . '"';
	if($wrap != '')
		$str .= ' wrap="' . strtolower($wrap) . '"';
	$str .= '>';
	if (isset($$_name))
		 $str .= htmlspecialchars($$_name);
	$str .= '</textarea>';
	return($str);
}
/* }}} */

/* {{{ proto string mkselect(string name, array options)
   Returns HTML format for a select box (dropdown). */
function mkselect ($_name, $options) {
	// needs register_globals = on
	global $$_name;
	$str  = "<select name=\"${_name}\">\n";
	$str .= "<option></option>\n";
	while(list($cid, $content) = each($options)) {
		$checked = '';
		if(isset($$_name) && $$_name == $cid)
			$checked = ' selected';
		$str .= "<option value=\"${cid}\"${checked}>${content}</option>\n";
	}
	$str .= "</select>\n";
	return($str);
}
/* }}} */

} // end _LIB_ESPHTML_FORMS
?>
