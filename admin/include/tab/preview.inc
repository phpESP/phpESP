<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$sid = XADDSLASHES($HTTP_SESSION_VARS['survey_id']);

	$sql = "SELECT COUNT(*) FROM question WHERE survey_id='${sid}'
	AND deleted='N' AND type_id='99'";
	$result = mysql_query($sql);
	$num_sections = 1 + mysql_result($result,0,0);
	mysql_free_result($result);

	if(empty($HTTP_POST_VARS['section']))
		$HTTP_POST_VARS['section']=1;
?>
<SCRIPT LANGUAGE="JavaScript">
<!-- // Begin // This should really go into <HEAD> tag

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
</SCRIPT>
<?php echo(_('This is a preview of how the survey will look.
In the preview the &quot;Next Page&quot; and 
&quot;Submit Survey&quot; are inactive.
The survey will use the background color of the
document in which it is embedded. If you have no
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
                        echo("<link rel=\"stylesheet\" href=\"".$GLOBALS['ESPCONFIG']['survey_css_dir'].$css_file."\"  type=\"text/css\">\n");                }
        }
?>
<table border="1" cellpadding="2" bgcolor="#FFFFFF">
	<tr>
		<td>
<?php
	render_survey($sid,$HTTP_POST_VARS['section']);
	if($HTTP_POST_VARS['section'] != $num_sections) {
		echo('<input type="button" value="'. _('Next Page') .'">');
	} else {
		echo('<input type="button" value="'. _('Submit Survey') .'">');
	}
?>
		</td>
	</tr>
</table>
