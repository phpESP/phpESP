<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	if (!empty($HTTP_POST_VARS['sid']))
		$sid = intval($HTTP_POST_VARS['sid']);
	elseif (!empty($HTTP_GET_VARS['sid']))
		$sid = intval($HTTP_GET_VARS['sid']);
	else
		$sid = '';

	$errstr = "";
	$bg = '';

	if ($sid) {
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
			$sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM survey s WHERE s.id = '$sid'";
			$sql1 = "SELECT a.realm, a.maxlogin, a.resume, a.navigate FROM access a
				WHERE a.survey_id = '$sid' ORDER BY a.realm";
		} else {
			$realms = array_to_insql(
				array_intersect(
					$HTTP_SESSION_VARS['acl']['pall'],
					array_merge(
						$HTTP_SESSION_VARS['acl']['pall'],
						$HTTP_SESSION_VARS['acl']['pdesign'])));
			$sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM survey s WHERE s.id = '$sid' AND NOT (status & " .STATUS_DELETED. ") AND (s.owner = '".
				$HTTP_SESSION_VARS['acl']['username'] ."' || s.realm $realms)";
			$sql1 = "SELECT a.realm, a.maxlogin, a.resume, a.navigate FROM access a, survey s
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
		elseif (!empty($HTTP_GET_VARS['op']))
			$op = $HTTP_GET_VARS['op'];
		else
			$op = '';

		if (!empty($HTTP_POST_VARS['realm']))
			$arealm = XADDSLASHES($HTTP_POST_VARS['realm']);
		elseif (!empty($HTTP_GET_VARS['realm']))
			$arealm = XADDSLASHES($HTTP_GET_VARS['realm']);

		if (isset($HTTP_POST_VARS['resume']))
			$resume = 'Y';
		else
			$resume = 'N';
		
		if (isset($HTTP_POST_VARS['navigate']))
			$navigate = 'Y';
		else
			$navigate = 'N';
		
		if (!empty($HTTP_POST_VARS['max']))
			$max = intval($HTTP_POST_VARS['max']);
		elseif (!empty($HTTP_GET_VARS['max']))
			$max = intval($HTTP_GET_VARS['max']);
		else
			$max = 0;

		if ($op == 'a') {
			if (empty($HTTP_POST_VARS['realm'])) {
				$errstr = mkerror(_('Please select a group.'));
			} else {
				$sql = "INSERT INTO access (survey_id, realm, maxlogin, resume, navigate)
						VALUES ('$sid', '$arealm', '$max', '$resume', '$navigate')";
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
	}
?>
<h2><?php echo(_('Survey Access')); ?></h2>

<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>

<div align="left">
<p><?php
	echo(_('This lets you control who has access to fill out a form.
Public surveys let anyone submit data.
Private surveys are restricted by Respondent Groups.')); ?></p>

<p><b><?php echo(_('Note')); ?>:</b>
<?php echo(_('You must use') .
	' <tt>'. substr(strrchr($ESPCONFIG['handler_prefix'], '/'), 1) .
	' &amp; '. substr(strrchr($ESPCONFIG['handler'], '/'), 1) . '</tt> ' .
	_('when using private surveys.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php if ($sid) { ?>
<input type="hidden" name="where" value="access">
<input type="hidden" name="sid" value="<?php echo($sid); ?>">
<input type="hidden" name="op" value="a">
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('ID')); ?></th><td colspan="5"><?php echo($sid); ?></td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Name')); ?></th><td colspan="5"><?php echo($name); ?></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Title')); ?></th><td colspan="5"><?php echo($title); ?></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Owner')); ?></th><td colspan="5"><?php echo($owner); ?></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Group')); ?></th><td colspan="5"><?php echo($realm); ?></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Public')); ?></th><td colspan="5"><?php echo($public); ?></tr>
	<tr bgcolor="#dddddd"><td colspan="6"><hr></td></tr>
	<tr><th align="left"><?php echo(_('Group')); ?></th>
		<th align="left"><?php echo(_('Max Responses')); ?></th>
		<th align="left"><?php echo(_('Save/Restore')); ?></th>
		<th align="left"><?php echo(_('Back/Forward')); ?></th>
		<td>&nbsp;</td></tr>
<?php
		$result = mysql_query($sql1);
		while ( list($arealm, $amax, $aresume, $anavigate) = mysql_fetch_row($result) ) { ?>
	<tr><td><?php echo($arealm); ?></td>
		<td><?php echo($amax); ?></td>
		<td><?php echo(($aresume == 'Y') ? _('Yes') : _('No')); ?></td>
		<td><?php echo(($anavigate == 'Y') ? _('Yes') : _('No')); ?></td>
		<td><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid&op=r&realm=" . urlencode($arealm)); ?>">
		<?php echo(_('Remove')); ?></a></td></tr>
<?php	} ?>
	<tr><td><?php echo($r); ?></td>
		<td><input type="text" name="max" size="5"></td>
		<td><input type="checkbox" name="resume" /></td>
		<td><input type="checkbox" name="navigate" /></td>
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
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,owner,realm,public FROM survey WHERE NOT (status & ' .STATUS_DELETED. ') ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				array_merge(
					$HTTP_SESSION_VARS['acl']['pall'],
					$HTTP_SESSION_VARS['acl']['pdesign'])));
		$sql = "SELECT id,name,title,owner,realm,public
			FROM survey WHERE NOT (status & " .STATUS_DELETED. ") AND (owner = '".
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
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
