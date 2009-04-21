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

    $_GET['precision'] = intval($_GET['precision']);
    $_GET['totals'] = intval($_GET['totals']);
    $_GET['test'] = intval($_GET['test']);

	// see if surveys have been selected
	if(!empty($_REQUEST['sids'])) {
		$sid = intval($_REQUEST['sids']);
        $_REQUEST['sids'] = htmlspecialchars($_REQUEST['sids']);
		if($_GET['test']) {
			echo("<p><b>". _('Testing Survey...') .'</b> ('. _('SIDS') ." = ". $sid .")</p>\n");
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
			$ret = survey_merge(split("\+",$_REQUEST['sids']),$_GET['precision'],$_GET['totals']);
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
<h2><?php echo(_('Merge Survey Results')); ?></h2>
<?php echo(_('Pick Surveys to Merge')); ?>
</form>
    <div id="error"></div>
<form id="merge" action="<?php echo($GLOBALS['ESPCONFIG']['ME']) ?>?where=merge" method="post">
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
<input type="button" value="<?php echo(_('Merge')); ?>" onclick="merge(this.form.list2)" name="B3" />
    <input type="hidden" id="sids" name="sids" value="">
		</td>
		<td class="center">
		<select multiple="multiple" size="10" name="list2">
		</select>
		</td>
	</tr>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
