<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	/* get the sid ... if any */
	$sid = intval($HTTP_GET_VARS['sid']);
	if(!empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);

	/* sid selected for testing */
	if(!empty($sid)) {
		/* check status and ACL before testing */
		$sql = "SELECT status,owner FROM survey WHERE id=${sid}";
		$result = mysql_query($sql);
		if((list($status,$owner) = mysql_fetch_row($result)) &&
			($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
				$HTTP_SESSION_VARS['acl']['seeall'] == 'Y' ||
				$HTTP_SESSION_VARS['acl']['username'] == $owner ||
				auth_no_access('to access this survey')) &&
			($status & STATUS_TEST) && 
			!($status & (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE))) {
			/* cleared for testing ... was that enough tests? */
?>
</form>
<b><?php echo(_('Testing Survey...')); ?></b> (<?php echo(_('SID')." = $sid"); ?>)
<p>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
			/* set things up for the handler to take over ... */
			$test = TRUE;
			unset($HTTP_GET_VARS['sid']);
			unset($HTTP_POST_VARS['sid']);
			$HTTP_POST_VARS['userid'] = 'test';
			$tmp = $ESPCONFIG['ME'];
			$HTTP_SERVER_VARS['PHP_SELF'] = $ESPCONFIG['ME'].'?where=test';
			include($ESPCONFIG['handler']);
			/* restore stuff after the handler does it's job */
			$ESPCONFIG['ME'] = $tmp;
?>
</td></tr></table>
<a href="<?php echo($ESPCONFIG['ME']); ?>?where=results&sid=<?php echo($sid); ?>&test=1"><?php echo(_('View Results')); ?></a><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<form>
<?php
		}
		mysql_free_result($result);
		return;
	}
	
	/* no survey selected ... show a menu to choose from */
	
	/* check with ACL for allowed surveys */
	$statusbad = (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE);
	$statusok = STATUS_TEST;
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
		$HTTP_SESSION_VARS['acl']['seeall'] == 'Y')
		$sql = "SELECT id,name,title,owner FROM survey 
			WHERE (status & $statusok) AND NOT (status & $statusbad)
			ORDER BY id DESC";
	else
		$sql = "SELECT id,name,title,owner FROM survey WHERE owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."' 
			AND (status & $statusok) AND NOT (status & $statusbad)
			ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<h2><?php echo(_('Test a Survey')); ?></h2>
<?php echo(_('Pick Survey to Test')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
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
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=${sid}"); ?>"><?php echo(_('Test')); ?></a></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&test=1&sid=${sid}"); ?>"><?php echo(_('Results')); ?></a></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
