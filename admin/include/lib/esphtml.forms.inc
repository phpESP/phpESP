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
function mkradio ($name, $value) {
	// needs register_globals = on
	global $$name;
	$str = '<input type="radio" name="' . htmlspecialchars($name) .'" value="' . htmlspecialchars($value) .'"';
	if ( $$name == $value )
		$str .= ' checked';
	$str .= '>';
	return($str);
}
/* }}} */
	
/* {{{ proto string mkcheckbox(string name, string value)
   Returns HTML format for a check box. */
function mkcheckbox ($name, $value) {
	// needs register_globals = on
	global $$name;
	$str = '<input type="checkbox" name="' . htmlspecialchars($name) .'[]" value="' . htmlspecialchars($value) .'"';
	if ( @in_array($value,$$name ) )
		$str .= ' checked';
	$str .= '>';
	return($str);
}
/* }}} */

/* {{{ proto string mktext(string name, int size)
   Returns HTML format for a text entry line. */
function mktext ($name, $size = 20) {
	// needs register_globals = on
	global $$name;
	return('<input type="text" size="'. intval($size) .'" name="'. htmlspecialchars($name) .'" value="'. htmlspecialchars($$name) .'">');
}
/* }}} */

/* {{{ proto string mkpass(string name)
   Returns HTML format for a password entry line. */
function mkpass ($name) {
	return('<input type="password" name="'. htmlspecialchars($name) .'">');
}
/* }}} */

/* {{{ proto string mktextarea(string name, int rows, int cols, string wrap_type)
   Returns HTML format for a text entry box. */
function mktextarea ($name,$rows,$cols,$wrap) {
	// needs register_globals = on
	global $$name;
	$str = '<textarea name="' . htmlspecialchars($name) .'"';
	if($rows > 0)
		$str .= ' rows="' . $rows . '"';
	if($cols > 0)
		$str .= ' cols="' . $cols . '"';
	if($wrap != '')
		$str .= ' wrap="' . strtolower($wrap) . '"';
	$str .= '>' . htmlspecialchars($$name) . '</textarea>';
	return($str);
}
/* }}} */

/* {{{ proto string mkselect(string name, array options)
   Returns HTML format for a select box (dropdown). */
function mkselect ($name, $options) {
	// needs register_globals = on
	global $$name;
	$str  = "<select name=\"${name}\">\n";
	$str .= "<option></option>\n";
	while(list($cid, $content) = each($options)) {
		$checked = '';
		if($$name == $cid)
			$checked = ' selected';
		$str .= "<option value=\"${cid}\"${checked}>${content}</option>\n";
	}
	$str .= "</select>\n";
	return($str);
}
/* }}} */

} // end _LIB_ESPHTML_FORMS
?>
