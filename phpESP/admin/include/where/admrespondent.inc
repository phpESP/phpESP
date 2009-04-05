<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>
// <jflemer@acm.rpi.edu>

    /* SuperUser or Designers with "Administer Respondents(puser)" permissions only*/
    if($_SESSION['acl']['superuser'] != 'Y' &&
        count($_SESSION['acl']['puser']) == 0 &&
        !auth_no_access(_('to access this form'))) {
        return;
    }

	$bg1 =& $ESPCONFIG['bgalt_color1'];
	$bg2 =& $ESPCONFIG['bgalt_color2'];

	$errstr = '';
	$u = '';
	$r = '';
    $arr = array();

	/* abort */
	if(isset($_POST['cancel'])) {
		unset($_POST['submit']);
		unset($_POST['delete']);
		include(esp_where('respondents'));
		return;
	}

	/* delete user */
	if(isset($_POST['delete'])) {
		unset($_POST['submit']);
		$u = _addslashes($_POST['u']);
		$r = _addslashes($_POST['r']);
		$r2 = _stripslashes($_POST['r']);
		/* check acl */
		if($_SESSION['acl']['superuser'] == 'Y' ||
			in_array($r2, $_SESSION['acl']['puser']) ||
			auth_no_access(_('to access this group'))) {
			$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['respondent_table']." WHERE username=$u AND realm=$r";
			if(!execute_sql($sql) || affected_rows() < 1) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot delete account.') .' ('.
					ErrorMsg() .')');
			}
		}
		if(empty($errstr)) {
			include(esp_where('respondents'));
			return;
		}
	}
	/* submitted */
	if(isset($_POST['submit'])) {
		// for the following 4, we check for emptyness further down,
		// so we only do the addslashes if they are not empty
		if (!empty($_POST['u'])) {
			$u = _addslashes($_POST['u']);
		} else {
			$u = '';
		}
		if (!empty($_POST['r'])) {
			$r = _addslashes($_POST['r']);
			$r2 = _stripslashes($_POST['r']);
		} else {
			$r = '';
			$r2 = '';
		}
		if (!empty($_POST['password'])) {
			$u_password = _addslashes($_POST['password']);
		} else {
			$u_password = '';
		}
		if (!empty($_POST['disabled'])) {
			$u_disabled = _addslashes($_POST['disabled']);
		} else {
			$u_disabled = _addslashes("N");
		}

		// these 3 are not checked for emptyness, so we do the
		// addslashes immediately, otherwise the sql is broken
		$u_fname     = _addslashes($_POST['fname']);
		$u_lname     = _addslashes($_POST['lname']);
		$u_email     = _addslashes($_POST['email']);

		if(!empty($_POST['ex_year']) ||
				!empty($_POST['ex_month']) ||
				!empty($_POST['ex_day'])) {
			if(empty($_POST['ex_day'])) {
				$u_ex_day = 1;
			} else {
				$u_ex_day = intval($_POST['ex_day']);
			}
			if(empty($_POST['ex_month'])) {
				$u_ex_month = 1;
			} else {
				$u_ex_month = intval($_POST['ex_month']);
			}
			if(empty($_POST['ex_year'])) {
				$now = getdate(time());
				$u_ex_year = $now['year'];
			} else {
				$u_ex_year = intval($_POST['ex_year']);
				if($u_ex_year < 2000)
					$u_ex_year += 2000;
			}

			$u_expiration = sprintf("%04d%02d%02d%06d",
				$u_ex_year,$u_ex_month,$u_ex_day,0);
		} else {
			$u_expiration = "00000000000000"; #YYYYMMDDHHmmss
			$u_ex_year    = '';
			$u_ex_month   = '';
			$u_ex_day     = '';
		}

		/* new user */
        /* check acl */
        if($_SESSION['acl']['superuser'] == 'Y' ||
            in_array($r2, $_SESSION['acl']['puser']) ||
            auth_no_access(_('to access this group'))) {
            $sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['respondent_table']." WHERE username=$u AND realm=$r";
            $result = execute_sql($sql);
            if(record_count($result) < 1) {
            	if(empty($u) || empty($r) || empty($u_password)) {
            		$errstr .= mkerror(_('Username, Password, and Group are required.'));
            	} else {
            		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['respondent_table']."
            			(username,realm,password,changed)
            			VALUES ($u,$r,".db_crypt($u_password).", ".sys_time_stamp().")";
            		if(!execute_sql($sql)) {
            			$u = '';
            			$r = '';
            			$errstr .= mkerror(_('Error adding account.') .' ('.
            				ErrorMsg() .')');
            		}
            	}
            }
            db_close($result);
            /* change user data */
            if(!empty($u) && !empty($r)) {
            	if (!empty($u_password))
            		$u_password = "password=".db_crypt($u_password).",";
            	$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['respondent_table']." SET
            		$u_password
            		fname=$u_fname,
            		lname=$u_lname,
            		email=$u_email,
            		disabled=$u_disabled,
            		changed=".sys_time_stamp().",
            		expiration=$u_expiration WHERE username=$u AND realm=$r";
            	if(!execute_sql($sql)) {
            		/* unsucessfull -- abort */
            		$errstr .= mkerror(_('Cannot change account data.') .' ('.
            			ErrorMsg() .')');
            	}
            }
        }
        if(empty($errstr)) {
            include(esp_where('respondents'));
            return;
        }
	} else if(isset($_GET['u']) && isset($_GET['r'])) {
		$u = _addslashes($_GET['u']);
		$r = _addslashes($_GET['r']);
	} else {
		$u = '';
		$r = '';
		$u_expiration = '';
		$u_disabled = 'N';
	}

	/* load ACL */
	if(!empty($u) && !empty($r)) {
		$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['respondent_table']." WHERE username=$u AND realm=$r";
		$result = execute_sql($sql,"","ADODB_FETCH_ASSOC");
		if($arr = fetch_row($result)) {
			#foreach(array(
			#	'username', 'fname', 'lname', 'email',
			#	'realm', 'disabled', 'expiration') as $col)
			#	{ $$col = $arr[$col]; }
			#$u =& $username;
			#$r =& $realm;
			#if(intval($expiration) > 0) {
			$arr['u'] =$arr['username'];
			$arr['r'] =$arr['realm'];
			if(intval($arr['expiration']) > 0) {
				$arr['ex_year']  = substr($arr['expiration'],0,4);
				$arr['ex_month'] = substr($arr['expiration'],5,2);
				$arr['ex_day']   = substr($arr['expiration'],8,2);
			} else {
				$arr['ex_year']  = '';
				$arr['ex_month'] = '';
				$arr['ex_day']   = '';
			}
		} else {
			$errstr .= mkerror(_('Account not found.') .' ('.
				ErrorMsg() .')');
		}
        db_close($result);
	}

?>
<h2><?php echo(_('Respondent Account Administration')); ?></h2>
<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>
<input type="hidden" name="where" value="admrespondent" />
<table cellspacing="0" cellpadding="4" style="width: 350px">
	<tr>
<?php
	echo("<th class=\"right\">". _('Username') ."</th>\n");
	if(empty($u))
		$u_u = mktext('u', 16, 16, $arr);
	else
		$u_u = mkhidden('u', $arr) . "<tt>$u</tt>";
	echo("<td class=\"left\">$u_u</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Password') ."</th>\n");
	echo("<td class=\"left\">". mkpass('password') ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Group') ."</th>\n");
	if(empty($r)) {
		$u_r = '<select name="r"><option></option>';
		$groups = array();
		if($_SESSION['acl']['superuser'] == 'Y') {
			$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
			$result = execute_sql($sql);
			while( list($g) = fetch_row($result)) {
				array_push($groups, $g);
                $result->MoveNext();
			}
			db_close($result);
		} else {
			$groups =& $_SESSION['acl']['puser'];
		}
		foreach($groups as $g) {
			$u_r .= "<option value=\"$g\">$g</option>";
		}
		$u_r .= '</select>';
	} else {
		$u_r = mkhidden('r', $arr) . "<tt>$r</tt>";
	}
	echo("<td class=\"left\">$u_r</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('First Name') ."</th>\n");
	echo("<td class=\"left\">". mktext('fname', 16, 16, $arr) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Last Name') ."</th>\n");
	echo("<td class=\"left\">". mktext('lname', 24, 24, $arr) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Email') ."</th>\n");
	echo("<td class=\"left\">". mktext('email', 24, 64, $arr) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Expiration') ."</th>\n");
	echo("<td class=\"left\">".
		mktext('ex_year', 4, 0, $arr) .' '.
		mktext('ex_month', 2, 0, $arr) .' '.
		mktext('ex_day', 2, 0, $arr) .' ('.
			_('year') .' '.
			_('month') .' '.
			_('day'). ")</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Disabled') ."</th>\n");
	echo("<td class=\"left\">". mkselect('disabled',array('Y' => _('Yes'), 'N' => _('No')), $arr) ."</td>\n");
?>
	</tr><tr>
		<th colspan="2" class="center">
			<input type="submit" name="submit" value="<?php echo(_('Update')); ?>"/>&nbsp;
			<input type="submit" name="cancel" value="<?php echo(_('Cancel')); ?>"/>&nbsp;
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>"/>
		</th>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
