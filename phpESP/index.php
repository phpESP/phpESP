<?php

# $Id$

    if (isset($_SERVER))  $s =& $_SERVER;
    else                  $s =& $HTTP_SERVER_VARS;

    $url = 
        ((isset($s['HTTPS']) && $s['HTTPS'] == 'on') ? 'https://' : 'http://') .
        $s['SERVER_NAME'] . dirname($s['SCRIPT_NAME']) . '/admin/manage.php';

    header("Location: $url");
?>
