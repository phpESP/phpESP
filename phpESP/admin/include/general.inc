<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");
	session_register("new_survey");

	if(!empty($newid))
		$survey_id = $newid;

	// load current values from DB if survey_id exists (in session)
	if(!empty($survey_id)) {
		$fields = array('name','title','subtitle','email','thank_head','thank_body','info');
		$sql = "SELECT " . join(',',$fields) . " FROM surveys WHERE id='${survey_id}'";
		$result = mysql_query($sql);
		$arr = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
		foreach($fields as $f) {
			$$f = $arr["$f"];
		}
	}
?>
The information on this tab applies to the whole survey. Fill out this page
then go to the <b>Questions</b> tab to add/edit individual survey questions.
<hr>
<table width="100%" border="0" cellspacing="0">
	<tr valign="top">
		<td><b>Name</b></td>
		<td>
			<input type=text size=20 name="name" maxlength="20" value="<?php echo(htmlspecialchars($name)); ?>"><br>
			Internal survey name. This is used for all further access to this survey. (no spaces, alpha-numeric only) [Required]</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Title</b></td>
		<td>
			<input type=text size=60 maxlength=60 name="title" value="<?php echo(htmlspecialchars($title)); ?>"><br>
			Title of the survey. This appears at the top of every page of the survey. (freeform, including spaces) [Required]
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Subtitle</b></td>
		<td>
			<input type=text size=60 maxlength=128 name="subtitle" value="<?php echo(htmlspecialchars($subtitle)); ?>"><br>
			Subtitle of the survey. Appears below the title. (freeform, including spaces)
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Additional Info</b></td>
		<td>
			<textarea rows=5 cols=60 name="info" wrap="VIRTUAL"><?php echo($info); ?></textarea><br>
			Text to be displayed on the survey before any questions. (ie. instructions, background info, etc.)
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Email</b></td>
		<td>
			<input type="text" name="email" size="30" value="<?php echo(htmlspecialchars($email)); ?>"><br>
			Sends a copy of each sumbission to address (or leave blank for no email backup).</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Thank You Heading</b></td>
		<td>
			<input type="text" name="thank_head" size="30" value="<?php
				echo(htmlspecialchars($thank_head)); ?>"><br>
			Heading on the &quot;Thank You&quot; page (appearing after a user
			completes a survey). This is displayed in bold.</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFF"></td></tr>
	<tr valign="top">
		<td><b>Thank You Text</b></td>
		<td>
			<textarea rows=5 cols=60 name="thank_body" wrap="VIRTUAL"><?php
				echo($thank_body); ?></textarea><br>
			Body text on the &quot;Thank You&quot; page. This is displayed
			below the Thank You Heading.</td>
	</tr>
</table>
