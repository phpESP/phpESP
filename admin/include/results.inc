<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// see if a survey has been selected
	if(!empty($sid)) {
		if($test) {
			echo("<p><b>Testing Survey...</b> (SID = $sid)</p>\n");
		}
		
?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
		if(is_array($sid))
			$ret = survey_merge($sid,$precision,$totals);
		else
			$ret = survey_results($sid,$precision,$totals,$qid,$cids);

		if(!empty($ret))
			echo("<font color=\"$cf_error_color\">$ret</font>\n");
?>
</td></tr></table>
<?php
		if($test) {
			echo("<a href=\"${ME}?where=test&sid=${sid}\">Test Survey</a><br>\n");
		}
?>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$sql = "SELECT id,name,title,status FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>View Survey Results</h2>
Pick Survey to View
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		// only include active or done surveys viewing!
		// this should be done in SQL
		if($status & (STATUS_ACTIVE | STATUS_DONE & ~STATUS_DELETED)) {
			if($bg != "#ffffff")
				$bg = "#ffffff";
			else
				$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td>
			<a href="<?php echo("${ME}?where=results&sid=${sid}"); ?>">
				<?php echo($name); ?>
			</a>
		</td>
		<td>
			<?php echo($title); ?>
		</td>
	</tr>
<?php
		}
	}
?>
</table>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
