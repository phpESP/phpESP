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
<h2><?php echo(_('Manage Designer Accounts')); ?></h2>
<p><?php echo(_('Click on a username to edit, or click on add new user below.')); ?></p>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="<?php echo($bg); ?>">
		<th align="left"><?php echo(_('Username')); ?></th>
		<th align="left"><?php echo(_('Password')); ?></th>
		<th align="left"><?php echo(_('First Name')); ?></th>
		<th align="left"><?php echo(_('Last Name')); ?></th>
		<th align="left"><?php echo(_('Email')); ?></th>
		<th align="left"><?php echo(_('Realm')); ?></th>
		<th align="left">&nbsp;</th>
	</tr>
<?php
	while($arr = mysql_fetch_array($result, MYSQL_ASSOC)) {
		foreach (array(
			'username','firstname','lastname','email','realm')
			as $col) {	if (empty($arr[$col])) { $arr[$col] = '&nbsp;'; } }
		$arr['password'] = '******';
		if($arr['disabled'] == 'N')
			$arr['disabled'] = '&nbsp;';
		else
			$arr['disabled'] = '('. _('disabled') .')';
		
		if ($bg == $ESPCONFIG['bgalt_color1'])
			$bg =& $ESPCONFIG['bgalt_color2'];
		else
			$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td>
			<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=editmanager&u=". $arr['username']); ?>">
				<?php echo($arr['username']);  ?>
			</a>
		</td>
		<td><?php echo($arr['password']);  ?></td>
		<td><?php echo($arr['firstname']); ?></td>
		<td><?php echo($arr['lastname']);  ?></td>
		<td><?php echo($arr['email']);     ?></td>
		<td><?php echo($arr['realm']);     ?></td>
		<td><?php echo($arr['disabled']);  ?></td>
	</tr>
<?php
	}
	if ($bg == $ESPCONFIG['bgalt_color1'])
		$bg =& $ESPCONFIG['bgalt_color2'];
	else
		$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<th colspan="7">
			<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=editmanager"); ?>">
				<?php echo(_('Add a new Designer')); ?>
			</a>
		</th>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
