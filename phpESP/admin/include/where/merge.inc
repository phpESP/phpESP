<?php

# $Id$

// Written by Romans Jasins
// <roma@latnet.lv>

	// see if surveys have been selected
	if(!empty($HTTP_GET_VARS['sids'])) {
		$sid = intval($HTTP_GET_VARS['sids']);
		if($HTTP_GET_VARS['test']) {
			echo("<p><b>". _('Testing Survey...') .'</b> ('. _('SIDS') ." = ". $HTTP_GET_VARS['sids'] .")</p>\n");
		}

		/* check ACLs for permissions ...
		 * XXX only check the first of the SIDS for ownership */
		if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
				auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) ||
				($HTTP_GET_VARS['test'] &&
					$HTTP_SESSION_VARS['acl']['seeall'] == 'Y') ||
				auth_no_access('to access this survey')) {
?>
<table bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" border="1" width="95%"><tr><td>
<?php
			$ret = survey_merge(split(" ",$HTTP_GET_VARS['sids']),$HTTP_GET_VARS['precision'],$HTTP_GET_VARS['totals']);
			if(!empty($ret))
				echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		}
		return;
	}

	// otherwise:
	// build a table of surveys to choose from ...
	/* check with ACL for allowed surveys */
	$statusok = (STATUS_ACTIVE | STATUS_DONE & ~STATUS_DELETED);
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
		$HTTP_SESSION_VARS['acl']['seeall'] == 'Y')
		$sql = "SELECT id,name,title,owner FROM survey
			WHERE (status & $statusok)
			ORDER BY id DESC";
	else
		$sql = "SELECT id,name,title,owner FROM survey WHERE owner='".
			$HTTP_SESSION_VARS['acl']['username'] ."'
			AND (status & $statusok)
			ORDER BY id DESC";
	$result = mysql_query($sql);

?>
<SCRIPT LANGUAGE="JavaScript">
<!-- // Begin // This should really go into <HEAD> tag

function windowOpener(windowTitle,errMsg) {
  msgWindow=window.open("","displayWindow","menubar=no,alwaysRaised=yes,dependent=yes,width=300,height=200,scrollbars=yes,resizable=no");
  msgWindow.document.write
      ("<HTML><HEAD><TITLE>"+windowTitle+"</TITLE></HEAD>");
  msgWindow.document.write
      ("<BODY><CENTER><BIG><B>"+errMsg+"</B></BIG></CENTER></BODY></HTML>");
}

function merge(box) {
	if(box.options.length >= 2){
		ml = new Array();
		for(var i=0; i<box.options.length; i++) {
			ml[i] = box.options[i].value;
			sidsArray=ml;
		}
		sids = sidsArray.join("+");
		location.href = "<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=merge"); ?>&sids="+sids;
	} else {
		windowTitle="<?php echo(_('Error!')); ?>";
		errMsg="<?php echo(_('You need to select at least two surveys!')); ?>";
		windowOpener(windowTitle,errMsg);
	}
}

function move(fbox,tbox) {
	for(var i=0; i<fbox.options.length; i++) {
		if(fbox.options[i].selected && fbox.options[i].value != "") {
			var no = new Option();
			no.value = fbox.options[i].value;
			no.text = fbox.options[i].text;
			tbox.options[tbox.options.length] = no;
			fbox.options[i].value = "";
			fbox.options[i].text = "";
		}
	}
	BumpUp(fbox);
}

function BumpUp(box)  {
	for(var i=0; i<box.options.length; i++) {
		if(box.options[i].value == "")  {
			for(var j=i; j<box.options.length-1; j++)  {
				box.options[j].value = box.options[j+1].value;
				box.options[j].text = box.options[j+1].text;
			}
			var ln = i;
			break;
		}
	}
	if(ln < box.options.length)  {
		box.options.length -= 1;
		BumpUp(box);
	}
}
// End -->
</SCRIPT>
<h2><?php echo(_('Merge Survey Results')); ?></h2>
<?php echo(_('Pick Surveys to Merge')); ?>
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th width="33%"><?php echo(_('List of Surveys')); ?></th>
		<th width="34%">&nbsp;</th>
		<th width="33%"><?php echo(_('Surveys to Merge')); ?></th>
	</tr>
	<tr>
		<td align="center">
		<select multiple size="10" name="list1">
			<?php while(list($sid, $name, $title, $owner) = mysql_fetch_row($result)) { ?>
			<option value="<?php echo($sid); ?>"><?php echo($name); ?></option>
			<?php } mysql_free_result($result); ?>
		</select>
		</td>
		<td align="center">
<input type="button" value="   &gt;&gt;   " onclick="move(this.form.list1,this.form.list2)" name="B1"><br>
<input type="button" value="   &lt;&lt;   " onclick="move(this.form.list2,this.form.list1)" name="B2"><br>
<input type="button" value="Merge" onclick="merge(this.form.list2)" name="B3">
		</td>
		<td align="center">
		<select multiple size="10" name="list2">
		</select>
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
