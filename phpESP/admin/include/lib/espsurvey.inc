<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* espsurvey.inc -- Functions to work with phpESP surveys.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
* @_PATTERNS_@
* // get the ID, name, title, owner, and realm for all surveys the current user can access
* $ok = survey_get_for_current_user($surveys);
* if ($ok) {
*     // use $surveys === array (1 => array ('id' => 1, 'name' => 'foo', ...), ...);
* }
*
* // get the where clause to limit a SQL query for the current user
* $where = survey_fetch_limiting_where();
* $sql = sprintf('SELECT * FROM %s WHERE %s', $GLOBALS['ESPCONFIG']['survey_table'], $where);
*
* @_NOTES_@
* Interpretation of the open constants returned by survey_open():
* Constant                 Meaning
* ------------------------ -----------------------------------------------------------------------------------
* STATUS_OPEN              The current time is within the open & close dates, or those dates aren't given
* STATUS_CLOSED_TOO_EARLY  The current time is before the open date
* STATUS_CLOSED_TOO_LATE   The current time is after the close date
*
*/
// {{{ constants

define('STATUS_OPEN',             0);
define('STATUS_CLOSED_TOO_EARLY', 1);
define('STATUS_CLOSED_TOO_LATE',  2);

// }}}

/* survey inquisition */
// ... when you know the survey ID only, use these

// ... when you already have information from the survey, use these
// {{{ survey_status_is_edit()         Is the survey being edited?

function survey_status_is_edit($status) {
    return (is_numeric($status) && (0 === (int)$status) ? true : false);
}

// }}}
// {{{ survey_status_is_active()       Is the survey in active status?

function survey_status_is_active($status) {
    return survey_status_is($status, STATUS_ACTIVE);
}

// }}}
// {{{ survey_status_is_done()         Is the survey in done status?

function survey_status_is_done($status) {
    return survey_status_is($status, STATUS_DONE);
}

// }}}
// {{{ survey_status_is_deleted()      Is the survey in deleted status?

function survey_status_is_deleted($status) {
    return survey_status_is($status, STATUS_DELETED);
}

// }}}
// {{{ survey_status_is_test()         Is the survey in test status?

function survey_status_is_test($status) {
    return survey_status_is($status, STATUS_TEST);
}

// }}}
// {{{ survey_status_is()              Does the survey status match the given bit?

function survey_status_is($status, $bit) {
    return (is_numeric($status) && ($bit === ((int)$status & $bit)) ? true : false);
}

// }}}
// {{{ survey_open()                   Is "now" within the given open and close date times?

function survey_open($open_date, $close_date, $now = null) {
    // figure out if we have valid open and closing dates
    // ... exclude nulls and empty dates
    $has_open_date  = (empty($open_date)  || '0000-00-00 00:00:00' == $open_date  ? false : true);
    $has_close_date = (empty($close_date) || '0000-00-00 00:00:00' == $close_date ? false : true);

    // ... convert to time stamps, and if failure, assume the date is empty
    $badts = (is_callable('version_compare') && version_compare('5.1.0', PHP_VERSION, '<=') ? false : -1);
    if ($has_open_date) {
        $open_date_ts = (is_int($open_date) ? $open_date : strtotime($open_date));
        if ($badts === $open_date_ts) {
            $has_open_date = false;
        }
    }
    if ($has_close_date) {
        $close_date_ts = (is_int($close_date) ? $close_date : strtotime($close_date));
        if ($badts === $close_date_ts) {
            $has_close_date = false;
        }
    }

    $nowts = (is_int($now) ? $now : (is_null($now) ? time() : strtotime($now)));

    // if there's an open date, but the current time is before then, we're too early
    if ($has_open_date && $nowts < $open_date_ts) {
        return STATUS_CLOSED_TOO_EARLY;

    // if there's a close date, but the current time is after then, we're too late
    } else if ($has_close_date && $close_date_ts < $nowts) {
        return STATUS_CLOSED_TOO_LATE;

    // otherwise, we're open for business
    } else {
        return STATUS_OPEN;
    }
}

// }}}


/* designer/administrator survey recovery methods */
// {{{ survey_get_for_current_user()   Get the ID and information about ACTIVE or DONE surveys the CURRENT user can see

function survey_get_for_current_user(&$surveys) {
     // get the where clause appropriate for the current user
     $where = survey_fetch_limiting_where();

     // get the survey data based on those limitations
     $sql =<<<EOSQL
SELECT id,name,title,owner,realm
 FROM {$GLOBALS['ESPCONFIG']['survey_table']}
WHERE {$where}
ORDER BY id DESC
EOSQL;

    return survey_get_from_sql($sql, $surveys);
}

// }}}
// {{{ survey_fetch_limiting_where()   Return the SQL where clause to apply to a survey table query for the current user
// NOTE: The returned where clause is guaranteed to be well-formed and valid for the default table structure.
// NOTE: If you are using this with aliases, pass the correct column name for status, owner, and realm.

function survey_fetch_limiting_where($statusColumn = 'status', $ownerColumn = 'owner', $realmColumn = 'realm') {
     // we start by limiting everyone to just the active or done surveys
     $statusok = db_bin($statusColumn, STATUS_ACTIVE | STATUS_DONE);
     $statusno = db_bin($statusColumn, STATUS_DELETED);
     $where = "($statusok) AND (NOT $statusno)";

     // now if we're not a superuser, we also limit to those we own or are in our realm
     if (isset($_SESSION['acl']) && is_array($_SESSION['acl'])) {
         if (isset($_SESSION['acl']['superuser']) && 'Y' != $_SESSION['acl']['superuser']) {
             // get the realms to which the user has access
             // NOTE: $realms will include the SQL operator: IS, =, etc.
             $realms = array_to_insql(
                           array_intersect($_SESSION['acl']['pall'], $_SESSION['acl']['pdata'])
                       );

             // if we are looking for that realm in just one column
             if (is_string($realmColumn)) {
                 // get the access to that one column
                 $access = sprintf('%s %s', $realmColumn, $realms);

             // but if we're looking for that realm in multiple columns
             } else if (is_array($realmColumn)) {
                 // get the access to any of those columns
                 $access = array ();
                 foreach ($realmColumn as $column) {
                     $access[] = sprintf('%s %s', $column, $realms);
                 }
                 $access = implode(' OR ', $access);

             // if we're given something else, fail gracefully closed
             } else {
                 $access = '1=0';
             }

             // build the SQL where the user owns the survey or has access to it
             $where .= sprintf(' AND (%s=%s OR %s)', $ownerColumn, _addslashes($_SESSION['acl']['username']), $access);
         }
     }

     return $where;
}

// }}}


/* respondent survey (and associated information) recovery methods */
// {{{ survey_get_in_realm()           Get the surveys that a given realm may access (as driven by access table)

function survey_get_in_realm($realm, &$surveys) {
     // get the survey data based on those limitations
     // NOTE: SF Bug ID: 2674394: We need to give the realm since we've got realm in two places in the SQL below
     // NOTE: (tblSurvey.realm and tblAccess.realm).
     $where = survey_fetch_limiting_where('status','owner',array ('tblSurvey.realm', 'tblAccess.realm'));
     $realm = _addslashes($realm);
     $sql =<<<EOSQL
SELECT DISTINCT tblSurvey.id, tblSurvey.name, tblSurvey.title, tblSurvey.owner, tblSurvey.open_date, tblSurvey.close_date
  FROM {$GLOBALS['ESPCONFIG']['survey_table']} AS tblSurvey
  JOIN {$GLOBALS['ESPCONFIG']['access_table']} AS tblAccess
    ON tblAccess.survey_id=tblSurvey.id
 WHERE (tblSurvey.realm = $realm OR tblAccess.realm = $realm) AND ($where)
 ORDER BY id DESC
EOSQL;

    return survey_get_from_sql($sql, $surveys);
}

// }}}
// {{{ survey_get_public()             Get the surveys that are publically available

function survey_get_public(&$surveys) {
     // get the survey data based on those limitations
     $where = survey_fetch_limiting_where();
     $sql =<<<EOSQL
SELECT id,name,title,owner,open_date,close_date
  FROM {$GLOBALS['ESPCONFIG']['survey_table']}
 WHERE (public = 'Y') AND ($where)
 ORDER BY id DESC
EOSQL;

    return survey_get_from_sql($sql, $surveys);
}

// }}}
// {{{ survey_get_privilege()          Get the access information for one or more surveys, optionally constrained by realm

function survey_get_privilege(&$privileges, $sids, $realm = null) {
    // construct the limit clauses
    if (is_scalar($sids)) {
        $sids = array ($sids);
    } else if (! is_array($sids)) {
        return false;
    }
    $limit  = sprintf('(tblAccess.survey_id %s)', array_to_insql($sids));
    $limit .= (isset($realm) ? sprintf(' AND (tblAccess.realm=%s)', _addslashes($realm)) : '');

    // get the access columns
    $sql =<<<EOSQL
SELECT survey_id, realm, maxlogin, resume, navigate
  FROM {$GLOBALS['ESPCONFIG']['access_table']} AS tblAccess
 WHERE $limit
EOSQL;

    return survey_get_from_sql($sql, $privileges, 'survey_id');
}

// }}}
// {{{ survey_get_responses()          Get the response information for one or more surveys, optionally constrained by username

function survey_get_responses(&$responses, $sids, $username = null) {
    if (is_scalar($sids)) {
        $sids = array ($sids);
    } else if (! is_array($sids)) {
        return false;
    }
    $limit  = sprintf('(survey_id %s)', array_to_insql($sids));
    $limit .= (isset($username) ? sprintf(' AND (tblResponse.username=%s)', _addslashes($username)) : '');

    $sql =<<<EOSQL
SELECT survey_id, username, submitted, complete, ip
  FROM {$GLOBALS['ESPCONFIG']['response_table']} AS tblResponse
 WHERE $limit
EOSQL;
    return survey_get_from_sql($sql, $responses, 'survey_id');
}

// }}}
// {{{ survey_get_accessibility()      Determine accessibility of one or more surveys, optionally pinned to user or realm

/*
NOTE1: A survey is "accessible" when it's:
 (a) active:    status is active
 (b) open:      survey is open (between open & close dates, if given)
 (c) available: (user given and ((no max login) or (number of complete responses < max))) or (no user given)
 (d) visible:   (survey is public) OR (in user's realm)

NOTE2: Each row will have 5 columns: id, active, open, available, visible.  Possible values are:
 o        id: >= 1
 o    active: === 1 if active, === 0 otherwise
 o      open: either STATUS_OPEN, STATUS_CLOSED_TOO_EARLY, or STATUS_CLOSED_TOO_LATE
 o available: === 1 if available, === 0 otherwise
 o   visible: === 1

*/

function survey_get_accessibility(&$accessibility, $sids, $username = null, $realm = null) {
    $yes = _addslashes('Y');

    // construct the accessibilty clauses
    // ... active
    $statusActive = db_bin('tblSurvey.status', STATUS_ACTIVE);
    $statusActive =<<<EOSQL
       CASE WHEN ($statusActive) THEN 1
       ELSE 0
       END
EOSQL;

    // ... open
    $now = sys_time_stamp();
    $STATUS_CLOSED_TOO_EARLY = STATUS_CLOSED_TOO_EARLY;
    $STATUS_CLOSED_TOO_LATE  = STATUS_CLOSED_TOO_LATE;
    $STATUS_OPEN             = STATUS_OPEN;
    $openForBiz   =<<<EOSQL
        CASE
        WHEN NOT (tblSurvey.open_date IS NULL OR '0000-00-00 00:00:00' = tblSurvey.open_date) AND $now < tblSurvey.open_date
        THEN $STATUS_CLOSED_TOO_EARLY
        WHEN NOT (tblSurvey.close_date IS NULL OR '0000-00-00 00:00:00' = tblSurvey.close_date) AND tblSurvey.close_date < $now
        THEN $STATUS_CLOSED_TOO_LATE
        ELSE $STATUS_OPEN
        END
EOSQL;

    // ... available
    if (isset($username)) {
        $username = _addslashes($username);
        $mayRespond =<<<EOSQL
            CASE WHEN (
                       0 = COALESCE(maxlogin,0) OR
                       (
                         SELECT COUNT(*)
                           FROM {$GLOBALS['ESPCONFIG']['response_table']} as tblResponse
                          WHERE tblResponse.survey_id=tblAccess.survey_id AND
                                tblResponse.username=$username AND
                                tblResponse.complete=$yes
                       ) < maxlogin
                      )
            THEN 1
            ELSE 0
            END
EOSQL;
    } else {
        $mayRespond = '1';
    }

    // ... visible
    if (is_scalar($sids)) {
        $sids = array ($sids);
    } else if (! is_array($sids)) {
        return false;
    }

    if (isset($realm)) {
        $realm = _addslashes($realm);
        $limit = sprintf(
                     '(tblSurvey.id %s) AND (tblSurvey.public=%s OR tblAccess.realm=%s)',
                     array_to_insql($sids), $yes, $realm
                 );
    } else {
        $limit = sprintf(
                     '(tblSurvey.id %s) AND (tblSurvey.public=%s)',
                     array_to_insql($sids), $yes
                 );
    }

    // compute
    $sql =<<<EOSQL
SELECT tblSurvey.id,
       ($statusActive) AS active,
       ($openForBiz)   AS open,
       ($mayRespond)   AS available,
       1               AS visible
  FROM {$GLOBALS['ESPCONFIG']['survey_table']} AS tblSurvey
  LEFT
  JOIN {$GLOBALS['ESPCONFIG']['access_table']} AS tblAccess
    ON tblAccess.survey_id=tblSurvey.id
 WHERE $limit
EOSQL;

    return survey_get_from_sql($sql, $accessibility);
}

// }}}


/* helper functions */
// {{{ survey_get_from_sql()           Get an array of survey data from an SQL statement

function survey_get_from_sql($sql, &$surveys, $surveyIDCol = 'id') {
    // execute the SQL
    $res = execute_sql($sql, null, ADODB_FETCH_ASSOC);
    if (! $res) {
        // give up, select bombed
        $GLOBALS['errmsg'] = mkerror(_('Unable to get list of surveys for current user'));
        return false;
    }

    // put dataset into a nice array
    // NOTE: if there are multiple rows with the same survey ID, they're combined so that you have an array
    $needsConversion = true;
    $surveys = array ();
    while ($row = fetch_row($res)) {
        $res->MoveNext();
        // make sure we have an ID column
        if (isset($row[$surveyIDCol])) {
            $id = $row[$surveyIDCol];
        } else {
            continue;
        }

        // add in the survey information to the previous survey information
        if (isset($surveys[$id])) {
            if ($needsConversion) {
                // we've collided, but not yet converted to an array: do so
                $surveys[$id] = array ($surveys[$id]);
                $needsConversion = false;
            }
            $surveys[$id][] = $row;
        } else {
            $surveys[$id] = $row;
        }
    }
    db_close($res);

    return true;
}

// }}}
// {{{ survey_merge_sets()             Combine two or more survey sets into one
// NOTE: This is like array_merge(), but squashes duplicate surveys that appear

function survey_merge_sets(&$merged, $a /* ... */) {
    // make sure we have an initial array
    if (! is_array($merged)) {
        $merged = array ();
    }

    // add in surveys from the second argument on, skipping duplicates
    for ($i = 1; $i < func_num_args(); $i++) {
        $set = func_get_arg($i);
        if (! is_array($set)) {
            continue;
        }
        foreach ($set as $sid => $survey) {
            if (! array_key_exists($sid, $merged)) {
                $merged[$sid] = $survey;
            }
        }
    }

    return true;
}

// }}}
// {{{ survey_fetch_url_by_survey_name() Return the URL to a survey with a given name
function survey_fetch_url_by_survey_name($name) {
    return rtrim($GLOBALS['ESPCONFIG']['base_url'], '/') . '/public/survey.php?name=' . htmlentities($name);
}

// }}}

?>
