<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

/* {{{ proto bool survey_export_csv(int survey_id, string filename)
	Exports the results of a survey to a CSV file.
	Returns true on success.
	*/
function survey_export_csv($sid, $filename) {
	$umask = umask(0077);
	$fh = fopen($filename, 'w');
	umask($umask);
	if(!$fh)
		return 0;

	$columns = array();
	$types = array();
	$arr = array();
	
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

	$sql = "SELECT q.name, q.type_id
		FROM question q
		WHERE q.survey_id = $sid AND
		q.deleted = 'N'
		ORDER BY position";
	$result = mysql_query($sql);
	$numcols = mysql_num_rows($result);
	while( list( $col, $type ) = mysql_fetch_row($result) ) {
		array_push($columns, $col);
		array_push($types, $id_to_csv_map[$type]);
	}
	mysql_free_result($result);		
	
	$num = 0;
	
	fputs($fh, join(',', $columns) . "\n");

	$sql = "SELECT id FROM response 
		WHERE survey_id='$sid' AND complete='Y'
		ORDER BY submitted";
	$result = mysql_query($sql);
	while($row = mysql_fetch_row($result)) {
		// get the response
		$response = response_select_named($sid, $row[0]);
		
		$arr = array();
		
		// merge it
		for($i = 0; $i < $numcols; $i++) {
			if($types[$i])
				array_push($arr, '"'. $response[$columns[$i]] .'"');
			else
				array_push($arr, $response[$columns[$i]]);
		}
		fputs($fh, join(',', $arr) . "\n");
	}
	mysql_free_result($result);
	
	fflush($fh);
	fclose($fh);
	
	return 1;
}
?>
