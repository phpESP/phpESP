<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php if (file_exists(ESP_BASE . '/README')) { ?>
<table bgcolor="#ffffff" border="0"><tr><td><pre>
<?php include(ESP_BASE . '/README'); ?>
</pre></td></tr></table>
<?php
	} else {
		echo('<p>' . mkwarn(_('README not found.')) . "</p>\n");
	}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
