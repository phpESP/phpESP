<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

$sid = intval($HTTP_GET_VARS['sid']);
if(!empty($HTTP_POST_VARS['sid']))
	$sid = intval($HTTP_POST_VARS['sid']);

if(!$sid) { ?>
<h3><?php echo(_('CSV Report')); ?></h3>
<input type="hidden" name="where" value="csv">
<?php echo(_('SID')); ?>: <input type="text" name="sid">
<input type="submit" value="<?php echo(_('Go')); ?>">
<br>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
	return;
	}


/**
function res_merge(&$dest, &$arr, $num, $prefix = '') {
	while( list($key, $val) = each($arr) ) {
		$foo = join('.', array($prefix, $key));
		if(is_array($val)) {
			res_merge($dest, $val, $num, $foo);
		} else {
			if(!in_array($foo,array_keys($dest)))
				$dest[$foo] = array();
			$dest[$foo][$num] = $val;
		}
	}
}
/**/

function res_merge(&$dest, &$arr, $num) {
	while( list($key, $val) = each($arr) ) {
		if(is_array($val))
			$val = join(',',$val);
		if(!in_array($key,array_keys($dest)))
			$dest[$key] = array();
		$dest[$key][$num] = $val;
	}
}

function res_print(&$table, $num) {
	@reset($table);
	echo("<tr><th>"._('Response')."</th>");
	for($i = 1; $i <= $num; $i++) {
		echo("<th>#$i</th>");
	}
	echo("</tr>\n");
	while( list($key, $val) = each($table)) {
		echo("<tr><th>$key</th>");
		for($i = 0; $i < $num; $i++) {
			echo("<td>". htmlspecialchars(@$val[$i]) ."</td>");
		}
		echo("</tr>\n");
	}
}

function res_csv(&$table, $num) {
	@reset($table);
	echo('"'._('Response').'"');
	for($i = 1; $i <= $num; $i++) {
		echo(",\"#$i\"");
	}
	echo("\n");
	while( list($key, $val) = each($table)) {
		echo("\"$key\"");
		for($i = 0; $i < $num; $i++) {
			echo(",\"". addslashes(@$val[$i]) ."\"");
		}
		echo("\n");
	}
}


	/* check ACLs for permissions */
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
			auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) ||
			($HTTP_GET_VARS['test'] && 
				$HTTP_SESSION_VARS['acl']['seeall'] == 'Y') ||
			auth_no_access('to access this survey')) {

		$table = array();
		$num = 0;

		$sql = "SELECT id FROM response 
			WHERE survey_id='$sid' AND complete='Y'
			ORDER BY submitted";
		$result = mysql_query($sql);
		while($row = mysql_fetch_row($result)) {
			$response = response_select_named($sid, $row[0]);
			res_merge($table, $response, $num++);
		}
		mysql_free_result($result);
?>
<table bgcolor="#ffffff" border="1" width="95%">
<?php
		res_print($table, $num);
?>
</table>
<table bgcolor="#ffffff" border="1" width="95%"><tr><td><pre>
<?php res_csv($table, $num); ?>
</pre></td></tr></table>
<?php } ?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
