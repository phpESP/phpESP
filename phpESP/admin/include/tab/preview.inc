<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	$sid = _addslashes($_SESSION['survey_id']);

	$sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id=${sid}
	AND deleted='N' AND type_id='99'";
	$result = execute_sql($sql);
	$num_sections = 1 + $result->fields[0];
	db_close($result);

	if(empty($_POST['section']))
		$_POST['section']=1;
?>
<script type="text/javascript">
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
function validate() {
    return true;
}


// End -->
</script>
<?php echo(_('This is a preview of how this survey will look.
The survey will use the background
color of the document in which it is embedded. If you have no
further changes click <b>Finish</b> at the bottom of
this page.')); ?>
<?php	if($num_sections > 1) { ?>
<br /><strong><?php echo(_('Section')); ?>:</strong>
<?php		for($i=0;$i<$num_sections;$i++) {	?>
<input type="submit" name="section" value="<?php echo($i+1); ?>" />
<?php		}
	} ?>
<input type="hidden" name="where" value="tab" />
<input type="hidden" name="old_tab" value="preview" />
<hr />
<?php // lets make the css style available during the preview.
      // this should really go into the head section.....but should still work
        $sql = "SELECT theme FROM ".$GLOBALS['ESPCONFIG']['survey_table']." where id = ${sid}";
        if ($result = execute_sql($sql)) {
                if (record_count($result) > 0) {
                        list($css_file) = fetch_row($result);
                        db_close($result);
                        echo("<link rel=\"stylesheet\" href=\"".$GLOBALS['ESPCONFIG']['css_url'].$css_file."\"  type=\"text/css\">\n");                }
        }
?>
<table style="border: 1px solid black;" cellpadding="2">
	<tr>
		<td>
<?php
	survey_render($sid,$_POST['section']);
#	if ($ESPCONFIG['auth_response']) {
#		if ($_POST['section'] > 1) {
#        	echo('<input type="button" value="' . _('Previous Page'). '" /> ');
#        }
#	    echo ('<input type="button" value="'. _('Save') .'" /> ');
#	}
#	if($_POST['section'] != $num_sections) {
#		echo('<input type="button" value="'. _('Next Page') .'" /> ');
#	} else {
#		echo('<input type="button" value="'. _('Submit Survey') .'" />');
#	}
?>
		</td>
	</tr>
</table>
