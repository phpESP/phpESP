<TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0" WIDTH="98%" ALIGN="CENTER" BGCOLOR="<?php echo($GLOBALS['ESPCONFIG']['table_bgcolor']); ?>">
  <TR>
    <TD>
<!--      <P><IMG SRC="<?php
	if(!empty($GLOBALS['ESPCONFIG']['banner'])) {
		echo($GLOBALS['ESPCONFIG']['banner']);
	} else {
		echo($GLOBALS['ESPCONFIG']['name'].".gif");
	}
?>" WIDTH="460" HEIGHT="80"></P> -->
      <P><FONT SIZE="+2"><TT><?php for($i=0;$i<strlen($GLOBALS['ESPCONFIG']['name']);$i++) echo($GLOBALS['ESPCONFIG']['name'][$i] .' '); ?></TT></FONT></P>
      <FORM METHOD="POST" NAME="phpesp" ACTION="<?php echo($GLOBALS['ESPCONFIG']['ME']); ?>">
      <TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0" WIDTH="100%">
        <TR ALIGN="CENTER" VALIGN="MIDDLE">
          <TD>
