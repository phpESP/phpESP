<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$sids = array();	// all existing survey IDs

	$sql = 'SELECT id FROM surveys ORDER BY id';
	$result = mysql_query($sql);
	$num_s = mysql_num_rows($result);
	for($i=0;$i<$num_s;$i++) {
		$sids[] = mysql_result($result,$i,0);
	}
	mysql_free_result($result);

	foreach($sids as $sid) {
		$status = 0;
		$act = "status_act_${sid}";
		$done = "status_done_${sid}";
		$del = "status_del_${sid}";
		$status = $$act + $$done + $$del;
		@mysql_query("UPDATE surveys SET status='${status}' WHERE id='${sid}'");
	}
	include($PIECES."/manage".$EXT);
?>
