<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// see if a survey has been selected
	if(!empty($HTTP_GET_VARS['sid'])) {
		if($HTTP_GET_VARS['test']) {
			echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = ". $HTTP_GET_VARS['sid'] .")</p>\n");
		}
		
?>
<table bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" border="1" width="95%"><tr><td>
<?php
		$ret = survey_results($HTTP_GET_VARS['sid'],
			$HTTP_GET_VARS['precision'],
			$HTTP_GET_VARS['totals'],
			$HTTP_GET_VARS['qid'],
			$HTTP_GET_VARS['cids']);
		if(!empty($ret))
			echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php
		if($test) {
			echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=". $HTTP_GET_VARS['sid'] ."\">". _('Test Survey') ."</a><br>\n");
		}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$sql = 'SELECT id,name,title,status FROM survey ORDER BY id DESC';
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Survey Results')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
	</tr>
<?php
	while(list($sid,$name,$title,$status) = mysql_fetch_row($result)) {
		// only include active or done surveys viewing!
		// this should be done in SQL
		if($status & (STATUS_ACTIVE | STATUS_DONE & ~STATUS_DELETED)) {
			if($bg != $ESPCONFIG['bgalt_color1'])
				$bg = $ESPCONFIG['bgalt_color1'];
			else
				$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td>
			<a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&sid=${sid}"); ?>">
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
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
