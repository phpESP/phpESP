<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

    if (!defined('ESP-FIRST-INCLUDED')) {
        echo "In order to conduct surveys, please include phpESP.first.php in your php script, not handler-prefix.php!";
        exit;
    }

    if (defined('ESP-HANDLER-PREFIX'))
        return;

    define('ESP-HANDLER-PREFIX', true);

    $GLOBALS['errmsg'] = '';

    if(isset($_REQUEST['results']) || isset($_REQUEST['results'])) {
        $GLOBALS['errmsg'] = mkerror(_('Error processing survey: Security violation.'));
        return;
    }

    if (isset($sid) && !empty($sid)) {
        $sid = intval($sid);
    }
    else if (isset($_REQUEST['sid']) && !empty($_REQUEST['sid'])) {
        $sid = intval($_REQUEST['sid']);
    }

    if(!isset($sid) || empty($sid)) {
        blur('/public/dashboard.php');
        assert('false; // NOTREACHED');
    }

    if(!isset($_css)) {
        $_css = "";
    }

    if (!isset($_title)) {
        $_title = "";
    }

    if (!isset($survey_name)) {
        $survey_name = "";
    }

    if (empty($_REQUEST['rid']))
        $request_rid = 0;
    else
        $request_rid = intval($_REQUEST['rid']) ?
                intval($_REQUEST['rid']) : 0;

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
            if (isset($_REQUEST['username']) && ($_REQUEST['username'] != "")) {
                $_SESSION['espuser'] = $_REQUEST['username'];
            }
            if (isset($_SESSION['espuser'])) {
                $espuser = $_SESSION['espuser'];
            }
            else {
                $espuser = "";
            }

            if (isset($_REQUEST['password']) && ($_REQUEST['password'] != "")) {
                $_SESSION['esppass'] = $_REQUEST['password'];
            }
            if (isset($_SESSION['esppass'])) {
                $esppass = $_SESSION['esppass'];
            }
            else {
                $esppass = "";
            }

        }

        if(!survey_auth($sid, $espuser, _addslashes($esppass), $esppass, $_css, $_title))
            return;

        if (auth_get_option('resume')) {
            $_SESSION['rid'] = auth_get_rid($sid, $espuser, $request_rid);

            if (!empty($_SESSION['rid']) && (!isset($_SESSION['sec']) ||
	        	 empty($_SESSION['sec']) || intval($_SESSION['sec']) < 1))
            {
		$section_to_return_to=response_select_max_sec($sid,$_SESSION['rid']);
                // we let people return to previously filled in sections
                // if defined in the URL request
                if (isset($_GET['sec']) && intval($_GET['sec'])>0 &&
		    $_GET['sec']<=$section_to_return_to) {
                    $_SESSION['sec'] = intval($_GET['sec']);
                } else {
                    $_SESSION['sec'] = $section_to_return_to;
                }
            }
        }
    }

    $num_sections = survey_num_sections($sid);
    if (!isset($_SESSION['sec']) || empty($_SESSION['sec']) || $_SESSION['sec']>$num_sections) {
            $_SESSION['sec'] = 1;
        } else {
            $_SESSION['sec'] = (intval($_SESSION['sec']) > 0) ?
                            intval($_SESSION['sec']) : 1;
    }

    define('ESP-AUTH-OK', true);

?>
