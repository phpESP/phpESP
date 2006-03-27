<?php

    /* $Id$ */

    /* vim: set tabstop=4 shiftwidth=4 expandtab: */

    // Written by James Flemer
    // <jflemer@alum.rpi.edu>

    /* {{{ proto array survey_generate_results(int format, int survey_id)
    Exports the results of a survey to an array.
    */
    function survey_generate_results($format, $sid) {
        $output = array();
        $id_to_csv_map = array(
            '0',	// 0: unused
            '1',	// 1: bool -> boolean
            '1',	// 2: text -> string
            '1',	// 3: essay -> string
            '1',	// 4: radio -> string
            '1',	// 5: check -> string
            '1',	// 6: dropdn -> string
            '0',	// 7: rating -> number
            '0',	// 8: rate -> number
            '1',	// 9: date -> string
            '0' 	// 10: numeric -> number
        );
        $types = array(
            0,
            0,
            1,
        );

        //Generate Columns:
        $columns = array(
            'RESPONSE',
            'SUBMITTED',
            'USERNAME',
        );
        switch ($format) {
            case 'spss_csv':
                // SPSS Output
                $numcols = 3;
                $columns2 = $columns;
                $sql = "SELECT q.id, q.type_id FROM {$GLOBALS['ESPCONFIG']['question_table']} q
                WHERE q.survey_ID = $sid AND q.deleted = 'N' and q.type_id < 50 ORDER BY position";
                $result = execute_sql($sql);
                $qcount = 0; // rating and multiple select (checkbox) count as one
                while ( list ($qid, $type) = fetch_row($result)) {
                    $result->MoveNext();
                    if($type == '8' || $type == '5') { //rating question or multiple select (checkbox)
                    $sql = "SELECT c.id FROM ".$GLOBALS['ESPCONFIG']['question_table']." q,
                    ".$GLOBALS['ESPCONFIG']['question_choice_table']." c WHERE q.id = $qid
                    AND q.id = c.question_id ORDER BY c.id ASC";
                    $result2 = execute_sql($sql);
                    $ratecols=0;
                    $qcount++;
                    while( list($cid) = fetch_row($result2) ) {
                        $result2->MoveNext();
                        $ratecols++;
                        array_push($columns, array("Q".$qcount."_".$ratecols));
                        array_push($columns2, $qid."_".$cid);
                        array_push($types, $id_to_csv_map[$type]);
                    }
                    $numcols=$numcols+$ratecols;
                    db_close($result2);
                    } else {
                        $numcols++;
                        $qcount++;
                        array_push($columns, array("Q".$qcount));
                        array_push($columns2, $qid);
                        array_push($types, $id_to_csv_map[$type]);
                    }
                }
                db_close($result);

                // Generate Data (This could get tricky)
                $sql = "SELECT r.id,r.submitted,r.username
                FROM ".$GLOBALS['ESPCONFIG']['response_table']." r
                WHERE r.survey_id=$sid AND r.complete='Y'
                ORDER BY r.submitted";
                $result = execute_sql($sql);

                $columns[0] = array('RESPONSE');
                $columns[1] = array('SUBMITTED');
                $columns[2] = array('USERNAME');

                while ($row = fetch_row($result)) {
                    $result->MoveNext();

                    //Get the Response
                    $response = response_select($sid, $row[0], 'type_id,name');
                    array_push($columns[0], $row[0]);
                    array_push($columns[1], $row[1]);
                    array_push($columns[2], $row[2]);

                    // Merge Repsonse into output array
                    for($i = 3; $i < $numcols; $i++) {
                        $key = split("_",$columns2[$i]);

                        if(isset($response[$columns2[$i]]) || isset($response[$key[0]])){
                            // This is a hack for multiple select,
                            // seems easier than modifying response_select
                            if (isset($response[$key[0]][0]) && is_array($response[$key[0]][0])){
                                // check box (only question type that returns an array)
                                $match = 0;
                                for ($j = 0; $j < sizeof ($response[$key[0]]); $j++) {
                                    if ($response[$key[0]][$j][3] == 'other'.'_'.$key[1]) {
                                        array_push($columns[$i],"Other: ".$response[$key[0].'_'.$key[1]][2]);
                                        $match = 1;
                                    } else if ($response[$key[0]][$j][3] == $key[1]) {
                                        array_push($columns[$i],$response[$key[0]][$j][2]);
                                        $match = 1;
                                    }
                                }
                                if ($match == 0)
                                array_push($columns[$i], " ");
                            } elseif (isset($response[$columns2[$i]][0]) && $response[$columns2[$i]][0] == 8) {
                                // rating
                                array_push($columns[$i], $response[$columns2[$i]][3]);
                            } elseif(isset($response[$columns2[$i]][0]) && ($response[$columns2[$i]][0]==4 || $response[$columns2[$i]][0]==6)){
                                /* radio or dropdown*/
                                if($response[$columns2[$i]][2] == "!other"){
                                    list(,$c)=split("_", $response[$columns2[$i]][3]);
                                    array_push($columns[$i], "Other: ".$response[$columns2[$i].'_'.$c][2]);
                                } else {
                                    array_push($columns[$i], $response[$columns2[$i]][2]);
                                }
                            } else {
                                array_push($columns[$i], $response[$columns2[$i]][2]);
                            }
                        } else {
                            array_push($columns[$i], " ");
                        }
                    }

                }
                db_close($result);

                $output = array();
                for($i = 0; $i < $numcols; $i++) {
                    $x = 0;
                    $temp_val = "";
                    foreach($columns[$i] as $key=>$value) {
                        if (!isset($output[$x])) {
                            $output[$x] = array();
                        }
                        if (is_array($value)) {
                            $temp_val = join(',', $value);
                        }
                        else {
                            $temp_val = $value;
                        }
                        switch ($types[$i]) {
                            case 1: /* string */
                                /* Excel seems to allow "\n" inside a quoted string, but
                                * "\r\n" is used as a record separator and so "\r" may
                                * not occur within a cell. So if one would like to preserve
                                * new-lines in a response, remove the "\n" from the
                                * regex below.
                                */
                                $temp_val = ereg_replace("[\r\n\t]", ' ', $temp_val);
                                $temp_val = ereg_replace('"', '""', $temp_val);
                                $temp_val = '"'. $temp_val .'"';
                                /* fall through */
                                break;
                        }
                        $output[$x][$i] = $temp_val;
                        $x++;
                    }
                }
                return $output;
                break;
                //NOT IMPLEMENTED YET
                break;

            case 'xml':
                //NOT IMPLEMENTED YET
                break;

            case 'csv_short_header':
                //pass through
            case 'tab':
                $columns2=$columns;  	//columns2 is used to relate columns to data returned from response_select() 
                $numcols = 3;		//total columns
                $qcount=0;		//total questions (rating questions count as 1)
                $sql = "SELECT q.id, q.type_id
                FROM ".$GLOBALS['ESPCONFIG']['question_table']." q
                WHERE q.survey_id = $sid AND
                q.deleted = 'N' AND q.type_id < 50
                ORDER BY position";
                $result = execute_sql($sql);
                while( list( $qid, $type ) = fetch_row($result) ) {
                    $result->MoveNext();
                    if($type == '8') { //rating question
                    $sql = "SELECT c.id FROM ".$GLOBALS['ESPCONFIG']['question_table']." q,
                    ".$GLOBALS['ESPCONFIG']['question_choice_table']." c WHERE q.id = $qid AND q.id = c.question_id
                    ORDER BY position";
                    $result2 = execute_sql($sql);
                    $ratecols=0;
                    $qcount++;
                    while( list($cid) = fetch_row($result2) ) {
                        $result2->MoveNext();
                        $ratecols++;
                        array_push($columns, $qcount."_".$ratecols);
                        array_push($columns2, $qid."_".$cid);
                        array_push($types, $id_to_csv_map[$type]);
                    }
                    $numcols=$numcols+$ratecols;
                    db_close($result2);
                    } else {
                        $numcols++;
                        $qcount++;
                        array_push($columns, $qcount);
                        array_push($columns2, $qid);
                        array_push($types, $id_to_csv_map[$type]);
                    }
                }
                array_push($output, $columns);
                db_close($result);

                //Generate data:
                $sql = "SELECT r.id,r.submitted,r.username FROM ".$GLOBALS['ESPCONFIG']['response_table']." r
                WHERE r.survey_id=$sid AND r.complete='Y'
                ORDER BY r.submitted";
                $result = execute_sql($sql);
                while($row = fetch_row($result)) {
                    $result->MoveNext();
                    // get the response
                    $response = response_select($sid, $row[0], 'type_id,name');
                    $arr = array();
                    array_push($arr, $row[0]);
                    array_push($arr, $row[1]);
                    array_push($arr, $row[2]);
                    // merge it
                    for($i = 3; $i < $numcols; $i++) {
                        if(isset($response[$columns2[$i]])){
                            if(isset($response[$columns2[$i]][0]) && is_array($response[$columns2[$i]][0])){
                                /* check box */
                                $tempresult=array();
                                foreach ($response[$columns2[$i]] as $checkboxdata) {
                                    if($checkboxdata[2]=="!other"){
                                        list(,$c)=split("_", $checkboxdata[3]);
                                        array_push($tempresult,"Other: ".$response[$columns2[$i].'_'.$c][2]);

                                    }
                                    else{
                                        array_push($tempresult,$checkboxdata[2]);
                                    }
                                }
                                $thisresult=implode(",",$tempresult);
                            }
                            elseif(isset($response[$columns2[$i]][0]) && $response[$columns2[$i]][0]==8 ){
                                /* rating */
                                $thisresult=$response[$columns2[$i]][3]; //db has these as 0 to N-1, lets add 1
                            }
                            elseif(isset($response[$columns2[$i]][0]) && ($response[$columns2[$i]][0]==4 || $response[$columns2[$i]][0]=6)){
                                /* radio or dropdown*/
                                if($response[$columns2[$i]][2]=="!other"){
                                    list(,$c)=split("_", $response[$columns2[$i]][3]);
                                    $thisresult="Other: ".$response[$columns2[$i].'_'.$c][2];
                                }
                                else{
                                    $thisresult=$response[$columns2[$i]][2];
                                }
                            }
                            else{
                                $thisresult=$response[$columns2[$i]][3];
                            }
                        }
                        else{
                            $thisresult="";
                        }
                        switch ($types[$i]) {
                            case 2: /* special */
                                break;
                            case 1: /* string */
                                if (isset($response[$columns2[$i]])) {
                                    /* Excel seems to allow "\n" inside a quoted string, but
                                    * "\r\n" is used as a record separator and so "\r" may
                                    * not occur within a cell. So if one would like to preserve
                                    * new-lines in a response, remove the "\n" from the
                                    * regex below.
                                    */
                                    $thisresult = ereg_replace("[\r\n\t]", ' ', $thisresult);
                                    $thisresult = ereg_replace('"', '""', $thisresult);
                                    $thisresult = '"'. $thisresult .'"';
                                }
                                /* fall through */
                            case 0: /* number */
                                if (isset($response[$columns2[$i]])) {
                                    array_push($arr, $thisresult);
                                }
                                else {
                                    array_push($arr, '');
                                }
                                break;
                        }
                    }
                    array_push($output, $arr);
                }
                db_close($result);
                break;

            default:	
                //csv_full_header
                $numcols = 3;

                $sql = "SELECT q.id, q.name, q.type_id
                FROM ".$GLOBALS['ESPCONFIG']['question_table']." q
                WHERE q.survey_id = $sid AND
                q.deleted = 'N' AND q.type_id < 50
                ORDER BY position";
                $result = execute_sql($sql);
                $numcols += record_count($result);
                while( list( $qid, $col, $type ) = fetch_row($result) ) {
                    $result->MoveNext();
                    if ($type == 8) {
                        /* rate */
                        $sql = "SELECT ".$GLOBALS['ESPCONFIG']['adodb_conn']->Concat("q.name", "' '", "c.content").", c.id FROM ".$GLOBALS['ESPCONFIG']['question_table']." q, ".$GLOBALS['ESPCONFIG']['question_choice_table']." c WHERE q.id = $qid AND q.id = c.question_id";
                        $result2 = execute_sql($sql);
                        $numcols += record_count($result2) - 1;
                        while( list($col, $rid) = fetch_row($result2) ) {
                            $result2->MoveNext();
                            $str1 = $col;
                            do {
                                $str2 = $str1;
                                $str1 = eregi_replace(
                                    "(^| )(what|which|why|how|who|where|how|is|are|were|the|a|it|of|do|you|your|please|enter)[ ?]",
                                    " ", $str2);
                            } while ($str1 != $str2);
                            $col = $str1;
                            $col = trim(strtoupper(eregi_replace(
                                "[^A-Z0-9]+", " ", $col)));
                                $col = ereg_replace(' +','_',$col);
                                array_push($columns, array($qid."_".$rid=>$col));
                                array_push($types, $id_to_csv_map[$type]);
                        }
                        db_close($result2);
                    } else {
                        array_push($columns, array($qid=>$col));
                        array_push($types, $id_to_csv_map[$type]);
                    }
                }
                db_close($result);
                $num = 0;

                $sql = "SELECT r.id,r.submitted,r.username FROM ".$GLOBALS['ESPCONFIG']['response_table']." r
                WHERE r.survey_id=$sid AND r.complete='Y'
                ORDER BY r.submitted";
                $result = execute_sql($sql);
                $y = 0;
                if (record_count($result) == 0) {
                    $columns[0] = array('RESPONSE');
                    $columns[1] = array('SUBMITTED');
                    $columns[2] = array('USERNAME');
                }
                while($row = fetch_row($result)) {
                    $result->MoveNext();
                    if ($y == 0) {
                        $columns[0] = array($row[0]=>'RESPONSE');
                        $columns[1] = array($row[0]=>'SUBMITTED');
                        $columns[2] = array($row[0]=>'USERNAME');
                        $y = 1;
                    }
                    // get the response
                    $response = response_select_name($sid, $row[0]);
                    // merge it
                    for($i = 0; $i < $numcols; $i++) {
                        $match = 0;
                        foreach ($response as $key=>$value) {
                            if ($key === key($columns[$i])){
                                $match = 1;
                                array_push($columns[$i], $value[key($value)]);
                            }
                        }
                        if (($match == 0) and ($i > 2)) {
                            array_push($columns[$i], "");
                        }
                        if ($i <= 2) {
                            array_push($columns[$i],$row[$i]);
                        }
                    }
                }
                db_close($result);
                $output = array();
                for($i = 0; $i < $numcols; $i++) {
                    $x = 0;
                    $temp_val = "";
                    foreach($columns[$i] as $key=>$value) {
                        if (!isset($output[$x])) {
                            $output[$x] = array();
                        }
                        if (is_array($value)) {
                            $temp_val = join(',', $value);
                        }
                        else {
                            $temp_val = $value;
                        }
                        switch ($types[$i]) {
                            case 1: /* string */
                                /* Excel seems to allow "\n" inside a quoted string, but
                                * "\r\n" is used as a record separator and so "\r" may
                                * not occur within a cell. So if one would like to preserve
                                * new-lines in a response, remove the "\n" from the
                                * regex below.
                                */
                                $temp_val = ereg_replace("[\r\n\t]", ' ', $temp_val);
                                $temp_val = ereg_replace('"', '""', $temp_val);
                                $temp_val = '"'. $temp_val .'"';
                                /* fall through */
                                break;
                        }
                        $output[$x][$i] = $temp_val;
                        $x++;
                    }
                }
        }
        return $output;
    }
    /* }}} */

    /* {{{ proto bool survey_export_results(int type, int survey_id, string filename)
    Exports the results of a survey to a file.
    Returns true on success.
    */
    function survey_export_results($type, $sid, $filename) {
        $umask = umask(0077);
        $fh = fopen($filename, 'w');
        umask($umask);
        if(!$fh) {
            return 0;
        }
        switch ($type) {
            case 'spss_csv':
                $data = survey_generate_results($type, $sid);
                //do something with $data
                foreach ($data as $row) {
                    fputs($fh, implode(',', $row) . "\r\n");
                }
                break;

            case 'xml':
                //Not fully implemented yet
                $data = survey_generate_results($type, $sid);
                //do something with $data
                break;

            case 'tab':
                $data = survey_generate_results($type, $sid);
                foreach ($data as $row) {
                    fputs($fh, implode("\t", $row) . "\r\n");
                }
                break;

            default: //csv_full_header, csv_short_header
                $data = survey_generate_results($type, $sid);
                foreach ($data as $row) {
                    fputs($fh, implode(',', $row) . "\r\n");
                }
        }

        fflush($fh);
        fclose($fh);

        return 1;
    }
    /* }}} */
?>
