<!-- $Id$ -->
<TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0" WIDTH="98%" ALIGN="CENTER" BGCOLOR="<?php echo($cf_table_bgcolor); ?>">
  <TR>
    <TD>
<!--      <P><IMG SRC="<?php if($cf_banner) {echo($cf_banner);} else {echo($NAME.".gif");} ?>" WIDTH="460" HEIGHT="80"></P> -->
      <P><FONT SIZE="+2"><TT><?php for($i=0;$i<strlen($NAME);$i++) echo($NAME[$i] .' '); ?></TT></FONT></P>
      <FORM METHOD="POST" ACTION="<?php echo($ME); ?>">
      <TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0" WIDTH="100%">
        <TR ALIGN="CENTER" VALIGN="MIDDLE">
          <TD>
