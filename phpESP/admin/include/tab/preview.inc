<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$sql = "SELECT COUNT(*) FROM question WHERE survey_id='${survey_id}'
	AND deleted='N' AND type_id='99'";
	$result = mysql_query($sql);
	$num_sections = 1 + mysql_result($result,0,0);

	if(empty($section))
		$section=1;
?>
<?php echo(_('This is a preview of how the survey will look. In the preview the &quot;Next
Page&quot; and &quot;Submit Survey&quot; are inactive.
The survey will use the background color of the document in which it is
embedded. If you have no further changes click <b>Finish</b> at the bottom of
this page.')); ?>
<?php	if($num_sections > 1) { ?>
<br><b><?php echo(_('Section')); ?>:</b>
<?php		for($i=0;$i<$num_sections;$i++) {	?>
<input type="submit" name="section" value="<?php echo($i+1); ?>">
<?php		}
	} ?>
<input type="hidden" name="where" value="tab">
<input type="hidden" name="old_tab" value="preview">
<hr>
<table border="1" cellpadding="2" bgcolor="#FFFFFF">
	<tr>
		<td>
<?php
	$preview = 1;
	render_survey($survey_id,$section);
?>
<?php if($section != $num_sections) { ?><input type="button" value="Next Page">
<?php } else { ?><input type="button" value="Submit Survey">
<?php } ?>
		</td>
	</tr>
</table>
