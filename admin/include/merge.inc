<?php

# $Id$

// Written by Romans Jasins
// <roma@latnet.lv>

	// see if a survey has been selected
	if(!empty($sid)) {
		if(!isset($details))
			$details=1; // by default, show details in report
?>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td>
<?php
	$ret = survey_report($sid,$details);
	if($ret != '')
		echo("<font color=\"$cf_error_color\">$ret</font>\n");
?>
</td></tr></table>
<a href="<?php echo("${ME}?where=merge"); ?>">Go back to Merge Menu</a><br>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	$sql = "SELECT id,name,title,status FROM surveys WHERE NOT (status & '".STATUS_DELETED."') ORDER BY id DESC";
	$result = mysql_query($sql);

?>

<SCRIPT LANGUAGE="JavaScript">


<!-- Begin // This should really go into <HEAD> tag
sortitems = 0;  // Automatically sort items within lists? (1 or 0)

function windowOpener(windowTitle,errMsg) {
  msgWindow=window.open("","displayWindow","menubar=no,alwaysRaised=yes,dependent=yes,width=300,height=200,scrollbars=yes,resizable=no")
  msgWindow.document.write
      ("<HEAD><TITLE>"+windowTitle+"</TITLE></HEAD>")
  msgWindow.document.write
      ("<CENTER><BIG><B>"+errMsg+"</B></BIG></CENTER>")
}

function merge(box) {
if(box.options.length >= 2){
ml = new Array();
for(var i=0; i<box.options.length; i++) {
ml[i] = box.options[i].value;
sidsArray=ml;
sids = sidsArray.join("&sid[]=");
		}
window.open("manage.php?where=results&test=1&sid[]="+sids);
	}
else{
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
if (sortitems) SortD(tbox);
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

function SortD(box)  {
var temp_opts = new Array();
var temp = new Object();
for(var i=0; i<box.options.length; i++)  {
temp_opts[i] = box.options[i];
}
for(var x=0; x<temp_opts.length-1; x++)  {
for(var y=(x+1); y<temp_opts.length; y++)  {
if(temp_opts[x].text > temp_opts[y].text)  {
temp = temp_opts[x].text;
temp_opts[x].text = temp_opts[y].text;
temp_opts[y].text = temp;
temp = temp_opts[x].value;
temp_opts[x].value = temp_opts[y].value;
temp_opts[y].value = temp;
      }
   }
}
for(var i=0; i<box.options.length; i++)  {
box.options[i].value = temp_opts[i].value;
box.options[i].text = temp_opts[i].text;
   }
}
// End -->
</script>


<h2>Surveys Merging</h2>
Pick Surveys to Merge
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($cf_active_bgcolor); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<td><b>Surveys List</b></td>
		<td><b>Buttons</b></td>
		<td><b>Surves to Merge</b></td>
	</tr>
	<tr bgcolor="<?php echo($bg); ?>">
		<td>
		
		<select multiple size="5" name="list1">
			<?php while(list($sid, $name, $title) = mysql_fetch_row($result)) { ?>	
				<option value="<?php echo($sid); ?>"> <?php echo($name); ?> </option>
			<?php } ?>
		</select>	
		</td>
		<td>
<input type="button" value="   >>   " onclick="move(this.form.list1,this.form.list2)" name="B1"><br>
<input type="button" value="   <<   " onclick="move(this.form.list2,this.form.list1)" name="B2"><br>
<input type="button" value="Merge" onclick="merge(this.form.list2)" name="B3"><br>

</td>
<td><select multiple size="5" name="list2">
</select></td>

	</tr>
</table>
<a href="<?php echo("${ME}?where=manage"); ?>">Go back to Management Interface</a>
