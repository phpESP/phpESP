<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if (!empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);
	else
		$sid = intval($HTTP_GET_VARS['sid']);

	$errstr = "";

	if ($sid) {
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
			$sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM survey s WHERE s.id = '$sid'";
			$sql1 = "SELECT a.realm, a.maxlogin FROM access a
				WHERE a.survey_id = '$sid' ORDER BY a.realm";
		} else {
			$realms = array_to_insql(
				array_intersect(
					$HTTP_SESSION_VARS['acl']['pall'],
					array_merge(
						$HTTP_SESSION_VARS['acl']['pall'],
						$HTTP_SESSION_VARS['acl']['pdesign'])));
			$sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM survey s WHERE s.id = '$sid' AND (s.owner = '".
				$HTTP_SESSION_VARS['acl']['username'] ."' || s.realm $realms)";
			$sql1 = "SELECT a.realm, a.maxlogin FROM access a, survey s
				WHERE a.survey_id = '$sid' AND s.id=a.survey_id AND (s.owner = '".
				$HTTP_SESSION_VARS['acl']['username'] ."' || s.realm $realms) ORDER BY a.realm";
		}
		$result = mysql_query($sql);
		if (mysql_num_rows($result) < 1) {
			$sid = 0;
		}
	}
	if ($sid) {
		list($name,$title,$owner,$realm,$public) = mysql_fetch_row($result);
		mysql_free_result($result);
		
		if (!empty($HTTP_POST_VARS['op']))
			$op = $HTTP_POST_VARS['op'];
		else
			$op = $HTTP_GET_VARS['op'];

		if (!empty($HTTP_POST_VARS['realm']))
			$arealm = XADDSLASHES($HTTP_POST_VARS['realm']);
		else
			$arealm = XADDSLASHES($HTTP_GET_VARS['realm']);

		if (!empty($HTTP_POST_VARS['max']))
			$max = intval($HTTP_POST_VARS['max']);
		else
			$max = intval($HTTP_GET_VARS['max']);
		
		if ($op == 'a') {
			if (empty($HTTP_POST_VARS['realm'])) {
				$errstr = mkerror(_('Please select a group.'));
			} else {
				if (!$max) $max = 0;
				$sql = "INSERT INTO access (survey_id, realm, maxlogin) VALUES ('$sid', '$arealm', '$max')";
				mysql_query($sql);
			}
		} elseif ($op == 'r') {
			$sql = "DELETE FROM access WHERE survey_id = '$sid' AND realm = '$arealm'";
			mysql_query($sql);
		} elseif ($op == 'v') {
			$sql = "UPDATE survey SET public = 'N' WHERE id = '$sid'";
			mysql_query($sql);
			$sid = 0;
		} elseif ($op == 'p') {
			$sql = "UPDATE survey SET public = 'Y' WHERE id = '$sid'";
			mysql_query($sql);
			$sid = 0;
		}
	}
	if ($sid) {
		if ($public == 'N')
			$public = _('Private');
		else
			$public = _('Public');

		$r = '<select name="realm"><option></option>';
		$groups = array();
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
			$sql = "SELECT name FROM realm";
			$result = mysql_query($sql);
			while( list($g) = mysql_fetch_row($result) ) {
				array_push($groups, $g);
			}
			mysql_free_result($result);
		} else {
			$groups =& $HTTP_SESSION_VARS['acl']['pgroup'];
		}
		foreach($groups as $g) {
			$r .= "<option value=\"$g\">$g</option>";
		}
		$r .= '</select>';
?>
<h2><?php echo(_('Survey Access')); ?></h2>

<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>

<div align="left">
<p><?php
	echo(_('This lets you control who has access to fill out a form.
Public surveys let anyone submit data.
Private surveys are restricted by Respondent Groups.')); ?></p>

<p><?php
	echo(_('NOTE: This does not currently work on embedded forms.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<input type="hidden" name="where" value="access">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="op" value="a">
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('ID')); ?></th><td><?php echo($sid); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Name')); ?></th><td><?php echo($name); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Title')); ?></th><td><?php echo($title); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Owner')); ?></th><td><?php echo($owner); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Group')); ?></th><td><?php echo($realm); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Public')); ?></th><td><?php echo($public); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><td colspan="3"><hr></td></tr>
	<tr><th align="left"><?php echo(_('Group')); ?></th><th align="left"><?php echo(_('Max Logins')); ?></th><td>&nbsp;</td></tr>
<?php
		$result = mysql_query($sql1);
		while ( list($arealm, $amax) = mysql_fetch_row($result) ) { ?>
	<tr><td><?php echo($arealm); ?></td><td><?php echo($amax); ?></td>
		<td><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid&op=r&realm=" . urlencode($arealm)); ?>">Remove</a></td></tr>
<?php	} ?>
	<tr><td><?php echo($r); ?></td>
		<td><input type="text" name="max" size="5"></td>
		<td><input type="submit" value="<?php echo(_('Add')); ?>"></td></tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}
?>
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Group')); ?></th>
		<th align="left"><?php echo(_('Public')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,owner,realm,public FROM survey ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				array_merge(
					$HTTP_SESSION_VARS['acl']['pall'],
					$HTTP_SESSION_VARS['acl']['pdesign'])));
		$sql = "SELECT id,name,title,owner,realm,public
			FROM survey WHERE (owner = '".
			$HTTP_SESSION_VARS['acl']['username'] ."' || realm $realms) ORDER BY id DESC";
	}
	$result = mysql_query($sql);

	while(list($sid,$name,$title,$owner,$realm,$public) = mysql_fetch_row($result)) {				
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
		
		if ($public == 'N') {
			$public = _('Private');
			$op     = '<a href="'. $GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid&op=p\">". _('Make Public') .'</a>';
		} else {
			$public = _('Public');
			$op     = '<a href="'. $GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid&op=v\">". _('Make Private') .'</a>';
		}
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid"); ?>"><?php echo($name); ?></a></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($public); ?></td>
		<td><?php echo($op); ?></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
