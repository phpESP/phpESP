<?php

# $Id$

## XXX: This should be moved to the lib/ or function/ directory.
## -James

	/* {{{ proto void navigate_responses(int sid, int rid)
	   Builds Previous/Next Navigation allowing easy navigation
	   through individual survey responses. */ 
	function navigate_responses($sid,$rid){
		$sql = "SELECT R.id
			FROM response R
			WHERE R.survey_id='${sid}' AND
			R.complete = 'Y'
			ORDER BY R.id";
		$result = mysql_query($sql);
		$total = mysql_num_rows($result);
		if ($total < 1) {
			$errmsg = _('Error opening survey.').''._('No responses found.')."[ID:${sid}]";
			return ($errmsq);
			if ($GLOBALS['ESPCONFIG']['DEBUG'])
				echo ("<!-- \$errmsg = '$errmsg' -->\n");
		}
		else {
			$ridarray = array();
			while ($row = mysql_fetch_row($result)) {
				array_push($ridarray, $row[0]);
			}
			mysql_free_result($result);

			$rows_per_page = 1;
			$pages = ceil($total / $rows_per_page);
			echo "<center>";

			//let's create the dynamic links now
			$arrcount = sizeof($ridarray);
			$counter = 0;
			echo "<input type=\"hidden\" name=\"sid\" value=\"$sid\">";
			echo "<input type=\"hidden\" name=\"rid\" value=\"\">";
			echo "<input type=\"hidden\" name=\"where\" value=\"results\">";

			//determine the pos of selected rid in the rid array....
			foreach ($ridarray as $x){
				$counter++;
				if ($x == $rid){
					$current_array_pos = $counter-1;
				}
			}
			echo "<h2>Navigate Individual Respondent Submissions</h2><br>";

			// The "Previous" Link.
			if ($current_array_pos > 0) {
				$showprev = $ridarray[$current_array_pos-1];
				$url = ("\"processRequest(".$showprev."); return false;\"");
				echo "<a href=\"javascript:;\" onClick=".$url.">Previous</a>\n";
			}

			//create survey respondent links.
			for ($i = 1; $i < $pages + 1; $i++) {
				$ridd = $ridarray[$i-1];
				echo "<input type=\"hidden\" name=\"rid".$i."\" value=\"$ridd\">";
				$url = ("\"processRequest(document.phpesp.rid".$i.".value); return false;\"");
				if ($current_array_pos == $i-1){
					echo "| <b><a href=\"javascript:;\" onClick=".$url.">".$i."</a></b> |\n";
				}
				else {
					echo "| <a href=\"javascript:;\" onClick=".$url.">".$i."</a> |\n";
				}
			}

			// The "Next" Link.
			if ($current_array_pos < $arrcount-1) {
				$shownext = $ridarray[$current_array_pos+1];
				$url = ("\"processRequest(".$shownext."); return false;\"");
				echo "<a href=\"javascript:;\" onClick=".$url.">Next</a>\n";
			}
			echo "</center>";
		}
?>
		<script language="JavaScript">
		<!--
			function processRequest(rids){
				document.phpesp.rid.value=rids;
				document.phpesp.submit();
			}
		//-->
		</script>
<?php
	}
	/* }}} */	
?>
