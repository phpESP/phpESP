<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

/* NOTE: In order for these to work you must have:
 *    register_globals = On
 * in php.ini
 */

// bool[]	arr_has_answers(void);
// !exit	goto_thankyou(int sid, char* referer);
// char*	check_required(int sid, int section);
// char*->char*	retrieve_values(int sid, int section);
// int		insert_values(int sid, int section, int rid);
// bool		complete_response(int rid);
// char*	email_values(int sid, int rid);
// void		clear_results(int sid);

if(!defined('_FUNCS')) {
 	define('_FUNCS',TRUE);


	// redirect to thank you page for survey ID 'sid'
	// exits PHP!
	function goto_thankyou($sid,$referer) {
		global $cf_thank_body;
		global $cf_thank_head;
		$sql = "SELECT thanks_page,thank_head,thank_body FROM surveys WHERE id='${sid}'";
		$result = mysql_query($sql);
		list($thank_url,$thank_head,$thank_body) = mysql_fetch_row($result);
		if((empty($thank_head)  && empty($thank_body)) && !empty($thank_url)) {
?>
<script language="JavaScript">
<!--
window.location="<?php echo($thank_url); ?>"
//-->
</script>
<noscript><h2>Thank You for completing this survey.</h2>
Please click <a href="<?php echo($thank_url); ?>">here</a> to continue.
</noscript>
<?php
			exit;
		}

		if(empty($thank_body) && empty($thank_head)) {
			$thank_body = $cf_thank_body;
			$thank_head = $cf_thank_head;
		}
?>
<h2><?php echo($thank_head); ?></h2>
<blockquote><?php echo($thank_body); ?></blockquote>
<a href="<?php echo($referer); ?>">Return</a>
<?php
		return;
	}




	// clear the results of a survey (non-reversable)
	function clear_results($sid) {
		$sql = "SELECT id FROM survey_questions WHERE survey_id='${sid}'";
		$result = mysql_query($sql);
		$qids = array();
		while(list($qid) = mysql_fetch_row($result)) {
			array_push($qids, $qid);
		}
		mysql_free_result($result);
		$qidstr = "question_id IN " . ereg_replace("([0-9]+)","'\\1'", "(" . join(",", $qids) . ")");
	
		$tables = array('answers_bool','answers_multiple','answers_other','answers_rank','answers_single','answers_text');
		foreach($tables as $table) {
			$sql = "DELETE FROM ${table} WHERE ${qidstr}";
			$result = mysql_query($sql);
		}
		$sql = "DELETE FROM survey_responses WHERE survey_id=${sid}";
		mysql_query($sql);

		return;
	}

	require($PIECES."/funcs_html".$EXT);
	require($PIECES."/funcs_render".$EXT);
	require($PIECES."/function.survey_results".$EXT);
	require($PIECES."/function.survey_report".$EXT);
	require($PIECES."/function.survey_copy".$EXT);
	require($PIECES."/function.survey_purge".$EXT);
//	require($PIECES."/library.merge_utils".$EXT);
	require($PIECES."/function.survey_merge".$EXT);

} // end if !defined(_FUNCS)

?>
