<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	/* check user's ACL to see if they have rights to
	 * edit/create _any_ survey */
	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' && 
		$HTTP_SESSION_VARS['acl']['screate'] != 'Y') {
		if(!auth_no_access(_('to create or edit surveys')))
			return;
	}
	
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
		$HTTP_SESSION_VARS['acl']['seeall'] == 'Y') {
		$sql = 'SELECT id,name,title,owner FROM survey WHERE status=0 ORDER BY id DESC';
	} else {
		$sql = "SELECT id,name,title,owner FROM survey WHERE status=0 AND owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."' ORDER BY id DESC";
	}
	$result = mysql_query($sql);

?>
<h2><?php echo(_('Edit a Survey')); ?></h2>
<?php echo(_('Pick Survey to Edit')); ?>
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
			<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=tab&newid=${sid}"); ?>">
				<?php echo($name); ?>
			</a>
		</td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
