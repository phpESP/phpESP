<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	$HTTP_SESSION_VARS['new_survey'] = FALSE;
	$survey_name = "";
    $sql = "SELECT name FROM survey WHERE id = '". $HTTP_SESSION_VARS['survey_id']. "'";
    if ($result = mysql_query($sql)) {
	    if (mysql_num_rows($result) > 0)
		    $survey_name = mysql_result($result, 0, 0);
		mysql_free_result($result);
	}

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

	<p><?php echo(_('Once activated you can also access the survey directly
from the following URL.')); ?></p>
<blockquote><tt>
	<?php echo($ESPCONFIG['auto_handler']."?name=".$survey_name); ?>
</tt></blockquote>

	<p><?php echo(_('You must activate this survey before you can collect
results. Once a survey is active, you may no longer make any
changes to it. You may activate this survey by choosing
<b>Change the Status of an Existing Survey</b> from the
Management Interface.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<br>&nbsp;
