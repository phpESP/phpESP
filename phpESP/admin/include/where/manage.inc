<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	@session_destroy();

?>
<h2><?php echo(_('Management Interface')); ?></h2>
<table border="0" cellspacing="0" cellpadding="0" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr>
		<td>
			<!-- <font size="+1"><?php echo(_('Choose a function')); ?>:</font> -->
		</td>
	</tr>
	<tr>
		<td>
			<table border="0" cellspacing="0" cellpadding="0" align="center"><tr><td>
			<ul>
				<li><a href="<?php echo("${ME}?where=new");     ?>"><?php echo(_('New Survey Design')); ?></a></li>
				<li><a href="<?php echo("${ME}?where=edit");    ?>"><?php echo(_('Edit an Existing Survey')); ?></a></li>
				<li><a href="<?php echo("${ME}?where=copy");    ?>"><?php echo(_('Make a Copy of an Existing Survey')); ?></a></li>
				<li><a href="<?php echo("${ME}?where=status");  ?>"><?php echo(_('Change the Status of an Existing Survey')); ?></a>
					<?php echo(_('(active/end/delete)')); ?></li>
				<li><a href="<?php echo("${ME}?where=test");    ?>"><?php echo(_('Test a Survey')); ?></a></li>
				<li><a href="<?php echo("${ME}?where=results"); ?>"><?php echo(_('View the Results of a Survey')); ?></a></li>
				<li><a href="<?php echo("${ME}?where=TODO");    ?>"><?php echo(_('View the list of things still to do')); ?></a>
					<?php echo(_('(development goals)')); ?></li>
				<li><a href="<?php echo("${ME}?where=README");  ?>"><?php echo(_('View the README')); ?></a>
					<?php echo(_('(help)')); ?></li>
			</ul>
			</td></tr></table>
		</td>
	</tr>
</table>
