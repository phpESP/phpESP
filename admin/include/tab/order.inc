<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

$sid = $HTTP_SESSION_VARS['survey_id'];
$sql = "SELECT id,type_id,position,content FROM ".$GLOBALS['ESPCONFIG']['question_table']."
WHERE survey_id=$sid AND deleted='N'
ORDER BY position";
$result = execute_sql($sql);
$max = record_count($result);
?>
<?php echo(_('Change the order that questions are
presented by choosing the desired position from the
list.')); ?>
<hr>
<script language="javascript">
<!-- // comment
function move(f,bDir,sName) {
    var el = f.elements[sName]
    var idx = el.selectedIndex
    if (idx==-1) 
    alert("You must first select the item to reorder.")
    else {
        var nxidx = idx+( bDir? -1 : 1)
        if (nxidx<0) nxidx=el.length-1
        if (nxidx>=el.length) nxidx=0
        var oldVal = el[idx].value
        var oldText = el[idx].text
        el[idx].value = el[nxidx].value
        el[idx].text = el[nxidx].text
        el[nxidx].value = oldVal
        el[nxidx].text = oldText
        el.selectedIndex = nxidx
   }
}

function saveOrder() {
        var orderList = "";
        for (var i=0; i < document.phpesp.questions.length; i++){
                orderList = orderList + document.phpesp.questions.options[i].value;
                if (i + 1 != document.phpesp.questions.length){
                        orderList = orderList + "|";
                }
        }
        document.phpesp.questions_order.value = orderList;
}

// comment -->
</script>
<input type="hidden" name="tab" value="">
<input type="hidden" name="questions_order" value="">
        <?php
            if ($max > 20) {
                $max = 20;
            }
        ?>
    <select name="questions" size="<?php echo($max)?>">
        <?php
        $i = 1;
        while( list($qid, $tid, $pos, $content) = fetch_row($result)) {
            $result->MoveNext();
            $dots = "";
            if($tid != 99) {
                if (strlen($content) > 100) {
                   $dots = "...";
                }
                echo('<option value="'.$qid.'">'.$i++.'. '.substr($content,0, 100).$dots.'</option>');
            }
            else {
                ++$sec;
                echo('<option value="'.$qid.'"><b>'._('----- Section Break -----').'</b></option>\n');
			}
        }
		db_close($result);
        ?>
        </select>
<hr>
<input type="button" value="Up" onclick="javascript:move(this.form,true,'questions')"> | <input type="button" value="Down" onclick="javascript:move(this.form, false, 'questions')"> | <input type="submit" name="addbreak" value="<?php echo(_('Add Section Break')); ?>"> | <input type="submit" value="Remove" name="remove">
    <br /><br />
