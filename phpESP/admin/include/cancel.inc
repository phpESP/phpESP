<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");

	if(!$survey_id) {
		// no survey INSERTed yet, so just go back to management interface
		@session_destroy();
		include($PIECES."/index".$EXT);
		return;
	}

	$sql = "SELECT status FROM surveys WHERE id='${survey_id}'";
	$result = mysql_query($sql);
	if(mysql_num_rows($result) < 1) {
		@session_destroy();
		include($PIECES."/index".$EXT);
		return;
	}
	$status = mysql_result($result,0,0);
	$status |= STATUS_DELETED;
	$sql = "UPDATE surveys SET status='${status}' WHERE id='${survey_id}'";
	$result = mysql_query($sql);
	@session_destroy();
	include($PIECES."/index".$EXT);
	return;
?>
