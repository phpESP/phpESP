<?php

// if not superuser, get out
if($_SESSION['acl']['superuser'] != 'Y') {
   echo("<br><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
   exit;
} else if (isset($_POST['action']) && $_POST['action']=="update") {
// The update is called for

   // we do the check again, so updates can't happen again if you refresh the page
   $db_version = get_dbversion();
   $new_version = $ESPCONFIG['version'];
   $db_type=$ESPCONFIG['adodb_database_type'];
   if (version_compare($db_version,$new_version,"eq")) {
      echo(_('Update not needed'));
   } else {
      echo (_('Update available'));
      // determine which upgrades are needed
      $all_sql_files=get_update_files($db_version);
      // then do the update file by file
      foreach ($all_sql_files as $sqlfile) {
         $sqlbuffer = "";
         if ($sqlfile=="") {
	   continue;
         }
         $sqlcontent = @file(ESP_BASE."/scripts/db/".$db_type."_".$sqlfile);
         for($o=0;$o<count($sqlcontent);$o++) {
	    // ignore sql comment
            if(substr(trim($sqlcontent[$o]),0,2) != '--' and strlen(trim($sqlcontent[$o]))>0) $sqlbuffer .= $sqlcontent[$o];
         }

         // sql statements are always seperated by ";"
         // so now we have an array with each element an sql statement
         $sql_array = explode(";",$sqlbuffer);

         unset($sqlbuffer);
         $sqlErrors = false;
         for($i=0;$i<count($sql_array);$i++) {
            if(strlen(trim($sql_array[$i])) > 0) {
 	      if(!execute_sql($sql_array[$i])) {
 	          $sqlErrors = true;
	          $errstr.= _('Error executing sql statement: ').$sql_array[$i]."<br />";
 	      }
            }
         }

      } # end foreach

      if ($sqlErrors) {
         echo(mkerror(_('Update FAILED'))."<br />\n");
         echo($errstr."<br />\n");
      } else {
         echo (_('Update done'));
      }
   } # end if (version_compare...
} else if (isset($_POST['action']) && $_POST['action']=="versionupdate") {
      echo (_('Updating from old version ').$_POST['version']);
?>
<form enctype="multipart/form-data" method="post" action="<?php echo ($GLOBALS['ESPCONFIG']['ME']); ?>?where=update">
<input type="hidden" name="where" value="update" />
<input type="hidden" name="action" value="update" />
<input type="hidden" name="version" value="<?php echo $_POST['version'];?>" />
<br />
<input type="submit" value="<?php echo _('Update'); ?>" name="submit" />
</form>

<?php
} else {
// we check the version
   $db_version = get_dbversion();
   $new_version = $ESPCONFIG['version'];
   if (version_compare($db_version,$new_version,"eq")) {
      echo(_('Update not needed'));
   } else {
      echo (_('Update available'));
?>
<form enctype="multipart/form-data" method="post" action="<?php echo ($GLOBALS['ESPCONFIG']['ME']); ?>?where=update">
<input type="hidden" name="where" value="update" />
<input type="hidden" name="action" value="versionupdate" />
<br />
<?php echo _('Current version:');?>
<input type="text" name="version"  value="<?php echo $db_version;?>" />
<br />
<input type="submit" value="<?php echo _('Go'); ?>" name="submit" />
</form>

<?php
   }
}

echo("<br><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");

?>

