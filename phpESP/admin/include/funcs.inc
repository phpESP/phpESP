<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

    if (!defined('STATUS_ACTIVE')) {
        define('STATUS_ACTIVE',  0x01);
        define('STATUS_DONE',    0x02);
        define('STATUS_DELETED', 0x04);
        define('STATUS_TEST',    0x08);
    }

    if(get_magic_quotes_gpc()) {
        function _addslashes($a)    { return($a); }
        function _stripslashes($a)  { return(stripslashes($a)); }
    } else {
        function _addslashes($a)    { return(addslashes($a)); }
        function _stripslashes($a)  { return($a); }
    }
    set_magic_quotes_runtime(0);

    function esp_where($where = null)
    {
        $cfg =& $GLOBALS['ESPCONFIG'];
        if(empty($where))
            $where = 'index';
        $where = strtolower(ereg_replace(' +','_',$where));
        if(!ereg('^[A-Za-z0-9_]+$',$where))    // Valid chars are [A-Za-z0-9_]
            $where = 'index';
        if(!file_exists($cfg['include_path'].'/where/'.$where.$cfg['extension']))
            $where = 'index';
        if(!file_exists($cfg['include_path'].'/where/'.$where.$cfg['extension'])) {
            echo('<b>'. _('Unable to open include file. Check INI settings. Aborting.'). '</b>');
            exit;
        }
        return($cfg['include_path'].'/where/'.$where.$cfg['extension']);
    }

    function esp_init_db() {
        $_ext = 'mysql.so';
        if (substr(PHP_OS, 0, 3) == 'WIN') {
            $_ext = 'php_mysql.dll';
        }
        if (!extension_loaded('mysql') && !ini_get('safe_mode')
            && ini_get('enable_dl') && !@dl($_ext)) {
            echo "<b>FATAL: Unable to load the PHP mysql extension ($_ext). Aborting.</b>";
            exit;
        }
        unset($_ext);
    
        $cfg =& $GLOBALS['ESPCONFIG'];
        if (isset($cfg['db_conn']))
            return;

        $cfg['db_conn'] = @mysql_connect(
                $cfg['db_host'], $cfg['db_user'], $cfg['db_pass']);
        if ($cfg['db_conn'] !== false) {
            if (mysql_select_db($cfg['db_name'], $cfg['db_conn']) === false) {
                mysql_close($cfg['db_conn']);
                $cfg['db_conn'] = false;
            }
        }
        if ($cfg['db_conn'] === false) {
            header('HTTP/1.0 503 '. _('Service Unavailable'));
            echo('<html><head><title>HTTP 503 '. _('Service Unavailable') .'</title></head>');
            echo('<body><h1>HTTP 503 '. _('Service Unavailable') .'</h1>');
            echo(mkerror(_('Connection to database failed. Please check configuration.')));
            if ($cfg['DEBUG'])
                echo("<br>\n". mkerror(mysql_errno().": ".mysql_error()));
            echo('</body></html>');
            exit;
        }
    }
    
    // redirect to thank you page for survey ID 'sid'
    // exits PHP!
    function goto_thankyou($sid,$referer) {
        $cfg =& $GLOBALS['ESPCONFIG'];

        $sql = "SELECT thanks_page,thank_head,thank_body FROM ".$cfg['survey_table']." WHERE id='${sid}'";
        $result = mysql_query($sql);
        list($thank_url,$thank_head,$thank_body) = mysql_fetch_row($result);
        mysql_free_result($result);
        if(!empty($thank_url)) {
            if(!headers_sent()) {
                header("Location: $thank_url");
                exit;
            }
?>
<script language="JavaScript">
<!--
window.location="<?php echo($thank_url); ?>"
//-->
</script>
<noscript>
<h2 class="thankhead">Thank You for completing this survey.</h2>
<blockquote class="thankbody">Please click
<a class="returnlink" href="<?php echo($thank_url); ?>">here</a>
to continue.</blockquote>
</noscript>
<?php
            exit;
        }

        if(empty($thank_body) && empty($thank_head)) {
            $thank_body = $GLOBALS['ESPCONFIG']['thank_body'];
            $thank_head = $GLOBALS['ESPCONFIG']['thank_head'];
        }
?>
<h2 class="thankhead"><?php echo($thank_head); ?></h2>
<blockquote class="thankbody"><?php echo($thank_body); ?></blockquote>
<a class="returnlink" href="<?php echo($referer); ?>">Return</a>
<?php
        return;
    }

    function goto_saved($url) {
?>
<blockquote class="thankbody">
<?php echo
_('Your progress has been saved.  You may return at any
time to complete this survey.  To do so, simply bookmark
the link below.  You may be prompted for your username
and password to complete the survey.') ?></blockquote>
<a class="returnlink" href="<?php echo $url; ?>"><?php echo _('Resume survey'); ?></a>
<?php
        return;
    }

    require_once($ESPCONFIG['include_path']."/lib/espcross".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/espauth".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/esphtml.forms".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/esphtml.results".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/espmerge".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/espresponse".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/lib/espsql".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_copy".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_merge".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_purge".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_render".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_report".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_results".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_update".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/survey_export_csv".$ESPCONFIG['extension']);
    require_once($ESPCONFIG['include_path']."/function/account_upload".$ESPCONFIG['extension']);

?>
