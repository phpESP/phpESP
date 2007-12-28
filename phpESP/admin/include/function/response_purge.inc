<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>

/* {{{ proto void response_purge(array/int response_id)
   Purges all traces of response_id from the database.
   Returns void. */
function response_purge($qids) {
	if(is_int($qids)) {
		$qidstr = '='.$qids;
	} else if (is_string($qids)) {
		$qids = split(" ",$qids);
	}
	if(is_array($qids)) {
		$qidstr = array_to_insql($qids);
	}

	// work from the bottom up...
	// start with the survey results
	$tables = array('response_bool','response_date','response_multiple','response_other','response_rank','response_single','response_text');
	foreach($tables as $table) {
		$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG'][$table.'_table']." WHERE response_id ${qidstr}";
		$result = execute_sql($sql);
	}
	$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['response_table']." WHERE id ${qidstr}";
	$result = execute_sql($sql);

	return;
}
/* }}} */

?>
