<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>
// <jflemer@acm.rpi.edu>

	/* load only accounts available to _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
			!auth_no_access(_('to access this form'))) {
		return;
	}
	
	$errstr = '';
	if(!empty($HTTP_POST_VARS['r']) && !empty($HTTP_POST_VARS['t'])) {
        $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['realm_table']." (name, title, changed)
        VALUES (". _addslashes($HTTP_POST_VARS['r']) .", ".
        _addslashes($HTTP_POST_VARS['t']).", ". sys_time_stamp().")";
		if(!execute_sql($sql))
			$errstr = _('Error adding group.') . ' (' . ErrorMsg() .')';
	}
	if(!empty($HTTP_GET_VARS['del'])) {
		if($HTTP_GET_VARS['del'] == 'superuser') {
			$errstr = _('Error deleting group.');
		} else {
			$sql = "SELECT count(d.username) FROM ".$GLOBALS['ESPCONFIG']['designer_table']." d WHERE d.realm=". _addslashes($HTTP_GET_VARS['del']);
			$count = get_one($sql);
			if ($count > 0) {
				$errstr = _('Group is not empty.');
			} else {
				$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['realm_table']." WHERE name=". _addslashes($HTTP_GET_VARS['del']);
				if(!execute_sql($sql))
					$errstr = _('Error deleting group.');
			}
		}
	}
	$sql = 'SELECT r.name, r.title, count(d.username) FROM '.$GLOBALS['ESPCONFIG']['realm_table'].' r LEFT JOIN '.$GLOBALS['ESPCONFIG']['designer_table'].' d ON r.name=d.realm GROUP BY r.name, r.title ORDER BY r.name';
	$sql2 = 'SELECT count(d.username) FROM '.$GLOBALS['ESPCONFIG']['realm_table'].' r LEFT JOIN '.$GLOBALS['ESPCONFIG']['respondent_table'].' d ON r.name=d.realm GROUP BY r.name ORDER BY r.name';
	$result = execute_sql($sql);
	$result2 = execute_sql($sql2);
	
	$i = 0;
	$bg = $ESPCONFIG['bgalt_color2'];
?>
<h2><?php echo(_('Manage Groups')); ?></h2>
<?php if(!empty($errstr)) echo('<p>'. mkerror($errstr) ."</p>\n"); ?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<input type="hidden" name="where" value="groups">
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="<?php echo($bg); ?>">
		<th align="left"><?php echo(_('Group')); ?></th>
		<th align="left"><?php echo(_('Description')); ?></th>
		<th align="left"><?php echo(_('Members')); ?></th>
		<td>&nbsp;</td>
	</tr>
<?php
	while(list($realm, $title, $count) = fetch_row($result)) {
        $result->MoveNext();
        $result2->Move($i++);
		$count += $result2->fields[0];

		if ($bg == $ESPCONFIG['bgalt_color1'])
			$bg =& $ESPCONFIG['bgalt_color2'];
		else
			$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($realm);  ?></td>
		<td><?php echo($title);  ?></td>
		<td><?php echo($count);  ?></td>
		<td><?php echo ($count ?
			"&nbsp;" :
			"<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=groups&del=$realm\">". _('Delete') ."</a>"); ?></td>
	</tr>
<?php
	}
	db_close($result);
	db_close($result2);
	
	if ($bg == $ESPCONFIG['bgalt_color1'])
		$bg =& $ESPCONFIG['bgalt_color2'];
	else
		$bg =& $ESPCONFIG['bgalt_color1'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><input type="text" size="16" maxlength="16" name="r"></td>
		<td><input type="text" size="32" maxlength="64" name="t"></td>
		<td>&nbsp;</td>
		<td><input type="submit" name="add" value="<?php echo(_('Add')); ?>"</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
