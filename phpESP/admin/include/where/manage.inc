<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$base =& $GLOBALS['ESPCONFIG']['ME'];

?>
<h2><?php echo(_('Management Interface')); ?></h2>
<p><?php echo(_('Username') .': <tt>'. $HTTP_SESSION_VARS['acl']['username'] .'</tt>'); ?></p>
<table border="0" cellspacing="0" cellpadding="0" align="center" bgcolor="<?php echo($GLOBALS['ESPCONFIG']['active_bgcolor']); ?>" width="95%">
	<tr>
		<td>
			<!-- <font size="+1"><?php echo(_('Choose a function')); ?>:</font> -->
		</td>
	</tr>
	<tr>
		<td>
			<table border="0" cellspacing="0" cellpadding="0" align="center"><tr><td>
			<ul>
				<li><a href="<?php echo("${base}?where=new");     ?>"><?php echo(_('New Survey Design')); ?></a></li>
				<li><a href="<?php echo("${base}?where=edit");    ?>"><?php echo(_('Edit an Existing Survey')); ?></a></li>
				<li><a href="<?php echo("${base}?where=copy");    ?>"><?php echo(_('Make a Copy of an Existing Survey')); ?></a></li>
				<li><a href="<?php echo("${base}?where=status");  ?>"><?php echo(_('Change the Status of an Existing Survey')); ?></a>
					<?php echo(_('(active/end/delete)')); ?></li>
				<li><a href="<?php echo("${base}?where=test");    ?>"><?php echo(_('Test a Survey')); ?></a></li>
				<li><a href="<?php echo("${base}?where=results"); ?>"><?php echo(_('View the Results of a Survey')); ?></a></li>
				<li><a href="<?php echo("${base}?where=merge");   ?>"><?php echo(_('Merge the results from two Surveys')); ?></a></li>
				<li><a href="<?php echo("${base}?where=report");  ?>"><?php echo(_('View a survey report')); ?></a></li>
			</ul>
			<ul>
				<li><a href="<?php echo("${base}?where=passwd");  ?>"><?php echo(_('Change your password')); ?></a></li>
				<li><a href="<?php echo("${base}?where=managers");?>"><?php echo(_('Manage Designer Accounts')); ?></a></li>
				<li><a href="<?php echo("${base}?where=users");   ?>"><?php echo(_('Manage Respondent Accounts')); ?></a></li>
			</ul>
			<ul>
				<li><a href="<?php echo("${base}?where=TODO");    ?>"><?php echo(_('View the list of things still to do')); ?></a>
					<?php echo(_('(development goals)')); ?></li>
				<li><a href="<?php echo("${base}?where=README");  ?>"><?php echo(_('View the README')); ?></a>
					<?php echo(_('(help)')); ?></li>
			</ul>
			<ul>
				<li><a href="<?php echo("${base}?where=logout");  ?>"><?php echo(_('Log out')); ?></a></li>
			</ul>
			</td></tr></table>
		</td>
	</tr>
</table>
&nbsp;
