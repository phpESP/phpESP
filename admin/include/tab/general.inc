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
		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info','public');
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
			<input type="text" size="20" name="name" maxlength="64" value="<?php
				if(isset($survey['name'])) echo(htmlspecialchars($survey['name'])); ?>">
			<font color="#ff0000"><em>Required</em></font><br>
			<?php echo(
			_('Survey filename.') .' '.
			_('This is used for all further access to this survey.')
			.' ('. _('no spaces') .', '. _('alpha-numeric only') .')'); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Owner')); ?></b></td>
		<td><?php
			$HTTP_POST_VARS['realm'] =& $survey['realm'];
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

			echo(htmlspecialchars($HTTP_SESSION_VARS['acl']['username']) . "&nbsp;/&nbsp;");
			echo(mkselect('realm',$realms) . "  <font color=\"#ff0000\"><em>Required</em></font><br>\n");
			echo(_('User and Group that owns this survey.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Title')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=60 name="title" value="<?php
				if(isset($survey['title'])) echo(htmlspecialchars($survey['title'])); ?>">
			<font color="#ff0000"><em>Required</em></font><br>
			<?php echo(_('Title of this survey.') .' '.
			_('This appears at the top of every page of this survey.')
			.' ('. _('free-form') .', '. _('including spaces') .')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Subtitle')); ?></b></td>
		<td>
			<input type="text" size=60 maxlength=128 name="subtitle" value="<?php
			if(isset($survey['subtitle'])) echo(htmlspecialchars($survey['subtitle'])); ?>"><br>
			<?php echo(_('Subtitle of this survey.') .' '.
			_('Appears below the title.') .' ('. _('free-form') .', '. _('including spaces'). ')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Additional Info')); ?></b></td>
		<td>
			<textarea rows=5 cols=60 name="info" wrap="VIRTUAL"><?php if(isset($survey['info'])) echo($survey['info']); ?></textarea><br>
			<?php echo(_('Text to be displayed on this survey before any fields.')); ?>
			(<?php echo(_('ie. instructions, background info, etc.')); ?>)
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Confirmation Page')); ?></b></td>
		<td>
			<input type="text" name="thanks_page" size="60" maxlength="255" value="<?php
				if(isset($survey['thank_head'])) echo(htmlspecialchars($survey['thanks_page'])); ?>">
			<?php echo(_('(URL)')); ?><br>
			<?php echo(_('The URL to which a user is redirected after completing this survey.')); ?>
			<br><b><?php echo(_('OR')); ?></b><br>
			<input type="text" name="thank_head" size="30" value="<?php
				if(isset($survey['thank_head'])) echo(htmlspecialchars($survey['thank_head'])); ?>">
			<?php echo(_('(heading text)')); ?><br>
			<textarea rows=5 cols=60 name="thank_body" wrap="VIRTUAL"><?php
				if(isset($survey['thank_body'])) echo($survey['thank_body']); ?></textarea>
			<?php echo(_('(body text)')); ?><br>
 			<?php echo(_('Heading (in bold) and body text for the &quot;Confirmation&quot; page
					displayed after a user completes this survey.')); ?>
			<?php echo(_('(URL, if present, takes precedent over confirmation text.)')); ?>
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
		<td><b><?php echo(_('Theme')); ?></b></td>
                <td><?php
			$themes_array = array();
			$dir = dir($ESPCONFIG['survey_css']);
				$dir->rewind();
				while ($file=$dir->read()) {
					if (stristr($file,".css")) {
						$pos = strrpos($file, ".");
						$name = substr($file, 0,$pos);
						$themes_array[$file] = $name;
					}
				}
				$dir->close();

			echo(mkselect('theme',$themes_array) . '<br>' .
            		_('Select a theme (css) to use with this survey.')); ?></td>
        </tr>
<!--
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
        <td><b><?php echo(_('Options')); ?></b></td>
		<td><?php echo(mkcheckbox('options','resume') . '&nbsp;' .
				_('Allow survey respondents to save/resume. (Survey login required.)')); ?></td>
	</tr>
	<tr valign="top">
		<td>&nbsp;</td>
		<td><?php echo(mkcheckbox('options','navigate') . '&nbsp;' .
				_('Allow survey respondents to go forward and back between survey sections.')); ?></td>
-->                
</table>
