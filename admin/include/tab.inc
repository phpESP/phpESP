<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* This function is a hack to handle input-type=image tags.
 * (e.g.  <input type="image" name="tab" src="clickme.gif"> )
 * You can't set a 'value' attribute in such a tag. When you
 * click on an image, browsers create two form variables based
 * on the 'name' attribute: <name>_x & <name>_y, the coordinates
 * of where the user clicked on the image.
 *
 * To make this work with my 'where=<module>' wrapper, this
 * function checks for certain variables of the form "blah_x"
 * and "blah_y" and includes 'blah'.
 */

	if(!isset($cf_tabs))
		return;
		
	if(stristr($tab,"continue")) {
		switch($old_tab) {
			case 'general':
				$tab='questions';
				break;
			case 'questions':
				$tab='order';
				break;
			case 'order':
				$tab='preview';
				break;
			default:
				$tab='general';
		}
	}

	if(empty($tab)) {
		foreach($cf_tabs as $tab) {
			if(!empty(${$tab . "_x"}) && !empty(${$tab . "_y"})) {
				if(!$cf_insecure) {
					if(!ereg('^[A-Za-z0-9_]+$',$tab))	// Valid chars are [A-Za-z0-9_]
						$tab="index";
				}
				break;
			}
			$tab = $old_tab;
		}
	} else {
		$tab = strtolower($tab);
	}
	include($PIECES."/update_db".$EXT);	// update the database before switching tabs
	if(!file_exists($PIECES."/".$tab.$EXT)) {
		include($PIECES."/index".$EXT);
		return;
	}
?>
<h2>Survey Design</h2><font size="+2"><a href="<?php echo($PHP_SELF); ?>?where=help"
		  target="_blank">Help</a></font>


<table border="0" cellspacing="0" cellpadding="0" align="center" width="95%">
  <tr align="left" valign="bottom">
    <td>
      <input type="hidden" name="where" value="tab">
      <input type="hidden" name="old_tab" value="<?php echo($tab); ?>">
	  <table border="0" cellspacing="0" cellpadding="0">
		<tr>
		  <td><input type="image" border="0" name="general"   src="<?php echo($IMAGE_PATH); ?>general_<?php $a='d'; if($tab == 'general') $a='a'; echo($a); ?>.gif"   width="160" height="43"></td>
		  <td><input type="image" border="0" name="questions" src="<?php echo($IMAGE_PATH); ?>questions_<?php $a='d'; if($tab == 'questions') $a='a'; echo($a); ?>.gif" width="186" height="43"></td>
		  <td><input type="image" border="0" name="order"     src="<?php echo($IMAGE_PATH); ?>order_<?php $a='d'; if($tab == 'order') $a='a'; echo($a); ?>.gif"     width="126" height="43"></td>
		  <td><input type="image" border="0" name="preview"   src="<?php echo($IMAGE_PATH); ?>preview_<?php $a='d'; if($tab == 'preview') $a='a'; echo($a); ?>.gif"   width="160" height="43"></td>
		  <td><input type="image" border="0" name="finish"    src="<?php echo($IMAGE_PATH); ?>finish_<?php $a='d'; if($tab == 'finish') $a='a'; echo($a); ?>.gif"   width="114" height="43"></td>
		</tr>
	  </table>
    </td>
  </tr>
  <tr valign="top" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>">
    <td>
<?php include($PIECES."/".$tab.$EXT); ?>
    </td>
  </tr>
</table>
<?php if($tab=='finish') return; ?>
<?php if($new_survey) { ?>
Click cancel to cancel this survey, or click continue to proceed to the next
tab.<br>
<input type="submit" name="where" value="Cancel">
<?php if($tab=='preview') { ?>
	<input type="submit" name="tab" value="Finish">
<?php 	return; } ?>
<input type="submit" name="tab" value="Continue &gt;">
<?php return; } ?>
<p>The survey title and other general fields are on the <b>General</b> tab.
Individual survey questions are added and modified on the <b>Questions</b> tab.
Questions may be re-ordered or deleted from the <b>Order</b> tab.
You may see a preview of your survey at any time, by going to the <b>Preview</b>
tab. If you have no further changes click <b>Finish</b> to go back to the
Management Interface.<br><a href="<?php echo($PHP_SELF); ?>?where=help"
		  target="_blank">Click here to open the Help window</a>.</p>
<input type="submit" name="tab" value="General">
<input type="submit" name="tab" value="Questions">
<input type="submit" name="tab" value="Order">
<input type="submit" name="tab" value="Preview">
<input type="submit" name="tab" value="Finish">
