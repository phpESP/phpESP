<!--
function activateConfirm() {
    if (confirm("Warning! Once activated, this survey can no longer be edited.  Any further changes must be done on a copy.")) {
	return true;
    }
    return false;
}

function cancelConfirm() {
    if (confirm("Warning! This survey has not been saved.  Canceling now will remove any changes.")) {
	return true;
    }
    return false;
}

function exportSubmit(type, f) {
    f.where.value=type;
    f.submit();
}

function clearTextInputs() {
	var i = 1;
    while (document.forms[1].elements["choice_content_" + i]) {
        document.forms[1].elements["choice_content_" + i].value = "";
        i++;
    }
}

function addAnswerLine() {
    var el = document.getElementById('answerlines');
    var numchoice = document.getElementById('num_choices').value;
    numchoice++;
    var tablerow = el.insertRow(numchoice+1);
    var tablecell = tablerow.insertCell(-1);
    tablecell.innerHTML = numchoice+".";
    tablecell.className = "numbered";
    tablecell = tablerow.insertCell(-1);
    var text = "<input type=\"hidden\" name=\"choice_id_"+numchoice+"\" value=\"\" />\n";
    text = text+"<input type=\"text\" size=\"60\" name=\"choice_content_"+numchoice+"\" value=\"\" />\n";
    tablecell.innerHTML = text;
    tablecell.className = "left";
    document.getElementById('num_choices').value = numchoice;
}
    
    
function validate() {
    return true;
}
-->
