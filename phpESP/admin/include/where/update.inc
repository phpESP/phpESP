<?php

// if not superuser, get out
if($_SESSION['acl']['superuser'] != 'Y') {
   echo("<br><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
   exit;
} else if (isset($_POST['action']) && $_POST['action']=="showsql") {
   show_generated_sql();
} else if (isset($_POST['action']) && $_POST['action']=="update") {
// The update is called for

   // we do the check again, so updates can't happen again if you refresh the page
   $db_version = get_dbversion();
   $new_version = $ESPCONFIG['version'];
   $db_type=$ESPCONFIG['adodb_database_type'];
   if (version_compare($db_version,$new_version,"eq")) {
      echo(_('Update not needed'));
   } else {
      echo (_('Updating')."<br />");
      $result=set_dbschema();
      if (!$result) {
         echo(mkerror(_('Update FAILED'))."<br />\n");
      } else {
         echo (_('Update done')."<br />");
      }

      // the schema is now ok, so we now update the version
      $sql="DELETE FROM ".$GLOBALS['ESPCONFIG']['version_table'];
      $result = execute_sql($sql);
      if (!$result) {
         echo(mkerror(_('Version delete FAILED'))."<br />\n");
      }
      $sql="INSERT INTO ".$GLOBALS['ESPCONFIG']['version_table']." (versionid) VALUES ('$new_version')";
      $result = execute_sql($sql);
      if (!$result) {
         echo(mkerror(_('Version update FAILED'))."<br />\n");
      }
   } # end if (version_compare...
} else {
// we check the version
   $db_version = get_dbversion();
   $new_version = $ESPCONFIG['version'];
   if (version_compare($db_version,$new_version,"eq")) {
      echo(_('Update not needed'));
   } else {
      echo (_('Update available'));
?>

<br />
<form enctype="multipart/form-data" method="post" action="<?php echo ($GLOBALS['ESPCONFIG']['ME']); ?>?where=update">
<input type="hidden" name="where" value="update" />
<input type="hidden" name="action" value="showsql" />
<br />
<input type="submit" value="<?php echo _('Show generated SQL'); ?>" name="submit" />
</form>

<form enctype="multipart/form-data" method="post" action="<?php echo ($GLOBALS['ESPCONFIG']['ME']); ?>?where=update">
<input type="hidden" name="where" value="update" />
<input type="hidden" name="action" value="update" />
<br />
<input type="submit" value="<?php echo _('Update the DB'); ?>" name="submit" />
</form>

<?php
   }
}

echo("<br><a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");

?>

