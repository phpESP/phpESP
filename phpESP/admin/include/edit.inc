<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_destroy();
	$curr_q=1;

//	$sql = "SELECT * FROM surveys ORDER BY name";
	$sql = "SELECT * FROM surveys ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2>Edit a Survey</h2>
Pick Survey to Edit
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>ID</b></td>
		<td><b>Name</b></td>
		<td><b>Title</b></td>
	</tr>
<?php
	while($survey = mysql_fetch_array($result)) {
		// only include "inactive" surveys for editing!
		if(!($survey["status"])) {

			if($bg != "#ffffff")
				$bg = "#ffffff";
			else
				$bg = "#eeeeee";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($survey['id']); ?></td>
		<td>
			<a href="<?php echo($ME); ?>?where=tab&tab=general&newid=<?php echo($survey['id']); ?>">
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
