<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php if (file_exists($ESPCONFIG['include_path']."/../../TODO")) { ?>
<table><tr><td><pre>
<?php include($ESPCONFIG['include_path']."/../../TODO"); ?>
</pre></td></tr></table>
<?php
	} else {
		echo('<p>' . mkwarn(_('Todo list not found.')) . "</p>\n");
	}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
?>
