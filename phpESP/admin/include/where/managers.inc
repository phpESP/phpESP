<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>
// <jflemer@acm.rpi.edu>

	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
		$HTTP_SESSION_VARS['acl']['managers'] != 'Y' &&
		!auth_no_access('to manage designers')) {
		return;
	}

	$section = 'manager';

	$sql = "SELECT * FROM $section";
	$result = mysql_query($sql);
	
	$bg = $ESPCONFIG['bgalt_color1'];
?>
<h2><?php
	echo(_('User Manager') .': '. _('Designers'));
?></h2>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="<?php echo($bg); ?>">
		<th><?php echo(_('Username')); ?></th>
		<th><?php echo(_('Password')); ?></th>
		<th><?php echo(_('First Name')); ?></th>
		<th><?php echo(_('Last Name')); ?></th>
		<th><?php echo(_('Email')); ?></th>
		<th><?php echo(_('Realm')); ?></th>
	</tr>
<?php
	while($arr = mysql_fetch_array($result, MYSQL_ASSOC)) {
		foreach (array(
			'username','firstname','lastname','email','realm')
			as $col) {	if (empty($arr[$col])) { $arr[$col] = '&nbsp;'; } }
		$arr['password'] = '******';
		
		if ($bg == $ESPCONFIG['bgalt_color1'])
			$bg =& $ESPCONFIG['bgalt_color2'];
		else
			$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($arr['username']);  ?></td>
		<td><?php echo($arr['password']);  ?></td>
		<td><?php echo($arr['firstname']); ?></td>
		<td><?php echo($arr['lastname']);  ?></td>
		<td><?php echo($arr['email']);     ?></td>
		<td><?php echo($arr['realm']);     ?></td>
	</tr>
<?php	}	?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
