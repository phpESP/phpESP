<?php
                                                                                                                                         
/* $Id$ */
                                                                                                                                         
/* vim: set tabstop=4 shiftwidth=4 expandtab: */

                                                                                                                                              
/* {{{ proto adodb_recordset execute_sql(string message, integer limit, string assoc)
   Executes $command and Returns a adodb_recordset */
function execute_sql($command, $limit=null, $assoc=ADODB_FETCH_NUM) {

    $cfg =& $GLOBALS['ESPCONFIG'];
    $cfg['adodb_conn']->SetFetchMode($assoc);
    if ($limit) {
        $result = $cfg['adodb_conn']->SelectLimit($command,$limit);
    }
    else {   
        $result = $cfg['adodb_conn']->Execute($command);
    }

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}
/* }}} */

/* {{{ proto integer affected_rows()
   Returns number of rows effected by last update/insert */
function affected_rows() {

    $cfg =& $GLOBALS['ESPCONFIG']; 
    $result = $cfg['adodb_conn']->Affected_Rows();

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}
/* }}} */

/* {{{ proto adodb_row fetch_row(adodb_recordset &$recordset)
    Returns a adodb_row from a adodb_recordset */
function fetch_row(&$recordset) {
    $result = $recordset->fields;

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}
/* }}} */

/* {{{ proto adodb_recordset_row get_row(string $command)
    Executes $command and Returns the first row of a 
    adodb_recordset*/
function get_row($command) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    $result = $cfg['adodb_conn']->GetRow("$command");

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}
/* }}} */

/* {{{ proto adodb_recordset_field get_one(string $command)
    Executes $command and Returns the first field of the first row of
    a adodb_recordset  */
function get_one($command) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    $result = $cfg['adodb_conn']->GetOne($command);

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}

/* }}} */

/* {{{ proto integer record_count( adodb_recordset &$recordset)
    Returns the number of rows in &$recordset */
function record_count(&$recordset) {
    $result = $recordset->RecordCount();

    if ($result) {
        return $result;
    }
    else {
        return 0;
    }
}
/* }}} */

/* {{{ proto integer insert_id(string $sequence)
    Returns the last auto-incremented value.
    $sequence is required for Postgres.*/
function insert_id($sequence="") {
    $cfg =& $GLOBALS['ESPCONFIG'];
    switch ($cfg['adodb_database_type']) {
        case "postgres":
        case "oracle":
            $sql = "select currval('".$sequence."')";
            return $cfg['adodb_conn']->GetOne($sql); 
        default:
            return $cfg['adodb_conn']->Insert_ID();
    }
}
/* }}} */

/* {{{ proto db_close(adodb_recordset &$recordset)
    Closes &$recordset */
function db_close(&$recordset) {
    $recordset->Close();
}
/* }}} */

/* {{{ proto string db_qstr(string $data)
    Returns a string properly quoted for the 
    active database. */
function db_qstr($data) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    $result = $cfg['adodb_conn']->qstr($data);
    return $result;
}
/* }}} */

/* {{{ proto string db_bin(string $status, string $code)
    Returns a string properly formated for the active database
    to handle bitwise comparisons. */
function db_bin($status, $code) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    switch ($cfg['adodb_database_type']) {
        case "postgres":
            return "bin_compare($status, $code)";
        case "oracle":
            return "bitand($status, $code)";
        default:
            return "($status & $code)";
    }
}
/* }}} */

/* {{{ proto string db_date(string $data)
    Returns a string properly formated for the active database's
    date format. */
function db_date($data) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    $result = $cfg['adodb_conn']->DBDate($data);
    return $result;
}
/* }}} */

/* {{{ proto adodb_time_stamp sys_time_stamp()
    Returns a adodb_time_stamp suitable for use with the active
    database. */
function sys_time_stamp() {
    $cfg =& $GLOBALS['ESPCONFIG'];
    
    return $cfg['adodb_conn']->sysTimeStamp;
}
/* }}} */

/* {{{ proto string db_crypt(string $password)
    Returns a string properly formated for the active database's
    password format. */
function db_crypt($password) {
    $cfg =& $GLOBALS['ESPCONFIG'];
    switch ($cfg['adodb_database_type']) {
        case "postgres":
        case "oracle":
            return "md5($password)";
        case "sqlite":
            return _addslashes(md5($password));
        default: 
            return "PASSWORD($password)";
    }
}
/* }}} */

/* {{{ proto integer ErrorNo()
    Returns the number for the last error of the active database. */
function ErrorNo() {
   $cfg =& $GLOBALS['ESPCONFIG'];

   return $cfg['adodb_conn']->ErrorNo();
}
/* }}} */

/* {{{ proto string ErrorMsg()
    Returns the message for the last error of the active database. */
function ErrorMsg() {
   $cfg =& $GLOBALS['ESPCONFIG'];

   return $cfg['adodb_conn']->ErrorMsg();
}
/* }}} */

/* {{{ proto password_upgrade(string $username, string $password, string $table) {
    Migrates passwords from mySQL pre 4.1 to 4.1 crypt.
    */
function password_upgrade($username, $password, $table) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    if ($cfg['adodb_database_type'] == "mysql") {
        $info = $cfg['adodb_conn']->ServerInfo();
        $version = $info['version'];
        if ($version > 4.0) {
            $username = _addslashes($username);
            $sql = "SELECT * FROM ".$table." WHERE
            username = ".$username." AND
            password = OLD_PASSWORD($password) AND
            disabled = 'N' AND
            (expiration = '0000-00-00 00:00:00' OR expiration > ". sys_time_stamp().")";
            $accres = execute_sql( $sql, "", ADODB_FETCH_ASSOC );
            if (!$accres) {
                echo(ErrorMsg());
            }
            if (record_count($accres) == 1) {
			// A matching row was found - upgrade his password.
                $sql = "UPDATE $table SET password = PASSWORD($password) WHERE username = $username";
                $accres = execute_sql( $sql, "", ADODB_FETCH_ASSOC );
                if (!$accres) {
                    echo(ErrorMsg());
                }
            
            }
        }
    }
}
/* }}} */

/* {{{ proto string MetaTables()
    Returns an array of tables for the active database. */
function MetaTables() {
   $cfg =& $GLOBALS['ESPCONFIG'];

   return $cfg['adodb_conn']->MetaTables('TABLES');
}
/* }}} */
