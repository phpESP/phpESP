<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// see if a survey has been selected
	if(!empty($HTTP_GET_VARS['sid'])) {
		$sid = intval($HTTP_GET_VARS['sid']);
		
		if($HTTP_GET_VARS['test']) {
			echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = $sid)</p>\n");
		}

		/* check ACLs for permissions */
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
				auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) ||
				($HTTP_GET_VARS['test'] && 
					$HTTP_SESSION_VARS['acl']['seeall'] == 'Y') ||
				auth_no_access('to access this survey')) {
?>
<table bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" border="1" width="95%"><tr><td>
<?php
			$precision = '';
			if(!empty($HTTP_GET_VARS['precision']))
				$precision = intval($HTTP_GET_VARS['precision']);
			$totals = '';
			if(!empty($HTTP_GET_VARS['totals']))
				$totals = intval($HTTP_GET_VARS['totals']);
			$qid = '';
			if(!empty($HTTP_GET_VARS['qid']))
				$qid = intval($HTTP_GET_VARS['qid']);
			$ret = survey_results($sid, $precision, $totals, $qid,
				XADDSLASHES($HTTP_GET_VARS['cids']));
			if(!empty($ret))
				echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php
			if($HTTP_GET_VARS['test']) {
				echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=". $HTTP_GET_VARS['sid'] ."\">". _('Test Survey') ."</a><br>\n");
			}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		}
		return;
	}		

// No SurveyId specified, so build a table of
// surveys to choose from ...

	/* check with ACL for allowed surveys */
	$statusok = (STATUS_ACTIVE | STATUS_DONE & ~STATUS_DELETED);
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
		$HTTP_SESSION_VARS['acl']['seeall'] == 'Y')
		$sql = "SELECT id,name,title,owner FROM survey 
			WHERE (status & $statusok)
			ORDER BY id DESC";
	else
		$sql = "SELECT id,name,title,owner FROM survey WHERE owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."' 
			AND (status & $statusok)
			ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Survey Results')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$owner) = mysql_fetch_row($result)) {

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
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
