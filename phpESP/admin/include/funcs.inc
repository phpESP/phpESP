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

	if(get_magic_quotes_gpc()) {
		function XADDSLASHES($a) { return($a); }
	} else {
		function XADDSLASHES($a) { return(addslashes($a)); }
	}
	set_magic_quotes_runtime(0);

	function esp_where($where) {
		if(empty($where))
			$where = 'index';
		$where = strtolower(ereg_replace(' +','_',$where));
		if(!ereg('^[A-Za-z0-9_]+$',$where))	// Valid chars are [A-Za-z0-9_]
			$where = 'index';
		if(!file_exists($GLOBALS['ESPCONFIG']['include_path'].'/where/'.$where.$GLOBALS['ESPCONFIG']['extension']))
			$where = 'index';
		if(!file_exists($GLOBALS['ESPCONFIG']['include_path'].'/where/'.$where.$GLOBALS['ESPCONFIG']['extension'])) {
			echo('<b>'. _('Unable to open include file. Check INI settings. Aborting.'). '</b>');
			exit;
		}
		return($GLOBALS['ESPCONFIG']['include_path'].'/where/'.$where.$GLOBALS['ESPCONFIG']['extension']);
	}
	
	// redirect to thank you page for survey ID 'sid'
	// exits PHP!
	function goto_thankyou($sid,$referer) {
		$thank_body = $GLOBALS['ESPCONFIG']['thank_body'];
		$thank_head = $GLOBALS['ESPCONFIG']['thank_head'];
		$sql = "SELECT thanks_page,thank_head,thank_body FROM survey WHERE id='${sid}'";
		$result = mysql_query($sql);
		list($thank_url,$thank_head,$thank_body) = mysql_fetch_row($result);
		mysql_free_result($result);
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
			$thank_body = $GLOBALS['ESPCONFIG']['thank_body'];
			$thank_head = $GLOBALS['ESPCONFIG']['thank_head'];
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
		$sql = "SELECT id FROM question WHERE survey_id='${sid}'";
		$result = mysql_query($sql);
		$qids = array();
		while(list($qid) = mysql_fetch_row($result)) {
			array_push($qids, $qid);
		}
		mysql_free_result($result);
		$qidstr = 'question_id ' . array_to_insql($qids);
	
		$tables = array('response_bool','response_multiple','response_other','response_rank','response_single','response_text');
		foreach($tables as $table) {
			$sql = "DELETE FROM ${table} WHERE ${qidstr}";
			$result = mysql_query($sql);
		}
		$sql = "DELETE FROM response WHERE survey_id=${sid}";
		mysql_query($sql);

		return;
	}

	require($ESPCONFIG['include_path']."/lib/espauth".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/lib/esphtml.forms".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/lib/esphtml.results".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/lib/espmerge".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/lib/espresponse".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/lib/espsql".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_copy".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_merge".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_purge".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_render".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_report".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_results".$ESPCONFIG['extension']);
	require($ESPCONFIG['include_path']."/function/survey_update".$ESPCONFIG['extension']);

} // end if !defined(_FUNCS)

?>
