<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	unset($sid);
	$sid = $HTTP_SESSION_VARS['survey_id'];

	// load current values from DB if $sid exists (in session)
	$survey = array();
	if(!empty($sid) && $updated) {
		$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id='${sid}'";
		$result = mysql_query($sql);
		$survey = mysql_fetch_array($result,MYSQL_ASSOC);
		mysql_free_result($result);
	} else {
		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info','public');
	   	foreach ($fields as $f) {
            if(!empty($HTTP_POST_VARS[$f]))
	    	    $survey[$f] = _stripslashes($HTTP_POST_VARS[$f]);
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
			<?php echo mktext('name', 20, 64, $survey); ?>
			<font color="#ff0000"><em><?php echo _('Required'); ?></em></font><br>
			<?php echo(
			_('Survey filename.') .' '.
			_('This is used for all further access to this survey.')
			.' ('. _('no spaces') .', '. _('alpha-numeric only') .')'); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Owner')); ?></b></td>
		<td><?php
			$realms = array();
			if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
				$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
				$rr = mysql_query($sql);
				while(list($r) = mysql_fetch_row($rr))
					$realms[$r] = $r;
				mysql_free_result($rr);
			} else {
				foreach($HTTP_SESSION_VARS['acl']['pdesign'] as $r)
					$realms[$r] = $r;
			}

			echo(htmlspecialchars($HTTP_SESSION_VARS['acl']['username']) . "&nbsp;/&nbsp;");
			echo(mkselect('realm',$realms, $survey) . "  <font color=\"#ff0000\"><em>". _('Required') ."</em></font><br>\n");
			echo(_('User and Group that owns this survey.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Title')); ?></b></td>
		<td>
			<?php echo mktext('title', 60, 60, $survey); ?>
			<font color="#ff0000"><em><?php echo _('Required'); ?></em></font><br>
			<?php echo(_('Title of this survey.') .' '.
			_('This appears at the top of every page of this survey.')
			.' ('. _('free-form, including spaces') .')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Subtitle')); ?></b></td>
		<td>
			<?php echo mktext('subtitle', 60, 128, $survey); ?><br>
			<?php echo(_('Subtitle of this survey.') .' '.
			_('Appears below the title.') .' ('. _('free-form, including spaces'). ')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Additional Info')); ?></b></td>
		<td>
			<?php echo mktextarea('info', 5, 60, 'virtual', $survey); ?><br>
			<?php echo(_('Text to be displayed on this survey before any fields.
(i.e. instructions, background info, etc.)')); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Confirmation Page')); ?></b></td>
		<td>
			<?php echo mktext('thanks_page', 60, 255, $survey) .'&nbsp;'.
                    _('(URL)'); ?><br>
			<?php echo(_('The URL to which a user is redirected after completing this survey.')); ?>
			<br><b><?php echo(_('OR')); ?></b><br>
			<?php echo mktext('thank_head', 30, 0, $survey) .'&nbsp;'.
                    _('(heading text)'); ?><br>
			<?php echo mktextarea('thank_body', 5, 60, 'virtual', $survey) .
                    '&nbsp;' . _('(body text)'); ?><br>
 			<?php echo(_('Heading (in bold) and body text for the &quot;Confirmation&quot; page
displayed after a user completes this survey.')); ?>
			<?php echo(_('(URL, if present, takes precedent over confirmation text.)')); ?>
			</td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
	<tr valign="top">
		<td><b><?php echo(_('Email')); ?></b></td>
		<td>
            <?php echo mktext('email', 30, 0, $survey); ?><br>
			<?php echo(_('Sends a copy of each submission to address (or leave blank for no email backup).')); ?></td>
	</tr>
	<tr><td colspan="2"><hr width="90%" size="0" color="#FFFFFF"></td></tr>
        <tr valign="top">
		<td><b><?php echo(_('Theme')); ?></b></td>
                <td><?php
			$themes_array = array();
			$dir = dir($ESPCONFIG['css_path']);
				$dir->rewind();
				while ($file=$dir->read()) {
					if (stristr($file,".css")) {
						$pos = strrpos($file, ".");
						$name = substr($file, 0,$pos);
						$themes_array[$file] = $name;
					}
				}
				$dir->close();

			echo(mkselect('theme',$themes_array, $survey) . '<br>' .
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
