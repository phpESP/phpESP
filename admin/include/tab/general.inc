<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$sid = XADDSLASHES($HTTP_SESSION_VARS['survey_id']);

	// load current values from DB if $sid exists (in session)
	$survey = array();
	if(!empty($sid) && $updated) {
		$sql = "SELECT * FROM survey WHERE id='${sid}'";
		$result = mysql_query($sql);
		$survey = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
	} else {
		$fields = array('name','title','subtitle','email','thank_head','thank_body','info','public');
		foreach ($fields as $f) {
			$survey[$f] = $HTTP_POST_VARS[$f];
		}
	}
?>
<?php echo(_('The information on this tab applies to the whole survey.
Fill out this page then go to the <b>Questions</b> tab to
add/edit individual survey questions.')); ?>

<hr>
<table width="100%" border="0" cellspacing="0">
	<tr valign="top">
		<td><b><?php echo(_('Name')); ?></b></td>
		<td>
			<input type="text" size=20 name="name" maxlength="20" value="<?php
				echo(htmlspecialchars($survey['name'])); ?>"><br>
			<?php echo(
			_('Internal survey name.') .' '.
			_('This is used for all further access to this survey.')
			.' ('. _('no spaces') .', '. _('alpha-numeric only') .') ['. _('Required') .']'); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Title')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=60 name="title" value="<?php
				echo(htmlspecialchars($survey['title'])); ?>"><br>
			<?php echo(_('Title of the survey.') .' '.
			_('This appears at the top of every page of the survey.')
			.' ('. _('free-form') .', '. _('including spaces') .') [' ._('Required').']'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Subtitle')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=128 name="subtitle" value="<?php
			echo(htmlspecialchars($survey['subtitle'])); ?>"><br>
			<?php echo(_('Subtitle of the survey.') .' '.
			_('Appears below the title.') .' ('. _('free-form') .', '. _('including spaces'). ')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Additional Info')); ?></b></td>
		<td>
			<textarea rows=5 cols=60 name="info" wrap="VIRTUAL"><?php echo($survey['info']); ?></textarea><br>
			<?php echo(_('Text to be displayed on the survey before any questions.')); ?>
			(<?php echo(_('ie. instructions, background info, etc.')); ?>)
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Public')); ?></b></td>
		<td>
			<select name="public">
			<option value="Y"<?php if($survey['public'] != 'N') echo(' selected="selected"'); ?>><?php echo(_('Yes')); ?></option>
			<option value="N"<?php if($survey['public'] == 'N') echo(' selected="selected"'); ?>><?php echo(_('No')); ?></option>
			</select><br>
			<?php echo(_('Public surveys do not require a respondent to login before taking a survey.')); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Email')); ?></b></td>
		<td>
			<input type="text" name="email" size="30" value="<?php echo(htmlspecialchars($survey['email'])); ?>"><br>
			<?php echo(_('Sends a copy of each sumbission to address (or leave blank for no email backup).')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Thank You Heading')); ?></b></td>
		<td>
			<input type="text" name="thank_head" size="30" value="<?php
				echo(htmlspecialchars($survey['thank_head'])); ?>"><br>
			<?php echo(_('Heading on the &quot;Thank You&quot; page (appearing after a user completes a survey). This is displayed in bold.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Thank You Text')); ?></b></td>
		<td>
			<textarea rows=5 cols=60 name="thank_body" wrap="VIRTUAL"><?php
				echo($survey['thank_body']); ?></textarea><br>
			<?php echo(_('Body text on the &quot;Thank You&quot; page. This is displayed below the Thank You Heading.')); ?></td>
	</tr>
</table>
