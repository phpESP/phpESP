<?php
        if (!defined('ESP_BASE'))
                define('ESP_BASE', dirname(dirname(__FILE__)) .'/');

        $CONFIG = ESP_BASE . 'admin/phpESP.ini.php';
        if(!file_exists($CONFIG)) {
                echo("<b>FATAL: Unable to open config file Aborting.</b>");
                exit;
        }
        if(!extension_loaded('mysql')) {
                echo('<b>FATAL: Mysql extension not loaded. Aborting.</b>');
                exit;
        }
        require_once($CONFIG);

        esp_init_adodb();
	if (isset($_REQUEST['lang'])) {
		esp_setlocale_ex($_REQUEST['lang']);
	}
	if (isset($lang)) {
		esp_setlocale_ex($lang);
	}


	if (!isset($_SESSION)) session_start();

        if(!empty($_REQUEST['submit'])) {
		$sid=intval($_POST['sid']);
                $msg = response_check_answers($sid,$_REQUEST['sec']);

		if ($ESPCONFIG['use_captcha']) {
        		require_once(ESP_BASE.'public/captcha_check.php');
			$msg .= response_check_captcha("captcha_check",0);
		}

		// if the parameter test is set in the URL
		// and the survey is in fact in the test stage
		// then don't set the cookie
		if (isset($_REQUEST['test'])) {
			$sql = "SELECT status, name FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE id=${sid}";
			$result = execute_sql($sql);
			if ($result && record_count($result) > 0) {
			    list ($status, $name) = fetch_row($result);
			} else {
			    $status = 0;
			}
			if ($status & STATUS_TEST) {
				$test = 1;
			} else {
				$test = 0;
			}
		} else {
			$test = 0;
		}

                if(empty($msg) && !$test) {
                        // Added for cookie auth, to eliminate double submits
                        $cookiename="survey_".$sid;
                        $expire=time()+60*60*24*$GLOBALS['ESPCONFIG']['limit_double_postings'];
                        $res=setcookie($cookiename,"done",$expire,"/");
                }
        }


?>
