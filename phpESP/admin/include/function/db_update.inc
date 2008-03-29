<?php

function get_version() {
   $all_tables=MetaTables();
   $version_table=$GLOBALS['ESPCONFIG']['version_table'];
   if (in_array($version_table,$all_tables)) {
      $sql = "SELECT version FROM $version_table";
      $result = execute_sql($sql);
      $db_version = get_one($sql);
      if (empty($db_version)) {
	return("2.0.2");
      } else {
        return($db_version);
      }
   } else {
      return("2.0.2");
   }
}

function get_update_files($version) {
   $upgrades_content=@file(ESP_BASE."/scripts/db/upgrades.txt");
   $found=false;
   $to_upgrade="";
   foreach ($upgrades_content as $line) {
      $line=trim($line);
      if (!$found && $line != "#$version") {
	 continue;
      }
      if (!$found && $line == "#$version") {
	$found=true;
	continue;
      }
      // once we have found the correct version
      // ignore future versions mentioned
      if ($found && substr($line,0,1) == "#") {
	continue;
      }
      $to_upgrade.=$line.";";
   }
   return (explode(";",$to_upgrade));
}

?>
