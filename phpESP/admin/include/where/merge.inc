<?php

# $Id$

// Written by Romans Jasins
// <roma@latnet.lv>

	// see if surveys have been selected
	if(!empty($sids)) {
		if($test) {
			echo("<p><b>". _('Testing Survey...') ."</b> (SIDS = $sids)</p>\n");
		}
		
		// turn it into an array, rather than space seperated list
		$sids = split(" ",$sids);
?>
<table bgcolor="<?php echo($cf_active_bgcolor); ?>" border="1" width="95%"><tr><td>
<?php
		$ret = survey_merge($sids,$precision,$totals);
		if(!empty($ret))
			echo("<font color=\"$cf_error_color\">$ret</font>\n");
?>
</td></tr></table>
<?php echo("<a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}
	
	// otherwise:
	// build a table of surveys to choose from ...
	$sql = "SELECT id,name,title,status FROM surveys WHERE NOT (status & '".STATUS_DELETED."') ORDER BY name,id";
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
		location.href = "<?php echo("${ME}?where=merge"); ?>&sids="+sids;
	} else {
		windowTitle="Error!";
		errMsg="You need to select at least two surveys!";
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
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th width="33%"><?php echo(_('List of Surveys')); ?></th>
		<th width="34%">&nbsp;</th>
		<th width="33%"><?php echo(_('Surveys to Merge')); ?></th>
	</tr>
	<tr>
		<td align="center">
		<select multiple size="10" name="list1">
			<?php while(list($sid, $name, $title) = mysql_fetch_row($result)) { ?>
			<option value="<?php echo($sid); ?>"><?php echo($name); ?></option>
			<?php } ?>
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
<?php echo("<a href=\"${ME}?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
