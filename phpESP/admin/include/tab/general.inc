<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	unset($sid);
	$sid = $_SESSION['survey_id'];

	// load current values from DB if $sid exists (in session)
	$survey = array();
	if(!empty($sid) && $updated) {
		$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
		$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
		$survey = fetch_row($result);
		db_close($result);
	} else {
		$fields = array('name','realm','title','subtitle','email','theme','thanks_page','thank_head','thank_body','info','public','auto_num','open_date','close_date');
	   	foreach ($fields as $f) {
            if(!empty($_POST[$f]))
	    	    $survey[$f] = _stripslashes($_POST[$f]);
            else
                $survey[$f] = '';
        }
	}

    // if the date values are zeroes, something bad went in -- correct it on the display so as to not panic the user
    if ('0000-00-00 00:00:00' == $survey['open_date']) {
        $survey['open_date'] = '';
    }
    if ('0000-00-00 00:00:00' == $survey['close_date']) {
        $survey['close_date'] = '';
    }
?>
<?php echo(_('The information on this tab applies to the whole survey.
Fill out this page then go to the <strong>Fields</strong> tab to
edit individual fields.')); ?>
<script type="text/javascript">
<!--
function validate() {
    return true;
}
-->
</script>
<hr />
<table cellspacing="0" cellpadding="0">
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Name')); ?></strong></td>
		<td class="left">
			<?php echo mktext('name', 20, 64, $survey); ?>
			<span class="red"><em><?php echo _('Required'); ?></em></span><br />
			<?php echo(
			_('Survey filename.') .' '.
			_('This is used for all further access to this survey.')
			.' ('. _('no spaces') .', '. _('alpha-numeric only') .')'); ?></td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Owner')); ?></strong></td>
		<td class="left"><?php
			$realms = array();
			if(isset($_SESSION['acl']['superuser']) && $_SESSION['acl']['superuser'] == 'Y') {
				$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
				$rr = execute_sql($sql);
				while(list($r) = fetch_row($rr)) {
                    $rr->MoveNext();
					$realms[$r] = $r;
                }
				db_close($rr);
			} else {
				foreach($_SESSION['acl']['pdesign'] as $r)
					$realms[$r] = $r;
			}

			echo(htmlspecialchars($_SESSION['acl']['username']) . "&nbsp;/&nbsp;");
			echo(mkselect('realm',$realms, $survey) . "  <span class=\"red\"><em>". _('Required') ."</em></span><br />\n");
			echo(_('User and Group that owns this survey.')); ?></td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Title')); ?></strong></td>
		<td class="left">
			<?php echo mktext('title', 60, 60, $survey); ?>
			<span class="red"><em><?php echo _('Required'); ?></em></span><br />
			<?php echo(_('Title of this survey.') .' '.
			_('This appears at the top of every page of this survey.')
			.' ('. _('free-form, including spaces') .')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Subtitle')); ?></strong></td>
		<td class="left">
			<?php echo mktext('subtitle', 60, 128, $survey); ?><br />
			<?php echo(_('Subtitle of this survey.') .' '.
			_('Appears below the title.') .' ('. _('free-form, including spaces'). ')'); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Additional Info')); ?></strong></td>
		<td class="left">
			<?php echo mktextarea('info', 5, 60, 'virtual', $survey); ?><br />
			<?php echo(_('Text to be displayed on this survey before any fields.
(i.e. instructions, background info, etc.)')); ?>
		</td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Confirmation Page')); ?></strong></td>
		<td class="left">
			<?php echo mktext('thanks_page', 60, 255, $survey) .'&nbsp;'.
                    _('(URL)'); ?><br />
			<?php echo(_('The URL to which a user is redirected after completing this survey.')); ?>
			<br /><strong><?php echo(_('OR')); ?></strong><br />
			<?php echo mktext('thank_head', 30, 0, $survey) .'&nbsp;'.
                    _('(heading text)'); ?><br />
			<?php echo mktextarea('thank_body', 5, 60, 'virtual', $survey) .
                    '&nbsp;' . _('(body text)'); ?><br />
 			<?php echo(_('Heading (in bold) and body text for the &quot;Confirmation&quot; page
displayed after a user completes this survey.')); ?>
			<?php echo(_('(URL, if present, takes precedent over confirmation text.)')); ?>
			</td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Email')); ?></strong></td>
		<td class="left">
            <?php echo mktext('email', 30, 0, $survey); ?><br />
			<?php echo(_('Sends a copy of each submission to address (or leave blank for no email backup).')); ?></td>
	</tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
        <tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Theme')); ?></strong></td>
                <td class="left"><?php
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

			echo(mkselect('theme',$themes_array, $survey) . '<br />' .
            		_('Select a theme (css) to use with this survey.')); ?></td>
        </tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Automatic numbering')); ?></strong></td>
                <td class="left"><?php
			if (empty($survey["auto_num"])) {
				$survey["auto_num"]='Y';
			}
			echo(mkselect('auto_num',array('Y'=>_('Yes'),'N'=>_('No')), $survey) . '<span class="red"><em>'. _('Required') .'</em></span><br />' .
            		_('Select if questions are to be numbered automatically when showing the survey.')); ?>
		</td>
        </tr>
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Survey opening date')); ?></strong></td>
        <td class="left"><?php echo
			mktext('open_date', 16, null, $survey) . '<br />' .
            _('The <em>earliest</em> date and time respondents may respond to survey.') . '<br />' .
            _('If date given without a time, midnight on the given date is assumed.') . '<br />' .
            _('If not supplied, respondents may respond immediately once survey activated.')
		?></td>
        </tr>
	<tr style="vertical-align: top">
		<td class="right"><strong><?php echo(_('Survey closing date')); ?></strong></td>
        <td class="left"><?php echo
			mktext('close_date', 16, null, $survey) . '<br />' .
            _('When provided, the <em>latest</em> date and time respondents may respond to survey.') . '<br />' .
            _('If date given without a time, midnight on the given date is assumed.') . '<br />' .
            _('If not supplied, respondents may respond indefinitely until survey marked as done.')
		?></td>
        </tr>
<!--
	<tr><td colspan="2"><hr class="line1" /></td></tr>
	<tr style="vertical-align: top">
        <td><strong><?php echo(_('Options')); ?></strong></td>
		<td><?php echo(mkcheckbox('options','resume') . '&nbsp;' .
				_('Allow survey respondents to save/resume. (Survey login required.)')); ?></td>
	</tr>
	<tr style="vertical-align: top">
		<td>&nbsp;</td>
		<td><?php echo(mkcheckbox('options','navigate') . '&nbsp;' .
				_('Allow survey respondents to go forward and back between survey sections.')); ?></td>
-->                
</table>
