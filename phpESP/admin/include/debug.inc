<SCRIPT LANGUAGE="JavaScript">
<!-- // Begin // This should really go into <HEAD> tag

function windowOpener(windowTitle,errMsg) {
  msgWindow=window.open("","displayWindow","menubar=no,alwaysRaised=yes,dependent=yes,width=600,height=500,scrollbars=yes,resizable=yes");
  msgWindow.document.write
      ("<HTML><HEAD><TITLE>"+windowTitle+"</TITLE></HEAD>");
  msgWindow.document.write
      ("<BODY><PRE>"+errMsg+"</PRE></BODY></HTML>");
}

function debugWindow () {
 windowTitle="esp debug window";
 errMsg="<?php
 	echo("\\nSESSIONID: ".addcslashes(session_id(),"\0..\31")."\\n");
	if(SID) echo(addcslashes(join('', file(session_save_path()."/sess_".session_id())),"\0..\31"));
 	echo("\\nHTTP_SESSION_VARS:\\n");
  	foreach ($HTTP_SESSION_VARS as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
 	echo("\\nESPCONFIG:\\n");
  	foreach ($ESPCONFIG as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
 	echo("\\nHTTP_GET_VARS:\\n");
  	foreach ($HTTP_GET_VARS as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
 	echo("\\nHTTP_POST_VARS:\\n");
  	foreach ($HTTP_POST_VARS as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
 	echo("\\nHTTP_SERVER_VARS:\\n");
  	foreach ($HTTP_SERVER_VARS as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
 	echo("\\nHTTP_ENV_VARS:\\n");
  	foreach ($HTTP_ENV_VARS as $key => $value) {
		echo(addcslashes("$key => $value\\n","\0..\31"));
	}
	?>";
 windowOpener(windowTitle, errMsg);
}
// End -->
</SCRIPT>
<input type="button" value="debug" onClick="debugWindow()">	
