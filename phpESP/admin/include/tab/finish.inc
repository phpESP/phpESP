<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	include($PIECES."/update_db".$EXT);	// update the database before finishing
	
	$new_survey=0;

?>
<br>
<b>Survey Completed</b>
<div align="left">
	<p>You have completed this survey design.</p>

	<p>To insert this survey into your web page, copy the text
	below, and paste it into the HTML of your page.</p>

<blockquote><tt>
	&lt;?php $sid=<?php echo($survey_id); ?>;
	include("<?php echo($cf_handler); ?>");?&gt;
</tt></blockquote> 

	<p>You must activate this survey before you can collect
	results. Once a survey is active, you may no longer make any
	changes to it. You may activate this survey by chosing
	<b>Change the Status of an Existing Survey</b> from the
	Mangement Interface.</p>
</div>

<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<br>&nbsp;
