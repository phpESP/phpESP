<?php

// $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

if(!defined('_FUNCTION_ISTRUE')) {
	define('_FUNCTION_ISTRUE',TRUE);

	// boolean istrue(mixed $mixed);

	// This function takes a mixed variable, and
	// returns TRUE (1) when it is: a non-zero integer/float,
	// a string containing a 'y' or a 't' (case insensative),
	// or if it is the constant TRUE. Else it returns FALSE (0).

	function istrue($mixed) {
		if(is_long($mixed))
			return ($mixed && TRUE);
		if(is_double($mixed))
			return ($mixed && TRUE);
		if(is_string($mixed)) {
			$s = strtoupper($mixed);
			return(strstr($s, "Y") || strstr($s, "T"));
		}
		if(is_array($mixed))
			return 0;	// can't eval array, return FALSE
		if(is_object($mixed))
			return 0;	// can't eval object, return FALSE
		if($mixed == TRUE)
			return TRUE;
		return 0;
	}

} // end _FUNCTION_ISTRUE
