<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

if(!defined('_FUNCS_HTML')) {
	define('_FUNCS_HTML',TRUE);

	// return HTML format for an warning message
	function mkwarn ($msg) {
		global $cf_warn_color;
		return('<font color="'. $cf_warn_color .'" size="+1">'. $msg ."</font>\n");
	}

	// return HTML format for an error message
	function mkerror ($msg) {
		global $cf_error_color;
		return('<font color="'. $cf_error_color .'" size="+1">[ '. $msg ." ]</font>\n");
	}

	// return HTML format for a radio button
	function mkradio ($name, $value) {
		global $$name;	// needs register_globals = on
		$str = '<input type="radio" name="' . htmlspecialchars($name) .'" value="' . htmlspecialchars($value) .'"';
		if ( $$name == $value )
			$str .= ' checked';
		$str .= '>';
		return($str);
	}

	// return HTML format for a check box
	function mkcheckbox ($name, $value) {
		global $$name;	// needs register_globals = on
		$str = '<input type="checkbox" name="' . htmlspecialchars($name) .'[]" value="' . htmlspecialchars($value) .'"';
		if ( @in_array($value,$$name ) )
			$str .= ' checked';
		$str .= '>';
		return($str);
	}

	// return HTML format for a text entry line
	function mktext ($name) {
		global $$name;	// needs register_globals = on
		return('<input type="text" name="'. htmlspecialchars($name) .'" value="'. htmlspecialchars($$name) .'">');
	}

	// return HTML format for a text entry box
	function mktextarea ($name,$rows,$cols,$wrap) {
		global $$name;	// needs register_globals = on
		$str = '<textarea name="' . htmlspecialchars($name) .'"';
		if($rows > 0)
			$str .= ' rows="' . $rows . '"';
		if($cols > 0)
			$str .= ' cols="' . $cols . '"';
		if($wrap != '')
			$str .= ' wrap="' . strtolower($wrap) . '"';
		$str .= '>' . $$name . '</textarea>';
		return($str);
	}

	// return HTML format for a select box (dropdown)
	function mkselect ($name,$options) {
		global $$name;
		$str  = '<select name="' .$name .'">' . "\n";
		$str .= "<option></option>\n";
		while(list($cid, $content) = each($options)) {
			$checked = '';
			if($$name == $cid)
				$checked = ' selected';
			$str .= '<option value="' . $cid .'"'. $checked .">${content}</option>\n";
		}
		$str .= "</select>\n";
		return($str);
	}

}	// end _FUNCS_HTML

?>
