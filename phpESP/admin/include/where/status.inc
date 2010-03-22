<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	if(!empty($_GET['sid']))
		$sid = intval($_GET['sid']);

	$bg = '';

	/* operation selected ... */
	if(!empty($_GET['op'])) {
		$sql = "SELECT status,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
		$result = execute_sql($sql);
		if((list($old_status, $owner, $realm) = fetch_row($result))) {
			$result->MoveNext();
			$access = false;
			$err = false;
			$status = $old_status;
			// trying to perform some operation
			switch(strtolower($_GET['op'])) {
				case 'c':	// Clear
					/* only _superuser_s can do this */
					if($_SESSION['acl']['superuser'] == 'Y') {
						$access = true;
					} else {
						$access = false;
					}
					$status = 0;
					break;
				case 't':	// test
					/* only the owner or a group editor+design */
					if($owner == $_SESSION['acl']['username'] ||
						in_array($realm, array_intersect(
							$_SESSION['acl']['pall'],
							$_SESSION['acl']['pdesign']))) {
						$access = true;
					}
					$status = STATUS_TEST;
					if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
						$err = true;
					break;
				case 'm':	// Edit
					/* only the owner or a group editor+design */
					if($owner == $_SESSION['acl']['username'] ||
						in_array($realm, array_intersect(
							$_SESSION['acl']['pall'],
							$_SESSION['acl']['pdesign']))) {
						$access = true;
					}
					$status = STATUS_EDIT;
					if($old_status & ( STATUS_DELETED | STATUS_DONE | STATUS_ACTIVE ) )
						$err = true;
					else
						response_delete_all($sid);
					break;
				case 'a':	// activate
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $_SESSION['acl']['pstatus']) &&
						($owner == $_SESSION['acl']['username'] ||
						in_array($realm, $_SESSION['acl']['pall']))) {
						$access = true;
					}
					$status = STATUS_ACTIVE;
                    // here we also check that we don't activate twice, because
                    // it would delete all current responses ...
					if($old_status & ( STATUS_ACTIVE | STATUS_DELETED | STATUS_DONE ) )
						$err = true;
					else
						response_delete_all($sid);
					break;
				case 'e':	// End
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $_SESSION['acl']['pstatus']) &&
						($owner == $_SESSION['acl']['username'] ||
						in_array($realm, $_SESSION['acl']['pall']))) {
						$access = true;
					}
					$status = STATUS_DONE;
					if($old_status & (STATUS_DONE | STATUS_DELETED) )
						$err = true;
					break;
				case 'd':	// Delete
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $_SESSION['acl']['pstatus']) &&
						($owner == $_SESSION['acl']['username'] ||
						in_array($realm, $_SESSION['acl']['pall']))) {
						$access = true;
					}
					$status = STATUS_DELETED;
					if($old_status & STATUS_DELETED)
						$err = true;
					break;
			}
			/* superuser overrides all */
			if($_SESSION['acl']['superuser'] == 'Y')
				$access = true;
			if($access || auth_no_access(_('to access this survey'))) {
				if(!$err) {
					$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['survey_table']." SET status=$status WHERE id=${sid}";
					execute_sql($sql);
				} else {
					print mkwarn(_('Can not set survey status.'));
					print mkerror(_('Status') .': '. $old_status);
				}
			}
		}
	}
?>
<h2><?php echo(_('Survey Status')); ?></h2>
<div style="text-align: left">

<ul>
<li><?php echo(_('<b>Test</b> transitions a survey into testing mode. At which point you may
perform a live test by taking the survey, and viewing the results. The survey may be switched 
back into <b>Edit</b> mode if additional edits are needed.')); ?></li>

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
<table cellspacing="0" cellpadding="4">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
		<th><?php echo(_('Status')); ?></th>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = 'SELECT id,name,title,status,owner,realm FROM '.$GLOBALS['ESPCONFIG']['survey_table'].' ORDER BY id DESC';
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				array_merge(
					$_SESSION['acl']['pall'],
					$_SESSION['acl']['pdesign'])));
		$sql = "SELECT id,name,title,status,owner,realm
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE (owner = ".
			_addslashes($_SESSION['acl']['username']) ." || realm $realms) ORDER BY id DESC";
	}
	$result = execute_sql($sql);

	$realms = array_intersect(
			$_SESSION['acl']['pstatus'],
			array_merge(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['pstatus']));

	while(list($sid,$name,$title,$status,$owner,$realm) = fetch_row($result)) {
        $result->MoveNext();
		$stat = _('Editing');
		$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=status&op=t&sid=${sid}")."\">". _('Test') ."</a>";
		$act  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=status&op=a&sid=${sid}")."\" onclick='return activateConfirm()'>". _('Activate') ."</a>";
		$done = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=status&op=e&sid=${sid}")."\">". _('End') ."</a>";
		$del  = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=status&op=d&sid=${sid}")."\">". _('Archive') ."</a>";

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
			$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=status&op=m&sid=${sid}")."\">". _('Edit') ."</a>";
		} else {
			$done = '&nbsp;';
		}

		/* whack things back to permissions set by ACL
		 * for everyone _not_ superuser */
		if($_SESSION['acl']['superuser'] != 'Y' &&
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
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
        <?php 
        if ($stat == 'Active') {?>
            <td><a href="<?php print $ESPCONFIG['autopub_url']."?sid=".$sid; ?>"><?php echo($name); ?></a></td>
        <?php
	} elseif ($stat == 'Testing') { ?>
	   <td><a href="<?php print $ESPCONFIG['autopub_url']."?sid=".$sid."&test=1"; ?>"><?php echo($name); ?></a></td>
        <?php
	} elseif ($stat == 'Editing') { ?>
	   <td><a href="<?php print $ESPCONFIG['ME'].htmlentities("?where=tab&newid=${sid}"); ?>"><?php echo($name); ?></a></td>
	<?php
        } else { ?>
		<td><?php echo($name); ?></td>
        <?php } ?>
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
