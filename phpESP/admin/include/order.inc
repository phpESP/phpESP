<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	session_register("survey_id");

	$url = $PHP_SELF . '?where=tab&tab=order';

	if(!empty($op)) {
		// trying to perform some operation
		switch(strtolower($op)) {
			case 'u':	// move up
				$sql = "SELECT position FROM survey_questions WHERE id='${qid}'";
				$result = mysql_query($sql);
				$my_pos = mysql_result($result,0,0);
				$sql = "SELECT id,position FROM survey_questions WHERE survey_id='${survey_id}' AND status='' AND position < '${my_pos}' ORDER BY position DESC LIMIT 1";
				$result = mysql_query($sql);
				if(mysql_num_rows($result) != 1)
					break;
				list($her_qid,$her_pos) = mysql_fetch_row($result);

				// set "above" quesiton to my position
				$sql = "UPDATE survey_questions SET position='${my_pos}' WHERE id='${her_qid}'";
				if(!@mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					// go back to the way we were
					mysql_query("UPDATE survey_questions SET position='${my_pos}' WHERE id='${qid}'");
					break;
				}
				// set "my" question to "above" position
				$sql = "UPDATE survey_questions SET position='${her_pos}' WHERE id='${qid}'";
				if(!@mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					break;
				}
				break;

			case 'x':	// delete
				$sql = "SELECT status FROM survey_questions WHERE id='${qid}'";
				$result = mysql_query($sql);
				$status = mysql_result($result,0,0) | STATUS_DELETED;
				mysql_free_result($result);
				$sql = "UPDATE survey_questions SET status='${status}' WHERE id='${qid}'";
				$result = mysql_query($sql);
				break;

			case 'd':	// move down
				$sql = "SELECT position FROM survey_questions WHERE id='${qid}'";
				$result = mysql_query($sql);
				$my_pos = mysql_result($result,0,0);
				$sql = "SELECT id,position FROM survey_questions WHERE survey_id='${survey_id}' AND status='' AND position > '${my_pos}' ORDER BY position ASC LIMIT 1";
				$result = mysql_query($sql);
				if(mysql_num_rows($result) != 1)
					break;
				list($her_qid,$her_pos) = mysql_fetch_row($result);

				// set "below" quesiton to my position
				$sql = "UPDATE survey_questions SET position='${my_pos}' WHERE id='${her_qid}'";
				if(!@mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					// go back to the way we were
					mysql_query("UPDATE survey_questions SET position='${my_pos}' WHERE id='${qid}'");
					break;
				}
				// set "my" question to "below" position
				$sql = "UPDATE survey_questions SET position='${her_pos}' WHERE id='${qid}'";
				if(!@mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					break;
				}
				break;
				
			case 'b':	// add section break
				$sql = "SELECT MAX(position) FROM survey_questions WHERE survey_id='${survey_id}' AND status=''";
				$result = mysql_query($sql);
				$position = mysql_result($result,0,0) + 10;
				
				$sql = "INSERT INTO survey_questions (survey_id,type_id,position,content) VALUES
				('${survey_id}',99,'${position}','break')";
				$result = mysql_query($sql);
				break;

		} // end select
	} // end op check

	$sql = "SELECT id,type_id,position,content FROM survey_questions WHERE survey_id='${survey_id}' AND status='' ORDER by position";
	$result = mysql_query($sql);

	$i=0;
?>
Change the order that questions are presented by clicking on the up and down arrows.
Remove a question by clicking on the X.
<hr>
<table>
<?php	while(list($qid,$tid,$pos,$content) = mysql_fetch_row($result)) { ?>
	<tr>
		<td>
<map name="Q<?php echo($qid); ?>">
<area shape="rect" coords="0,0,11,22"  href="<?php echo($url); ?>&op=u&qid=<?php echo($qid); ?>" alt="Move Up">
<area shape="rect" coords="11,0,22,22" href="<?php echo($url); ?>&op=x&qid=<?php echo($qid); ?>" alt="Delete">
<area shape="rect" coords="22,0,32,22" href="<?php echo($url); ?>&op=d&qid=<?php echo($qid); ?>" alt="Move Down">
</map><img src="<?php echo($IMAGE_PATH); ?>knobs.gif" border="0" usemap="#Q<?php echo($qid); ?>">
		</td>
		<td>
<?php
			if($tid != 99)
				echo(++$i .'. '. $content);
			else
				echo('<b>----- Section Break -----</b>');
?>
		</td>
	</tr>
<?php	} ?>
</table>
<hr>
<a href="<?php echo($url); ?>&op=b">Click here to add a section (page) break</a>.
