<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

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

	<p><?php echo(_('Next, you should return to the Management Interface to test your survey. <br>
                To do this, you much change the <b>status</b> of your survey to <i>Testing</i>. <br>
                You may then choose to <b>test</b> your survey. <br>
                If anything is incorrect, you can change the status of your survey back to <i>Editing</i> and make changes.
                ')); ?></p>

	<p><?php echo(_('Once you are satisfied with the status of your survey, you can activate it.')); ?></p>

	<p><?php echo(_('You must activate this survey before you can collect results. 
                <br>Once a survey is active, you may no longer make any changes to it. 
<br>You may activate this survey by contacting the Survey Center administrators.
<br>Be sure to have your login and survey name available.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<br>&nbsp;
