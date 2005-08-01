<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by Romans Jasins
// <roma@latnet.lv>

    if(!isset($_GET['precision'])) {
        $_GET['precision'] = "";
    }
    if(!isset($_GET['totals'])) {
        $_GET['totals'] = "";
    }
    if(!isset($_GET['test'])) {
        $_GET['test'] = "";
    }

	// see if surveys have been selected
	if(!empty($_GET['sids'])) {
		$sid = intval($_GET['sids']);
		if($_GET['test']) {
			echo("<p><b>". _('Testing Survey...') .'</b> ('. _('SIDS') ." = ". $_GET['sids'] .")</p>\n");
		}

		/* check ACLs for permissions ...
		 * XXX only check the first of the SIDS for ownership */
		if($_SESSION['acl']['superuser'] == 'Y' ||
				auth_is_owner($sid, $_SESSION['acl']['username']) ||
				($_GET['test'] &&
					$_SESSION['acl']['seeall'] == 'Y') ||
				auth_no_access('to access this survey')) {
?>
<table><tr><td>
<?php
			$ret = survey_merge(split(" ",$_GET['sids']),$_GET['precision'],$_GET['totals']);
			if(!empty($ret))
				echo("<span class=\"error\">$ret</span>\n");
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
	if($_SESSION['acl']['superuser'] == 'Y' ||
		$_SESSION['acl']['seeall'] == 'Y')
		$sql = "SELECT id,name,title,owner FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
			WHERE ".db_bin("status", $statusok) ."
			ORDER BY id DESC";
	else
		$sql = "SELECT id,name,title,owner FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE owner=".
			_addslashes($_SESSION['acl']['username']) ."
			AND ".db_bin("status",$statusok)."
			ORDER BY id DESC";
	$result = execute_sql($sql);

?>
<script type="text/javascript">
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
</script>
<h2><?php echo(_('Merge Survey Results')); ?></h2>
<?php echo(_('Pick Surveys to Merge')); ?>
<table cellspacing="0" cellpadding="4">
	<tr>
		<th><?php echo(_('List of Surveys')); ?></th>
		<th>&nbsp;</th>
		<th><?php echo(_('Surveys to Merge')); ?></th>
	</tr>
	<tr>
		<td class="center">
		<select multiple="multiple" size="10" name="list1">
			<?php while(list($sid, $name, $title, $owner) = fetch_row($result)) { 
                    $result->MoveNext()
                    ?>
			<option value="<?php echo($sid); ?>"><?php echo($name); ?></option>
			<?php } db_close($result); ?>
		</select>
		</td>
		<td class="center">
<input type="button" value="   &gt;&gt;   " onclick="move(this.form.list1,this.form.list2)" name="B1" /><br />
<input type="button" value="   &lt;&lt;   " onclick="move(this.form.list2,this.form.list1)" name="B2" /><br />
<input type="button" value="Merge" onclick="merge(this.form.list2)" name="B3" />
		</td>
		<td class="center">
		<select multiple="multiple" size="10" name="list2">
		<option></option>
		</select>
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
