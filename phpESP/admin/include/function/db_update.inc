<?php

require_once($GLOBALS['ESPCONFIG']['adodb_path'].'adodb-xmlschema03.inc.php');
function get_dbschema() {
   global $DB_PREFIX;
   $cfg =& $GLOBALS['ESPCONFIG'];
   $schema = new adoSchema($cfg['adodb_conn']);
   $schema->setPrefix($DB_PREFIX,false);
   return($schema->ExtractSchema());
}

function show_generated_sql() {
#print_r(get_dbschema());
#die;
   global $DB_PREFIX;
   $cfg =& $GLOBALS['ESPCONFIG'];
   $schema = new adoSchema($cfg['adodb_conn']);
   $schema->setPrefix($DB_PREFIX,false);
   $sql = $schema->ParseSchema(ESP_BASE."/scripts/db/schema.xml");
   print($schema->PrintSQL('HTML'));
   return;
}

function set_dbschema() {
   global $DB_PREFIX;
   $cfg =& $GLOBALS['ESPCONFIG'];
   $schema = new adoSchema($cfg['adodb_conn']);
   $schema->setPrefix($DB_PREFIX,false);
   $sql = $schema->ParseSchema(ESP_BASE."/scripts/db/schema.xml");
   return($schema->ExecuteSchema());
}

function initialize_db() {
   global $DB_PREFIX;
   $cfg =& $GLOBALS['ESPCONFIG'];
   $schema = new adoSchema($cfg['adodb_conn']);
   $schema->setPrefix($DB_PREFIX,false);
   $sql = $schema->ParseSchema(ESP_BASE."/scripts/db/data.xml");
   return($schema->ExecuteSchema());
}

function db_dump() {
   global $DB_PREFIX;
   $cfg =& $GLOBALS['ESPCONFIG'];
   $schema = new adoSchema($cfg['adodb_conn']);
   $schema->setPrefix($DB_PREFIX,false);
   return($schema->ExtractSchema(true));
}


function get_dbversion() {
   $all_tables=MetaTables();
   $question_table=$GLOBALS['ESPCONFIG']['question_table'];
   // if we can't find the question table, we assume a fresh install
   // and we return 0.0.0 as the version for this
   if (!in_array($question_table,$all_tables)) {
      return("0.0.0");
   }
   $version_table=$GLOBALS['ESPCONFIG']['version_table'];
   if (in_array($version_table,$all_tables)) {
      // the version table exists
      $sql = "SELECT versionid FROM $version_table";
      $result = execute_sql($sql);
      $db_version = get_one($sql);
      return($db_version);
   } else {
      // the version table doesn't exist
      return("");
   }
}

?>
