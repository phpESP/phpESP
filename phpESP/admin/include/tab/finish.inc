<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	$_SESSION['new_survey'] = false;
	$survey_name = "";
    $sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id = ". $_SESSION['survey_id'];
    if ($result = execute_sql($sql)) {
	    if (record_count($result) > 0)
		    $survey_name = $result->fields[0];
		db_close($result);
	}

    // initialize the survey's statistics
    esp_require_once('/lib/espsurveystat');
    survey_stat_init($_SESSION['survey_id']);
?>
<script type="text/javascript">
function validate() {
    return true;
}
</script>
<br />
<strong><?php echo(_('Survey Design Completed')); ?></strong>
<div class="left">
	<p><?php echo(_('You have completed this survey design.')); ?></p>

	<p><?php echo(_('To insert this survey into your web page, copy the text
below, and paste it into the HTML of your page.')); ?></p>

<blockquote><div><tt>
	&lt;?php $sid=<?php echo($_SESSION['survey_id']); ?>;
	include("<?php echo($ESPCONFIG['handler']); ?>");?&gt;
</tt></div></blockquote>

	<p><?php echo(_('Once activated you can also access the survey directly
from the following URL.')); ?></p>
<blockquote><div><tt>
	<?php echo($ESPCONFIG['autopub_url']."?sid=".$_SESSION['survey_id']); ?>
</tt></div></blockquote>

	<p><?php echo(_('You must activate this survey before you can collect
results. Once a survey is active, you may no longer make any
changes to it. You may activate this survey by choosing
<b>Change the Status of an Existing Survey</b> from the
Management Interface.')); ?></p>
</div>

<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<br />&nbsp;
