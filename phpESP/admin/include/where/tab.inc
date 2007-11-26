<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

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
	 * is saved, and $_SESSION['foo'] is ignored.
	 * When Register_Globals is off, the global $foo is ignored
	 * and $_SESSION['foo'] is saved.
	 * By making $_SESSION['foo'] a copy of $foo
	 * when reg_glob is on, we can use $_SESSION uniformly
	 * from here on, regardless of the INI settings.
	 */
	if(ini_get('register_globals')) {
		$_SESSION['survey_id']  = intval($survey_id);
		$_SESSION['new_survey'] = $new_survey;
		$_SESSION['last_tab']   = $last_tab;
		$_SESSION['curr_q']     = $curr_q;
	}
	
	global $errstr;
	
	if(@$_GET['where'] == 'new') {
		$_SESSION['survey_id']  = '';
		$_SESSION['new_survey'] = '';
		$_SESSION['last_tab']   = '';
		$_SESSION['curr_q']     = '';
	}		
	
	if(!empty($_GET['newid'])) {
		$_SESSION['survey_id']  = intval($_GET['newid']);
		$_POST['tab']           = 'general';
		$_SESSION['last_tab']   = '';
		$_SESSION['new_survey'] = false;
	}

	if(empty($_SESSION['survey_id'])) {
		$_SESSION['survey_id']  = '';
		$_SESSION['new_survey'] = true;
	}

	if(empty($_SESSION['new_survey']))
		$_SESSION['new_survey'] = empty($_SESSION['survey_id']);
		
	/* check ACL to see if user is allowed to design
	 * _this_ survey */
	if(!empty($_SESSION['survey_id'])) {
		$srealm = auth_get_survey_realm($_SESSION['survey_id']);
		if($_SESSION['acl']['superuser'] != 'Y' &&
				!auth_is_owner($_SESSION['survey_id'], $_SESSION['acl']['username']) &&
				!in_array($srealm, array_intersect(
						$_SESSION['acl']['pdesign'],
						$_SESSION['acl']['pall'])) &&
				!auth_no_access(_('to access this form'))) {
			return;
		}
	}

	/* check user's ACL to see if they have rights to
	 * edit/create _any_ survey */
	if($_SESSION['acl']['superuser'] != 'Y' &&
			count($_SESSION['acl']['pdesign']) < 1 &&
			!auth_no_access(_('to access this form'))) {
    //        echo "SESSION: ".$_SESSION['acl']['superuser'];
		return;
	}
		

	if(empty($ESPCONFIG['tabs'])) {
		echo('<b>'. _('No tabs defined. Please check your INI settings.'). '</b>');
		return;
	}
	
	$tab = &$_POST['tab'];
		
/* XXX I18n do not translate */
	if(stristr($tab,'continue')) {
		switch($_SESSION['last_tab']) {
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
			$x = @$_POST["${tab}_x"];
			$y = @$_POST["${tab}_y"];
			if(!empty($x) && !empty($y)) {
				break;
			}
			$tab = $_SESSION['last_tab'];
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
	$updated = survey_update($_SESSION['survey_id'],$tab,$_SESSION['last_tab']);
?>
</div>
</form>
<form method="post" id="tabs" onsubmit="validate();" action="<?php echo($GLOBALS['ESPCONFIG']['ME']); ?>">
<div>

<h2>Survey Design</h2>
<?php if(!empty($errstr)) echo(mkerror($errstr)."<br />\n"); ?>
<span class="large">
<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=help"); ?>" onclick="window.open(this.href,'_blank');return false;"><?php echo(_('Help')); ?></a>
</span>
      <input type="hidden" name="where" value="tab" />
<div id="tabmenu">
      <ul id="sections">
		  <li><input type="image" alt="<?php echo(_('General')); ?>"   name="general"   src="<?php echo($ESPCONFIG['image_url']); ?>general_<?php $a='d'; if($tab == 'general') $a='a'; echo($a); ?>.gif" /></li>
		  <li><input type="image" alt="<?php echo(_('Questions')); ?>" name="questions" src="<?php echo($ESPCONFIG['image_url']); ?>questions_<?php $a='d'; if($tab == 'questions') $a='a'; echo($a); ?>.gif" /></li>
		  <li><input type="image" alt="<?php echo(_('Order')); ?>"     name="order"     src="<?php echo($ESPCONFIG['image_url']); ?>order_<?php $a='d'; if($tab == 'order') $a='a'; echo($a); ?>.gif" /></li>
		  <li><input type="image" alt="<?php echo(_('Preview')); ?>"   name="preview"   src="<?php echo($ESPCONFIG['image_url']); ?>preview_<?php $a='d'; if($tab == 'preview') $a='a'; echo($a); ?>.gif" /></li>
		  <li><input type="image" alt="<?php echo(_('Finish')); ?>"    name="finish"    src="<?php echo($ESPCONFIG['image_url']); ?>finish_<?php $a='d'; if($tab == 'finish') $a='a'; echo($a); ?>.gif" /></li>
      </ul>
</div>
<table>
  <tr>
    <td>
<?php include($GLOBALS['ESPCONFIG']['include_path'].'/tab/'.$tab.$GLOBALS['ESPCONFIG']['extension']); $_SESSION['last_tab'] = $tab; ?>
    </td>
  </tr>
</table>
<?php if($tab=='finish') {echo('&nbsp;'); return;} ?>
<?php if($_SESSION['new_survey']) { ?>
<?php echo(_('Click cancel to cancel this survey, or click continue to proceed to the next tab.')); ?><br />
<input type="submit" name="where" value="Cancel" onclick='return cancelConfirm()' />
<?php if($tab=='preview') { ?>
	<input type="submit" name="tab" value="Finish" />
<?php 	return; } ?>
<input type="submit" name="tab" value="Continue &gt;" />
<?php return; } ?>
<p><?php echo(_('The survey title and other general fields are on the <strong>General</strong> tab.
Individual survey questions are added and modified on the <strong>Questions</strong> tab.
Questions may be re-ordered or deleted from the <strong>Order</strong> tab.
You may see a preview of your survey at any time, by going to the <strong>Preview</strong>
tab. If you have no further changes click <strong>Finish</strong> to go back to the
Management Interface.')); ?><br />
<a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=help"); ?>" onclick="window.open(this.href,'_blank');return false;"><?php echo(_('Click here to open the Help window.')); ?></a></p>
<input type="submit" name="tab" value="General" />
<input type="submit" name="tab" value="Questions" />
<input type="submit" name="tab" value="Order" />
<input type="submit" name="tab" value="Conditions" />
<input type="submit" name="tab" value="Preview" />
<input type="submit" name="tab" value="Finish" />
</form>
<form method="post" action="" id="null">
<div>
