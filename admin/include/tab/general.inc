<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	unset($sid);
	$sid = $HTTP_SESSION_VARS['survey_id'];

	// load current values from DB if $sid exists (in session)
	$survey = array();
	if(!empty($sid) && $updated) {
		$sql = "SELECT * FROM survey WHERE id='${sid}'";
		$result = mysql_query($sql);
		$survey = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
	} else {
		$fields = array('name','realm','title','subtitle','email','theme','thank_head','thank_body','info','public');
	   	foreach ($fields as $f) {
            if(!empty($HTTP_POST_VARS[$f])) 
	    	    $survey[$f] = DESLASH($HTTP_POST_VARS[$f]);
            else
                $survey[$f] = ''; 
        }
	}
?>
<?php echo(_('The information on this tab applies to the whole survey.
Fill out this page then go to the <b>Fields</b> tab to
edit individual fields.')); ?>

<hr>
<table width="95%" border="0" cellspacing="0" cellpadding="0">
	<tr valign="top">
		<td><b><?php echo(_('Name')); ?></b></td>
		<td>
			<input type="text" size="20" name="name" maxlength="20" value="<?php
				if(isset($survey['name'])) echo(htmlspecialchars($survey['name'])); ?>">
			<font color="#ff0000"><em>Required</em></font><br>
			<?php echo(
			_('Survey filename.') .' '.
			_('This is used for all further access to this survey.')
			.' ('. _('no spaces') .', '. _('alpha-numeric only') .')'); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Group')); ?></b></td>
		<td><?php
			$GLOBALS['realm'] =& $survey['realm'];
			$realms = array();
			if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
				$sql = "SELECT name FROM realm";
				$rr = mysql_query($sql);
				while(list($r) = mysql_fetch_row($rr))
					$realms[$r] = $r;
				mysql_free_result($rr);
			} else {
				foreach($HTTP_SESSION_VARS['acl']['pdesign'] as $r)
					$realms[$r] = $r;
			}

			echo(mkselect('realm',$realms) . "  <font color=\"#ff0000\"><em>Required</em></font><br>\n");
			echo(_('Group that owns this survey.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Title')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=60 name="title" value="<?php
				if(isset($survey['title'])) echo(htmlspecialchars($survey['title'])); ?>">
			<font color="#ff0000"><em>Required</em></font><br>
			<?php echo(_('Title of the survey.') .' '.
			_('This appears at the top of every page of the survey.')
			.' ('. _('free-form') .', '. _('including spaces') .')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Subtitle')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=128 name="subtitle" value="<?php
			if(isset($survey['subtitle'])) echo(htmlspecialchars($survey['subtitle'])); ?>"><br>
			<?php echo(_('Subtitle of the survey.') .' '.
			_('Appears below the title.') .' ('. _('free-form') .', '. _('including spaces'). ')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Additional Info')); ?></b></td>
		<td>
			<textarea rows=5 cols=60 name="info" wrap="VIRTUAL"><?php if(isset($survey['info'])) echo($survey['info']); ?></textarea><br>
			<?php echo(_('Text to be displayed on the survey before any fields.')); ?>
			(<?php echo(_('ie. instructions, background info, etc.')); ?>)
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Email')); ?></b></td>
		<td>
			<input type="text" name="email" size="30" value="<?php if(isset($survey['email'])) echo(htmlspecialchars($survey['email'])); ?>"><br>
			<?php echo(_('Sends a copy of each submission to address (or leave blank for no email backup).')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
        <tr valign="top">
                <td><b><?php echo(_('Survey Theme')); ?></b></td>
                <td><?php
			$GLOBALS['theme'] =& $survey['theme'];
			$themes_array = array();
			chdir($ESPCONFIG['survey_css']);
			$dir = dir(".");
			$dir->rewind();
			while ($file=$dir->read()) {
				if (stristr($file,".css")) {
					$pos = strrpos($file, ".");
					$name = substr($file, 0,$pos);
					$themes_array[$file] = $name;
				}
			}
			$dir->close();
		?>
			<?php echo(mkselect('theme',$themes_array)) ?><br>
                        <?php echo(_('Select a theme (css) to use with this survey.')); ?></td>
        </tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Confirmation Heading')); ?></b></td>
		<td>
			<input type="text" name="thank_head" size="30" value="<?php
				if(isset($survey['thank_head'])) echo(htmlspecialchars($survey['thank_head'])); ?>"><br>
			<?php echo(_('Heading on the &quot;Confirmation&quot; page (appearing after a user completes a survey). This is displayed in bold.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Confirmation Text')); ?></b></td>
		<td>
			<textarea rows=5 cols=60 name="thank_body" wrap="VIRTUAL"><?php
				if(isset($survey['thank_body'])) echo($survey['thank_body']); ?></textarea><br>
			<?php echo(_('Body text on the &quot;Confirmation&quot; page. This is displayed below the Confirmation Heading.')); ?></td>
	</tr>
</table>
