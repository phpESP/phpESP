<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

	$url = $ESPCONFIG['ME'] .'?where=tab&tab=order';

	$op  = $HTTP_GET_VARS['op'];
	$qid = XADDSLASHES($HTTP_GET_VARS['qid']);
	$sid = $HTTP_SESSION_VARS['survey_id'];
	
	if(!empty($op)) {
		// trying to perform some operation
		switch(strtolower($op)) {
			case 'u':	// move up
				$sql = "SELECT position FROM question WHERE id='${qid}'";
				$result = mysql_query($sql);
				$my_pos = mysql_result($result,0,0);
				mysql_free_result($result);
				$sql = "SELECT id,position 
					FROM question 
					WHERE survey_id='${sid}' AND
					deleted='N' AND
					position < '${my_pos}'
					ORDER BY position DESC
					LIMIT 1";
				$result = mysql_query($sql);
				if(mysql_num_rows($result) != 1)
					break;
				list($her_qid,$her_pos) = mysql_fetch_row($result);
				mysql_free_result($result);

				// set "above" quesiton to my position
				$sql = "UPDATE question SET position='${my_pos}' WHERE id='${her_qid}'";
				if(!mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					// go back to the way we were
					mysql_query("UPDATE question SET position='${my_pos}' WHERE id='${qid}'");
					break;
				}
				// set "my" question to "above" position
				$sql = "UPDATE question SET position='${her_pos}' WHERE id='${qid}'";
				if(!mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					break;
				}
				break;

			case 'x':	// delete
				$sql = "UPDATE question SET deleted='Y' WHERE id='${qid}'";
				$result = mysql_query($sql);
				break;

			case 'd':	// move down
				$sql = "SELECT position FROM question WHERE id='${qid}'";
				$result = mysql_query($sql);
				$my_pos = mysql_result($result,0,0);
				$sql = "SELECT id,position
					FROM question
					WHERE survey_id='${sid}' AND
					deleted='N' AND
					position > '${my_pos}'
					ORDER BY position ASC
					LIMIT 1";
				$result = mysql_query($sql);
				if(mysql_num_rows($result) != 1)
					break;
				list($her_qid,$her_pos) = mysql_fetch_row($result);
				mysql_free_result($result);

				// set "below" quesiton to my position
				$sql = "UPDATE question SET position='${my_pos}' WHERE id='${her_qid}'";
				if(!mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					// go back to the way we were
					mysql_query("UPDATE question SET position='${my_pos}' WHERE id='${qid}'");
					break;
				}
				// set "my" question to "below" position
				$sql = "UPDATE question SET position='${her_pos}' WHERE id='${qid}'";
				if(!mysql_query($sql)) {
					echo(mkerror(mysql_errno().": ".mysql_error()));
					break;
				}
				break;
				
			case 'b':	// add section break
				$sql = "SELECT MAX(position)
					FROM question
					WHERE survey_id='${sid}' AND
					deleted='N'";
				$result = mysql_query($sql);
				$position = mysql_result($result,0,0) + 10;
				mysql_free_result($result);
				
				$sql = "INSERT INTO question (survey_id,type_id,position,content)
				VALUES ('${sid}', 99, '${position}', 'break')";
				$result = mysql_query($sql);
				break;

		} // end select
	} // end op check

	$sql = "SELECT id,type_id,position,content
	FROM question
	WHERE survey_id='${sid}' AND
	deleted='N'
	ORDER BY position";
	$result = mysql_query($sql);

	$i=0;
?>
<?php echo(_('Change the order that questions are
presented by clicking on the up and down arrows.
Remove a question by clicking on the X.')); ?>
<hr>
<table>
<?php	while(list($qid,$tid,$pos,$content) = mysql_fetch_row($result)) { ?>
	<tr>
		<td>
<map name="Q<?php echo($qid); ?>">
<area shape="rect" coords="0,0,11,22"  href="<?php echo($url); ?>&op=u&qid=<?php echo($qid); ?>" alt="<?php echo(_('Move Up')); ?>">
<area shape="rect" coords="11,0,22,22" href="<?php echo($url); ?>&op=x&qid=<?php echo($qid); ?>" alt="<?php echo(_('Delete')); ?>">
<area shape="rect" coords="22,0,32,22" href="<?php echo($url); ?>&op=d&qid=<?php echo($qid); ?>" alt="<?php echo(_('Move Down')); ?>">
</map><img src="<?php echo($ESPCONFIG['image_path']); ?>knobs.gif" border="0" usemap="#Q<?php echo($qid); ?>">
		</td>
		<td>
<?php
			if($tid != 99)
				echo(++$i .'. '. $content);
			else
				echo('<b>'._('----- Section Break -----').'</b>');
?>
		</td>
	</tr>
<?php
		}
		mysql_free_result($result);
?>
</table>
<hr>
<a href="<?php echo($url); ?>&op=b"><?php echo(_('Click here to add a section (page) break.')); ?></a>
