<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	$sid = _addslashes($HTTP_SESSION_VARS['survey_id']);

	$sql = "SELECT COUNT(*) FROM question WHERE survey_id='${sid}'
	AND deleted='N' AND type_id='99'";
	$result = mysql_query($sql);
	$num_sections = 1 + mysql_result($result,0,0);
	mysql_free_result($result);

	if(empty($HTTP_POST_VARS['section']))
		$HTTP_POST_VARS['section']=1;
?>
<script language="JavaScript">
<!-- // Begin <?php // This should really go into <head> tag ?>

function other_check(name)
{
  other = name.split("_");
  var f = document.phpesp;
  for (var i=0; i<=f.elements.length; i++) {
    if (f.elements[i].value == "other_"+other[1]) {
      f.elements[i].checked=true;
      break;
    }
  }
}
// End -->
</script>
<?php echo(_('This is a preview of how this survey will look.
In the preview the survey navigation buttons are inactive, use
the section number buttons to view different sections. Some
navigation buttons may not appear on your final survey, depending
on what access it is assigned. The survey will use the background
color of the document in which it is embedded. If you have no
further changes click <b>Finish</b> at the bottom of
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
<?php // lets make the css style available during the preview.
      // this should really go into the head section.....but should still work
        $sql = "SELECT theme FROM survey where id = '${sid}'";
        if ($result = mysql_query($sql)) {
                if (mysql_num_rows($result) > 0) {
                        list($css_file) = mysql_fetch_row($result);
                        mysql_free_result($result);
                        echo("<link rel=\"stylesheet\" href=\"".$GLOBALS['ESPCONFIG']['css_url'].$css_file."\"  type=\"text/css\">\n");                }
        }
?>
<table border="1" cellpadding="2" bgcolor="#FFFFFF">
	<tr>
		<td>
<?php
	survey_render($sid,$HTTP_POST_VARS['section']);
	if ($ESPCONFIG['auth_response']) {
		if ($HTTP_POST_VARS['section'] > 1) {
        	echo('<input type="button" value="' . _('Previous Page'). '"> ');
        }
	    echo ('<input type="button" value="'. _('Save') .'"> ');
	}
	if($HTTP_POST_VARS['section'] != $num_sections) {
		echo('<input type="button" value="'. _('Next Page') .'"> ');
	} else {
		echo('<input type="button" value="'. _('Submit Survey') .'">');
	}
?>
		</td>
	</tr>
</table>
