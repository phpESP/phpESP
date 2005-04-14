<?php
/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */
?>
<div id="main">
  <div id="header">
	<!--<p><img src="<?php
	if(!empty($GLOBALS['ESPCONFIG']['banner'])) {
		echo($GLOBALS['ESPCONFIG']['banner']);
	} else {
		echo($GLOBALS['ESPCONFIG']['name'].".gif");
	}
	?>" width="460" height="80"></p> -->
      <p><tt><?php for($i=0;$i<strlen($GLOBALS['ESPCONFIG']['name']);$i++) echo($GLOBALS['ESPCONFIG']['name'][$i] .' '); ?></tt></p>
  </div> <!-- end header div -->
  <div id="body">
      <div class="contents">
      <form method="post" id="phpesp" action="<?php echo($GLOBALS['ESPCONFIG']['ME']); ?>">
	<div>
