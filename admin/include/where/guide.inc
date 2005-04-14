<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php if (file_exists(ESP_BASE . '/docs/GUIDE')) { ?>
<table><tr><td><pre>
<?php include(ESP_BASE . '/docs/GUIDE'); ?>
</pre></td></tr></table>
<?php
	} else {
		echo('<p>' . mkwarn(_("User's guide not found.")) . "</p>\n");
	}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
