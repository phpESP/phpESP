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
				case 'n':	// Notify admin to activate
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $HTTP_SESSION_VARS['acl']['pstatus']) &&
						($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, $HTTP_SESSION_VARS['acl']['pall']))) {
						$access = TRUE;
					}
					$status = $old_status | STATUS_PENDING;
					if($old_status & ( STATUS_DELETED | STATUS_DONE ) ){
						$err = TRUE;
                    } else {
                        // notify admin
                        $admin = $GLOBALS['ESPCONFIG']['admin_email'];
                        mail("$admin", "** activation request ** [survey ".$sid."]",
                                "The owner of survey " . $sid . " has requested that it be activated.\n" .
                                "Please visit http://phpesp.survey.ucsb.edu/admin/index.php?where=status\n" .
                                "to change the status of survey " . $sid .  " from 'Pending' to 'Active'");
						clear_results($sid);
                    }
					break;
				case 'a':	// activate
					/* only the owner+stauts or a group editor+status */
					if(in_array($realm, $HTTP_SESSION_VARS['acl']['pstatus']) &&
						($owner == $HTTP_SESSION_VARS['acl']['username'] ||
						in_array($realm, $HTTP_SESSION_VARS['acl']['pall']))) {
						$access = TRUE;
					}
//					$status = $old_status | STATUS_ACTIVE;
					$status =  STATUS_ACTIVE;
echo "status=" . $status;
					if($old_status & ( STATUS_DELETED | STATUS_DONE ) )
						$err = TRUE;
					else {
                        // notify owner that their survey is being activated, what the url is, etc.
                        $name_sql = "SELECT owner,name FROM survey WHERE id=".$sid;
                        $name_result = mysql_query($name_sql, $GLOBALS['ESPCONFIG']['db_conn']);
                        $name_object = mysql_fetch_object($name_result);
                        $survey_name = $name_object->name;
                        $owner_login = $name_object->owner;

                        $email_sql = "SELECT email FROM admin_users WHERE login='".$owner_login."'";
                        $email_result = mysql_query($email_sql, $GLOBALS['ESPCONFIG']['auth_db_conn']);
                        $email_object = mysql_fetch_object($email_result);
                        $owner_email = $email_object->email;
                        mail ($owner_email, "** survey activated ** [".$survey_name."]",
                                "Your survey [".$survey_name."] has been activated.\n" .
                                "It can be accessed at: \n" .
                                "http://research.survey.ucsb.edu/survey/" . $survey_name . "\n" .
                                "or alternately (for the secure page): \n" .
                                "https://research.survey.ucsb.edu/survey/" . $survey_name . " \n" .
                                "\n" .
                                "Please contact the survey center administrators when you would like your survey deactivated.\n" .
                                "You may collect live data at anytime via the web interface: \n" .
                                "http://phpesp.survey.ucsb.edu/admin/ \n");

						clear_results($sid);
                    }
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
            // modified by stevem: now admins can only edit/test, only superuser can activate, etc..
            // first disallow everything but test and edit, then allow superuser to do stuff..
            // they shouldnt be able to get here (no button for them to do this) but in case
            // they try to get around by rewriting url.
            //
            if (strtolower($HTTP_GET_VARS['op']) == 't' || 
                    strtolower($HTTP_GET_VARS['op']) == 'm' || 
                    strtolower($HTTP_GET_VARS['op']) == 'n' ){
                $access = TRUE;
            }
            else $access = FALSE;
			if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y')
				$access = TRUE;
			$sql = "UPDATE survey SET status='${status}' WHERE id='${sid}'";
			if($access || auth_no_access(_('to change the status of this survey in this manner'))) {
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
<li><?php echo(_('<b>Test</b> - transitions a survey into testing mode. At which point you may
perform a live test by taking the survey, and viewing the results. You will not
be able to make any further changes to the survey once you have switched to
test mode.')); ?></li>
<li><?php echo(_('<b>Edit</b> - transitions a survey into editing mode. At which point you may
edit the survey, adding and organizing questions, etc. You will not be able to test the
survey until it is changed into <b>Test</b> mode.')); ?></li>
<li><?php echo(_('<b>Notify</b> - notifies the adminstrator that this survey is ready to be
published. The owner of the survey will be notified when the survey is active, and where
it will be located (i.e. the URL).')); ?></li>

<?php 
    if ($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
?>
<li><?php echo(_('<b>Activate</b> transitions a survey into active more. In this mode the
survey is open for production use, and may be put online. This will clear any
results from testing mode (if any). No further editing of survey is
allowed.')); ?></li>

<li><?php echo(_('<b>End</b> transitions a survey into ended mode. In this mode, no edits are
possible, no users may take the survey (it is inactive), but results are still
viewable from the results menu.')); ?></li>

<li><?php echo(_('<b>Archive</b> removes this survey. It is still stored in the database, but
no further interaction is allowed. You may <b>not</b> view the results of an
archived survey.')); ?></li>
<?php
    } else {
?>
<li><?php echo(_('In order to <b>Deactivate</b> your survey, please contact the administrators of
the survey center. Note that once a survey is deactivated, it cannot be reactivated.')); ?></li>
<?php
    }
?>
</ul>

</div>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<table border="0" align="center" cellspacing="0" cellpadding="4" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Status')); ?></th>
		<td>&nbsp;</td>
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
			$HTTP_SESSION_VARS['acl']['username'] 
           // ."' || realm $realms
            ."') ORDER BY id DESC"
            ;
	}
	$result = mysql_query($sql);

	$realms = array_intersect(
			$HTTP_SESSION_VARS['acl']['pstatus'],
			array_merge(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pstatus']));

	while(list($sid,$name,$title,$status,$owner,$realm) = mysql_fetch_row($result)) {
		$stat   = _('Editing');
		$test   = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=t&sid=${sid}\">". _('Test') ."</a>";
		$act    = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=a&sid=${sid}\">". _('Activate') ."</a>";
		$done   = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=e&sid=${sid}\">". _('End') ."</a>";
		$del    = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=d&sid=${sid}\">". _('Archive') ."</a>";
		$notify = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=n&sid=${sid}\">". _('Notify Admin') ."</a>";
		
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
			$test = $act = $done = $del = $notify = '&nbsp;';
			continue;
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
			$test = $act = $done = $notify = '&nbsp;';
		} elseif($status & STATUS_PENDING) {
			$stat = _('<b>Pending</b>');
            if ($HTTP_SESSION_VARS['acl']['superuser'] != 'Y'){
    			$test = $act = $notify = '&nbsp;';
            } else {
    			$test = $notify = '&nbsp;';
            }
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
			$test = $act = $notify = '&nbsp;';
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
			$done = '&nbsp;';
			$test = "<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=status&op=m&sid=${sid}\">". _('Edit') ."</a>";
		} else {
			$done = '&nbsp;';
		}
        if ($HTTP_SESSION_VARS['acl']['superuser'] == 'Y'){
            $notify = '&nbsp;';
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
		<td><?php echo($stat); ?></td>
		<td><?php echo($test); ?></td>
		<td><?php echo($notify); ?></td>

<?php
    if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
?>
		<td><?php echo($act); ?></td>
		<td><?php echo($done); ?></td>
		<td><?php echo($del); ?></td>
<?php
    } else {
?>
		<td colspan=4>&nbsp;</td>
<?php
    }
?>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
