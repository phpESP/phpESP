<?php

 # $Id$
/* {{{ proto void mkcrossformat (array weights, integer qid)
  Builds HTML to allow for cross tabulation/analysis reporting. */
	function mkcrossformat($counts,$qid,$tid){
		$cids = array();
		$cidCount = 0;
		// let's grab the cid values for each of the questions
		// that we allow cross analysis on.
		if ($tid == 1){
			$sql = "SELECT choice_id from response_bool
				WHERE question_id = $qid
				ORDER BY response_id";	
		}
		else {
			$sql = "SELECT id FROM question_choice 
				WHERE question_id = $qid 
				ORDER BY id";
		}
		$result = mysql_query($sql);
		while ($cid = mysql_fetch_row($result)){
			array_push($cids,$cid[0]) ;
		}
		mysql_free_result($result);
        	echo ("<table width=\"90%\" border=\"0\">\n");
                echo ("<tbody>\n");
       		while(list($content,$num) = each($counts)) {
			if($bg != $GLOBALS['ESPCONFIG']['bgalt_color1'])
                        	$bg = $GLOBALS['ESPCONFIG']['bgalt_color1'];
                	else
                        	$bg = $GLOBALS['ESPCONFIG']['bgalt_color2'];
                	echo ("<tr bgcolor=\"$bg\">\n");
                	echo ("<td width=\"34\" height=\"23\" align=\"left\" valign=\"top\" bgcolor=\"#0099FF\">");
			echo ("<div align=\"center\">\n");
                	echo ("<input type=\"checkbox\" name=\"cids[]\" value=\"".$cids[$cidCount++]."\">\n");
              		echo ("</div></td>\n");
                	echo ("<td width=\"506\" align=\"left\">".$content."</td>\n");
                	echo ("</tr>\n");
		}
                echo ("</tbody>\n");
                echo ("</table>	\n");	
	}
/* }}} */
?>
