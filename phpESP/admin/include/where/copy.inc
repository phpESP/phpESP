<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	/* if survey has been selected ... */
	if(!empty($HTTP_GET_VARS['sid'])) {
		$sid = intval($HTTP_GET_VARS['sid']);
		
		$srealm = auth_get_survey_realm($sid);
		
		/* check ACL to see if user is allowed to copy
		 * _this_ survey */
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$HTTP_SESSION_VARS['acl']['pdesign'],
						$HTTP_SESSION_VARS['acl']['pall'])) &&
				!auth_no_access(_('to access this survey'))) {
			return;
		}

		/* copy the survey */
		if(!survey_copy($sid)) {
			echo(mkerror(_('Error copying survey.') ." (". mysql_error() .")") . "<br>\n");
			echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
			return;
		}
?>
<script language="JavaScript"><!--
window.location="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=manage"); ?>"
//-->
</script>
<?php
		echo("<noscript><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a></noscript>\n");
		return;
	}

	/* load names and titles of all surveys available to
	 * _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,status,owner,realm FROM survey ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdesign']));
		$sql = "SELECT id,name,title,status,owner,realm
			FROM survey WHERE NOT (status & ". STATUS_DELETED .") AND (owner = '".
			$HTTP_SESSION_VARS['acl']['username'] .
            //"' || realm $realms" .
            "') ORDER BY id DESC";
	}
	$result = mysql_query($sql);

?>
<h2><?php echo(_('Copy Survey')); ?></h2>
<?php echo(_('
Choose a survey of which to make a copy. The copy will have the same
status of a newly created survey. You will be able to edit the
survey, and will have to activate it before use.')); ?>
<table border="0" cellpadding="4" cellspacing="0" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Status')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Group')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$status,$owner,$realm) = mysql_fetch_row($result)) {
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
		} elseif($status & STATUS_DONE) {
			$stat = -('Ended');
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
		} else {
			$stat = _('Editing');
		}
		
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=copy&sid=${sid}"); ?>"><?php echo($name); ?></a></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($stat); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
