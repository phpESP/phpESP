<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	if(!empty($sid)) { 
		if($test) {
?>
<p><b>Testing Survey...</b> (SID = <?php echo($sid); ?>)</p>
<?php	} ?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php survey_results($sid); ?>
</td></tr></table>
<?php
		if($test) {
?>
<a href="<?php echo($ME); ?>?where=test&sid=<?php echo($sid); ?>">Test Survey</a><br>
<?php	} ?>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
<?php
		return;
	}

//	$sql = "SELECT * FROM surveys ORDER BY name";
	$sql = "SELECT * FROM surveys ORDER BY id DESC";
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
	while($survey = mysql_fetch_array($result)) {
		// only include active or done surveys viewing!
		if($survey["status"] & (STATUS_ACTIVE | STATUS_DONE & ~STATUS_DELETED)) {
			if($bg != "#ffffff")
				$bg = "#ffffff";
			else
				$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($survey['id']); ?></td>
		<td>
			<a href="<?php echo($ME); ?>?where=results&sid=<?php echo($survey['id']); ?>">
				<?php echo($survey['name']); ?>
			</a>
		</td>
		<td>
			<?php echo($survey['title']); ?>
		</td>
	</tr>
<?php
		}
	}
?>
</table>
<a href="<?php echo($ME); ?>?where=manage">Go back to Management Interface</a>
