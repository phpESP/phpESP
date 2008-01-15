<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* espsurveystat.inc -- Functions to work with phpESP survey statistics.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
* @_PATTERNS_@
* // To record a statistic:
* esp_require_once('/lib/espsurveystat');
* survey_stat_increment(SURVEY_STAT_LOGINFAIL, $surveyID); // increments login failure count for survey_id=$surveyID
* survey_stat_decrement(SURVEY_STAT_SUSPENDED, $surveyID); // decrements suspended survey count for survey_id=$surveyID
* survey_stat_set(SURVEY_STAT_ABANDONED, $surveyID, 12); // sets abandoned statistic to 12 for survey_id=$surveyID
*
* // To get all results for a survey:
* $stats = survey_stat_fetch_all($sid);
* // $stats = array ('loginfail' => 12, 'abandoned' => 10, 'suspended' => 9, 'completed' => 40);
*
* // To get all statistics for all surveys the current user can access
* $stats = stat_fetch_with_summary();
* // $stats = array (
* //               1 => array ('loginfail' => 12, 'abandoned' => 10, 'suspended' =>  9, 'completed' => 40),
* //               2 => array ('loginfail' => 10, 'abandoned' => 20, 'suspended' =>  5, 'completed' => 21)
* //              '' => array ('loginfail' => 22, 'abandoned' => 30, 'suspended' => 14, 'completed' => 61),
* //          );
* // NOTE that summary row is the last one
*
* @_NOTES_@
* ADDING A NEW STATISTIC
* To add a new statistic, do three things:
* 1. Augment the survey_statistics table with the desired column
* 2. Add a SURVEY_STAT_XXX define, such that the define value is exactly the same as the column created in #1
* 3. Begin using the survey_stat_*() methods in your code, using the define created in #2. Indicate where you
*    are recording statistics in the "RECORD POINTS" block below
* 4. Add a column to the statistics management module (admin/include/where/statistic.inc)
* 5. Update the help file (admin/include/where/help.inc) and language catalog (locale/*) to reflect the new statistic
*
* RECORD POINTS
* Statistics are accumulated in the following spots:
* - loginfail incremented in /admin/lib/espauth-*.inc:survey_auth()
* - attempted incremented in /public/handler.php, just prior to the survey being rendered
* - abandoned incremented in /public/handler.php, just prior to the survey being rendered
* - suspended incremented in /admin/include/funcs.inc, in goto_save()
* - completed incremented in /admin/include/funcs.inc, in goto_thankyou()
* - loginfail is NEVER decremented
* - attempted is NEVER decremented
* - abandoned decremented in /admin/include/funcs.inc, in goto_save() and goto_thankyou()
* - suspended decremented in /public/handler.php, just after loading previous responses
* - completed decremented in admin/include/function/response_purge.inc, in response_purge()
*
*/

// constants to be used throughout the code
// NOTE: the define value must correspond exactly to a column in the survey_statistics table
define('SURVEY_STAT_LOGINFAIL', 'loginfail');
define('SURVEY_STAT_ATTEMPTED', 'attempted');
define('SURVEY_STAT_ABANDONED', 'abandoned');
define('SURVEY_STAT_SUSPENDED', 'suspended');
define('SURVEY_STAT_COMPLETED', 'completed');


// {{{ stat_fetch_with_summary()       Return all metrics for all surveys, with summary
// NOTE: row keyed by '' is the summary row.
// NOTE: If you want to get all statistics, overriding the access control on the user, pass false as first param.

function stat_fetch_with_summary($limitToCurrentUser = true) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // get the surveys
    if ($limitToCurrentUser) {
        // NOTE: we are going to fully qualify the access control column names to prevent any collisions that might
        // NOTE: happen in the future owing a statistics column named "status", "owner", or "realm"
        esp_require_once('/lib/espsurvey');
        $where = survey_fetch_limiting_where(
                     "{$cfg['survey_table']}.status",
                     "{$cfg['survey_table']}.owner",
                     "{$cfg['survey_table']}.realm"
                 );
        $sql = sprintf(
                   'SELECT * FROM %1$s JOIN %2$s ON %2$s.id=%1$s.survey_id WHERE %3$s',
                   $cfg['survey_statistics_table'],
                   $cfg['survey_table'],
                   $where
               );
    } else {
        $sql = sprintf('SELECT * FROM %1$s', $cfg['survey_statistics_table']);
    }
    $res = execute_sql($sql, null, ADODB_FETCH_ASSOC);

    // turn the record set into a nice array, with a summary line
    if ($res && 0 <= record_count($res)) {
        $results = array ();
        $summary = array ();
        while ($row = fetch_row($res)) {
            $res->MoveNext();
            // get the survey ID and waste it, so that only statistics remain
            $sid = $row['survey_id'];
            unset($row['survey_id']);

            // store the results
            $results[$sid] = $row;

            // accumulate totals
            foreach ($row as $key => $val) {
                if (isset($summary[$key])) {
                    $summary[$key] += $val;
                } else {
                    $summary[$key] = $val;
                }
            }
        }
        if (! empty($summary)) {
            $results[null] = $summary;
        }
        return $results;
    } else {
        $GLOBALS['errmsg'] = mkerror(_('Unable to fetch statistics from table'));
        return false;
    }
}

// }}}
// {{{ survey_stat_fetch_all()         Return all metrics for a particular survey

function survey_stat_fetch_all($sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // try and get all the columns for this survey
    $sql = sprintf('SELECT * FROM %1$s WHERE survey_id=%2$s', $cfg['survey_statistics_table'], $sid);
    $res = execute_sql($sql, null, ADODB_FETCH_ASSOC);
    if ($res) {
        // now get them from the set, remove the bookkeeping values, and return them
        $values = fetch_row($res);
        unset($values['survey_id']);
        return $values;
    } else {
        // oh well, can't do anything but move along
        $GLOBALS['errmsg'] = mkerror(_('Unable to fetch statistics for survey'));
        return false;
    }
}

// }}}
// {{{ survey_stat_fetch()             Return a metric for a particular survey

function survey_stat_fetch($metric, $sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // get the particular metric
    $sql = sprintf('SELECT %1$s FROM %2$s WHERE survey_id=%3$s', $metric, $cfg['survey_statistics_table'], $sid);
    $res = execute_sql($sql);
    if ($res) {
        // pull it out and return it
        list ($value) = fetch_row($res);
        return $value;
    } else {
        // oh well, give up and move along
        $GLOBALS['errmsg'] = mkerror(_('Unable to fetch statistic for survey'));
        return false;
    }
}

// }}}
// {{{ survey_stat_increment()         Increment a metric for a particular survey
// NOTE: This function must make the database update atomically -- do not do a SELECT then UPDATE

function survey_stat_increment($metric, $sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // try and update
    $sql = sprintf('UPDATE %1$s SET %2$s=%2$s+1 WHERE survey_id=%3$s', $cfg['survey_statistics_table'], $metric, $sid);
    $res = execute_sql($sql);
    if (0 == affected_rows()) {
        // well, no update, try an insert
        $sql = sprintf('INSERT INTO %1$s (survey_id,%2$s) VALUES (%3$s,1)', $cfg['survey_statistics_table'], $metric, $sid);
        $res = execute_sql($sql);
        if (0 == affected_rows()) {
            // alright, give up as neither the update nor the insert worked
            $GLOBALS['errmsg'] = mkwarn(_('Unable to increment statistic for survey'));
            return false;
        }
    }

    // either the update or the insert worked... hooray, move along.
    return true;
}

// }}}
// {{{ survey_stat_decrement()         Decrement a non-zero metric for a particular survey
// NOTE: This function must make the database update atomically -- do not do a SELECT then UPDATE

function survey_stat_decrement($metric, $sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // try and update the value
    // NOTE: only decrement when current value is more than 0
    // NOTE: if you need to set negative statistics, use survey_stat_set();
    $sql = sprintf(
               'UPDATE %1$s SET %2$s=%2$s-1 WHERE survey_id=%3$s AND 0 < %2$s',
               $cfg['survey_statistics_table'], $metric, $sid
           );
    $res = execute_sql($sql);
    if (0 < affected_rows()) {
        // yay, it worked... move along
        return true;
    } else {
        // either the table structure is wrong (for example, someone gave a metric that isn't a column) or the
        // row does not yet exist.  Either way, it's a code problem, because you shouldn't be trying to update
        // columns that don't exist, and you shouldn't be decrementing statistics that haven't been incremented.
        $GLOBALS['errmsg'] = mkwarn(_('Unable to decrement statistic for survey'));
        return false;
    }
}

// }}}
// {{{ survey_stat_set()               Set a metric for a particular survey to a particular value
// NOTE: This function must make the database update atomically -- do not do a SELECT then UPDATE

function survey_stat_set($metric, $sid, $value) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // try and update with the given value
    $sql = sprintf(
               'UPDATE %1$s SET %2$s=%4$s WHERE survey_id=%3$s',
               $cfg['survey_statistics_table'], $metric, $sid, $value
           );
    $res = execute_sql($sql);
    if (0 == affected_rows()) {
        // well, that failed, try to insert with the given value
        $sql = sprintf(
                   'INSERT INTO %1$s (survey_id,%2$s) VALUES (%3$s,%4$s)',
                   $cfg['survey_statistics_table'], $metric, $sid, $value
               );
        $res = execute_sql($sql);
        if (0 == affected_rows()) {
            // alright, we're out of options... warn and move along
            $GLOBALS['errmsg'] = mkwarn(_('Unable to set statistic for survey'));
            return false;
        }
    }

    // either an update or an insert succeeded... yay.
    return true;
}

// }}}
// {{{ survey_stat_reset()             Reset all the metrics for a particular survey

function survey_stat_reset($sid) {
    $ok = true;
    $ok = $ok && survey_stat_purge($sid);
    $ok = $ok && survey_stat_init($sid);
    return $ok;
}

// }}}
// {{{ survey_stat_init()              Initialize all the metrics for a given survey

function survey_stat_init($sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // insert the survey ID, which will rely on the default value settings defined in the schema
    $sql = sprintf('INSERT INTO %1$s (survey_id) VALUES (%2$s)', $cfg['survey_statistics_table'], $sid);
    $res = execute_sql($sql);
    if (0 == affected_rows()) {
        // well, that failed... not too bad, though: warn and move along
        $GLOBALS['errmsg'] = mkerror(_('Unable to initialize statistics for survey'));
        return false;
    }

    // insert succeeded, sweet
    return true;
}

// }}}
// {{{ survey_stat_purge()             Remove all statistics for a given survey

function survey_stat_purge($sid) {
    $cfg =& $GLOBALS['ESPCONFIG'];

    // try and update with the given value
    $sql = sprintf('DELETE FROM %1$s WHERE survey_id=%2$s', $cfg['survey_statistics_table'], $sid);
    $res = execute_sql($sql);
    if (0 == affected_rows()) {
        // alright, that failed and we're out of options... error and move along
        $GLOBALS['errmsg'] = mkerror(_('Unable to purge statistics for survey'));
        return false;
    }

    // delete succeeded, sweet
    return true;
}

// }}}

?>
