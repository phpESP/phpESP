<?php

# $Id$
// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>
//
// Modified by Aaron Axelsen
// <axelseaa@amadmax.com>
?>
<a name="top"></a>
<table bgcolor="#ffffff" border=0 width="98%">
    <tr>
	<td colspan="2" style="border-bottom:1px dashed #000000"><h1>Survey Resource</h1>
	</td>
	<tr>
	    <td style="vertical-align:top; width:125px; border-right:1px dashed #000000;">
		<div class="menu">

		    <a href="#new" style="text-decoration:none;">Create a Survey</a>
		    <ul>
			<li><a href="#general_tab" style="text-decoration:none; font-size:80%;">General Tab</a></li>
			<li><a href="#question_tab" style="text-decoration:none; font-size:80%;">Question Tab</a></li>
			<li><a href="#order_tab" style="text-decoration:none; font-size:80%;">Order Tab</a></li>
			<li><a href="#preview_tab" style="text-decoration:none; font-size:80%;">Preview Tab</a></li>
			<li><a href="#finish_tab" style="text-decoration:none; font-size:80%;">Finish Tab</a></li>
		    </ul>
		    <a href="#edit" style="text-decoration:none;">Edit a Survey</a><br />
		    <a href="#test" style="text-decoration:none;">Test a Survey</a><br />
		    <a href="#status" style="text-decoration:none;">Publishing a Survey</a><br />
		    <a href="#themes" style="text-decoration:none;">Themes</a><br />
		    <ul>
			<li><a href="#create" style="text-decoration:none; font-size:80%;">Create</a></li>
			<li><a href="#assign" style="text-decoration:none; font-size:80%;">Assign</a></li>
		    </ul>
		    <a href="#types" style="text-decoration:none;">Response Types/Uses</a>
		    <ul>
			<li><a href="#yes_no" style="text-decoration:none; font-size:80%;">Yes/No</a></li>
			<li><a href="#single_line" style="text-decoration:none; font-size:80%;">Single Line</a></li>
			<li><a href="#essay" style="text-decoration:none; font-size:80%;">Essay</a></li>
			<li><a href="#radio" style="text-decoration:none; font-size:80%;">Radio Buttons</a></li>
			<li><a href="#check" style="text-decoration:none; font-size:80%;">Check Boxes</a></li>
			<li><a href="#dropdown" style="text-decoration:none; font-size:80%;">Dropdown Box</a></li>
			<li><a href="#rating" style="text-decoration:none; font-size:80%;">Rate</a></li>
			<li><a href="#date" style="text-decoration:none; font-size:80%;">Date</a></li>
			<li><a href="#numeric" style="text-decoration:none; font-size:80%;">Numeric</a></li>
		    </ul>
		    <a href="#results" style="text-decoration:none;">Survey Results</a>
		    <ul>
			<li><a href="#analysis" style="text-decoration:none; font-size:80%;">Cross Analysis</a></li>
			<li><a href="#tabulation" style="text-decoration:none; font-size:80%;">Cross Tabulation</a></li>
			<li><a href="#csv" style="text-decoration:none; font-size:80%;">Exporting to CSV</a></li>
		    </ul>
		    <a href="#admin" style="text-decoration:none;">Administration</a>
		    <ul>
			<li><a href="#batch" style="text-decoration:none; font-size:80%;">Batch Account Creation</a></li>
		    </ul>

		</div>

	    </td>
	    <td rowspan="2">

		<h2 id="new">Creating a Survey</h2>

		<p style="padding-left:10px;">Click <b><a href="<?php echo($ESPCONFIG['base_url']);?>manage.php?where=new" target="_blank">Create a New Survey</a></b> from the Management Interface.</p>
		<h3 style="padding-left:10px;"><a name="general_tab"></a>General Tab:</h3>
		<ul>
		    <li>
		    <p>Enter a name for the survey in the <b>name</b>
		    field. Do not use spaces in this name, think of
		    this as a filename. Underscore's are acceptable.</p>
		    </li>
		    <li>
		    <p>Choose a <b>group</b> to own this survey.  Unless 
		    you are creating a survey as root, you will only be 
		    permitted to choose the group of which you are a member.</p>
		    </li>
		    <li>
		    <p>Fill out the <b>title</b>, <b>subtitle</b>, and
		    <b>info</b> fields. These fields will be used in
		    the creation of a header for the final survey.</p>
		    </li>
		    <li>
		    <p>If you would like to be emailed a copy of each
		    submitted survey, enter a valid email address in
		    the <b>email</b> field. Leave this BLANK, unless 
		    your data needs to be backed up.</p>
		    </li>
		    <li>
		    <p>If you would like to theme your survey select the
		    appropriate theme from the dropdown list. This will
		    establish a link to a specific css style sheet for
		    your survey. To design a theme please refer to the section:
		    <b><a href="#themes">Themes</a></b>.</p>
		    </li>
		    <li>
		    <p>The <b>Confirmation Page</b> is the page users
		    will be shown after filling out the survey online.
		    Fill in the <b>heading</b> and <b>body text</b> for
		    the this page, or leave them blank to use the
		    default.</p>
		    </li>
		    <li>
		    <p>Click continue, or click the <b>Questions</b>
		    tab at the top to proceed to the questions
		    section.</p>
		    </li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<h3 style="padding-left:10px;"><a name="question_tab"></a>Questions Tab:</h3>
		<ul>
		    <li>	
		    <p>Enter the text of your question (i.e. <em>What
			is your favorite color?</em>) in the
		    <b>question</b> box. Optionally enter a <b>field
			name</b> for this question, if you leave it blank
		    one will be generated for you.</p>

		    <p>If you would like to require the user to respond
		    to this question, select <b>yes</b> in the
		    <b>required</b> field.</p>

		    <p>Choose the <b>type of response</b> for this
		    question.[<a href="#types">Click here to see Examples</a>.] Different types may
		    have parameters to change how they behave, consult
		    the chart below for the use of the <b>length</b>
		    and <b>precision</b> fields.</p>

		    <table border="0">
			<tr><th align="left">Type</th><th align="left">Length</th><th align="left">Precision</th></tr>
			<tr><td colspan="3"><hr size="0" noshadow="true"></td></tr>
			<tr><td>Yes/No</td><td>n/a</td><td>n/a</td></tr>
			<tr><td>Text</td><td>length</td><td>max length</td></tr>
			<tr><td>Essay</td><td>columns</td><td>rows</td></tr>
			<tr><td>Radio</td><td>n/a</td><td>n/a</td></tr>
			<tr><td>Checkboxes</td><td>min #</td><td>max # <em>(not implemented yet)</em></td></tr>
			<tr><td>Dropdown</td><td>n/a</td><td>n/a</td></tr>
			<tr><td>Rate</td><td>1..N</td><td>Use "N/A"</td></tr>
			<tr><td>Date</td><td>n/a</td><td>n/a</td></tr>
			<tr><td>Numeric</td><td>length</td><td>precision</td></tr>
		    </table>

		    <p>If you chose a response type that has answer
		    options, fill in one answer per line on the bottom
		    half of the form. If you need more lines, click
		    <b>Add another answer line</b>. [Question types
		    with answer options are: Check Boxes, Dropdown Box,
		    Radio Buttons, Rate.] For check boxes and radio
		    buttons, you may enter <tt>&quot;!other&quot;</tt>
		    on a line to create a fill in the blank option. An
		    &quot;Other&quot; box defaults to using the prompt
		    <em>Other: </em>, but is configurable by using the
		    format:
		    <br><center><tt>!other=prompt text</tt></center></p>

		    <p>Add more questions by clicking the <b>New
			Question</b> button. Edit/View existing questions
		    by clicking the question numbers at the top of the
		    form.<p>

		    <p>Click continue, or click the <b>Questions</b>
		    tab at the top to proceed to the questions
		    section.</p>
		    </li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<h3 style="padding-left:10px;"><a name="order_tab">Order Tab:</a></h3>
		<ul>
		    <li>
		    <p>On this tab, you can change the order of the
		    questions, delete questions, and insert
		    <b>section</b> breaks. A section break, divides
		    your survey into multiple pages (good for long
		    surveys).</p>
		    </li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<h3 style="padding-left:10px;"><a name="preview_tab">Preview Tab:</a></h3>
		<ul><li>
		    <p>Shows a preview of your survey. You can switch
		    to this tab at any time to see what your survey
		    will look like. If you would like to make changes,
		    go back to the appropriate tab and make the
		    changes. If you are satisfied with the survey,
		    click the <b>Finish</b> tab or button at the bottom
		    of the page.</p>

		    <p>NOTE: The <b>Next Page</b> and <b>Submit Survey</b>
		    buttons are inactive in the preview mode.</p>
		    </li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<h3 style="padding-left:10px;"><a name="finish_tab">Finish Tab:</a></h3>
		<ul><li>
		    <p>Shows you the block of PHP code that you need to
		    paste into the HTML of your webpage to embed the
		    survey.</p>
		    <p>Once a survey is finished, you may return to
		    editing it by choosing <b>Edit an Existing
			Survey</b> from the Management Interface. When all
		    final edits are done, you need to change the survey
		    status from <b>new</b> to <b>test</b> or
		    <b>active</b> mode. You can change the status by
		    choosing <b>Change the Status of an Existing
			Survey</b> from the Management Interface.</li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<hr style="width:100%;" />

		<h2 id="edit">Editing a Survey</h2>
		<ul>
		    <li>Editing a survey uses the same interface as creating
		    a new survey, refer to the help for creating a new
		    survey for more details.</li>
		</ul>

		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<hr style="width:100%;" />

		<h2 id="test">Testing a Survey</h2>
		<ul>
		    <li>After you have created a survey you can put it into
		    testing mode. This allows you to access a live copy of
		    it from the Management Interface. You can fill out the
		    survey, and view the results by choosing <b>Test a
			Survey</b>. In order to test a survey it must be set to
		    <b>test</b> mode from the <b>Status</b> section.</li>

		    <li>NOTE: Once a survey is moved from <b>new</b>
		    designation to <b>test</b> you can no longer make
		    changes. If you just want to see how it will look, not
		    test functionality, please use the <b>preview</b>
		    option available in when <b>editing</b> or
		    <b>creating</b> a survey.</li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<hr style="width:100%;" />

		<h2 id="status">Publishing a Survey</h2>
		<ul>
		    <li>Once you have created/edited a survey, and are ready
		    to make it available online you must activate it. Go to
		    the Management Interface, click <b><a href="<?php echo($ESPCONFIG['base_url']);?>manage.php?where=status" target="_blank">Change the Status of
			    a Survey</a></b>. Find the survey you want to
		    activate. Make note of the survey ID(SID) and the name(SURVEYNAME).
		    </li>

		    <li>NOTE: At this point you must <b>activate</b> the
		    survey. This is a one way operation. After it has been
		    activated, you can no longer edit or test this survey.
		    All results gathered in testing mode (if any) will be
		    cleared by activating it.</li>

		    <li>Click on the <b>Activate</b> link for your survey.
		    At this point your survey is active.</li>
		    <li>To insert the survey into an existing page you must place a PHP tag
		    in the HTML for the page. 
		    <ul>
			<li>NOTE: This code was also given to you on the <b>Finish</b> tab of the survey design.</li>
			<li>To access an active survey, goto 
			<p><tt><?php echo($ESPCONFIG['autopub_url']."?name=SURVEYNAME"); ?></tt></p>
			</li>
			<li>To embed an active survey within another php document, add the following code:</li>

			<p><tt>&lt;?php $sid=<em>SID</em>; $results=1; include('<?php echo($GLOBALS['ESPCONFIG']['handler']); ?>'); ?&gt;</tt></p>
		    </ul>
		    </li>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<hr style="width:100%;" />

		<h2 id="themes">Themes</h2>
		<h3 style="padding-left:10px;"><a name="create">Creating Themes</a></h3>
		<ul>
		    <li>To download a template to create your own css style sheet, click <a href="<?php echo($ESPCONFIG['base_url']);?>public/css/template.css" target="_blank">here</a>.</li>
		    <li>An example page outlining where css classes are applied is available <a href="<?php echo($ESPCONFIG['base_url']);?>examples/classes.html" target="_blank">here</a>.</li>
		</ul>
		<!--creating themes info
		<li>All themes (css style sheets) are contained within the css
		directory which is, by default, located in the public directory
		of this package (and its path is definable in the phpESP.ini.php file).
		Please ensure that all your css files are
		contained within this directory. In this directory you will find
		the <b>template.css</b>. To create a new theme simply copy the
		the template.css file and edit the class definitions.<b> Do not
		    alter the class names</b>. Greater style flexibility is assured
		by not redefining html tags but rather by assigning classes to
		these tags. <a href="../examples/classes.html" target="#themes">
		    Click here</a> to see what these classes actually define.
		Once you've saved your new theme it will become available for
		selection from the dropdown list that is found on the <b>general
		    tab</b> page.
		</li></ul>
		-->
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<h3 style="padding-left:10px;"><a name="assign">Assigning Themes</a></h3>
		<ul>
		    <li>Themes can be assigned either when you create a new survey, or when you edit 
		    an existing survey.  Select the <b><a href="#general_tab">General Tab</a></b>, 
		    navigate to the drop down box near the bottom of the page, and select the appropiate 
		    theme you wish to use.</li>
		</ul>

		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<hr style="width:100%;" />

		<h2 id="types">Response Types</h2>

		<a name="yes_no"></a><h3 style="padding-left:10px;">Yes/No</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use this for questions that will require a basic yes/no answer.</dd>
		    <li>Example:</li>
		    <dd><input type="radio" name="bool">Yes</dd>
		    <dd><input type="radio" name="bool">No</dd>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		<a name="single_line"></a><h3 style="padding-left:10px;">Single Line Text Entry</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use this for questions that require a one line answer.  Note: The maximum lenght of the 
		    textbox can be set using the <b>precision</b> field.</dd>
		    <li>Example:</li>
		    <dd><input type="text" size=30></dd>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		<a name="essay"></a><h3 style="padding-left:10px;">Essay</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use this for questions in requiring an answer in essay format.  Note: The col and row width of 
		    the essay area can be set be using the <b<length</b> and <b>precision</b> fields.</dd>
		    <li>Example:</li>
		    <dd><textarea cols="40" rows="4"></textarea></dd>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		<a name="radio"></a><h3 style="padding-left:10px;">Radio Buttons</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use radio buttons for questions that have a set of possible answers, but only one can be 
		    selected.</dd>
		    <li>Example:</li>
		    <dd><input type="radio" name="radio">Option 1</dd>
		    <dd><input type="radio" name="radio">Option 2</dd>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<a name="check"></a><h3 style="padding-left:10px;">Check Boxes</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use check boxes for questions that have a set of possible answers out of which multiple can be 
		    selected.</dd>
		    <li>Example:</lu>
		    <dd><input type="checkbox">Option 1</dd>
		    <dd><input type="checkbox">Option 2</dd>
		</ul>
		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<a name="dropdown"></a><h3 style="padding-left:10px;">Dropdown Box</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use this to drop down a list of possible selections.  For example, what state are you from?  
		    The drop down menu would then produce the list of states.</dd>
		    <li>Example:</li>
		    <dd><select>
			<option></option>
			<option>Option 1</option>
			<option>Option 2</option>
		    </select></dd>
		</ul>

		<p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		<a name="rating"></a><h3 style="padding-left:10px;">Rate (scale 1..N)</h3>
		<ul>
		    <li>Explanation:</li>
		    <dd>Use this for questions that require a rating.  You can have a rating scale of 1 to N, set by 
		    the <b>length</b> box.  Multiple options can also be added to the rating block as you see below.</dd>
		    <li>Example:</li>
		    <dd><blockquote>
			<table border="0" cellspacing="1" cellpadding="0">
			    <tr>
				<td>
				</td>
				<td width="40" align="center" bgcolor="#eeeeee">1</td>
				<td width="40" align="center" bgcolor="#dddddd">2</td>
				<td width="40" align="center" bgcolor="#eeeeee">3</td>
				<td width="40" align="center" bgcolor="#dddddd">4</td>
				<td width="40" align="center" bgcolor="#eeeeee">5</td>
				<td width="40" align="center" bgcolor="#dddddd">N/A</td>
			    </tr>
			    <tr>
				<td>Option 1</td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_124" value="1"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_124" value="2"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_124" value="3"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_124" value="4"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_124" value="5"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_124" value="N/A"></td>
			    </tr>
			    <tr>
				<td>Option 2</td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_125" value="1"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_125" value="2"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_125" value="3"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_125" value="4"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_125" value="5"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_125" value="N/A"></td>
			    </tr>
			    <tr>
				<td>Option 3</td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_126" value="1"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_126" value="2"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_126" value="3"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_126" value="4"></td>
				<td width="40" align="center" bgcolor="#eeeeee"><input type="radio" name="118_126" value="5"></td>
				<td width="40" align="center" bgcolor="#dddddd"><input type="radio" name="118_126" value="N/A"></td>
			    </tr>
			</table>
			</dd></blockquote>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		    <a name="date"></a><h3 style="padding-left:10px;">Date</h3>
		    <ul>
			<li>Explanation:</li>
			<dd>Use this for responses that require uses to submit back a date.</dd>
			<li>Example:</li>
			<dd><input type="text" name="date" size="10"> <em>(e.g. 4/21/2002)</em></dd>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		    <a name="numeric"></a><h3 style="padding-left:10px;">Numeric</h3>
		    <ul>
			<li>Explanation:</li>
			<dd>Use this for questions which you will only want a numeric answer.  Note: All non-numeric 
			responses are disgarded.</dd>
			<li>Example</li>
			<dd><input type="text" name="numeric" size="10"></dd>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		    <hr style="width:100%;" />
		    <h2 id="results">Survey Results</h2>
		    <h3 style="padding-left:10px;" id="analysis">Cross Analysis</h3>
		    <ul>
			<li>
			To cross analyse results from a survey choose a question by
			selecting the appropriate radio button to the left of the question. You must
			then choose one or more of the question's choices by selecting the appropriate
			checkbox under the chosen question. This will display the entire results of
			this survey based on the criteria you have chosen. At present, Cross Analysis
			is limited to single questions.
			<p><a href="<?php echo($ESPCONFIG['image_url']);?>cross_analysis.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis.jpg" width="500" border="0" /></a><br>
			<br>
			</li>
			<li>
			This will produce the following result:<br>
			<br>
			<a href="<?php echo($ESPCONFIG['image_url']);?>cross_analysis_result.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis_result.jpg" width="500" border="0"></a><br>
			<br>
			The resulting display shows all the responses where question 1 choice was
			&quot;Yes&quot;.
			</li>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		    <h3 style="padding-left:10px;" id="tabulation">Cross Tabulation</h3>
		    <ul>
			<li>
			Cross tabulation returns a result set based on a two question
			selection. This is achieved by choosing which question's options will form
			the rows or columns for the cross tabulated result set. Selecting a radio
			button in the red box to the right of the question indicates the row selection
			and selecting a radio button in the blue box to the right of the question
			indicates the column selection.</p>
			<p>
			<a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate.jpg" width="500" border="0"></a></p>
			</li>
			<li>
			<p>In the above example we have chosen to cross tabulate question1 and question
			4 where question 1 is the row selection and question 4 is the column selection.
			This returns the following result set:<br>
			<br>
			<a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result1.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result1.jpg" width="500" border="0"></a><br>
			</li>
			<li>
			Alternatively we can cross tabulate the same 2 questions but set question
			4 as the row selection and question 1 as the column selection as shown below:<br>
			<br>
			<a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate2.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate2.jpg" width="500" border="0"></a><br>
			<br>
			</li>
			<li>
			This produces the following result set:<br>
			<br>
			<a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result2.jpg" target="_blank">
			    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result2.jpg" width="500" border="0"></a></p>
			</li>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>


		    <h3 style="padding-left:10px;" id="csv">Exporting to CSV</h3>
		    <ul>
			<li>
			By choosing to export to CSV, you are selected to dump all the survey result data into 
			a text file seperate by comma's, which then has the ability to be opened with most 
			spreadsheet programs.
			</li>
			<li>
			From the Management Interface, select <b><a href="<?php echo($ESPCONFIG['base_url']);?>manage.php?where=export" target="_blank">Export Data to CSV</a></b> to 
			view all the active or archived surveys.  Simply click the <b>download</b> link listed behind the desired
			survey to obtain the CSV file.
			</li>
		    </ul>
		    <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>

		    <hr style="width:100%;" />

		    <h2 id="admin">Administration Tasks</h2>

		    <h3 style="padding-left:10px;" id="batch">Batch Account Creation</h3>
		    <ul>
			<li><p>On either the Respondent or Designer Managment Interfaces, click <b>Bulk Upload</b>. 
			You will be presented with a form asking for <b>File Type</b> and <b>File to Upload</b>.  
			From the <b>File Type</b> dropdown, select the file type you wish to upload.  From the
			<b>File to Upload</b> browse your local filesystem to find the specific file you are 
			uploading.</p>
			</li>
			<li><b>Tab Delimited, Data File Format:</b>
			<p>The tab delimited file should contain rows of text, terminated by a newline
			character(\n), with each field in the rows delimited by the tab character(\t).
			Each of the examples below should be a single row.</p>
			<ul>
			    <li><b>Respondent Format:</b>
			    <p>Username, Password and Group are required fields</p>
			    <b>Field Order:</b>
			    <p><tt>username\tpassword\tgroup\tfname\tlname\t
				email\texpiration\tdisabled\n</tt></p>
			    <b>Example Row:</b>
			    <p><tt>looser\tsecret\teditors\tJohn\tSmith\t
				smith@yahoo.com\t20011122\tN\n</tt></p>
			    </li>
			    <li><b>Designer Format:</b><p\>
			    <p>Username, Password and Group are required fields</p>
			    <b>Field Order:</b>
			    <p><tt>username\tpassword\tgroup\tfname\tlname\temail\t
				design\tstatus\texport\tgroupedit\tgroupadmin\tgrouprespondents\t
				expiration\tdisabled</tt></p>
			    <b>Example Row:</b>
			    <p><tt>looser\tsecret\teditors\tJohn\tSmith\tsmith@yahoo.com\t
				Y\tN\tY\tN\Y\tN\t20031122\tY\n</tt></p>
			    </li>
			    <li><b>CSV, Data File Format:</b><p>Not Yet Implemented</p></li>
			    <li><b>XML, Data File Format:</b><p>Not Yet Implemented</p></li>

			</ul>
			<p><a href="#top">Back to Top</a></p>

			</li>
		    </td>
		</tr>
	    </table>
