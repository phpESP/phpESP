<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

/* {{{ proto bool question_render(int $question_id, int respnse_number)
   Checks if conditions for qid are met */
function isequal (&$val1,&$val2) {
     return $val1==$val2;
}
function isnequal (&$val1,&$val2) {
     return $val1!=$val2;
}
function issmallerthan (&$val1,&$val2) {
     return $val1<$val2;
}
function isbiggerthan (&$val1,&$val2) {
     return $val1>$val2;
}

function question_conditioncheck($sid,$qid,$rid) {
  $conditions[0]="isequal";
  $conditions[1]="isnequal";
  $conditions[2]="issmallerthan";
  $conditions[3]="isbiggerthan";

  $return_result=TRUE;

  $sql = "SELECT * FROM ".$GLOBALS['ESPCONFIG']['condition_table']." WHERE q1_id='$qid'";
  $result = execute_sql($sql);  
  # If there are no conditions, we need to show the question, so we return true
  if (record_count($result)==0) {
     return true;
  }
  while(list($id,$sid,$q1_id,$q2_id,$cond,$cond_val) = fetch_row($result)) {
     $result->MoveNext();
     if ($rid>0) {
	$resp=array();
        $resp=response_select($sid, $rid, $col = null, array($q2_id));
	if (isset($resp[$q2_id])) {
	   $resp_val=$resp[$q2_id][0];
	   // for dropdown and some other types, this value is an array
	   // we take the value (for now) and not the index ...
	   if (is_array($resp_val)) {
		$resp_val=$resp_val[0];
	   }
	   if (call_user_func($conditions[$cond],$resp_val,$cond_val) == TRUE) {
	      # the condition is fullfilled, so we need to show the question
  	      return true;
	   }
	}
     }
  }
  return false;
}
/* }}} */
?>
