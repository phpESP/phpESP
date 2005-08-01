<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

    if (isset($_SERVER))  $s =& $_SERVER;
    //else                  $s =& $HTTP_SERVER_VARS;

    if (isset($s['HTTPS']) && $s['HTTPS'] == 'on') {
        $proto = 'https';
        $port  = 443;
    } else {
        $proto = 'http';
        $port  = 80;
    }

    if (isset($s['SERVER_PORT']) && $s['SERVER_PORT'] != $port) {
        $port = ':' . $s['SERVER_PORT'];
    } else {
        $port = '';
    }

    $url = sprintf('%s://%s%s%s%s', $proto, $s['SERVER_NAME'], $port,
            dirname($s['SCRIPT_NAME']), '/manage.php');

    header("Location: $url");
?>
