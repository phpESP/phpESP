<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$sid = intval($HTTP_GET_VARS['sid']);

	/* operation selected ... */
	if(!empty($HTTP_GET_VARS['op'])) {
		$sql = "SELECT status,owner,realm FROM survey WHERE id='${sid}'";
		$result = mysql_query($sql);
		if((list($old_status, $owner, $realm) = mysql_fetch_row($result))) {
			$access = FALSE;
			$err = FALSE;
			$status = $old_status;
			// trying to perform some operation
			switch(strtolower($HTTP_GET_VARS['op'])) {
				case 'c':	// Clear
					/* only _superuser_s can do this */
					if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
						$access = TRUE;
					} else {
						$access = FALSE;
					}
					$status = 0;
					break;
				case 't':	// test
					/* only the owner or a group editor+design */
					if($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, array_intersect(
							$HTTP_SESSION_VARS['acl']['pall'],
							$HTTP_SESSION_VARS['acl']['pdesign']))) {
						$access = TRUE;
					}
					$status = $old_status | STATUS_TEST;
					if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
						$err = TRUE;
					break;
				case 'm':	// Edit
					/* only the owner or a group editor+design */
					if($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, array_intersect(
							$HTTP_SESSION_VARS['acl']['pall'],
							$HTTP_SESSION_VARS['acl']['pdesign']))) {
						$access = TRUE;
					}
					$status = $old_status & ~STATUS_TEST;
					if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
						$err = TRUE;
					else
						clear_results($sid);
					break;
				case 'a':	// activate
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $HTTP_SESSION_VARS['acl']['pstatus']) &&
						($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, $HTTP_SESSION_VARS['acl']['pall']))) {
						$access = TRUE;
					}
					$status = $old_status | STATUS_ACTIVE;
					if($old_status & ( STATUS_DELETED | STATUS_DONE ) )
						$err = TRUE;
					else
						clear_results($sid);
					break;
				case 'e':	// End
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $HTTP_SESSION_VARS['acl']['pstatus']) &&
						($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, $HTTP_SESSION_VARS['acl']['pall']))) {
						$access = TRUE;
					}
					$status = $old_status | STATUS_DONE;
					if($old_status & STATUS_DELETED )
						$err = TRUE;
					break;
				case 'd':	// Delete
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $HTTP_SESSION_VARS['acl']['pstatus']) &&
						($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, $HTTP_SESSION_VARS['acl']['pall']))) {
						$access = TRUE;
					}
					$status = $old_status | STATUS_DELETED;
					break;
			}
			/* superuser overrides all */
			if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y')
				$access = TRUE;
			$sql = "UPDATE survey SET status='${status}' WHERE id='${sid}'";
			if($access || auth_no_access(_('to access this survey'))) {
				if(!$err) {
					mysql_query($sql);
				} else {
					mkwarn(_('Can not set survey status.'));
					mkerror(_('STATUS') .': '. $old_status);
				}
			}
		}
	}
?>
<h2><?php echo(_('Survey Status')); ?></h2>
<div align="left">

<ul>
<li><?php echo(_('<b>Test</b> transitions a survey into testing mode. At which point you may
perform a live test by taking the survey, and viewing the results. You will not
be able to make any further changes to the survey once you have switched to
test mode.')); ?></li>

<li><?php echo(_('<b>Activate</b> transitions a survey into active mode. In this mode the
survey is open for production use, and may be put online. This will clear any
results from testing mode (if any). No further editing of survey is
allowed.')); ?></li>

<li><?php echo(_('<b>End</b> transitions a survey into ended mode. In this mode, no edits are
possible, no users may take the survey (it is inactive), but results are still
viewable from the results menu.')); ?></li>

<li><?php echo(_('<b>Archive</b> removes this survey. It is still stored in the database, but
no further interaction is allowed. You may <b>not</b> view the results of an
archived survey.')); ?></li>
</ul>

</div>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Group')); ?></th>
		<th align="left"><?php echo(_('Status')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,status,owner,realm FROM survey ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				array_merge(
					$HTTP_SESSION_VARS['acl']['pall'],
					$HTTP_SESSION_VARS['acl']['pdesign'])));
		$sql = "SELECT id,name,title,status,owner,realm
			FROM survey WHERE (owner = '".
			$HTTP_SESSION_VARS['acl']['username'] ."' || realm $realms) ORDER BY id DESC";
	}
	$result = mysql_query($sql);

	$realms = array_intersect(
			$HTTP_SESSION_VARS['acl']['pstatus'],
			array_merge(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pstatus']));

	while(list($sid,$name,$title,$status,$owner,$realm) = mysql_fetch_row($result)) {
		$stat = _('Editing');
		$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=t&sid=${sid}\">". _('Test') ."</a>";
		$act  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=a&sid=${sid}\">". _('Activate') ."</a>";
		$done = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=e&sid=${sid}\">". _('End') ."</a>";
		$del  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=d&sid=${sid}\">". _('Archive') ."</a>";
		
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
			$test = $act = $done = $del = '&nbsp;';
			continue;
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
			$test = $act = $done = '&nbsp;';
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
			$test = $act = '&nbsp;';
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
			$done = '&nbsp;';
			$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=m&sid=${sid}\">". _('Edit') ."</a>";
		} else {
			$done = '&nbsp;';
		}
		
		/* whack things back to permissions set by ACL 
		 * for everyone _not_ superuser */
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!in_array($realm, $realms)) {
			$act  = '&nbsp;';
			$done = '&nbsp;';
			$del  = '&nbsp;';
		}
				
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($stat); ?></td>
		<td><?php echo($test); ?></td>
		<td><?php echo($act); ?></td>
		<td><?php echo($done); ?></td>
		<td><?php echo($del); ?></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
