<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$HTTP_SESSION_VARS['new_survey'] = FALSE;

?>
<br>
<b><?php echo(_('Survey Design Completed')); ?></b>
<div align="left">
	<p><?php echo(_('You have completed this survey design.')); ?></p>

	<p><?php echo(_('To insert this survey into your web page, copy the text
below, and paste it into the HTML of your page.')); ?></p>

<blockquote><tt>
	&lt;?php $sid=<?php echo($HTTP_SESSION_VARS['survey_id']); ?>;
	include("<?php echo($ESPCONFIG['handler']); ?>");?&gt;
</tt></blockquote> 

	<p><?php echo(_('You must activate this survey before you can collect
results. Once a survey is active, you may no longer make any
changes to it. You may activate this survey by chosing
<b>Change the Status of an Existing Survey</b> from the
Mangement Interface.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<br>&nbsp;
