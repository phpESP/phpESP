<?php
    	if (defined('ESP-FIRST-INCLUDED'))
        	return;
	define ('ESP-FIRST-INCLUDED',true);

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

	// submit is the last "page" of the survey
	// so to prevent cookie issues and such
	// we already check here for the captcha and set the cookie
	// since this php page needs to be included at the top of any
	// html code using this survey
        if(!empty($_REQUEST['submit'])) {
		$sid=intval($_POST['sid']);
                $msg = response_check_answers($sid,$_SESSION['rid'],$_SESSION['sec']);

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
