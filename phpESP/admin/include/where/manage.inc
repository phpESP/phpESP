<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

$base =& $GLOBALS['ESPCONFIG']['ME'];

?>
<h2><?php echo(_('Management Interface')); ?></h2>
<p><?php
    if($GLOBALS['ESPCONFIG']['auth_design']) {
        if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y')
        echo(mkwarn(_('Superuser')));
        else {
            echo(_('Username'));
        }
        echo(': <tt>'. $HTTP_SESSION_VARS['acl']['username'] .'</tt>');
    }
?></p>
			<!-- Survey Administration -->
			<div id="surveyAdmin">
                        <ul>
                            <li><a href="<?php echo("${base}?where=new");     ?>"><?php echo(_('Create a New Survey')); ?></a></li>
                            <li><a href="<?php echo("${base}?where=edit");    ?>"><?php echo(_('Edit an Existing Survey')); ?></a></li>
                            <li><a href="<?php echo("${base}?where=test");    ?>"><?php echo(_('Test a Survey')); ?></a></li>
                            <li><a href="<?php echo("${base}?where=copy");    ?>"><?php echo(_('Copy an Existing Survey')); ?></a></li>
                        </ul>
                        <ul>
                            <li><a href="<?php echo("${base}?where=status");  ?>"><?php echo(_('Change the Status of a Survey')); ?></a>
                            <?php echo(_('(active/end/delete)')); ?></li>
                            <?php if($GLOBALS['ESPCONFIG']['auth_response']) { ?>
                            <li><a href="<?php echo("${base}?where=access");  ?>"><?php echo(_('Change Access To a Survey')); ?></a> (<?php echo(_('Limit Respondents.')); ?>)</li>
                            <?php } ?>
                        </ul>
			</div>
			<!-- End Survey Administration -->
			<!-- Survey Results -->
			<div id="surveyResults">
                        <ul>
                            <li><a href="<?php echo("${base}?where=results"); ?>"><?php echo(_('View Results from a Survey')); ?></a></li>
                            <li><a href="<?php echo(htmlentities("${base}?where=results&type=cross")); ?>"><?php echo(_('Cross Tabulate Survey Results')); ?></a></li>
                            <li><a href="<?php echo("${base}?where=report");  ?>"><?php echo(_('View a Survey Report')); ?></a></li>
                            <li><a href="<?php echo("${base}?where=export");  ?>"><?php echo(_('Export Data')); ?></a></li>
                        </ul>
			</div>
			<!-- End Survey Results -->
                        <?php if($GLOBALS['ESPCONFIG']['auth_design']) { ?>
			<!-- User Administration -->
			<div id="userAdmin">
                        <ul>
                            <?php
                            if($GLOBALS['ESPCONFIG']['auth_type'] != 'ldap') { ?>
                            <li><a href="<?php echo("${base}?where=passwd");   ?>"><?php echo(_('Change Your Password')); ?></a></li>
                            <?php
                            }
                            if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
                            count($HTTP_SESSION_VARS['acl']['puser']) > 0) { ?>
                            <li><a href="<?php echo("${base}?where=respondents"); ?>"><?php echo(_('Manage Respondent Accounts')); ?></a></li>
                            <?php   }
                            if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y' ||
                            count($HTTP_SESSION_VARS['acl']['pgroup']) > 0) { ?>
                            <li><a href="<?php echo("${base}?where=designers");   ?>"><?php echo(_('Manage Designer Accounts')); ?></a></li>
                            <?php	if($HTTP_SESSION_VARS['acl']['superuser'] == 'Y') { ?>
                            <li><a href="<?php echo("${base}?where=groups"); ?>"><?php echo(_('Manage Groups')); ?></a></li>
                            <?php	}
                            } ?>
                        </ul>
			</div>
			<!-- End User Administration -->
                        <?php } /* AUTH_DESIGN */ ?>
                        <!-- Guide -->
			<div id="guide">
			<ul>
                            <!--				<li><a href="<?php echo("${base}?where=TODO");    ?>"><?php echo(_('View the list of things still to do')); ?></a>
                            <?php echo(_('(development goals)')); ?></li> -->
                            <li><a href="<?php echo("${base}?where=guide");  ?>"><?php echo(_('View the User &amp; Administrator Guide')); ?></a></li>
                        </ul>
			</div>
			<!-- End Guide -->
                        <?php if($GLOBALS['ESPCONFIG']['auth_design']) { ?>
			<!-- Logout -->
			<div id="logout">
                        <ul>
                            <li><a href="<?php echo("${base}?where=logout");  ?>"><?php echo(_('Log out')); ?></a></li>
                        </ul>
			</div>
			<!-- End Logout -->
                        <?php } /* AUTH_DESIGN */ ?>
&nbsp;
