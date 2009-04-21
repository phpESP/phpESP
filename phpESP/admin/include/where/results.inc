<?php

    /* $Id$ */

    /* vim: set tabstop=4 shiftwidth=4 expandtab: */

    // Written by James Flemer
    // For eGrad2000.com
    // <jflemer@alum.rpi.edu>

    $bg = '';
    require ($ESPCONFIG['include_path']."where/crosstab.inc");
    // see if a survey has been selected
    $sid = '';
    if (!empty($_POST['sid']))
    $sid = intval($_POST['sid']);
    elseif (!empty($_GET['sid']))
    $sid = intval($_GET['sid']);

    if (!empty($sid)) {
        $srealm = auth_get_survey_realm(_addslashes($sid));
        if(!empty($_GET['test'])) {
            /* check ACL to see if user is allowed to test
            * _this_ survey */
            if($_SESSION['acl']['superuser'] != 'Y' &&
            !auth_is_owner($sid, $_SESSION['acl']['username']) &&
            !in_array($srealm, array_intersect(
                $_SESSION['acl']['pdesign'],
                $_SESSION['acl']['pall'])) &&
                !auth_no_access(_('to access this survey'))) {
                    return;
                }
                echo("<p><b>". _('Testing Survey...') ."</b> (". _('SID') ." = $sid)</p>\n");
        } else {
            /* check ACL to see if user is allowed to get results to
            * _this_ survey */
            if($_SESSION['acl']['superuser'] != 'Y' &&
            !auth_is_owner($sid, $_SESSION['acl']['username']) &&
            !in_array($srealm, array_intersect(
                $_SESSION['acl']['pdata'],
                $_SESSION['acl']['pall'])) &&
                !auth_no_access(_('to access this survey'))) {
                    return;
                }
        }
        if (isset($_GET['type'])) {
            if ($_GET['type'] == "cross") {
                echo('<h2>'. _('Cross Tabulation') ."</h2>\n");
            }
        }
    ?>
    <font size="+2">
<a href="<?php echo($GLOBALS['ESPCONFIG']['ME']."?where=help"); ?>" target="_blank"><?php echo _('Help'); ?></a>
    </font>
    <br /><br />
<table class="results" cellpadding="4" cellspacing="0">
    <tr><td>
    <?php
        $precision = '';
        if (!empty($_GET['precision']))
           $precision = intval($_GET['precision']);
        $totals = '';
        if (!empty($_GET['totals']))
           $totals = intval($_GET['totals']);
        $qid = '';
        $cids = '';
        $rid = '';
        $qidc = '';
        $qidr = '';
        $guicross = '';
        if (!empty($_GET['qid']))
           $qid = intval($_GET['qid']);
        if (!empty($_GET['cids']))
           $cids = _addslashes($_GET['cids']);
        if (!empty($_POST['cids']))
            $cids = $_POST['cids'];     
        if (!empty($_POST['qid']))
           $qid = intval($_POST['qid']);
        if (!empty($_POST['qidc']))
           $qidc = intval($_POST['qidc']);
        if (!empty($_POST['qidr']))
           $qidr = intval($_POST['qidr']);
        if (!empty($_POST['rid']))
           $rid = intval($_POST['rid']);
        elseif (!empty($_GET['rid']))
           $rid = intval($_GET['rid']);
        if (!empty($_GET['type']))
           $guicross = $_GET['type'];
        if (!empty($rid)) {
            $ret = survey_results($sid, $precision, 0, '', '', $rid);
        } else if (!empty($guicross)){
            $ret = survey_results($sid, $precision, $totals, '','','', $guicross);
        } else if (!empty($qidc) || !empty($qidr)){
            cross_tabulation($sid, $qidc, $qidr, $qid, $cids);
        } else {
            $ret = survey_results($sid, $precision, $totals, $qid, $cids);
        }
        if(!empty($ret))
           echo("<font color=\"". $ESPCONFIG['error_color'] ."\">$ret</font>\n");
    ?>
    <?php
        if(!empty($_GET['test'])) {
            echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=test&test=1&sid=". $_GET['sid']) ."\">". _('Test Survey') ."</a><br>\n");
        }


        if (!empty($_GET['type'])) {
        ?>
        </td></tr>
        <tr><td align="center">
    <input type="reset" value="<?php echo _('Reset'); ?>" />
    <?php echo mksubmit('cross', _('Cross Tabulate')); ?>
        <br />
        <?php
        }
    ?>
    </td></tr></table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
    <?php
        return;
    }

    // No SurveyId specified, so build a table of
    // surveys to choose from ...

    /* load names and titles of all surveys available to
    * _this_ user */
    $statusok = (STATUS_ACTIVE | STATUS_DONE | STATUS_TEST);
    $statusno = STATUS_DELETED;
    if ($_SESSION['acl']['superuser'] == 'Y') {
        $sql = "SELECT id,name,title,owner,realm FROM ".$GLOBALS['ESPCONFIG']['survey_table']."
        WHERE NOT ".db_bin("status", $statusno)." AND ".db_bin("status", $statusok)." ORDER BY id DESC";
    } else {
        $realms = array_to_insql(
            array_intersect(
                $_SESSION['acl']['pall'],
                $_SESSION['acl']['pdata']));
                $sql = "SELECT id,name,title,owner,realm
                FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE NOT ".db_bin("status", $statusno)." AND ".db_bin("status", $statusok)." AND (owner = ".
                _addslashes($_SESSION['acl']['username']) ." || realm $realms) ORDER BY id DESC";
    }
    $result = execute_sql($sql);

    if (empty($_GET['type'])) {
    ?>
<h2><?php echo(_('View Survey Results')); ?></h2>
<?php echo(_('Pick Survey to View')); ?>

    <input type="hidden" name="rid" value="" />
    <input type="hidden" name="sid" value="" />
    <input type="hidden" name="where" value="results" />
    <?php
    } else {
        echo('<h2>'. _('Cross Tabulation') ."</h2>\n");
        echo(_('Pick Survey to Cross Tabulate'));
    }
?>
<table cellspacing="0" cellpadding="4">
    <tr class="header">
<th><?php echo(_('ID')); ?></th>
<?php if (empty($_GET['type'])) { ?>
<th><?php echo(_('Respondent')); ?></th>
    <?php }
    else { ?>
    <th></th>
<?php }?>
<th><?php echo(_('Name')); ?></th>
<th><?php echo(_('Title')); ?></th>
<th><?php echo(_('Owner')); ?></th>
<th><?php echo(_('Group')); ?></th>
<th><?php echo(_('Resp')); ?></th>
    </tr>
    <?php
        $count = 0;
        while(list($sid,$name,$title,$owner,$realm) = fetch_row($result)) {	
            $result->MoveNext();
            //let's see how many respondents we have for each survey.
            //angek: let's not include incomplete surveys in the count.
            // this occurs for multipage surveys as each page completed
            // is submitted and stored.
            $sql2 = "SELECT count(*) FROM ".$GLOBALS['ESPCONFIG']['response_table']." WHERE survey_id = $sid AND complete = 'Y'";
            $result2 = execute_sql($sql2);
            list($resp) = fetch_row($result2);
            db_close($result2);

            if($bg != $ESPCONFIG['bgalt_color1'])
            $bg = $ESPCONFIG['bgalt_color1'];
            else
            $bg = $ESPCONFIG['bgalt_color2'];
        ?>
    <tr style="background-color: <?php echo($bg); ?>;">
    <td><?php echo($sid); ?></td>
        <?php
            if (empty($_GET['type'])) {
                // grab all the rid values for the particular survey
                $sql = "SELECT R.id FROM ".$GLOBALS['ESPCONFIG']['response_table']." R ".
                "WHERE R.survey_id = ${sid} AND R.complete = 'Y' ".
                "ORDER BY R.id";
                $rid_result = execute_sql($sql);
                $count++;
            }
        ?>
        <td>
        <?php 
            if (empty($_GET['type'])) {
            ?>
        <select name="rid<?php echo $count; ?>" onchange="javascript:this.form.rid.value=this.form.rid<?php echo $count; ?>.options[this.form.rid<?php echo $count; ?>.selectedIndex].value; this.form.sid.value=<?php echo $sid; ?>; this.form.submit(); return false;">
            <option value=""></option>
            <?php
                $i = 0;
                while (list($rid) = fetch_row($rid_result)) {
                    $rid_result->MoveNext();
                ?>
            <option value="<?php echo $rid; ?>"><?php echo ++$i; ?></option>
                <?php
                }
            ?>
            </select> 
            <?php
            }
        ?>
        </td>
        <td>
        <?php
            if (!empty($_GET['type'])){
            ?>	
        <a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=results&sid=${sid}&type=cross")); ?>">
            <?php   }
                else {
                ?>
            <a href="<?php echo("". $GLOBALS['ESPCONFIG']['ME'] .htmlentities("?where=results&sid=${sid}")); ?>">
            <?php   } ?>
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
        db_close($result);
    ?>
    </table>
<?php echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n"); ?>
