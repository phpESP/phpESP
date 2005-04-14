<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>
// <jflemer@acm.rpi.edu>

    /* SuperUser or Designers with "Administer Designers(pgroup)" permissions only*/
    if(($HTTP_SESSION_VARS['acl']['superuser'] != 'Y') &&
        count($HTTP_SESSION_VARS['acl']['pgroup']) == 0 &&
        !auth_no_access(_('to access this form'))) {
        return;
    }
    
	$sql = 'SELECT username, fname, lname, realm, disabled, expiration FROM '.$GLOBALS['ESPCONFIG']['designer_table'];

	/* load only accounts available to _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y') {
		$realms = array_to_insql(
			array_merge(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['puser']));
		$sql .= " WHERE realm $realms";
	}

	$sql .= ' ORDER BY ';
	$base = $GLOBALS['ESPCONFIG']['ME'] .'?where=designers';
	if(!isset($HTTP_GET_VARS['u'])) $HTTP_GET_VARS['u'] = '';
	if(!isset($HTTP_GET_VARS['g'])) $HTTP_GET_VARS['g'] = '';
	if(!isset($HTTP_GET_VARS['f'])) $HTTP_GET_VARS['f'] = '';
	if(!isset($HTTP_GET_VARS['l'])) $HTTP_GET_VARS['l'] = '';
	if(!isset($HTTP_GET_VARS['s'])) $HTTP_GET_VARS['s'] = '';

	if($HTTP_GET_VARS['u'] == 'd') {
		$us = 'username DESC';
		$u = 'a';
	} else {
		$HTTP_GET_VARS['u'] = 'a';
		$us = 'username ASC';
		$u = 'd';
	}
	if($HTTP_GET_VARS['g'] == 'd') {
		$gs = 'realm DESC';
		$g = 'a';
	} else {
		$HTTP_GET_VARS['g'] = 'a';
		$gs = 'realm ASC';
		$g = 'd';
	}
	if($HTTP_GET_VARS['f'] == 'd' || $HTTP_GET_VARS['l'] == 'd') {
		$ls = 'lname DESC';
		$l = 'a';
		$fs = 'fname DESC';
		$f = 'a';
	} else {
		$HTTP_GET_VARS['l'] = 'a';
		$ls = 'lname ASC';
		$l = 'd';
		$HTTP_GET_VARS['f'] = 'a';
		$fs = 'fname ASC';
		$f = 'd';
	}
	if($HTTP_GET_VARS['s'] == 'g') {
		$sql .= "$gs, $us";
		$u = "&s=u&g=" . $HTTP_GET_VARS['g'] ."&u=". $HTTP_GET_VARS['u'];
		$g = "&s=g&g=$g&u=". $HTTP_GET_VARS['u'];
		$f = "&s=f";
		$l = "&s=l";
	} elseif($HTTP_GET_VARS['s'] == 'f' || $HTTP_GET_VARS['s'] == 'l') {
		$sql .= "$fs, $ls";
		$f = "&s=f&f=$f";
		$l = "&s=l&l=$l";
		$u = "&s=u";
		$g = "&s=g";
	} else {
		$sql .= "$us, $gs";
		$u = "&s=u&g=" . $HTTP_GET_VARS['g'] ."&u=$u";
		$g = "&s=g&g=" . $HTTP_GET_VARS['g'] ."&u=". $HTTP_GET_VARS['u'];
		$f = "&s=f";
		$l = "&s=l";
	}

	$result = execute_sql($sql);
	
	$bg = $ESPCONFIG['bgalt_color2'];
?>
<h2><?php echo(_('Manage Web Form Designer Accounts')); ?></h2>
<p><?php echo(_('Click on a username to edit, or click on add new user below.')); ?></p>
<table cellspacing="0" cellpadding="4" style="width: 550px;">
	<tr style="background-color: <?php echo($bg); ?>;">
		<th><a href="<?php echo(htmlentities($base . $u)); ?>"><?php echo(_('Username')); ?></a></th>
		<th><a href="<?php echo(htmlentities($base . $f)); ?>"><?php echo(_('First Name')); ?></a></th>
		<th><a href="<?php echo(htmlentities($base . $l)); ?>"><?php echo(_('Last Name')); ?></a></th>
		<th><a href="<?php echo(htmlentities($base . $g)); ?>"><?php echo(_('Group')); ?></a></th>
		<th>&nbsp;</th>
	</tr>
<?php
	while(list($u, $fname, $lname, $r, $d, $e) = fetch_row($result)) {
        $result->MoveNext();
		if($d == 'N')
			$d = '&nbsp;';
		else
			$d = '('. _('disabled') .')';
		
		if (empty($fname)) $fname = '&nbsp;';
		if (empty($lname)) $lname = '&nbsp;';
		
		if ($bg == $ESPCONFIG['bgalt_color1'])
			$bg =& $ESPCONFIG['bgalt_color2'];
		else
			$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<td class="left">
			<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=admdesigner&u=$u&r=$r")); ?>"><?php echo($u); ?></a>
		</td>
		<td class="left"><?php echo($fname); ?></td>
		<td class="left"><?php echo($lname); ?></td>
		<td class="left"><?php echo($r); ?></td>
		<td class="left"><?php echo($d); ?></td>
	</tr>
<?php
	}
	if ($bg == $ESPCONFIG['bgalt_color1'])
		$bg =& $ESPCONFIG['bgalt_color2'];
	else
		$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<th colspan="5" class="center">
			<strong><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=admdesigner")); ?>">
				<?php echo(_('Add a new Designer')); ?></a>
			&nbsp;&nbsp;&nbsp;&nbsp;
			<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=upload&account_type=designers")); ?>">
				<?php echo(_('Bulk Upload Designers')); ?></a></strong>
		</th>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
