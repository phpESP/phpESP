<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* This function is a hack to handle input-type=image tags.
 * (e.g.  <input type="image" name="tab" src="clickme.gif"> )
 * You cannot set a 'value' attribute in such a tag. When you
 * click on an image, browsers create two form variables based
 * on the 'name' attribute: <name>_x & <name>_y, the coordinates
 * of where the user clicked on the image.
 *
 * To make this work with my 'where=<module>' wrapper, this
 * function checks for certain variables of the form "blah_x"
 * and "blah_y" and includes 'blah'.
 */

	session_register('survey_id');
	session_register('new_survey');
	session_register('last_tab');
	session_register('curr_q');

	/* Use this hack to work around how PHP handles session vars.
	 * When Register_Globals is on, the value of the global $foo
	 * is saved, and $HTTP_SESSION_VARS['foo'] is ignored.
	 * When Register_Globals is off, the global $foo is ignored
	 * and $HTTP_SESSION_VARS['foo'] is saved.
	 * By making $HTTP_SESSION_VARS['foo'] a reference to $foo
	 * when reg_glob is on, we can use $HTTP_SESSION_VARS uniformly
	 * from here on, regardless of the INI settings.
	 */
	if(ini_get('register_globals')) {
		$HTTP_SESSION_VARS['survey_id']  = &$survey_id;
		$HTTP_SESSION_VARS['new_survey'] = &$new_survey;
		$HTTP_SESSION_VARS['last_tab']   = &$last_tab;
		$HTTP_SESSION_VARS['curr_q']     = &$curr_q;
	}
	
	global $errstr;
	
	if(@$HTTP_GET_VARS['where'] == 'new') {
		$HTTP_SESSION_VARS['survey_id']  = '';
		$HTTP_SESSION_VARS['new_survey'] = '';
		$HTTP_SESSION_VARS['last_tab']   = '';
		$HTTP_SESSION_VARS['curr_q']     = '';
	}		
	
	if(!empty($HTTP_GET_VARS['newid'])) {
		$HTTP_SESSION_VARS['survey_id']  = intval($HTTP_GET_VARS['newid']);
		$HTTP_POST_VARS['tab']           = 'general';
		$HTTP_SESSION_VARS['last_tab']   = '';
		$HTTP_SESSION_VARS['new_survey'] = FALSE;
	}

	if(empty($HTTP_SESSION_VARS['survey_id'])) {
		$HTTP_SESSION_VARS['survey_id']  = '';
		$HTTP_SESSION_VARS['new_survey'] = TRUE;
	}

	if(empty($HTTP_SESSION_VARS['new_survey']))
		$HTTP_SESSION_VARS['new_survey'] = empty($HTTP_SESSION_VARS['survey_id']);
		
	/* check ACL to see if user is allowed to design
	 * _this_ survey */
	if(!empty($HTTP_SESSION_VARS['survey_id'])) {
		$srealm = auth_get_survey_realm($HTTP_SESSION_VARS['survey_id']);
		if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
				!auth_is_owner($HTTP_SESSION_VARS['survey_id'], $HTTP_SESSION_VARS['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$HTTP_SESSION_VARS['acl']['pdesign'],
						$HTTP_SESSION_VARS['acl']['pall'])) &&
				!auth_no_access(_('to access this form'))) {
			return;
		}
	}

	/* check user's ACL to see if they have rights to
	 * edit/create _any_ survey */
	if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
			count($HTTP_SESSION_VARS['acl']['pdesign']) < 1 &&
			!auth_no_access(_('to design forms'))) {
		return;
	}
		

	if(empty($ESPCONFIG['tabs'])) {
		echo('<b>'. _('No tabs defined. Please check your ini settings.'). '</b>');
		return;
	}
	
	$tab = &$HTTP_POST_VARS['tab'];
		
/* XXX I18n do not translate */
	if(stristr($tab,'continue')) {
		switch($HTTP_SESSION_VARS['last_tab']) {
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
/* XXX I18n do not translate */

	if(empty($tab)) {
		foreach($ESPCONFIG['tabs'] as $tab) {
			$x = @$HTTP_POST_VARS["${tab}_x"];
			$y = @$HTTP_POST_VARS["${tab}_y"];
			if(!empty($x) && !empty($y)) {
				break;
			}
			$tab = $HTTP_SESSION_VARS['last_tab'];
		}
	}
	if(empty($tab))
		$tab = 'general';
	$tab = strtolower(ereg_replace(' +','_',$tab));
	if(!ereg('^[A-Za-z0-9_]+$',$tab))	// Valid chars are [A-Za-z0-9_]
		$tab = 'general';
	if(!file_exists($GLOBALS['ESPCONFIG']['include_path'].'/tab/'.$tab.$GLOBALS['ESPCONFIG']['extension']))
		$tab = 'general';
	if(!file_exists($GLOBALS['ESPCONFIG']['include_path'].'/tab/'.$tab.$GLOBALS['ESPCONFIG']['extension'])) {
		echo('<b>'. _('Unable to open include file. Check INI settings. Aborting.'). '</b>');
		exit;
	}
	
	$errstr = '';
	$updated = survey_update($HTTP_SESSION_VARS['survey_id'],$tab,$HTTP_SESSION_VARS['last_tab']);
?>
<h2>Survey Design</h2>
<?php if(!empty($errstr)) echo(mkerror($errstr)."<br>\n"); ?>
<font size="+2">
<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=help"); ?>" target="_blank"><?php echo(_('Help')); ?></a>
</font>

<table border="0" cellspacing="0" cellpadding="0" align="center" width="95%">
  <tr align="left" valign="bottom">
    <td>
      <input type="hidden" name="where" value="tab">
	  <table border="0" cellspacing="0" cellpadding="0">
		<tr>
		  <td><input type="image" border="0" alt="<?php echo(_('General')); ?>"   name="general"   src="<?php echo($ESPCONFIG['image_path']); ?>general_<?php $a='d'; if($tab == 'general') $a='a'; echo($a); ?>.gif"   width="160" height="43"></td>
		  <td><input type="image" border="0" alt="<?php echo(_('Questions')); ?>" name="questions" src="<?php echo($ESPCONFIG['image_path']); ?>questions_<?php $a='d'; if($tab == 'questions') $a='a'; echo($a); ?>.gif" width="186" height="43"></td>
		  <td><input type="image" border="0" alt="<?php echo(_('Order')); ?>"     name="order"     src="<?php echo($ESPCONFIG['image_path']); ?>order_<?php $a='d'; if($tab == 'order') $a='a'; echo($a); ?>.gif"     width="126" height="43"></td>
		  <td><input type="image" border="0" alt="<?php echo(_('Preview')); ?>"   name="preview"   src="<?php echo($ESPCONFIG['image_path']); ?>preview_<?php $a='d'; if($tab == 'preview') $a='a'; echo($a); ?>.gif"   width="160" height="43"></td>
		  <td><input type="image" border="0" alt="<?php echo(_('Finish')); ?>"    name="finish"    src="<?php echo($ESPCONFIG['image_path']); ?>finish_<?php $a='d'; if($tab == 'finish') $a='a'; echo($a); ?>.gif"   width="114" height="43"></td>
		</tr>
	  </table>
    </td>
  </tr>
  <tr valign="top" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>">
    <td>
<?php include($GLOBALS['ESPCONFIG']['include_path'].'/tab/'.$tab.$GLOBALS['ESPCONFIG']['extension']); $HTTP_SESSION_VARS['last_tab'] = $tab; ?>
    </td>
  </tr>
</table>
<?php if($tab=='finish') {echo('&nbsp;'); return;} ?>
<?php if($HTTP_SESSION_VARS['new_survey']) { ?>
<?php echo(_('Click cancel to cancel this survey, or click continue to proceed to the next tab.')); ?><br>
<input type="submit" name="where" value="Cancel">
<?php if($tab=='preview') { ?>
	<input type="submit" name="tab" value="Finish">
<?php 	return; } ?>
<input type="submit" name="tab" value="Continue &gt;">
<?php return; } ?>
<p><?php echo(_('The survey title and other general fields are on the <b>General</b> tab.
Individual survey questions are added and modified on the <b>Questions</b> tab.
Questions may be re-ordered or deleted from the <b>Order</b> tab.
You may see a preview of your survey at any time, by going to the <b>Preview</b>
tab. If you have no further changes click <b>Finish</b> to go back to the
Management Interface.')); ?><br>
<a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=help"); ?>" target="_blank"><?php echo(_('Click here to open the Help window.')); ?></a></p>
<input type="submit" name="tab" value="General">
<input type="submit" name="tab" value="Questions">
<input type="submit" name="tab" value="Order">
<input type="submit" name="tab" value="Preview">
<input type="submit" name="tab" value="Finish">
