<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>


    $bg = '';

	/* load names and titles of all surveys available to
	 * _this_ user */
	if($_SESSION['acl']['superuser'] == 'Y') {
		$sql = "SELECT s.id, s.name, s.title, s.owner, s.realm
		FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.status = 0 ORDER BY s.id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$_SESSION['acl']['pall'],
				$_SESSION['acl']['pdesign']));
		$sql = "SELECT s.id, s.name, s.title, s.owner, s.realm
			FROM ".$GLOBALS['ESPCONFIG']['survey_table']." s WHERE s.status = 0 AND (s.owner = ".
			_addslashes($_SESSION['acl']['username']) ." || s.realm $realms) ORDER BY id DESC";
	}
	$result = execute_sql($sql);

?>
<h2><?php echo(_('Edit a Survey')); ?></h2>
<?php echo(_('Pick Survey to Edit')); ?>
<table cellspacing="0" cellpadding="4">
	<tr class="header">
		<th><?php echo(_('ID')); ?></th>
		<th><?php echo(_('Name')); ?></th>
		<th><?php echo(_('Title')); ?></th>
		<th><?php echo(_('Owner')); ?></th>
		<th><?php echo(_('Group')); ?></th>
	</tr>
<?php
	while(list($sid,$name,$title,$owner,$realm) = fetch_row($result)) {
        $result->MoveNext();
		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<!-- Alternating Colors are set in the phpESP.ini.php file -->
	<tr style="background-color: <?php echo($bg); ?>;">
		<td><?php echo($sid); ?></td>
		<td>
			<a href="<?php echo(htmlentities($GLOBALS['ESPCONFIG']['ME'] ."?where=tab&newid=${sid}")); ?>"><?php echo($name); ?></a>
		</td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
	</tr>
<?php
	}
	db_close($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
