<SCRIPT LANGUAGE="JavaScript">
<!-- // Begin // This should really go into <HEAD> tag

function windowOpener(windowTitle,errMsg) {
  msgWindow=window.open("","displayWindow","menubar=no,alwaysRaised=yes,dependent=yes,width=600,height=500,scrollbars=yes,resizable=yes");
  msgWindow.document.write("<HTML><HEAD><TITLE>"+windowTitle+"</TITLE></HEAD>");
  msgWindow.document.write("<BODY><PRE>"+errMsg+"</PRE></BODY></HTML>");
}

function debugWindow () {
 windowTitle="esp debug window";
 errMsg=<?php
 	echo('"\\nSID: '. session_id() ."\\n\"\n");
	if (file_exists(session_save_path()."/sess_".session_id()))
		echo('+ "\\nSESSION: '.addcslashes(join('', file(session_save_path()."/sess_".session_id())),"\0..\31\\\"")."\\n\"\n");
	
 	echo("+ \"\\nACL:\\n\"\n");
	if(isset($HTTP_SESSION_VARS['acl'])) {
		foreach ($HTTP_SESSION_VARS['acl'] as $key => $value) {
			if(is_array($value)) { $v = '[ '. implode(' ', $value) .' ]'; } else { $v = $value; }
			echo("+ \"" . addcslashes("$key => $v","\0..\31\\\"") ."\\n\"\n");
		}
	}
	
	if(isset($HTTP_SESSION_VARS)) {
	 	echo("+ \"\\nHTTP_SESSION_VARS:\\n\"\n");
	  	foreach ($HTTP_SESSION_VARS as $key => $value) {
			echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
		}
	}
	
 	echo("+ \"\\nESPCONFIG:\\n\"\n");
  	foreach ($ESPCONFIG as $key => $value) {
		echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
	}
 	echo("+ \"\\nHTTP_GET_VARS:\\n\"\n");
  	foreach ($HTTP_GET_VARS as $key => $value) {
		echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
	}
 	echo("+ \"\\nHTTP_POST_VARS:\\n\"\n");
  	foreach ($HTTP_POST_VARS as $key => $value) {
		echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
	}
 	echo("+ \"\\nHTTP_SERVER_VARS:\\n\"\n");
  	foreach ($HTTP_SERVER_VARS as $key => $value) {
		echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
	}
 	echo("+ \"\\nHTTP_ENV_VARS:\\n\"\n");
  	foreach ($HTTP_ENV_VARS as $key => $value) {
		echo("+ \"" . addcslashes("$key => $value","\0..\31\\\"") ."\\n\"\n");
	}
	?>;
 windowOpener(windowTitle, errMsg);
}
// End -->
</SCRIPT>
<form name="debug"><input type="button" value="debug" onClick="debugWindow()"></form>
