<?php
/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James E Flemer
// <jflemer@alum.rpi.edu>

/* This is a script to let users sign-up for respondent accounts.
 * It will ask for the following information:
 *   o Username (*)
 *   o Email Address (*)
 *   o First Name
 *   o Last Name
 *   o Password (*)
 * and create a new respondent in the group $ESPCONFIG['signup_realm'].
 */

  if (!defined('ESP_BASE'))
    define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

        $CONFIG = ESP_BASE . 'admin/phpESP.ini.php';
        $DEFAULT_CONFIG = $CONFIG.'.default';
        $FIXED_CONFIG = $CONFIG.'.fixed';
        if(!file_exists($DEFAULT_CONFIG)) {
                echo("<b>FATAL: Unable to open default config file. Aborting.</b>");
                exit;
        }
        if(!file_exists($CONFIG)) {
                echo("<b>FATAL: Unable to open config file. Aborting.</b>");
                exit;
        }
        if(!file_exists($FIXED_CONFIG)) {
                echo("<b>FATAL: Unable to open fixed config file. Aborting.</b>");
                exit;
        }
        if(!extension_loaded('mysql')) {
                echo('<b>FATAL: Mysql extension not loaded. Aborting.</b>');
                exit;
        }
        require_once($DEFAULT_CONFIG);
        require_once($CONFIG);
        require_once($FIXED_CONFIG);
	
  esp_init_adodb();

  $fields = array(
      'username',
      'password',
      'email',
      'fname',
      'lname',
    );
  
  $rqd_fields = array(
      'username',
      'password',
      'password2',
      'email',
    );
  
  /* Set this value to override value from phpESP.ini. */
  $signup_realm = null;
  
  /* Make this false to generate full HTML, rather than embedable. */
  $embed = true;

  //$post =& $GLOBALS['HTTP_POST_VARS'];
  $post =& $_POST;
  unset($msg);
  
  /* sanity check the signup_realm */
  if ($signup_realm == null || empty($signup_realm))
    $signup_realm = $GLOBALS['ESPCONFIG']['signup_realm'];
  if ($signup_realm == null || empty($signup_realm)) {
    echo mkerror(_('Sorry, the account request form is disabled.'));
    return;
  }
  
  /* process form values */
  do if (isset($post['submit'])) {
    /* check for required fields */
    foreach ($rqd_fields as $f) {
      if (!isset($post[$f]) || empty($post[$f])) {
        $msg = '<font color="red">'. _('Please complete all required fields.') . '</font>';
        break;
      }
    }
    if (isset($msg))
      break;
    
    /* make sure passwords match */
    if ($post['password'] != $post['password2']) {
      $msg = '<font color="red">'. _('Passwords do not match.') . '</font>';
      break;
    }
    
    /* prepare sql statement */
    $sqlf = array();
    $sqlv = array();
    
    foreach ($fields as $f) {
    	if (isset($post[$f]) && !empty($post[$f])) {
        	array_push($sqlf, $f);
        	if ($f == 'password') {
          		array_push($sqlv, db_crypt(_addslashes($post[$f])));
        	}
        	else {
          		array_push($sqlv,  _addslashes($post[$f]) );
      		}
    	}
    }
    array_push($sqlf, 'realm');
    array_push($sqlv, _addslashes($signup_realm) );

    $sqlf = implode(',', $sqlf);
    $sqlv = implode(',', $sqlv);
    
    $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['respondent_table']." ($sqlf) VALUES ($sqlv)";
    
    /* execute statement */
    $res = execute_sql($sql);
    if (!$res) {
      $msg = '<font color="red">'. _('Request failed, please choose a different username.') .'</font>';
      if ($GLOBALS['ESPCONFIG']['DEBUG'])
        $msg .= mkerror(ErrorNo() . ': ' . ErrorMsg());
      break;
    }
    
    $msg = '<font color="blue">'. 
        sprintf(_('Your account, %s, has been created!'), htmlspecialchars($post['username'])) . '</font>';

    foreach ($fields as $f) {
      $post[$f] = null;
      unset($post[$f]);
    }
  } while(0);
  
  $rqd = '<font color="red">*</font>';
?>
<?php if (!$embed) { ?>
<html>
<head>
<title><?php echo _('Account Request Form'); ?></title>
<?php
    if(!empty($GLOBALS['ESPCONFIG']['favicon'])) {
        echo("<link rel=\"shortcut icon\" href=\"" . $GLOBALS['ESPCONFIG']['favicon'] . "\" />\n");
    }
?>
</head>
<body>
<?php } // !$embed ?>
<form method="post">
  <p><?php printf(
_('Please complete the following form to request an account.
Items marked with a %s are required.'), $rqd); ?></p>
<?php if (isset($msg) && !empty($msg)) echo "<p>$msg</p>\n"; ?>
  <table border="0"><tbody align="left">
  <tr>
    <td>&nbsp;</td>
    <th><?php echo _('First Name'); ?>:</th>
    <td><?php echo mktext('fname', 16, 16); ?></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
    <th><?php echo _('Last Name'); ?>:</th>
    <td><?php echo mktext('lname', 24, 24); ?></td>
  </tr>
  <tr>
    <td><?php echo $rqd; ?></td>
    <th><?php echo _('Email Address'); ?>:</th>
    <td><?php echo mktext('email', 30, 64); ?></td>
  </tr>
  <tr>
    <td><?php echo $rqd; ?></td>
    <th><?php echo _('Username'); ?>:</th>
    <td><?php echo mktext('username', 16, 16); ?></td>
  </tr>
  <tr>
    <td><?php echo $rqd; ?></td>
    <th><?php echo _('Password'); ?>:</th>
    <td><?php echo mkpass('password'); ?></td>
  </tr>
  <tr>
    <td><?php echo $rqd; ?></td>
    <th><?php echo _('Confirm Password'); ?>:</th>
    <td><?php echo mkpass('password2'); ?></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
    <td colspan="2"><?php echo mksubmit('submit'); ?></td>
  </tr>
  </tbody></table>
</form>
<?php if (!$embed) { ?>
</body>
</html>
<?php } // !$embed ?>
