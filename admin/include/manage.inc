<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	@session_destroy();

?>
<h2>Management Interface</h2>
<table border="0" cellspacing="0" cellpadding="0" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr>
		<td>
			<!-- <font size="+1">Choose a function:</font> -->
		</td>
	</tr>
	<tr>
		<td>
			<table border="0" cellspacing="0" cellpadding="0" align="center"><tr><td>
			<ul>
				<li><a href="<?php echo($PHP_SELF); ?>?where=new">New Survey Design</a></li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=edit">Edit an Existing Survey</a></li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=copy">Make a Copy of an Existing Survey</a></li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=status">Change the Status of an Existing Survey</a>
					(active/end/delete)</li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=test">Test a Survey</a></li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=results">View the Results of a Survey</a></li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=TODO">View the list of things still to do</a>
					(development goals)</li>
				<li><a href="<?php echo($PHP_SELF); ?>?where=README">View the README</a> (help)</li>
			</ul>
			</td></tr></table>
		</td>
	</tr>
</table>
