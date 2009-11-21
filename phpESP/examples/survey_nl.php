<?php
// example use of handler.php
$template_file = file_get_contents("http://blahblahblah.site.org/index.php?module=webpage&id=27");
$page_header = preg_replace ('/FORM_BODY.*/s','',$template_file);
$page_header = preg_replace ('/<div class="title">Survey<\/div>/s','',$page_header);
$page_footer = preg_replace ('/.*FORM_BODY/s','',$template_file);

 $sid=0;
 if (isset($_GET['sid'])) $sid=intval($_GET['sid']);
 $lang="nl_NL";
 # this must be included before any output happens
 include("public/phpESP.first.php");

 if ($sid>0) {
    echo $page_header;
    include("public/handler.php");
    echo $page_footer;
 }
?>
