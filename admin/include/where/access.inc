<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	if (!empty($_POST['sid']))
		$sid = intval($_POST['sid']);
	elseif (!empty($_GET['sid']))
		$sid = intval($_GET['sid']);
	else
		$sid = '';

	$errstr = "";
	$bg = '';

	if ($sid) {
		if($_SESSION['acl']['superuser'] == 'Y') {
			$sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.id = $sid";
			$sql1 = "SELECT a.realm, a.maxlogin, a.resume, a.navigate FROM ".$GLOBALS['ESPCONFIG']['access_table']." a
				WHERE a.survey_id = $sid ORDER BY a.realm";
		} else {
			$realms = array_to_insql(
				array_intersect(
					$_SESSION['acl']['pall'],
					array_merge(
						$_SESSION['acl']['pall'],
						$_SESSION['acl']['pdesign'])));
                $sql = "SELECT s.name, s.title, s.owner, s.realm, s.public
				FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.id = $sid AND NOT (status & " .STATUS_DELETED. ") AND                (s.owner = "._addslashes($_SESSION['acl']['username']) ." || s.realm $realms)";
			    $sql1 = "SELECT a.realm, a.maxlogin, a.resume, a.navigate 
                FROM ".$GLOBALS['ESPCONFIG']['access_table']." a, ".$GLOBALS['ESPCONFIG']['survey_table']." s
				WHERE a.survey_id = $sid AND s.id=a.survey_id AND (s.owner = ".
				_addslashes($_SESSION['acl']['username']) ." || s.realm $realms) ORDER BY a.realm";
		}
		$result = execute_sql($sql);
		if (record_count($result) < 1) {
			$sid = 0;
		}
	}
	if ($sid) {
		list($name,$title,$owner,$realm,$public) = fetch_row($result);
		db_close($result);

		if (!empty($_POST['op']))
			$op = $_POST['op'];
		elseif (!empty($_GET['op']))
			$op = $_GET['op'];
		else
			$op = '';

		if (!empty($_POST['realm']))
			$arealm = _addslashes($_POST['realm']);
		elseif (!empty($_GET['realm']))
			$arealm = _addslashes($_GET['realm']);

		if (isset($_POST['resume']))
			$resume = 'Y';
		else
			$resume = 'N';
	    $resume = _addslashes($resume);	
		
		if (isset($_POST['navigate']))
			$navigate = 'Y';
		else
			$navigate = 'N';
	    $navigate = _addslashes($navigate);
		
		if (!empty($_POST['max']))
			$max = intval($_POST['max']);
		elseif (!empty($_GET['max']))
			$max = intval($_GET['max']);
		else
			$max = 0;

		if ($op == 'a') {
			if (empty($_POST['realm'])) {
				$errstr = mkerror(_('Please select a group.'));
			} else {
				$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['access_table']." (survey_id, realm, maxlogin, resume, navigate)
						VALUES ($sid, $arealm, $max, $resume, $navigate)";
				execute_sql($sql);
			}
		} elseif ($op == 'r') {
			$sql = "DELETE FROM ".$GLOBALS['ESPCONFIG']['access_table']." WHERE survey_id = $sid AND realm = $arealm";
			execute_sql($sql);
		} elseif ($op == 'v') {
			$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['survey_table']." SET public = 'N' WHERE id = $sid";
			execute_sql($sql);
			$sid = 0;
		} elseif ($op == 'p') {
			$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['survey_table']." SET public = 'Y' WHERE id = $sid";
			execute_sql($sql);
			$sid = 0;
		}
	}
	if ($sid) {
		if ($public == 'N')
			$public = _('Private');
		else
			$public = _('Public');

		$r = '<select name="realm"><option></option>';
		$groups = array();
        $selected_groups = array();
        // if realm has already been added then do not include it for
        // reselection.
        $result = execute_sql($sql1);
        while (list($sg) = fetch_row($result)) {
            array_push($selected_groups, $sg);
            $result->MoveNext();
        }
        db_close($result);
		if($_SESSION['acl']['superuser'] == 'Y') {

			$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
			$result = execute_sql($sql);
			while( list($g) = fetch_row($result) ) {
                if (!in_array($g, $selected_groups)) {
				    array_push($groups, $g);
                }
                $result->MoveNext();
			}
			db_close($result);
        } else {
            $g =& $_SESSION['acl']['pdesign'];
            foreach ($g as $t) {
                if (!in_array($t, $selected_groups)) {
                    array_push($groups, $t);
                }
            }
        }
		foreach($groups as $g) {
			$r .= "<option value=\"$g\">$g</option>";
		}
		$r .= '</select>';
	}
?>
<h2><?php echo(_('Survey Access')); ?></h2>

<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>

<div style="text-align: left">
<p><?php
	echo(_('This lets you control who has access to fill out a form.
Public surveys let anyone submit data.
Private surveys are restricted by Respondent Groups.')); ?></p>

<p><b><?php echo(_('Note')); ?>:</b>
<?php printf(_('You must use %s when using private surveys.'),
	'<tt>'. substr(strrchr($ESPCONFIG['handler_prefix'], '/'), 1) .
	' &amp; '. substr(strrchr($ESPCONFIG['handler'], '/'), 1) . '</tt>'); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php if ($sid) { ?>
<input type="hidden" name="where" value="access" />
<input type="hidden" name="sid" value="<?php echo($sid); ?>" />
<input type="hidden" name="op" value="a" />
<table cellspacing="0" cellpadding="4">
	<tr class="header"><th align="left"><?php echo(_('ID')); ?></th><td colspan="5"><?php echo($sid); ?></td></tr>
	<tr class="header"><th align="left"><?php echo(_('Name')); ?></th><td colspan="5"><?php echo($name); ?></tr>
	<tr class="header"><th align="left"><?php echo(_('Title')); ?></th><td colspan="5"><?php echo($title); ?></tr>
	<tr class="header"><th align="left"><?php echo(_('Owner')); ?></th><td colspan="5"><?php echo($owner); ?></tr>
	<tr class="header"><th align="left"><?php echo(_('Group')); ?></th><td colspan="5"><?php echo($realm); ?></tr>
	<tr class="header"><th align="left"><?php echo(_('Public')); ?></th><td colspan="5"><?php echo($public); ?></tr>
	<tr><td colspan="6"><hr /></td></tr>
	<tr><th><?php echo(_('Group')); ?></th>
		<th><?php echo(_('Max Responses')); ?></th>
		<th><?php echo(_('Save/Restore')); ?></th>
		<th><?php echo(_('Back/Forward')); ?></th>
		<td>&nbsp;</td></tr>
<?php
		$result = execute_sql($sql1);
		while ( list($arealm, $amax, $aresume, $anavigate) = fetch_row($result) ) { 
            $result->MoveNext();
            ?>
	<tr><td><?php echo($arealm); ?></td>
		<td><?php echo($amax); ?></td>
		<td><?php echo(($aresume == 'Y') ? _('Yes') : _('No')); ?></td>
		<td><?php echo(($anavigate == 'Y') ? _('Yes') : _('No')); ?></td>
		<td><a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=access&sid=$sid&op=r&realm=") . urlencode($arealm)); ?>">
		<?php echo(_('Remove')); ?></a></td></tr>
<?php	} ?>
	<tr><td><?php echo($r); ?></td>
		<td><input type="text" name="max" size="5"></td>
		<td><input type="checkbox" name="resume" /></td>
		<td><input type="checkbox" name="navigate" /></td>
		<td><input type="submit" value="<?php echo(_('Add')); ?>"></td></tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}
?>
<table cellspacing="0" cellpadding="4">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
		<th><?php echo(_('Public')); ?></th>
		<td>&nbsp;</td>
	</tr>
<?php
	/* load names and titles of all surveys available to
	 * _this_ user */
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm,public FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE NOT ".db_bin("status",STATUS_DELETED)." ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				array_merge(
					$_SESSION['acl']['pall'],
					$_SESSION['acl']['pdesign'])));
		$sql = "SELECT id,name,title,owner,realm,public
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE NOT ".db_bin("status",STATUS_DELETED)." AND (owner = ".
			_addslashes($_SESSION['acl']['username']) ." || realm $realms) ORDER BY id DESC";
	}
	$result = execute_sql($sql);
	while(list($sid,$name,$title,$owner,$realm,$public) = fetch_row($result)) {
        $result->MoveNext();
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];

		if ($public == 'N') {
			$public = _('Private');
			$op     = '<a href="'. $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=access&sid=$sid&op=p")."\">". _('Make Public') .'</a>';
		} else {
			$public = _('Public');
			$op     = '<a href="'. $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=access&sid=$sid&op=v")."\">". _('Make Private') .'</a>';
		}
?>
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
		<td><a href="<?php echo(htmlentities($GLOBALS['ESPCONFIG']['ME'] ."?where=access&sid=$sid")); ?>"><?php echo($name); ?></a></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($public); ?></td>
		<td><?php echo($op); ?></td>
	</tr>
<?php
	}
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
