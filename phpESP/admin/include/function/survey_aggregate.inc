<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* survey_aggregate.inc -- Create a new, editable survey by aggregating the questions of one or more other surveys.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
*/

// {{{ survey_aggregate()              Aggregate multiple surveys together to create a brand new survey
// NOTE: Top-most survey information (name, title, owner, etc.) comes from first survey in aggregate list.

function survey_aggregate($sids) {
    // make sure we have valid arguments
    if (is_scalar($sids) && is_numeric($sids)) {
        $sids = array ($sids);
    } else if (! is_array($sids)) {
        return false;
    }

    // don't be silly
    if (0 == count($sids)) {
        return true;
    }

    // get the survey data from the first survey
    reset($sids);
    $first_sid = current($sids);

    $sql = sprintf('SELECT * FROM %s WHERE id=%d', $GLOBALS['ESPCONFIG']['survey_table'], $first_sid);
    $res = execute_sql($sql, '', ADODB_FETCH_ASSOC);
    if (1 == record_count($res)) {
        $survey = fetch_row($res);
    }
    db_close($res);
    if (! isset($survey)) {
        return false;
    }

    // update its data so that it's a new survey
    // ... clear the sid
    // ... change the owner
    // ... change the name
    // ... reset the status
    $survey['id']     = null;
    $survey['owner']  = (isset($_SESSION['acl']['username']) ? $_SESSION['acl']['username'] : $survey['owner']);
    $survey['name']  .= '_'. _('copy');
    $survey['status'] = 0;

    // check for 'name' conflict, and resolve
    $i = 0;
    $sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=". _addslashes($survey['name']);
    $res = execute_sql($sql);
    while (0 < $res->fields[0]) {
        $res->MoveNext();
        $sql = "SELECT COUNT(*) FROM ".$GLOBALS['ESPCONFIG']['survey_table']." WHERE name=". _addslashes($survey['name'].++$i);
        $res = execute_sql($sql);
    }
    if ($i) {
        $survey['name'] .= $i;
    }

    // create new survey
    $survey['changed'] = sys_time_stamp();
    $sql = sql_for_insert_into_table($GLOBALS['ESPCONFIG']['survey_table'], $survey);
    $res = execute_sql($sql);
    if (! $res) {
        return false;
    }
    $new_sid = insert_id('survey_id_seq');

    // copy the questions for each of the surveys
    // NOTE1: we increase the position through each loop so that the questions will follow each other
    // NOTE2: SFID 2672129: Question positions begin at 0.
    $pos = 0;
    foreach ($sids as $sid) {
        $pos = survey_copy_questions_and_conditions($sid, $new_sid, $pos);
        if (false === $pos) {
            return false;
        }
    }

    return $new_sid;
}

// }}}
// {{{ survey_copy_questions()         Copy questions from one survey to another, starting at a given position

function survey_copy_questions_and_conditions($sid, $new_sid, $pos = 0) {
    $has_choices = esp_type_has_choices();

    // make copies of all the questions
    // NOTE: This is done the hard way: SELECT and then iterated INSERT
    $question_fields = array('id', 'survey_id', 'name', 'type_id', 'result_id', 'length', 'precise', 
            'position', 'content', 'required', 'deleted', 'public');
    $sql = "SELECT ". join(', ',$question_fields)." FROM ".$GLOBALS['ESPCONFIG']['question_table']."
        WHERE survey_id=${sid} AND deleted='N'
        ORDER by position, id";
    $result = execute_sql($sql,"",ADODB_FETCH_ASSOC);

    array_shift($question_fields);
    while($question = fetch_row($result)) {
        $result->MoveNext();
        $tid = $question['type_id'];
        $qid = $question['id'];
        // fix some fields first
        $question['survey_id'] = $new_sid;
        $question['position'] = $pos++;

        // copy question to new survey
        array_shift($question);
        if(!isset($question['result_id'])) {
            $question['result_id'] = 'NULL';
        }
        $question['name'] = _addslashes($question['name']);
        $question['content'] = _addslashes($question['content']);
        $question['required'] = _addslashes($question['required']);
        $question['deleted'] = _addslashes($question['deleted']);
        $question['public'] = _addslashes($question['public']);
        $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." ";
        $sql .= '('. join(',', $question_fields) .') ';
        $sql .= 'VALUES ( '. join(',',array_values($question)) .' )';
        if(!execute_sql($sql))
            return(false);
        $new_qid = insert_id('question_id_seq');

        // make copies of all the question choices (if exist)
        if($has_choices[$tid]) {
            $question_choice_fields = array('question_id', 'content', 'value', 'feedback', 'credit');
            $sql = "SELECT ". join(', ',$question_choice_fields)."
                FROM ".$GLOBALS['ESPCONFIG']['question_choice_table']."
                WHERE question_id=${qid}
                ORDER BY id";
            $result2 = execute_sql($sql,"",ADODB_FETCH_ASSOC);
            while($choice = fetch_row($result2)) {
                $result2->MoveNext();
                $choice['question_id'] = $new_qid;
                $choice['content']     = _addslashes($choice['content']);
                $choice['value']       = _addslashes($choice['value']);
                $choice['feedback']    = _addslashes($choice['feedback']);
                $choice['credit']      = _addslashes($choice['credit']);

                $sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_choice_table']." ";
                $sql .= '('. join(',',$question_choice_fields) .') ';
                $sql .= 'VALUES ( '. join(',',array_values($choice)) .' )';
                if(!execute_sql($sql))
                    return(false);
            }
            db_close($result2);
        }
    }
    db_close($result);

    // copy conditions
    $sql = "
INSERT INTO ".$GLOBALS['ESPCONFIG']['condition_table']."
       (id,  survey_id, q1_id,    q2_id,    cond, cond_value)
SELECT NULL, $new_sid,  qnew1.id, qnew2.id, cond, cond_value
  FROM ".$GLOBALS['ESPCONFIG']['question_table']." qold1, 
        ".$GLOBALS['ESPCONFIG']['question_table']." qold2, 
        ".$GLOBALS['ESPCONFIG']['question_table']." qnew1,
        ".$GLOBALS['ESPCONFIG']['question_table']." qnew2, 
        ".$GLOBALS['ESPCONFIG']['condition_table']." c
 WHERE qold1.id=c.q1_id AND qold2.id=c.q2_id AND
       qold1.survey_id=$sid AND qold2.survey_id=$sid AND
       qnew1.survey_id=$new_sid AND qnew1.position=qold1.position AND
       qnew1.deleted=qold1.deleted AND
       qnew2.survey_id=$new_sid AND
       qnew2.position=qold2.position AND
       qnew2.deleted=qold2.deleted
    ";
    $result = execute_sql($sql);
    if (false === $result) {
        return false;
    }

    return $pos;
}

// }}}
// {{{ sql_for_insert_into_table()     Insert into a table

function sql_for_insert_into_table($table, $values) {
    foreach ($values as $field => $value) {
        if (is_null($value)) {
            $values[$field] = 'null';
        } else if (is_scalar($value)) {
            if (! is_numeric($value)) {
                if ($value == sys_time_stamp()) {
                } else {
                    $values[$field] = _addslashes($value);
                }
            }
        } else {
            trigger_error("Unknown value=[$value] at key=[$field]", E_USER_WARNING);
            $values[$field] = 'null';
        }
    }

    return sprintf(
        'INSERT INTO %s (%s) VALUES (%s)',
        $table,
        implode(',', array_keys($values)),
        implode(',', $values)
    );
}

// }}}

?>
