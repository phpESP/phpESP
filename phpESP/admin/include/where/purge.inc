<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	if($_SESSION['acl']['superuser'] != 'Y') {
		echo(mkerror(_('Only superusers allowed.'))."<br>\n");
		echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
		return;
	}

	$sids = array();
	if(!empty($_POST['list'])) {
		while(list(,$sid) = each($_POST['list'])) {
			array_push($sids, intval($sid));
		}
		survey_purge($sids);
	}

//	$sql = "
//SELECT s.id,s.name,s.title,s.status,s.owner,s.realm,COUNT(q.id) as count
//FROM survey s LEFT JOIN question q ON
	$sql = "
SELECT s.id,s.name,s.title,s.status,s.owner,s.realm,COUNT(q.id) as count
FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s LEFT JOIN ".$GLOBALS['ESPCONFIG']['question_table']." q ON
	s.id = q.survey_id AND q.deleted = 'N'
GROUP BY s.id, s.name, s.title, s.status, s.owner, s.realm
ORDER BY s.id DESC";
	$result = execute_sql($sql);

	$bg = $ESPCONFIG['bgalt_color2'];
?>
<h2><?php echo(_('Purge Surveys')); ?></h2>

	<p><?php echo(_('
This page is not directly on the main menu because it is
dangerous. This <b>completely</b> removes everything about a
survey from the database <b>forever</b>. All question info,
general info, results, etc. are purged from the database. Do
not do anything here that you are not completely certain
about. There is no confirmation, there is no turning
back.')); ?></p>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<table cellspacing="0" cellpadding="4">
	<tr style="background-color: <?php echo($bg); ?>">
		<td>&nbsp;</td>
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
		<th><?php echo(_("# Q's")); ?></th>
		<th><?php echo(_('Status')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$status,$owner,$realm,$qnum) = fetch_row($result)) {
        $result->MoveNext();
		if($status & STATUS_DELETED) {
			$stat = _('Archived');
		} elseif($status & STATUS_DONE) {
			$stat = _('Ended');
		} elseif($status & STATUS_ACTIVE) {
			$stat = _('Active');
		} elseif($status & STATUS_TEST) {
			$stat = _('Testing');
		} else {
			$stat = _('Editing');
		}
		
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr style="background-color: <?php echo($bg); ?>">
		<td><input type="checkbox" name="list[]" value="<?php echo($sid); ?>" /></td>
		<td><?php echo($sid); ?></td>
		<td><?php echo($name); ?></td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($qnum); ?></td>
		<td><?php echo($stat); ?></td>
	</tr>
<?php
	}
	
	if($bg != $ESPCONFIG['bgalt_color1'])
		$bg = $ESPCONFIG['bgalt_color1'];
	else
		$bg = $ESPCONFIG['bgalt_color2'];
?>
    <tr style="background-color: <?php echo($bg); ?>">
		<td colspan="8">
			<input type="hidden" name="where" value="purge" />
			<input type="reset" name="reset" value="<?php echo(_('Clear Checkboxes')); ?>" />
            <?php
            $confirmText = _('You are about to PERMANENTLY and IRREVOCABLY remove ALL information about the selected surveys. Click Ok if you are absolutely sure this is what you want to do.');
            ?>
			<input type="submit" name="submit"
            onclick='return confirm("<?php echo $confirmText; ?>")' onkeypress='return confirm ("<?php echo $confirmText; ?>");'
            value="<?php echo(_('Purge Surveys')); ?>" />
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
