<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

	$url = $ESPCONFIG['ME'] .'?where=tab&tab=order';

    if(isset($HTTP_POST_VARS['src']))
    	$src = intval($HTTP_POST_VARS['src']);
    else
        $src = 0;
    if (isset($HTTP_POST_VARS['dst']))
    	$dst = intval($HTTP_POST_VARS['dst']);
    else
        $dst = 0;
	$sid = $HTTP_SESSION_VARS['survey_id'];

	if(isset($HTTP_POST_VARS['addbreak'])) {
		$sql = "SELECT MAX(position)+1 FROM ".$GLOBALS['ESPCONFIG']['question_table']." WHERE survey_id='$sid'";
		$result = mysql_query($sql);
		$pos = mysql_result($result, 0, 0);
		mysql_free_result($result);

		$sql = "INSERT INTO ".$GLOBALS['ESPCONFIG']['question_table']." (survey_id,type_id,position,content)
				VALUES ('$sid', 99, '$pos', 'break')";
		mysql_query($sql);
	} elseif ($src > 0) {
		--$src;
		--$dst;

		$sql = "SELECT id,position FROM ".$GLOBALS['ESPCONFIG']['question_table']."
				WHERE survey_id='$sid' AND deleted='N'
				ORDER BY position";
		$result = mysql_query($sql);

		$max = mysql_num_rows($result);

		if ($src < $max) {
			$qid = mysql_result($result, $src, 0);

			if ($dst < 0) {
				// remove
				$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET deleted='Y' WHERE id='$qid' AND survey_id='$sid'";
				mysql_query($sql);
			} elseif (0 <= $dst && $dst < $max && $src != $dst) {
				// move
				if ($max - 1 > $dst) {
					if ($src < $dst)
						// move down
						++$dst;
					$dst = mysql_result($result, $dst, 1);
					$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET position=position+1
							WHERE survey_id='$sid' AND position >= '$dst'";
					mysql_query($sql);
				} else {
					$dst = mysql_result($result, $max-1, 1) + 1;
				}
				$sql = "UPDATE ".$GLOBALS['ESPCONFIG']['question_table']." SET position='$dst' WHERE id='$qid' AND survey_id='$sid'";
				mysql_query($sql);
			}
		}
		mysql_free_result($result);
	}

	$sql = "SELECT id,type_id,position,content FROM ".$GLOBALS['ESPCONFIG']['question_table']."
			WHERE survey_id='$sid' AND deleted='N'
			ORDER BY position";
	$result = mysql_query($sql);
	$max = mysql_num_rows($result);
	$sec = $num = 0;
?>
<?php echo(_('Change the order that questions are
presented by choosing the desired position from the
list.')); ?>
<hr>
<script language="javascript">
<!-- // comment
  function swap(src, dst) {
    document.phpesp.src.value=src;
    document.phpesp.dst.value=dst+1;
    document.phpesp.submit();
  }
  function remove(src) {
    document.phpesp.src.value=src;
    document.phpesp.dst.value=-1;
    document.phpesp.submit();
  }
  function editq(num) {
    if (document.phpesp.elements[1].name == 'tab') {
        document.phpesp.elements[1].value='Questions';
    }
    else {
      for (i = 1; i <= document.phpesp.elements.length; i++) {
	if (document.phpesp.elements[i].name == 'tab') {
	  document.phpesp.elements[i].value='Questions';
	  break;
	}
      }
    }
    document.phpesp.q.value=num;
    document.phpesp.submit();
	return false;
  }
// comment -->
</script>
<input type="hidden" name="tab" value="">
<input type="hidden" name="src" value="">
<input type="hidden" name="dst" value="">
<input type="hidden" name="q"   value="">
<table>
<?php
	while( list($qid, $tid, $pos, $content) = mysql_fetch_row($result)) {
		$num++;
		$select = '';
		for ( $i = 0; $i < $max; ) {
			$i++;
			if( $i == $num )
				$select .= "<option value=\"$i\" selected=\"selected\">$i</option>\n";
			else
				$select .= "<option value=\"$i\">$i</option>\n";
		}
?>
	<tr>
		<td>
		<select name="pos<?php echo($num); ?>" size="1" onchange="javascript:swap(<?php echo($num); ?>, this.form.pos<?php echo($num); ?>.selectedIndex);">
<?php echo($select); ?>
		</select>
		</td>
		<td>
<?php
			if($tid != 99)
				echo($content);
			else {
				++$sec;
				echo('<b>'._('----- Section Break -----').'</b>');
			}
?>
		</td>
		<td><input type="button" value="<?php echo(_('Remove')); ?>" onclick="javascript:remove(<?php echo($num); ?>);"></td>
		<td><?php if($tid != 99) { ?><input type="button" value="<?php echo(_('Edit')); ?>" onclick="javascript:editq(<?php echo($num-$sec); ?>);"><?php } ?></td>
	</tr>
<?php
		}
		mysql_free_result($result);
?>
</table>
<hr>
<input type="submit" name="addbreak" value="<?php echo(_('Add Section Break')); ?>">
