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
    $username = $HTTP_SESSION_VARS['acl']['username'];

	if ($sid) {
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
            $sql = "SELECT name, title, owner, public
                    FROM survey WHERE id=$sid";
		} else {
            $sql = "SELECT name, title, owner, public
                    FROM survey WHERE id=$sid AND owner='$username'";
		}
		$result = mysql_query($sql);
		if (mysql_num_rows($result) < 1) {
			$sid = 0;
		}
	}
	if ($sid) {
		list($name,$title,$owner,$public) = mysql_fetch_row($result);
		mysql_free_result($result);
		
		if (!empty($HTTP_POST_VARS['op']))
			$op = $HTTP_POST_VARS['op'];
		else
			$op = $HTTP_GET_VARS['op'];

		if ($op == 'a') {
            $login = $HTTP_POST_VARS['uname'];
            $survey_name = "phpESP_" . $sid;
            $password = $HTTP_POST_VARS['passwd'];
            $password2 = $HTTP_POST_VARS['passwd2'];
            if ($password != $password2)
                $errstr = "The passwords do not match, try again.";
            else {


                $cryptpw = crypt($password, substr($password, 0, 2));
    			$sql = "INSERT INTO users (login, survey_name, password, encrypt) 
                        VALUES ('$login', '$survey_name', '$cryptpw', 'y')";
                mysql_query($sql, $GLOBALS['ESPCONFIG']['auth_db_conn']);
            }
		} elseif ($op == 'r') {
            $login = $HTTP_GET_VARS['uname'];
            $survey_name = "phpESP_" . $sid;
			$sql = "DELETE FROM users 
                    WHERE login='$login' AND survey_name='$survey_name'";
            mysql_query($sql, $GLOBALS['ESPCONFIG']['auth_db_conn']);
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

		$groups = array();
	}
?>
<h2><?php echo(_('Survey Access')); ?></h2>

<?php if(!empty($errstr)) echo("<p><font color=white>$errstr</font></p>\n"); ?>

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
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('ID')); ?></th><td><?php echo($sid); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Name')); ?></th><td><?php echo($name); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Title')); ?></th><td><?php echo($title); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Owner')); ?></th><td><?php echo($owner); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><th align="left"><?php echo(_('Access')); ?></th><td><?php echo($public); ?></td><td>&nbsp;</td></tr>
	<tr bgcolor="#dddddd"><td colspan="3"><hr></td></tr>
<?php
echo '<tr><td>Authorized users:</td></tr>';
$sql1 = "SELECT login FROM users 
        WHERE survey_name='" . "phpESP_" . $sid . "'";
$result = mysql_query($sql1, $GLOBALS['ESPCONFIG']['auth_db_conn']);
?>
    <tr><td><b>Username</b></td><td><b>Password</b></td></tr>

<?
while (list($user) = mysql_fetch_row($result) ) { 
    echo "<tr><td>$user</td><td><i>hidden</i></td>";
?>
    <td><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid&op=r&uname=" . urlencode($user)); ?>">Remove</a></td></tr>
<?
}
?>
    <tr><td><input name=uname></td><td><input type=password name=passwd>&nbsp;<input type=password name=passwd2></td>
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
		<th align="left"><?php echo(_('Access')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
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
			$HTTP_SESSION_VARS['acl']['username'] .
            //"' || realm $realms".
            "') ORDER BY id DESC";
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
