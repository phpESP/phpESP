<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_destroy();

	if(!empty($sid)) {
		$sql = "SELECT status FROM surveys WHERE id=${sid}";
		$status = mysql_result(mysql_query($sql),0,0);
		if(($status & STATUS_TEST) && !($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE))) {
?>
</form>
<b>Testing Survey...</b> (SID = <?php echo($sid); ?>)
<p>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
			$test=1;
			$tmp = $PHP_SELF;
			$PHP_SELF .= "?where=test";
			include($cf_handler);
?>
</td></tr></table>
<a href="<?php echo($tmp); ?>?where=results&sid=<?php echo($sid); ?>&test=1">View Results</a><br>
<a href="<?php echo($tmp); ?>?where=manage">Go back to Management Interface</a>
<form>
<?php
			return;
		}
	}
	
	$sql = "SELECT id,name,title,status FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>Test a Survey</h2>
Pick Survey to Test
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		// only include "inactive" surveys for editing!
		if(!($status & STATUS_TEST) || 
			($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE)))
				continue;

		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><a href="<?php echo("${ME}?where=test&sid=${sid}"); ?>">Test</a></td>
		<td><a href="<?php echo("${ME}?where=results&test=1&sid=${sid}"); ?>">Results</a></td>
	</tr>
<?php
	}
?>
</table>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
