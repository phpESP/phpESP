<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

?>
<a name="top"></a>
<table bgcolor="#ffffff" border=2 width="98%"><tr><td>

<ul>
	<li><a href="#new"><font size="+2">Creating a new survey</font></a></li>
	<li><a href="#test"><font size="+2">Testing a survey</font></a></li>
	<li><a href="#types"><font size="+2">Response Types Examples</font></a></li>
	<li><a href="#edit"><font size="+2">Editing a survey</font></a></li>
	<li><a href="#status"><font size="+2">Putting a survey into production use</font></a></li>
</ul>

<hr>

<ul>
<li><h3><a name="new">Creating a new survey</a></h3>
	<ol>
	<li><p>Click <b>New Survey Design</b> from the Management
	Interface.</p></li>
	<li><b>General Tab:</b>
			<p>Enter a name for the survey in the <b>name</b> field.
			Do not use spaces in this name, think of this as a filename.</p>
			
			<p>Fill out the <b>title</b>, <b>subtitle</b>, and <b>info</b> fields.
			These fields will be used in the creation of a header for the final survey.</p>
			
			<p>If you would like to be emailed a copy of each submitted survey, enter a
			valid email address in the <b>email</b> field.</p>
			
			<p>The <b>Thank You Page</b> is the page users will be shown after filling out
			the survey online. Fill in the <b>heading</b> and <b>body text</b> for the
			Thank You Page, or leave them blank to use the default.</p>
			
			<p>Click continue, or click the <b>Questions</b> tab at the top to proceed
			to the questions section.</p>
	</li>
	<li><b>Questions Tab:</b>
		<p>Enter the text of your question (i.e. <em>What is your favorite color?</em>)
		in the <b>question</b> box.</p>	
		
		<p>If you would like to require the user to respond to this question,
		select <b>yes</b> in the <b>answer required</b> field.</p>
		
		<p>Choose the <b>type of response</b> for this question. [Click here to see <a
		href="#types">Examples</a>.]</p>

		<p>Choose the format for the results to be displayed in a report from the <b>result
		type</b> field.</p>
		
		<p>If you chose a response type that has answer options, fill in one answer
		per line on the bottom half of the form. If you need more lines, fill in
		the available lines first, then click <b>Add another answer line</b>.
		[Question types with answer options are: Check Boxes, Dropdown Box,
		Radio Buttons.]</p>
		
		<p>Add more questions by clicking the <b>New Question</b> button. 
		Edit/View existing questions by clicking the question numbers at
		the top of the form.<p>

		<p>Click continue, or click the <b>Questions</b> tab at the top to proceed
		to the questions section.</p>
	</li>
	<li><b>Order Tab:</b>
		<p>On this tab, you can re-arrange the order of the questions, delete questions,
		and insert <b>section</b> breaks. A section break, divides
		your survey into multiple pages (good for long surveys).</p>

		<p>Next to each question/section break are three buttons. The up arrow (on
		the left) moves the question up one space. The X removes the question or
		section break from the survey (<b>this can not be reversed</b>). The
		down arrow (on the right), moves the question down one space.</p>
	</li>
	<li><b>Preview Tab:</b>
		<p>Shows a preview of your survey. You can switch to this tab at any time
		to see what your survey will look like. If you would like to make
		changes, go back to the appropriate tab and make the changes. If you are
		satisfied with the survey, click the <b>Finish</b> tab or button at the
		bottom of the page.</p>
		
		<p>The <b>Next Page</b> and <b>Submit Survey</b> buttons are inactive
		in the preview mode.</p>
	</li>
	<li><b>Finish Tab:</b>
		<p>Shows you the HTML block that you need to paste into your webpage to include the survey.</p>
		<p>Once a survey is finished, you may come back and edit it from the Management Interface
		by choosing <b>Edit an Existing Survey</b>. When all final edits are done, you need
		to change the survey status from <b>new</b> to <b>test</b> or <b>active</b> mode.
		You can change the status by choosing <b>Change the Status of an Existing Survey</b> from
		the Management Interface.</li>
	</ol>
	<p><a href="#top">Back to Top</a></p>
</li>
<li><a name="test"><h3>Test Mode</h3></a>
	<p>After you have created a survey you can put it into testing mode. This allows you to
	access a live copy of it from the Management Interface. You can fill out the survey, and view
	the results by choosing <b>Test a Survey</b>. In order to test a survey it must be set to
	<b>test</b> mode from the <b>Status</b> section.</p>
	<p>NOTE: Once a survey is moved from <b>new</b> designation to <b>test</b> you can no longer make
	changes. If you just want to see how it will look, not test functionality, please use the <b>preview</b>
	option available in when <b>editing</b> or <b>creating</b> a survey.</p>
	<p><a href="#top">Back to Top</a></p>
</li>
<li><a name="types"><h3>Response Types</h3></a>
	<ul>
	<li>Check Boxes<br>
		<input type="checkbox">Option 1<br>
		<input type="checkbox">Option 2<br>
	</li>
	<li>Dropdown Box<br>
		<select>
			<option></option>
			<option>Option 1</option>
			<option>Option 2</option>
		</select>
	</li>
	<li>Essay<br>
		<textarea cols="40" rows="4"></textarea>
	</li>
	<li>Radio Buttons<br>
		<input type="radio" name="radio">Option 1<br>
		<input type="radio" name="radio">Option 2<br>
	</li>
	<li>Ranking<br>
		<table border=0 cellspacing=0 cellpadding=0>
		<tr>
			<td width=60><input type="radio" name="rank">1</td>
			<td width=60><input type="radio" name="rank">2</td>
			<td width=60><input type="radio" name="rank">3</td>
			<td width=60><input type="radio" name="rank">4</td>
			<td width=60><input type="radio" name="rank">5</td>
			<td width=60><input type="radio" name="rank">N/A</td>
		</tr>
		</table>
	</li>
	<li>Single Line Text Entry<br>
		<input type="text" size=30>
	</li>
	<li>Yes/No<br>
		<input type="radio" name="bool">Yes<br>
		<input type="radio" name="bool">No
	</li>
	</ul>
	<p><a href="#top">Back to Top</a></p>
</li>
<li>
	<a name="edit"><h3>Editing a survey</h3></a>
	<p>Editing a survey uses the same interface as creating a new survey, refer
	to the help for creating a new survey for more details.</p>
	<p><a href="#top">Back to Top</a></p>
</li>		
<li>
	<a name="status"><h3>Putting a survey into production use</h3></a>
	<p>Once you have created/edited a survey, and are ready to make it available
	online you must activate it. Go to the Management Interface, click <b>Change
	the Status of an Existing Survey</b>. Find the survey you want to activate
	(they are listed most recent first). Make note of the survey ID (the
	left most column).</p>
	<p>NOTE: At this point you must <b>activate</b> the survey. This is a one way
	operation. After it has been activated, you can no longer edit or test this
	survey. All results gathered in testing mode (if any) will be deleted.</p>
	<p>Click on the <b>Activate</b> link for your survey. At this point your survey is
	active. To insert the survey into an existing page you must place a PHP tag
	in the HTML for the page. Copy the text below and paste it into the HTML of
	the page. (It is suggested that you put this in the cell of a table.) Change
	the text <tt>[SID]</tt> to the survey ID of your survey (found on the
	status page).</p>
	<tt>&lt;?php $sid=[SID]; include("<?php echo($cf_handler); ?>"); ?&gt;</tt>
	<p>NOTE: This code was also given to you on the <b>Finish</b> tab of the survey
	design.</p>
	<p><a href="#top">Back to Top</a></p>
</li>		
</ul>

</td>
</tr>
</table>
