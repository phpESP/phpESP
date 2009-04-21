<?php

/* $Id$ */

/* vim: set tabstop=4 shiftwidth=4 expandtab: */

// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>

$sid = $_SESSION['survey_id'];
$sql = "SELECT id,type_id,position,content FROM ".$GLOBALS['ESPCONFIG']['question_table']."
WHERE survey_id=$sid AND deleted='N'
ORDER BY position";
$result = execute_sql($sql);
$max = record_count($result);
?>
<?php echo(_('Change the order that questions are
presented by selecting a question from the list, then use the up/down 
buttons to change its position.')); ?>
<hr>
<script type="text/javascript">
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

function validate() {
    var tabs = document.getElementById('tabs');
    var orderList = "";
    for (var i=0; i < tabs.questions.length; i++){
        orderList = orderList + tabs.questions.options[i].value;
        if (i + 1 != tabs.questions.length){
            orderList = orderList + "|";
        }
    }
    tabs.questions_order.value = orderList;
}

function removeItem() {
    var el = document.getElementById('questions');
    var idx = el.selectedIndex;
    var tabs = document.getElementById('tabs');
    if (idx == -1) {
        alert("You must first select an item to delete");
        return false;
    } else {
        tabs.removeit.value = 1;
        tabs.submit();
    }
    return true;
}
// comment -->
</script>
<input type="hidden" name="removeit" value="" />
<input type="hidden" name="questions_order" value="" />
        <?php
            if ($max > 20) {
                $max = 20;
            }
        ?>
    <select name="questions" id="questions" size="<?php echo($max)?>">
        <?php
        $i = 1;
        while( list($qid, $tid, $pos, $content) = fetch_row($result)) {
            $result->MoveNext();
            $dots = "";
            $content = strip_tags($content);
            if($tid != 99) {
                if (strlen($content) > 100) {
                   $dots = "...";
                }
                echo('<option value="'.$qid.'">'.$i++.'. '.substr($content,0, 100).$dots.'</option>');
            }
            else {
                //++$sec;
                echo('<option value="'.$qid.'"><strong>'._('----- Section Break -----').'</strong></option>\n');
			}
        }
		db_close($result);
        ?>
        </select>
<hr />
<input type="button" value="<?php echo(_('Up')); ?>" onclick="javascript:move(this.form,true,'questions')" /> | <input type="button" value="<?php echo(_('Down')); ?>" onclick="javascript:move(this.form, false, 'questions')"> | <input type="submit" name="addbreak" value="<?php echo(_('Add Section Break')); ?>"> | <input type="button" value="<?php echo(_('Remove')); ?>" name="remove" onclick="javascript:removeItem()" />
    <br /><br />
