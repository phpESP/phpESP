<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

// upload.inc written by Matthew Gregg
// <greggmc@musc.edu>
// Bulk upload from text files

    /* SuperUser or Designers with "Administer Group Members(pgroup)" permissions only*/
    if(($_SESSION['acl']['superuser'] != 'Y' &&
        (count($_SESSION['acl']['pgroup']) == 0 &&
        count($_SESSION['acl']['puser']) == 0)) &&
        !auth_no_access(_('to access this form'))) {
        return;
    }

	$bg1 =& $ESPCONFIG['bgalt_color1'];
	$bg2 =& $ESPCONFIG['bgalt_color2'];
    if(isset($_GET['account_type'])) {
            $account_type = htmlspecialchars($_GET['account_type']);
    }
    else {
        $account_type = "respondents";
    }
	$errstr = '';

// End the included form so we can do a multipart/form-data form
?>
</div>
</form>
<?php
	if(isset($_POST['submit'])) {
		unset($_POST['submit']);
		if(isset($_POST['account_type'])) {
			$account_type = htmlspecialchars($_POST['account_type']);
        }
		else {
			$account_type = "respondents";
        }

        $upload_type = $_POST['upload_type'];
        if (isset($_FILES['upload_file']['tmp_name'])) {
            $upload_file = $_FILES['upload_file']['tmp_name'];
        }
        else {
            $upload_file = "";
        }
		$group_acl	= array();


		if($_SESSION['acl']['superuser'] == 'Y') {
			$sql = "SELECT name FROM ".$GLOBALS['ESPCONFIG']['realm_table'];
			$result = execute_sql($sql);
			while (list($row) = fetch_row($result)) {
                $result->MoveNext();
				array_push($group_acl, $row);
			}
			db_close($result);
		} elseif(count($_SESSION['acl']['pgroup']) > 0) {
			$group_acl =& $_SESSION['acl']['pgroup'];
		} else {
            $group_acl =& $_SESSION['acl']['puser'];
		}
		if(!empty($upload_type) && !empty($account_type) && is_uploaded_file($upload_file)) {
			$file = file($upload_file);
			$exceptions = array();
			$success = account_upload($exceptions, $upload_type, $account_type, $group_acl, $file);
			if($success) {
				unset($_POST['account_type']);
				unset($_POST['submit']);
				include(esp_where("$account_type"));
                       		return;
			}
			elseif(count($exceptions) > 1) {
				$errstr .= mkerror(_('An error occurred during upload.  Rows that failed are listed below.'));
				$exception_table = ("<table width=\"95%\" border=\"0\" cellspacing=\"0\" cellpadding=\"4\" align=\"center\" \">\n");
				$bg = $ESPCONFIG['bgalt_color2'];
	 			$exception_table .= "<tr bgcolor=\"$bg\">\n";
				/* Build  exception rows */
				/* Header row*/
				while (list(, $col) = each ($exceptions[0])) {
					$exception_table .= "<th align=\"left\">$col</th>\n";
				}
				$exception_table .= "</tr>\n";
				/* Data rows*/
				next($exceptions);
				while (list(, $row) = each ($exceptions)) {
					if ($bg == $ESPCONFIG['bgalt_color1'])
	       	       	         		$bg =& $ESPCONFIG['bgalt_color2'];
        		        	else
                       				$bg =& $ESPCONFIG['bgalt_color1'];
					$exception_table .= "<tr valign=\"top\" bgcolor=\"$bg\">\n";
					foreach($row as $col) {
						$exception_table .= "<td>$col</td>\n";

					}
				}
				$exception_table .= "</tr>\n</table>";
			}
			else {
				$errstr .= mkerror(_('An error occurred during upload.  Please check the format of your text file.'));
			}
		}
		else {
				$errstr .= mkerror(_('An error occurred during upload.  Please complete all form fields.'));
		}
	}
?>
<h2><?php echo(_('Upload Account Information')); ?></h2>
<p><?php echo(_('All fields are required')) ?></p>
<?php if(!empty($errstr)) echo("<p>$errstr</p>\n"); ?>
<span class="large">
<a href="<?php echo($GLOBALS['ESPCONFIG']['ME'] ."?where=help#batch"); ?>" onclick="window.open(this.href,'_blank');return false;"><?php echo(_('Help')); ?></a>
</span>
<form enctype="multipart/form-data" method="post" action="<?php echo ($GLOBALS['ESPCONFIG']['ME']); ?>?where=upload">
<table cellspacing="0" cellpadding="4" style="width: 350px;">
<?php
		echo("<tr>\n<td class=\"right\">"._('File Type')."</td>\n");
		echo("<td class=\"left\">\n".mkselect('upload_type',array('tab' => _('Tab Delimited')))."</td>\n</tr>");
		echo("<tr>\n<td class=\"right\">"._('File to upload')."</td>\n");
		echo("<td class=\"left\">\n".mkfile('upload_file')."\n"."</td>\n</tr>");
?>
<tr>
<td colspan="2" class="center">
<input type="hidden" name="where" value="upload" />
<input type="hidden" name="account_type" value="<?php echo($account_type); ?>" />
<input type="submit" value="Upload" name="submit" />
</td>
</tr>
 </table>
</form>
<?php if(!empty($exception_table)) echo("<p>$exception_table</p>\n"); ?>
<form method="post" action="" id="null">
<div>

<?php
echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
?>

