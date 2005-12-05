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

function other_check(name)
{
    other = name.split("_");
    var f = document.phpesp_response;
    for (var i=0; i<=f.elements.length; i++) {
        if (f.elements[i].value == "other_"+other[1]) {
            f.elements[i].checked=true;
            break;
        }
    }
}

function merge(box) {
    if(box.options.length >= 2){
        ml = new Array();
        for(var i=0; i<box.options.length; i++) {
            ml[i] = box.options[i].value;
            sidsArray=ml;
        }
        sids = sidsArray.join("+");
        document.getElementById('sids').value = sids; 
        form = document.getElementById('merge');
        form.submit();
    } else {
        document.getElementById('error').innerHTML = "<h2>You must select at least two surveys before you can merge</h2>";
    }
}

function move(fbox,tbox) {
    for(var i=0; i<fbox.options.length; i++) {
        if(fbox.options[i].selected && fbox.options[i].value != "") {
            var no = new Option();
            no.value = fbox.options[i].value;
            no.text = fbox.options[i].text;
            tbox.options[tbox.options.length] = no;
            fbox.options[i].value = "";
            fbox.options[i].text = "";
        }
    }
    BumpUp(fbox);
}

function BumpUp(box)  {
    for(var i=0; i<box.options.length; i++) {
        if(box.options[i].value == "")  {
            for(var j=i; j<box.options.length-1; j++)  {
                box.options[j].value = box.options[j+1].value;
                box.options[j].text = box.options[j+1].text;
            }
            var ln = i;
            break;
        }
    }
    if(ln < box.options.length)  {
        box.options.length -= 1;
        BumpUp(box);
    }
}

-->
