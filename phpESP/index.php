<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

if ( isset($HTTP_SERVER_VARS) ) {
    $_SERVER = & $HTTP_SERVER_VARS;
}

if ( !isset($_SERVER) ) {
    exit('PHP Version lower 5.*. Exit!');
}

$s = & $_SERVER;

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

    $dir = dirname($s['SCRIPT_NAME']) == '/' ? '' :
           dirname($s['SCRIPT_NAME']);
    $url = sprintf(
        '%s://%s%s%s%s',
        $proto,
        $s['SERVER_NAME'],
        $port,
        $dir,
        '/admin/manage.php'
    );

    header("Location: $url");
?>
