<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	// clear the session
	@session_destroy();

	$sql = 'SELECT id,name,title FROM surveys WHERE status=0 ORDER BY id DESC';
	$result = mysql_query($sql);

?>
<h2><?php echo(_('Edit a Survey')); ?></h2>
<?php echo(_('Pick Survey to Edit')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b><?php echo(_('ID')); ?></b></td>
		<td><b><?php echo(_('Name')); ?></b></td>
		<td><b><?php echo(_('Title')); ?></b></td>
	</tr>
<?php
	while(list($sid,$name,$title) = mysql_fetch_row($result)) {
		if($bg != '#ffffff')
			$bg = '#ffffff';
		else
			$bg = '#eeeeee';
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($sid); ?></td>
		<td>
			<a href="<?php echo("${ME}?where=tab&tab=general&newid=${sid}"); ?>">
				<?php echo($name); ?>
			</a>
		</td>
		<td>
			<?php echo($title); ?>
		</td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
