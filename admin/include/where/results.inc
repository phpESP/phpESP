<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$bg = '';

	// see if a survey has been selected
    $sid = '';
	if (!empty($HTTP_POST_VARS['sid']))
        $sid = intval($HTTP_POST_VARS['sid']);
    elseif (!empty($HTTP_GET_VARS['sid']))
		$sid = intval($HTTP_GET_VARS['sid']);
    
    if (!empty($sid)) {
		$srealm = auth_get_survey_realm($sid);
		if(!empty($HTTP_GET_VARS['test'])) {
			/* check ACL to see if user is allowed to test
			 * _this_ survey */
			if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
					!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
					!in_array($srealm, array_intersect(
							$HTTP_SESSION_VARS['acl']['pdesign'],
							$HTTP_SESSION_VARS['acl']['pall'])) &&
					!auth_no_access(_('to access this survey'))) {
				return;
			}
			echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = $sid)</p>\n");
		} else {
			/* check ACL to see if user is allowed to get results to
			 * _this_ survey */
			if($HTTP_SESSION_VARS['acl']['superuser'] != 'Y' &&
					!auth_is_owner($sid, $HTTP_SESSION_VARS['acl']['username']) &&
					!in_array($srealm, array_intersect(
							$HTTP_SESSION_VARS['acl']['pdata'],
							$HTTP_SESSION_VARS['acl']['pall'])) &&
					!auth_no_access(_('to access this survey'))) {
				return;
			}
		}

?>
<table border="0" cellpadding="4" cellspacing="0" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
<tr><td>
<?php
		$precision = '';
		if(!empty($HTTP_GET_VARS['precision']))
			$precision = intval($HTTP_GET_VARS['precision']);
		$totals = '';
		if(!empty($HTTP_GET_VARS['totals']))
			$totals = intval($HTTP_GET_VARS['totals']);
		$qid = '';
        $cids = '';
        $rid = '';
		if(!empty($HTTP_GET_VARS['qid']))
			$qid = intval($HTTP_GET_VARS['qid']);
        if(!empty($HTTP_GET_VARS['cids']))
            $cids = XADDSLASHES($HTTP_GET_VARS['cids']);
		if(!empty($HTTP_POST_VARS['rid']))
			$rid = intval($HTTP_POST_VARS['rid']);
		elseif(!empty($HTTP_GET_VARS['rid']))
			$rid = intval($HTTP_GET_VARS['rid']);
		if(!empty($rid)) {
			$rid = intval($HTTP_GET_VARS['rid']);
            $ret = survey_results($sid, $precision, 0, '', '', $rid);
        } else {
		    $ret = survey_results($sid, $precision, $totals, $qid,
			    $cids);
        }
		if(!empty($ret))
			echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
?>
</td></tr></table>
<?php
		if(!empty($HTTP_GET_VARS['test'])) {
			echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=test&sid=". $HTTP_GET_VARS['sid'] ."\">". _('Test Survey') ."</a><br>\n");
		}
?>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
<?php
		return;
	}

// No SurveyId specified, so build a table of
// surveys to choose from ...

	/* load names and titles of all surveys available to
	 * _this_ user */
	$statusok = (STATUS_ACTIVE | STATUS_DONE);
	$statusno = STATUS_DELETED;
	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') {
		$sql = "SELECT id,name,title,owner,realm FROM survey
		WHERE NOT (status & $statusno) AND (status & $statusok) ORDER BY id DESC";
	} else {
		$realms = array_to_insql(
			array_intersect(
				$HTTP_SESSION_VARS['acl']['pall'],
				$HTTP_SESSION_VARS['acl']['pdata']));
		$sql = "SELECT id,name,title,owner,realm
			FROM survey WHERE NOT (status & $statusno) AND (status & $statusok) AND (owner = '".
			$HTTP_SESSION_VARS['acl']['username'] ."' || realm $realms) ORDER BY id DESC";
	}
	$result = mysql_query($sql);

?>
<h2><?php echo(_('View Survey Results')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>

<input type="hidden" name="rid" value="">
<input type="hidden" name="sid" value="">
<input type="hidden" name="where" value="results">
<table border="0" cellspacing="0" cellpadding="4" align="center" bgcolor="<?php echo($ESPCONFIG['active_bgcolor']); ?>" width="95%">
	<tr bgcolor="#dddddd">
		<th align="left"><?php echo(_('ID')); ?></th>
		<th align="left"><?php echo(_('Respondent')); ?></th>
		<th align="left"><?php echo(_('Name')); ?></th>
		<th align="left"><?php echo(_('Title')); ?></th>
		<th align="left"><?php echo(_('Owner')); ?></th>
		<th align="left"><?php echo(_('Group')); ?></th>
		<th align="left"><?php echo(_('Resp')); ?></th>
	</tr>
<?php
    $count = 0;
	while(list($sid,$name,$title,$owner,$realm) = mysql_fetch_row($result)) {

		$sql2 = "SELECT count(*) from response where survey_id = $sid";
		$result2 = mysql_query($sql2);
		list($resp) = mysql_fetch_row($result2);
    	mysql_free_result($result2);

		if($bg != $ESPCONFIG['bgalt_color1'])
			$bg = $ESPCONFIG['bgalt_color1'];
		else
			$bg = $ESPCONFIG['bgalt_color2'];
?>
	<tr bgcolor="<?php echo($bg); ?>">
        <td><?php echo($sid); ?></td>
<?php
        // grab all the rid values for the particular survey
        $sql = "SELECT R.id FROM response R ".
                "WHERE R.survey_id = '${sid}' AND R.complete = 'Y' ".
                "ORDER BY R.id";
        $rid_result = mysql_query($sql);
        $count++;
?>
        <td>
            <select name="rid<?php echo $count; ?>" onChange="javascript:this.form.rid.value=this.form.rid<?php echo $count; ?>.options[this.form.rid<?php echo $count; ?>.selectedIndex].value; this.form.sid.value=<?php echo $sid; ?>; this.form.submit(); return false;">
            <option value=""></option>
<?php
        $i = 0;
        while (list($rid) = mysql_fetch_row($rid_result)) {
?>
            <option value="<?php echo $rid; ?>"><?php echo ++$i; ?></option>
<?php
        }
?>
            </select>
        </td>
		<td>
			<a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] ."?where=results&sid=${sid}"); ?>">
				<?php echo($name); ?>
			</a>
		</td>
		<td><?php echo($title); ?></td>
		<td><?php echo($owner); ?></td>
		<td><?php echo($realm); ?></td>
		<td><?php echo($resp); ?></td>
	</tr>
<?php
	}
	mysql_free_result($result);
?>
</table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
