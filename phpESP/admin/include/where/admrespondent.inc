<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>
// <jflemer@acm.rpi.edu>

	$bg1 =& $ESPCONFIG['bgalt_color1'];
	$bg2 =& $ESPCONFIG['bgalt_color2'];

	$errstr = '';
	$u = '';
	$r = '';

	/* abort */
	if(isset($HTTP_POST_VARS['cancel'])) {
		unset($HTTP_POST_VARS['submit']);
		unset($HTTP_POST_VARS['delete']);
		include(esp_where('respondents'));
		return;
	}

	/* delete user */
	if(isset($HTTP_POST_VARS['delete'])) {
		unset($HTTP_POST_VARS['submit']);
		$u = XADDSLASHES($HTTP_POST_VARS['u']);
		$r = XADDSLASHES($HTTP_POST_VARS['r']);
		/* check acl */
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
			in_array($r, $HTTP_SESSION_VARS['acl']['pgroup']) ||
			auth_no_access(_('to access this group'))) {
			$sql = "DELETE FROM respondent WHERE username='$u' AND realm='$r'";
			if(!mysql_query($sql) || mysql_affected_rows() < 1) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot delete account.') .' ('.
					mysql_error() .')');
			}
		}
		if(empty($errstr)) {
			include(esp_where('respondents'));
			return;
		}
	}

	/* submitted */
	if(isset($HTTP_POST_VARS['submit'])) {
		$u         = XADDSLASHES($HTTP_POST_VARS['u']);
		$r         = XADDSLASHES($HTTP_POST_VARS['r']);
		$password  = XADDSLASHES($HTTP_POST_VARS['password']);
		$fname     = XADDSLASHES($HTTP_POST_VARS['fname']);
		$lname     = XADDSLASHES($HTTP_POST_VARS['lname']);
		$email     = XADDSLASHES($HTTP_POST_VARS['email']);
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

		if($r == 'superuser' && $HTTP_SESSION_VARS['acl']['superuser'] != 'Y')
			$r = '';

		/* new user */
		$sql = "SELECT * FROM respondent WHERE username='$u' AND realm='$r'";
		$result = mysql_query($sql);
		if(mysql_num_rows($result) < 1) {
			if(empty($u) || empty($r) || empty($password)) {
				$errstr .= mkerror(_('Username, Password, and Group are required.'));
			} else {
				$sql = "INSERT INTO respondent
					(username,realm,password)
					VALUES ('$u','$r',PASSWORD('$password'))";
				if(!mysql_query($sql)) {
					$u = '';
					$r = '';
					$errstr .= mkerror(_('Error adding account.') .' ('.
						mysql_error() .')');
				}
			}
		}
		mysql_free_result($result);

		/* change user data */
		if(!empty($u) && !empty($r)) {
			if(empty($disabled))
				$disabled = 'N';
			if (!empty($password))
				$password = "password=PASSWORD('$password'),";
			$sql = "UPDATE respondent SET
				$password
				fname='$fname',
				lname='$lname',
				email='$email',
				disabled='$disabled',
				changed=now(),
				expiration='$expiration'
			WHERE username='$u' AND realm='$r'";
			if(!mysql_query($sql)) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot change account data.') .' ('.
					mysql_error() .')');
			}
		}

		if(empty($errstr)) {
			include(esp_where('respondents'));
			return;
		}
	} else if(isset($HTTP_GET_VARS['u']) && isset($HTTP_GET_VARS['r'])) {
		$u = XADDSLASHES($HTTP_GET_VARS['u']);
		$r = XADDSLASHES($HTTP_GET_VARS['r']);
	} else {
		$u = '';
		$r = '';
		$expiration = '';
		$disabled = 'N';
	}

	/* load ACL */
	if(!empty($u) && !empty($r)) {
		$sql = "SELECT * FROM respondent WHERE username='$u' AND realm='$r'";
		$result = mysql_query($sql);
		if($arr = mysql_fetch_array($result, MYSQL_ASSOC)) {
			foreach(array(
				'username', 'fname', 'lname', 'email',
				'realm', 'disabled', 'expiration') as $col)
				{ $$col = $arr[$col]; }
			$u =& $username;
			$r =& $realm;
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
			$errstr .= mkerror(_('Account not found.') .' ('.
				mysql_error() .')');
		}
	}

?>
<h2><?php echo(_('Respondent Account Administration')); ?></h2>
<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>
<input type="hidden" name="where" value="admrespondent">
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>">
	<tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Username') ."</th>\n");
	if(empty($u))
		$u = mktext('u', 16, 16);
	else
		$u = mkhidden('u') . "<tt>$u</tt>";
	echo("<td bgcolor=\"$bg1\">$u</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Password') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mkpass('password') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Group') ."</th>\n");
	if(empty($r)) {
		$r = '<select name="r"><option></option>';
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
	} else {
		$r = mkhidden('r') . "<tt>$r</tt>";
	}
	echo("<td bgcolor=\"$bg1\"><tt>$r</tt></td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('First Name') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('fname', 16, 16) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Last Name') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('lname', 24, 24) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th bgcolor=\"$bg2\" align=\"right\">". _('Email') ."</th>\n");
	echo("<td bgcolor=\"$bg1\">". mktext('email', 24, 64) ."</td>\n");
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
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>">
		</th>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
