<?php

// $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

require_once("function.istrue.php");

// abstract
class Question {
	var $id;		// int: question id
	var $text;		// string: question text
	var $required;	// bool: is answer required
	
	function Question($t = "", $r = 0) {
		$this->text = $t;
		$this->required = istrue($r);
	}
	
	function setText($t) {
		$this->text = $t;
	}
	
	function getText() {
		return $this->text;
	}
	
	function isRequired() {
		return $this->required;
	}
	
	function setRequired($r) {
		$this->required = istrue($r);
	}
}

// abstract
class ListQuestion extends Question {
	var $list;
	
	function ListQuestion($t = "", $r = 0) {
		$this->text = $t;
		$this->required = istrue($r);
		$this->list = array();
	}
	
	function setListArray($array) {
		$this->list = array();
		if(is_array($array))
			$this->list = $array;
	}
	
	function pushList($array) {
		array_push($this->list, $array);
	}
	
	function getList() {
		return $this->list;
	}
}

class TextQuestion extends Question {
	var $width;		// int: width of text field
	var $height;	// int: height of text field
	var $wrap;		// bool: wrap text in box
	
	function TextQuestion($t = "", $w = 20, $h = 1, $p = 0, $r = 0) {
		$this->text = $t;
		$this->width = (int) $w;
		$this->height = (int) $h;
		$this->wrap = istrue($p);
		$this->required = istrue($r);
	}
	
	function getWidth() {
		return $this->width;
	}
	function setWidth($var) {
		$this->width = (int) $var;
	}
	
	function getHeight() {
		return $this->height;
	}
	function setHeight($var) {
		$this->height = (int) $var;
	}
	
	function getWrap() {
		return $this->wrap;
	}
	function setWrap($var) {
		$this->wrap = istrue($var);
	}
}

class BooleanQuestion extends Question {
	var $true;		// string: description of "true"
	var $false;		// string: description of "false"
	
	function setTrueFalse() {
		$true = "True";
		$false = "False";
	}
	
	function setYesNo() {
		$true = "Yes";
		$false = "No";
	}
	
	function getTrue() {
		return $this->true;
	}
	function setTrue($var) {
		$this->true = $var;
	}
	
	function getFalse() {
		return $this->false;
	}
	function setFalse($var) {
		$this->false = $var;
	}
}

class DropdownQuestion extends ListQuestion {
	function DropdownQuestion($t = "", $r = 0) {
		$this->text = $t;
		$this->required = istrue($r);
		$this->list = array();
	}
}

class RadioQuestion extends ListQuestion {
	function RadioQuestion($t = "", $r = 0) {
		$this->text = $t;
		$this->required = istrue($r);
		$this->list = array();
	}
}

class CheckboxQuestion extends ListQuestion {
	var $min;		// minimum number of boxes that must be checked
	var $max;		// maximum number of boxes that may  be checked
	
	function CheckboxQuestion($t = "", $n = 0, $x = 0, $r = 0) {
		$this->text = $t;
		$this->min = (int) $n;
		$this->max = (int) $x;
		$this->required = istrue($r);
		$this->list = array();
	}
	
	function getMin() {
		return $this->min;
	}
	function setMin($var) {
		$this->min = (int) $var;
	}
	
	function getMax() {
		return $this->max;
	}
	function setMax($var) {
		$this->max = (int) $var;
	}
}

class ScaledQuestion extends ListQuestion {
	var $withNA;	// bool: include a N/A column
	var $scale;		// array: items on the scale (in order)
	
	function ScaledQuestion($t = "", $r = 0) {
		$this->text     = $t;
		$this->required = istrue($r);
		$this->withNA   = 0;
		$this->scale    = array();
		$this->list     = array();
	}
	
	function setScaleNumeric($min, $max, $incr = 1) {
		$this->scale = array();
		for($i = $min; $i <= $max; $i += $incr) {
			array_push($this->scale, $i);
		}
	}
	
	function setScaleArray($array) {
		$this->scale = array();
		if(is_array($array))
			$this->scale = $array;
	}
	
	function getWithNA() {
		return $this->withNA;
	}
	function setWithNA($var) {
		$this->withNA = istrue($var);
	}
}

?>
