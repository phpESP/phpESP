<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>
// <jflemer@acm.rpi.edu>

	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
		$HTTP_SESSION_VARS['acl']['managers'] != 'Y' &&
		!auth_no_access('to manage designers')) {
		return;
	}

	$section = 'manager';
	
	$bg1 =& $ESPCONFIG['bgalt_color1'];
	$bg2 =& $ESPCONFIG['bgalt_color2'];
	
	$errstr = '';
	$u = '';
	
	/* abort */
	if(isset($HTTP_POST_VARS['cancel'])) {
		unset($HTTP_POST_VARS['submit']);
		unset($HTTP_POST_VARS['delete']);
	}
	
	/* delete user */
	if(isset($HTTP_POST_VARS['delete'])) {
		unset($HTTP_POST_VARS['submit']);
		unset($HTTP_GET_VARS['u']);
		$u         = XADDSLASHES($HTTP_POST_VARS['u']);
		$sql = "DELETE FROM manager WHERE username='$u'";
		if(!mysql_query($sql) || mysql_affected_rows() < 1) {
			/* unsucessfull -- abort */
			$username = $u;
			$errstr .= mkerror(_('Cannot delete user.') .' ('.
				mysql_error() .')');
		}
		$u = '';
	}

	/* submitted */
	if(isset($HTTP_POST_VARS['submit'])) {
		$u         = XADDSLASHES($HTTP_POST_VARS['u']);
		$username  = XADDSLASHES($HTTP_POST_VARS['username']);
		$password  = XADDSLASHES($HTTP_POST_VARS['password']);
		$firstname = XADDSLASHES($HTTP_POST_VARS['firstname']);
		$lastname  = XADDSLASHES($HTTP_POST_VARS['lastname']);
		$email     = XADDSLASHES($HTTP_POST_VARS['email']);
		$realm     = XADDSLASHES($HTTP_POST_VARS['realm']);
		$screate   = XADDSLASHES($HTTP_POST_VARS['screate']);
		$sactivate = XADDSLASHES($HTTP_POST_VARS['sactivate']);
		$send      = XADDSLASHES($HTTP_POST_VARS['send']);
		$sdelete   = XADDSLASHES($HTTP_POST_VARS['sdelete']);
		$seeall    = XADDSLASHES($HTTP_POST_VARS['seeall']);
		$users     = XADDSLASHES($HTTP_POST_VARS['users']);
		$managers  = XADDSLASHES($HTTP_POST_VARS['managers']);
		$superuser = XADDSLASHES($HTTP_POST_VARS['superuser']);
		$disabled  = XADDSLASHES($HTTP_POST_VARS['disabled']);

		if(!empty($HTTP_POST_VARS['ex_year']) ||
				!empty($HTTP_POST_VARS['ex_month']) ||
				!empty($HTTP_POST_VARS['ex_day'])) {
			if(empty($HTTP_POST_VARS['ex_day'])) {
				$ex_day = 1;
			} else {
				$ex_day = intval($HTTP_POST_VARS['ex_day']);
			}
			if(empty($HTTP_POST_VARS['ex_month'])) {
				$ex_month = 1;
			} else {
				$ex_month = intval($HTTP_POST_VARS['ex_month']);
			}
			if(empty($HTTP_POST_VARS['ex_year'])) {
				$now = getdate(time());
				$ex_year = $now['year'];
			} else {
				$ex_year = intval($HTTP_POST_VARS['ex_year']);
				if($ex_year < 2000)
					$ex_year += 2000;
			}

			$expiration = sprintf("%04d%02d%02d%06d",
				$ex_year,$ex_month,$ex_day,0);
		} else {
			$expiration = "0";
			$ex_year    = '';
			$ex_month   = '';
			$ex_day     = '';
		}
		
		/* new user */
		if(empty($u)) {
			if(empty($username) || empty($password)) {
				$errstr .= mkerror('Username and password required.');
			} else {
				$sql = "INSERT INTO manager
					(username,password) 
					VALUES ('$username',PASSWORD('$password'))";
				if($result = mysql_query($sql)) {
					$u = $username;
					$password = '';
				}
			}
		}
		
		/* change username */
		if(!empty($u) && $u != $username) {
			$sql = "UPDATE manager SET username='$username'
				WHERE username='$u'";
			if(!mysql_query($sql) || mysql_affected_rows() < 1) {
				/* unsucessfull -- abort */
				$username = $u;
				$errstr .= mkerror(_('Cannot change username.') .' ('.
					mysql_error() .')');
			} else {
				$u = $username;
			}
		}

		/* change user password */
		if(!empty($password)) {
			$sql = "UPDATE manager SET 
				password=PASSWORD('$password') WHERE username='$u'";
			if(!mysql_query($sql) || mysql_affected_rows() < 1) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot change user password.') .' ('.
					mysql_error() .')');
			}
		}

		/* change user data */
		if(!empty($u)) {
			$sql = "UPDATE manager SET 
				firstname='$firstname',
				lastname='$lastname',
				email='$email',
				realm='$realm',
				screate='$screate',
				sactivate='$sactivate',
				send='$send',
				sdelete='$sdelete',
				seeall='$seeall',
				users='$users',
				managers='$managers',";
			/* only superuser can set superuser status */
			if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
				$sql .= "superuser='$superuser',";
			}
			$sql .= "disabled='$disabled',
				changed=now(),
				expiration='$expiration'
			WHERE username='$u'";
			if(!mysql_query($sql) || mysql_affected_rows() < 1) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot change user data.') .' ('.
					mysql_error() .')');
			}
		}
	} else if(isset($HTTP_GET_VARS['u'])) {
		$u = XADDSLASHES($HTTP_GET_VARS['u']);
	} else {
		$screate  = 'Y';
		$sactivate= 'N';
		$send     = 'N';
		$sdelete  = 'N';
		$seeall   = 'N';
		$users    = 'N';
		$managers = 'N';
		$superuser= 'N';
	}

	/* load ACL */
	if(!empty($u)) {
		$sql = "SELECT * FROM manager WHERE username='$u'";
		$result = mysql_query($sql);
		if($arr = mysql_fetch_array($result, MYSQL_ASSOC)) {
			foreach(array(
				'username', 'firstname', 'lastname', 'email', 'realm', 
				'screate', 'sactivate', 'send', 'sdelete', 'seeall', 'users', 
				'managers', 'superuser', 'disabled', 'expiration') as $col)
				{ $$col = $arr[$col]; }
			if(intval($expiration) > 0) {
				$ex_year  = substr($expiration,0,4);
				$ex_month = substr($expiration,4,2);
				$ex_day   = substr($expiration,6,2);
			} else {
				$ex_year  = '';
				$ex_month = '';
				$ex_day   = '';
			}			
		} else {
			$errstr .= mkerror(_('User not found.') .' ('.
				mysql_error() .')');
		}
	}

?>
<h2><?php echo(_('Designer Administration')); ?></h2>
<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>
<input type="hidden" name="where" value="editmanager">
<input type="hidden" name="u" value="<?php echo($u); ?>">
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>">
	<tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Username') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('username') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Password') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkpass('password') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('First Name') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('firstname') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Last Name') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('lastname') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Email') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('email') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Realm') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('realm') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Expiration') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". 
		mktext('ex_year',4) .' '. 
		mktext('ex_month',2) .' '. 
		mktext('ex_day',2) .' ('. 
			_('year') .' '.
			_('month') .' '.
			_('day'). ")</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Disabled') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('disabled',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
		<th colspan="2" bgcolor="<?php echo($bg2); ?>">
			<input type="submit" name="submit" value="<?php echo(_('Update')); ?>">&nbsp;
			<input type="submit" name="cancel" value="<?php echo(_('Cancel')); ?>">&nbsp;
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>"></th>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Create/Edit Surveys') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('screate',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Activate Surveys') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('sactivate',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('End Surveys') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('send',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Archive Surveys') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('sdelete',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('See All Surveys') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('seeall',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Administer Respondants') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('users',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Administer Designers') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('managers',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Superuser') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkselect('superuser',array('Y' => _('Yes'), 'N' => _('No'))) ."</td>\n");
?>
	</tr><tr>
		<th colspan="2" bgcolor="<?php echo($bg2); ?>">
			<input type="submit" name="submit" value="<?php echo(_('Update')); ?>">&nbsp;
			<input type="submit" name="cancel" value="<?php echo(_('Cancel')); ?>">&nbsp;
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>"></th>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
