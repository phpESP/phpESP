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
		$sql = "SELECT status,owner,realm FROM survey WHERE id=${sid}";
		$result = mysql_query($sql);
		list($status,$owner,$realm) = mysql_fetch_row($result);
		mysql_free_result($result);
		
		/* check ACL to see if user is allowed to test
		 * _this_ survey */
		$realms = array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']);
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				$owner != $HTTP_SESSION_VARS['acl']['username'] &&
				!in_array($realm, $realms) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}
		if (($status & STATUS_TEST) && 
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
			define('AUTHHAND-PREFIX', TRUE);
			define('AUTHHAND-OK', TRUE);
			include($ESPCONFIG['handler']);
			#undefine('AUTHHAND-PREFIX');
			#undefine('AUTHHAND-OK');
?>
</td></tr></table>
<a href="<?php echo($ESPCONFIG['ME']); ?>?where=results&sid=<?php echo($sid); ?>&test=1"><?php echo(_('View Results')); ?></a><br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<form>
<?php
			mysql_free_result($result);
			return;
		}
	}
	
	/* no survey selected ... show a menu to choose from */
	
	/* check with ACL for allowed surveys */
	/* load names and titles of all surveys available to
	 * _this_ user */
	$statusbad = (STATUS_DONE | STATUS_DELETED | STATUS_ACTIVE);
	$statusok = STATUS_TEST;
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm FROM survey 
		WHERE (status & $statusok) AND NOT (status & $statusbad)
		ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']));
		$sql = "SELECT id,name,title,owner,realm
			FROM survey 
			WHERE (status & $statusok) AND NOT (status & $statusbad) AND
			(owner = '". $HTTP_SESSION_VARS['acl']['username'] ."' || realm $realms)
			ORDER BY id DESC";
	}
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
		<th align="left"><?php echo(_('Group')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<th align="left"><?php echo(_('Export')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$owner,$realm) = mysql_fetch_row($result)) {

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
		<td><?php echo($realm); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=${sid}"); ?>"><?php echo(_('Test')); ?></a></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&test=1&sid=${sid}"); ?>"><?php echo(_('Results')); ?></a></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=export&type=csv&test=1&sid=${sid}"); ?>"><?php echo(_('CSV')); ?></a>
		    (<a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=download&type=csv&test=1&sid=${sid}"); ?>"><?php echo(_('download')); ?></a>)
			<!-- / <a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=export&type=dbf&test=1&sid=${sid}"); ?>"><?php echo(_('DBF')); ?></a> -->
		</td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
