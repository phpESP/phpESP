<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

	// see if a survey has been selected
	if(!empty($sid)) {
		if(!isset($details))
			$details=1; // by default, show totals in results
?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
	$ret = survey_report($sid,$details);
	if($ret != '')
		echo("<font color=\"$cf_error_color\">$ret</font>\n");
?>
</td></tr></table>
<a href="<?php echo("${ME}?where=report"); ?>">Go back to Report Menu</a><br>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

//	$sql = "SELECT * FROM surveys ORDER BY name";
	$sql = "SELECT id,name,title,status FROM surveys WHERE NOT (status & '".STATUS_DELETED."') ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>View Survey Report</h2>
Pick Survey to View
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
	</tr>
<?php
	while(list($sid, $name, $title) = mysql_fetch_row($result)) {
		if($bg != "#ffffff")
			$bg = "#ffffff";
		else
			$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td>
			<a href="<?php echo($ME); ?>?where=report&sid=<?php echo($sid); ?>">
				<?php echo($name); ?>
			</a>
		</td>
		<td>
			<?php echo($title); ?>
		</td>
	</tr>
<?php
	}
?>
</table>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
