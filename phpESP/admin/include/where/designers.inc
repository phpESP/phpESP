<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>
// <jflemer@acm.rpi.edu>

    /* SuperUser or Designers with "Administer Designers(pgroup)" permissions only*/
    if(($_SESSION['acl']['superuser'] != 'Y') &&
        count($_SESSION['acl']['pgroup']) == 0 &&
        !auth_no_access(_('to access this form'))) {
        return;
    }
    
	$sql = 'SELECT username, fname, lname, realm, disabled, expiration FROM '.$GLOBALS['ESPCONFIG']['designer_table'];

	/* load only accounts available to _this_ user */
	if($_SESSION['acl']['superuser'] != 'Y') {
		$realms = array_to_insql(
			array_merge(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['puser']));
		$sql .= " WHERE realm $realms";
	}

	$sql .= ' ORDER BY ';
	$base = $GLOBALS['ESPCONFIG']['ME'] .'?where=designers';
	if(!isset($_GET['u'])) $_GET['u'] = '';
	if(!isset($_GET['g'])) $_GET['g'] = '';
	if(!isset($_GET['f'])) $_GET['f'] = '';
	if(!isset($_GET['l'])) $_GET['l'] = '';
	if(!isset($_GET['s'])) $_GET['s'] = '';

	if($_GET['u'] == 'd') {
		$us = 'username DESC';
		$u = 'a';
	} else {
		$_GET['u'] = 'a';
		$us = 'username ASC';
		$u = 'd';
	}
	if($_GET['g'] == 'd') {
		$gs = 'realm DESC';
		$g = 'a';
	} else {
		$_GET['g'] = 'a';
		$gs = 'realm ASC';
		$g = 'd';
	}
	if($_GET['f'] == 'd' || $_GET['l'] == 'd') {
		$ls = 'lname DESC';
		$l = 'a';
		$fs = 'fname DESC';
		$f = 'a';
	} else {
		$_GET['l'] = 'a';
		$ls = 'lname ASC';
		$l = 'd';
		$_GET['f'] = 'a';
		$fs = 'fname ASC';
		$f = 'd';
	}
	if($_GET['s'] == 'g') {
		$sql .= "$gs, $us";
		$u = "&s=u&g=" . $_GET['g'] ."&u=". $_GET['u'];
		$g = "&s=g&g=$g&u=". $_GET['u'];
		$f = "&s=f";
		$l = "&s=l";
	} elseif($_GET['s'] == 'f' || $_GET['s'] == 'l') {
		$sql .= "$fs, $ls";
		$f = "&s=f&f=$f";
		$l = "&s=l&l=$l";
		$u = "&s=u";
		$g = "&s=g";
	} else {
		$sql .= "$us, $gs";
		$u = "&s=u&g=" . $_GET['g'] ."&u=$u";
		$g = "&s=g&g=" . $_GET['g'] ."&u=". $_GET['u'];
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
