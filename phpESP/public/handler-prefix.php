<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

/* When using the authentication for responses you need to include
 * part of the script *before* your template so that the
 * HTTP Auth headers can be sent when needed.
 *
 * So, create your template file with this as the *first*
 * line:
 *   <?php $sid=<SID>; include('.../phpESP/public/handler-prefix.php'); ?>
 * then where you want to include the actual survey use this
 * line:
 *   <?php inlcude('.../phpESP/public/handler.php'); ?>
 *
 * For example, here is a very small template:
 *   <?php $sid=42; include('phpESP/public/handler-prefix.php'); ?>
 *   <html>
 *   <head><title>Example.com</title></head>
 *   <body>
 *   <table border="0" width="800">
 *   <tr><td colspan="2"><img src="example-banner.png"
 *       border="0" width="800" height="42"></td></tr>
 *   <tr><td><?php inlcude('example-sidebar.php'); ?></td>
 *       <td><?php include('phpESP/public/handler.php'); ?>
 *       </td>
 *   </tr>
 *   <tr><td colspan="2"><p align="right"><font size="-2">
 *       Copyright &copy; 2003. Example.com</font></p></td></tr>
 *   </table>
 *   </body>
 *   </html>
 *
 */

    if (defined('ESP-HANDLER-PREFIX'))
        return;

    define('ESP-HANDLER-PREFIX', true);

    if (!defined('ESP_BASE'))
        define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

    require_once(ESP_BASE . '/admin/phpESP.ini.php');
    require_once($ESPCONFIG['include_path']."/funcs".$ESPCONFIG['extension']);

    $GLOBALS['errmsg'] = '';
    session_start();

    if(isset($HTTP_GET_VARS['sid'])) {
        $GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
        return;
    }

    if(isset($HTTP_GET_VARS['results']) || isset($HTTP_POST_VARS['results'])) {
        $GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
        return;
    }

    if (isset($sid) && !empty($sid))
        $sid = intval($sid);
    else if (isset($HTTP_POST_VARS['sid']) && !empty($HTTP_POST_VARS['sid']))
        $sid = intval($HTTP_POST_VARS['sid']);

    if(!isset($sid) || empty($sid)) {
        $GLOBALS['errmsg'] = mkerror(_('Error processing survey: Survey not specified.'));
        return;
    }

    if(empty($HTTP_POST_VARS['userid'])) {
        // find remote user id (takes the first non-empty of the following)
        //  1. a GET variable named 'userid'
        //  2. the REMOTE_USER set by HTTP-Authentication
        //  3. the query string
        //  4. the remote ip address
        if (!empty($HTTP_GET_VARS['userid'])) {
            $HTTP_POST_VARS['userid'] = $HTTP_GET_VARS['userid'];
        } elseif(!empty($_SERVER['REMOTE_USER'])) {
            $HTTP_POST_VARS['userid'] = $HTTP_SERVER_VARS['REMOTE_USER'];
        } elseif(!empty($_SERVER['QUERY_STRING'])) {
            $HTTP_POST_VARS['userid'] = urldecode($_SERVER['QUERY_STRING']);
        } else {
            $HTTP_POST_VARS['userid'] = $_SERVER['REMOTE_ADDR'];
        }
    }

    if(empty($HTTP_POST_VARS['referer']))
        $HTTP_POST_VARS['referer'] = isset($_SERVER['HTTP_REFERER']) ?
            $_SERVER['HTTP_REFERER'] : '';

    if (empty($HTTP_POST_VARS['rid']))
        $HTTP_POST_VARS['rid'] = '';
    else
        $HTTP_POST_VARS['rid'] = intval($HTTP_POST_VARS['rid']) ?
                intval($HTTP_POST_VARS['rid']) : '';

    if($ESPCONFIG['auth_response']) {
        // check for authorization on the survey
        require_once($ESPCONFIG['include_path']."/lib/espauth".$ESPCONFIG['extension']);
        if ($GLOBALS['ESPCONFIG']['auth_mode'] == 'basic') {
            $espuser = ''; $esppass = '';
            if (isset($_SERVER['PHP_AUTH_USER']))
                $espuser = $_SERVER['PHP_AUTH_USER'];
            if (isset($_SERVER['PHP_AUTH_PW']))
                $esppass = $_SERVER['PHP_AUTH_PW'];
        }
        elseif ($GLOBALS['ESPCONFIG']['auth_mode'] == 'form') {
            if (!isset($HTTP_POST_VARS['username'])) {
                $HTTP_POST_VARS['username'] = "";
            }
            if ($HTTP_POST_VARS['username'] != "") {
                $_SESSION['espuser'] = $HTTP_POST_VARS['username'];
            }
            if (isset($_SESSION['espuser'])) {
                $espuser = $_SESSION['espuser'];
            }
            else {
                $espuser = "";
            }

            if (!isset($HTTP_POST_VARS['password'])) {
                $HTTP_POST_VARS['password'] = "";
            }
            if ($HTTP_POST_VARS['password'] != "") {
                $_SESSION['esppass'] = $HTTP_POST_VARS['password'];
            }
            if (isset($_SESSION['esppass'])) {
                $esppass = $_SESSION['esppass'];
            }
            else {
                $esppass = "";
            }

        }

        if(!survey_auth($sid, $espuser, _addslashes($esppass), $_css, $_title))
            return;

        if (auth_get_option('resume')) {
            $HTTP_POST_VARS['rid'] = auth_get_rid($sid, $espuser,
                    $HTTP_POST_VARS['rid']);

            if (!empty($HTTP_POST_VARS['rid']) && (empty($HTTP_POST_VARS['sec']) ||
                    intval($HTTP_POST_VARS['sec']) < 1))
            {
                $HTTP_POST_VARS['sec'] = response_select_max_sec($sid,
                        $HTTP_POST_VARS['rid']);
            }
        }
    }

    if (empty($HTTP_POST_VARS['sec']))
        $HTTP_POST_VARS['sec'] = 1;
    else
        $HTTP_POST_VARS['sec'] = (intval($HTTP_POST_VARS['sec']) > 0) ?
                intval($HTTP_POST_VARS['sec']) : 1;

    define('ESP-AUTH-OK', true);

?>
