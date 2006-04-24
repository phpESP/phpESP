<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// <jflemer@alum.rpi.edu>
// <jflemer@acm.rpi.edu>

    /* SuperUser or Designers with "Administer Designers(pgroup)" permissions only*/
    if(($_SESSION['acl']['superuser'] != 'Y' &&
        count($_SESSION['acl']['pgroup']) == 0) &&
        !auth_no_access(_('to access this form'))) {
        return;
    }

	$bg1 =& $ESPCONFIG['bgalt_color1'];
	$bg2 =& $ESPCONFIG['bgalt_color2'];

	$errstr = '';
	$u = '';
	$r = '';

	/* abort */
	if(isset($_POST['cancel'])) {
		unset($_POST['submit']);
		unset($_POST['delete']);
		include(esp_where('designers'));
		return;
	}

	/* delete user */
	if(isset($_POST['delete'])) {
		unset($_POST['submit']);
		$u = _addslashes($_POST['u']);
		$r = _addslashes($_POST['r']);
		/* check acl */
		if($_SESSION['acl']['superuser'] == 'Y' ||
			in_array($r, $_SESSION['acl']['pgroup']) ||
			auth_no_access(_('to access this group'))) {
			$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['designer_table']." WHERE username=$u AND realm=$r";
			if(!execute_sql($sql) || affected_rows() < 1) {
				/* unsucessfull -- abort */
				$errstr .= mkerror(_('Cannot delete account.') .' ('.
					ErrorMsg() .')');
			}
		}
		if(empty($errstr)) {
			include(esp_where('designers'));
			return;
		}
	}

	/* submitted */
	if(isset($_POST['submit'])) {
		$u         = _addslashes($_POST['u']);
		$r         = _addslashes($_POST['r']);
		$password  = _addslashes($_POST['password']);
		$fname     = _addslashes($_POST['fname']);
		$lname     = _addslashes($_POST['lname']);
		$email     = _addslashes($_POST['email']);
		$pdesign   = _addslashes($_POST['pdesign']);
		$pstatus   = _addslashes($_POST['pstatus']);
		$pdata     = _addslashes($_POST['pdata']);
		$pall      = _addslashes($_POST['pall']);
		$pgroup    = _addslashes($_POST['pgroup']);
		$puser     = _addslashes($_POST['puser']);
		$disabled  = _addslashes($_POST['disabled']);

		if(!empty($_POST['ex_year']) ||
				!empty($_POST['ex_month']) ||
				!empty($_POST['ex_day'])) {
			if(empty($_POST['ex_day'])) {
				$ex_day = 1;
			} else {
				$ex_day = intval($_POST['ex_day']);
			}
			if(empty($_POST['ex_month'])) {
				$ex_month = 1;
			} else {
				$ex_month = intval($_POST['ex_month']);
			}
			if(empty($_POST['ex_year'])) {
				$now = getdate(time());
				$ex_year = $now['year'];
			} else {
				$ex_year = intval($_POST['ex_year']);
				if($ex_year < 2000)
					$ex_year += 2000;
			}

			$expiration = sprintf("%04d%02d%02d%06d",
				$ex_year,$ex_month,$ex_day,0);
		} else {
			$expiration = "0000-00-00 00:00:00";
			$ex_year    = '';
			$ex_month   = '';
			$ex_day     = '';
		}

		/* new user */
        /* check acl */
        if($_SESSION['acl']['superuser'] == 'Y' ||
            in_array($r, $_SESSION['acl']['pgroup']) ||
            auth_no_access(_('to access this group'))) {
            $sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['designer_table']." WHERE username=$u AND realm=$r";
            $result = execute_sql($sql);
            if(!$result || record_count($result) < 1) {
            	if(empty($u) || empty($r) || empty($password)) {
            		$errstr .= mkerror(_('Username, Password, and Group are required.'));
            	} else {
            		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['designer_table']."
            			(username,realm,password,changed)
            			VALUES ($u,$r, ".db_crypt($password).", ".sys_time_stamp().")";
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
            	if(empty($pdesign))
            		$pdesign = 'Y';
            	$fields = array('pstatus','pdata','pall','pgroup','puser','disabled');
            	foreach($fields as $f) {
            		if(empty($$f))
            			$$f = 'N';
            	}
            	if (!empty($password))
            		$password = "password=".db_crypt($password).",";
            	$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['designer_table']." SET
            		$password
            		fname=$fname,
            		lname=$lname,
            		email=$email,
            		pdesign=$pdesign,
            		pstatus=$pstatus,
            		pdata=$pdata,
            		pall=$pall,
            		pgroup=$pgroup,
            		puser=$puser,
            		disabled=$disabled,
            		changed=".sys_time_stamp().",
            		expiration=".db_qstr($expiration)."
            	WHERE username=$u AND realm=$r";
            	if(!execute_sql($sql)) {
            		/* unsucessfull -- abort */
            		$errstr .= mkerror(_('Cannot change account data.') .' ('.
            			ErrorMsg() .')');
            	}
            }
        }

	    if(empty($errstr)) {
	    	include(esp_where('designers'));
	    	return;
	    }
	} else if(isset($_GET['u']) && isset($_GET['r'])) {
		$u = _addslashes($_GET['u']);
		$r = _addslashes($_GET['r']);
	} else {
		$u = '';
		$r = '';
		$expiration = '';
		$disabled = 'N';
		$pdesign  = 'Y';
		$pstatus  = 'N';
		$pdata    = 'N';
		$pall     = 'N';
		$pgroup   = 'N';
		$puser    = 'N';
	}

	/* load ACL */
	if(!empty($u) && !empty($r)) {
		$sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['designer_table']." WHERE username=$u AND realm=$r";
		$result = execute_sql($sql,"",ADODB_FETCH_ASSOC);
		if($arr = fetch_row($result)) {
			foreach(array(
				'username', 'realm', 'fname', 'lname', 'email',
				'pdesign', 'pstatus', 'pdata', 'pall', 'pgroup', 'puser',
				'disabled', 'expiration') as $col)
				{ $$col = $arr[$col]; }
			$u =& $username;
			$r =& $realm;
			if(intval($expiration) > 0) {
				$ex_year  = substr($expiration,0,4);
				$ex_month = substr($expiration,5,2);
				$ex_day   = substr($expiration,8,2);
			} else {
				$ex_year  = '';
				$ex_month = '';
				$ex_day   = '';
			}
		} else {
			$errstr .= mkerror(_('Account not found.') .' ('.
				ErrorMsg() .')');
		}
	}

?>
<h2><?php echo(_('Designer Account Administration')); ?></h2>
<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>
<input type="hidden" name="where" value="admdesigner" />
<table cellspacing="0" cellpadding="4" style="width: 450px;">
	<tr>
<?php
	echo("<th class=\"right\">". _('Username') ."</th>\n");
	if(empty($u))
		$u = mktext('u', 16, 16, $GLOBALS);
	else
		$u = mkhidden('u', $GLOBALS) . "<tt>$u</tt>";
	echo("<td class=\"left\">$u</td>\n");
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
		$r = '<select name="r"><option></option>';
		$groups = array();
		if($_SESSION['acl']['superuser'] == 'Y') {
			$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
			$result = execute_sql($sql);
			while( list($g) = fetch_row($result) ) {
				array_push($groups, $g);
                $result->MoveNext();
			}
			db_close($result);
		} else {
			$groups =& $_SESSION['acl']['pgroup'];
		}
		foreach($groups as $g) {
			$r .= "<option value=\"$g\">$g</option>";
		}
		$r .= '</select>';
	} else {
		$r = mkhidden('r', $GLOBALS) . "<tt>$r</tt>";
	}
	echo("<td class=\"left\"><tt>$r</tt></td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('First Name') ."</th>\n");
	echo("<td class=\"left\">". mktext('fname', 16, 16, $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Last Name') ."</th>\n");
	echo("<td class=\"left\">". mktext('lname', 24, 24, $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Email') ."</th>\n");
	echo("<td class=\"left\">". mktext('email', 24, 64, $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Expiration') ."</th>\n");
	echo("<td class=\"left\">".
		mktext('ex_year',4,4, $GLOBALS) .' '.
		mktext('ex_month',2,2, $GLOBALS) .' '.
		mktext('ex_day',2,2, $GLOBALS) .' ('.
			_('year') .' '.
			_('month') .' '.
			_('day'). ")</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Disabled') ."</th>\n");
	echo("<td class=\"left\">". mkselect('disabled',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) ."</td>\n");
?>
	</tr><!-- <tr>
		<th colspan="2" bgcolor="<?php echo($bg2); ?>">
			<input type="submit" name="submit" value="<?php echo(_('Update')); ?>">&nbsp;
			<input type="submit" name="cancel" value="<?php echo(_('Cancel')); ?>">&nbsp;
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>"></th>
	</tr>--><tr>
<?php
	echo("<th class=\"right\">". _('Design Surveys') ."</th>\n");
	echo("<td class=\"left\">". mkselect('pdesign',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Change Survey Status') ."</th>\n");
	echo("<td class=\"left\">". mkselect('pstatus',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) .
		'<!-- ('. _('Activate/End') .") --></td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Export Survey Data') ."</th>\n");
	echo("<td class=\"left\">". mkselect('pdata',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Group Editor') ."</th>\n");
	echo("<td class=\"left\">". mkselect('pall',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) .
		'<!-- ('. _('may edit <b>all</b> forms owned by this group') .") --></td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Administer Designers') ."</th>\n");
	echo("<td class=\"left\">". mkselect('pgroup',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) ."</td>\n");
?>
	</tr><tr>
<?php
	echo("<th class=\"right\">". _('Administer Respondents') ."</th>\n");
	echo("<td class=\"left\">". mkselect('puser',array('Y' => _('Yes'), 'N' => _('No')), $GLOBALS) ."</td>\n");
?>
	</tr><tr>
		<td colspan="2" class="center">
			<input type="submit" name="submit" value="<?php echo(_('Update')); ?>"/>&nbsp;
			<input type="submit" name="cancel" value="<?php echo(_('Cancel')); ?>"/>&nbsp;
			<input type="submit" name="delete" value="<?php echo(_('Delete')); ?>"/>
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
