<?php

# $Id$

echo("<DIV ALIGN=LEFT><PRE>");
include($ESPCONFIG['include_path']."/../../TODO");
echo("</PRE></DIV>\n");
echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
?>
