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

	if(isset($_POST['q_rid']) && !empty($_POST['q_rid'])) {
	   response_purge($_POST['q_rid']);
	}

	$bg = $ESPCONFIG['bgalt_color2'];
?>
<h2><?php echo(_('Purge response')); ?></h2>

	<p><?php echo(_('
This page is not directly on the main menu because it is
dangerous. This <b>completely</b> removes everything about a
response from the database <b>forever</b>. Do
not do anything here that you are not completely certain
about. There is no confirmation, there is no turning
back.')); ?></p>

<table cellspacing="0" cellpadding="4">
	<tr style="background-color: <?php echo($bg); ?>">
		<td><?php echo(_('Response')); echo(_('ID')); ?></td>
	</tr>
<?php
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr style="background-color: <?php echo($bg); ?>">
		<td><input type="text" name="q_rid" value="" /></td>
	</tr>
<?php
	if($bg != $ESPCONFIG['bgalt_color1'])
		$bg = $ESPCONFIG['bgalt_color1'];
	else
		$bg = $ESPCONFIG['bgalt_color2'];
?>
    <tr style="background-color: <?php echo($bg); ?>">
		<td colspan="8">
			<input type="hidden" name="where" value="response_purge" />
			<input type="submit" name="submit" value="<?php echo(_('Purge response')); ?>" />
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
