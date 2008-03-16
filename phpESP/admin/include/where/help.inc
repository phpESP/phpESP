<?php

# $Id$
// Written by James Flemer
// For eGrad2000.com
// <jflemer@alum.rpi.edu>
//
// Modified by Aaron Axelsen
// <axelseaa@amadmax.com>
?>

<table class="help">
  <tr>
    <td colspan="2" style="border-bottom:1px dashed #000000"><h1 id="top">Survey Resource</h1></td>
  </tr>
  <tr>
    <td style="vertical-align:top; width:125px; border-right:1px dashed #000000;"><div class="menu"> <a href="#new" style="text-decoration:none;">Create a Survey</a>
        <ul>
          <li><a href="#general_tab" style="text-decoration:none; font-size:80%;">General Tab</a></li>
          <li><a href="#question_tab" style="text-decoration:none; font-size:80%;">Question Tab</a></li>
          <li><a href="#order_tab" style="text-decoration:none; font-size:80%;">Order Tab</a></li>
          <li><a href="#condition_tab" style="text-decoration:none; font-size:80%;">Condition Tab</a></li>
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
          <li><a href="#textbox" style="text-decoration:none; font-size:80%;">Textbox</a></li>
          <li><a href="#essay" style="text-decoration:none; font-size:80%;">Essay</a></li>
          <li><a href="#radio" style="text-decoration:none; font-size:80%;">Radio Buttons</a></li>
          <li><a href="#check" style="text-decoration:none; font-size:80%;">Check Boxes</a></li>
          <li><a href="#dropdown" style="text-decoration:none; font-size:80%;">Dropdown Box</a></li>
          <li><a href="#rating" style="text-decoration:none; font-size:80%;">Rate</a></li>
          <li><a href="#date" style="text-decoration:none; font-size:80%;">Date</a></li>
          <li><a href="#numeric" style="text-decoration:none; font-size:80%;">Numeric</a></li>
          <li><a href="#session_text" style="text-decoration:none; font-size:80%;">Session text</a></li>
        </ul>
	<p>
        <a href="#types" style="text-decoration:none;">Response Types Examples</a>
	</p>
        <a href="#results" style="text-decoration:none;">Survey Results</a>
        <ul>
          <li><a href="#analysis" style="text-decoration:none; font-size:80%;">Cross Analysis</a></li>
          <li><a href="#tabulation" style="text-decoration:none; font-size:80%;">Cross Tabulation</a></li>
          <li><a href="#csv" style="text-decoration:none; font-size:80%;">Exporting to CSV</a></li>
        </ul>
        <a href="#stats" style="text-decoration:none;">Survey Statistics</a>
        <ul>
          <li><a href="#refresh_statistics" style="text-decoration:none; font-size:80%;">Refreshing Statistics</a></li>
          <li><a href="#reseting_statistics" style="text-decoration:none; font-size:80%;">Reseting Statistics</a></li>
        </ul>
        <a href="#admin" style="text-decoration:none;">Administration</a>
        <ul>
          <li><a href="#batch" style="text-decoration:none; font-size:80%;">Batch Account Creation</a></li>
        </ul>
      </div></td>
    <td rowspan="2"><h2 id="new">Creating a Survey</h2>
      <p style="padding-left:10px;">Click <strong><a href="manage.php?where=new" onclick="window.open(this.href,'_blank');return false;">Create a New Survey</a></strong> from the Management Interface.</p>
      <h3 style="padding-left:10px;">General Tab:</h3>
      <ul>
        <li>
          <p>Enter a name for the survey in the <strong>name</strong> field. Do not use spaces in this name, think of this as a filename. Underscore's are acceptable.</p>
        </li>
        <li>
          <p>Choose a <strong>group</strong> to own this survey. Unless you are creating a survey as root, you will only be permitted to choose the group of which you are a member.</p>
        </li>
        <li>
          <p>Fill out the <strong>title</strong>, <strong>subtitle</strong>, and <strong>info</strong> fields. These fields will be used in the creation of a header for the final survey.</p>
        </li>
        <li>
          <p>If you would like to be emailed a copy of each submitted survey, enter a valid email address in the <strong>email</strong> field. Leave this BLANK, unless your data needs to be backed up.</p>
        </li>
        <li>
          <p>If you would like to theme your survey select the appropriate theme from the dropdown list. This will establish a link to a specific css style sheet for your survey. To design a theme please refer to the section: <strong><a href="#themes">Themes</a></strong>.</p>
        </li>
        <li>
          <p>The <strong>Confirmation Page</strong> is the page users will be shown after filling out the survey online. Fill in the <strong>heading</strong> and <strong>body text</strong> for the this page, or leave them blank to use the default.</p>
        </li>
        <li>
          <p>Click continue, or click the <strong>Questions</strong> tab at the top to proceed to the questions section.</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;">Questions Tab:</h3>
      <ul>
        <li>
          <p>Enter the text of your question (i.e. <em>What is your favorite color?</em>) in the <strong>question</strong> box. Optionally enter a <strong>field name</strong> for this question, if you leave it blank one will be generated for you.</p>
          <p>If you would like to require the user to respond to this question, select <strong>yes</strong> in the <strong>required</strong> field.</p>
          <p>Choose the <strong>type of response</strong> for this question.[<a href="#types">Click here to see Examples</a>.] Different types may have parameters to change how they behave, consult the chart below for the use of the <strong>length</strong> and <strong>precision</strong> fields.</p>
          <table>
            <tr>
              <th align="left">Type</th>
              <th align="left">Length</th>
              <th align="left">Precision</th>
            </tr>
            <tr>
              <td colspan="3"><hr /></td>
            </tr>
            <tr>
              <td>Yes/No</td>
              <td>n/a</td>
              <td>n/a</td>
            </tr>
            <tr>
              <td>Textbox</td>
              <td>length of the box</td>
              <td>max length of the text</td>
            </tr>
            <tr>
              <td>Essay</td>
              <td>columns</td>
              <td>rows</td>
            </tr>
            <tr>
              <td>Radio</td>
              <td>n/a</td>
              <td>n/a</td>
            </tr>
            <tr>
              <td>Checkboxes</td>
              <td>min #</td>
              <td>max # <em>(not implemented yet)</em></td>
            </tr>
            <tr>
              <td>Dropdown</td>
              <td>n/a</td>
              <td>n/a</td>
            </tr>
            <tr>
              <td>Rate</td>
              <td>1..N</td>
              <td>Use "N/A"</td>
            </tr>
            <tr>
              <td>Date</td>
              <td>n/a</td>
              <td>n/a</td>
            </tr>
            <tr>
              <td>Numeric</td>
              <td>length</td>
              <td>precision</td>
            </tr>
          </table>
          <p>If you chose a response type that has answer options, fill in one answer per line on the bottom half of the form. If you need more lines, click <strong>Add another answer line</strong>. [Question types with answer options are: Check Boxes, Dropdown Box, Radio Buttons, Rate.] For check boxes and radio buttons, you may enter <tt>&quot;!other&quot;</tt> on a line to create a fill in the blank option. An &quot;Other&quot; box defaults to using the prompt <em>Other: </em>, but is configurable by using the format: <br />
          </p>
          <div style="text-align: center"> <tt>!other=prompt text</tt> </div>
          <p>Add more questions by clicking the <strong>New Question</strong> button. Edit/View existing questions by clicking the question numbers at the top of the form. </p>
          <p> </p>
          <p>Click continue, or click the <strong>Questions</strong> tab at the top to proceed to the questions section.</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;"><a id="order_tab">Order Tab:</a></h3>
      <ul>
        <li>
          <p>On this tab, you can change the order of the questions, delete questions, and insert <strong>section</strong> breaks. A section break divides your survey into multiple pages (good for long surveys).</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;"><a id="condition_tab">Condition Tab:</a></h3>
      <ul>
        <li>
          <p>On this tab, you can change define conditions on questions, so a question is only shown when a condition is met.<br />
	     All conditions are "OR" conditions, so if one conditions is fullfilled, the question is shown.<br />
	     A question that has a condition must be on the next page than the question/value that is used to compare it with. Use the "Order tab" to insert a section break where wanted (this is NOT done automatically).<br />
             Tip: a Yes/No question can only be compared using the values "Yes or "No".
	</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;"><a id="preview_tab">Preview Tab:</a></h3>
      <ul>
        <li>
          <p>Shows a preview of your survey. You can switch to this tab at any time to see what your survey will look like. If you would like to make changes, go back to the appropriate tab and make the changes. If you are satisfied with the survey, click the <strong>Finish</strong> tab or button at the bottom of the page.</p>
          <p>NOTE: The <strong>Next Page</strong> and <strong>Submit Survey</strong> buttons are inactive in the preview mode.</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;"><a id="finish_tab">Finish Tab:</a></h3>
      <ul>
        <li>
          <p>Shows you the block of PHP code that you need to paste into the HTML of your webpage to embed the survey.</p>
          <p>Once a survey is finished, you may return to editing it by choosing <strong>Edit an Existing Survey</strong> from the Management Interface. When all final edits are done, you need to change the survey status from <strong>new</strong> to <strong>test</strong> or <strong>active</strong> mode. You can change the status by choosing <strong>Change the Status of an Existing Survey</strong> from the Management Interface.</p>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="edit">Editing a Survey</h2>
      <ul>
        <li>Editing a survey uses the same interface as creating a new survey, refer to the help for creating a new survey for more details.</li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="test">Testing a Survey</h2>
      <ul>
        <li>After you have created a survey you can put it into testing mode. This allows you to access a live copy of it from the Management Interface. You can fill out the survey, and view the results by choosing <strong>Test a Survey</strong>. In order to test a survey it must be set to <strong>test</strong> mode from the <strong>Status</strong> section.</li>
        <li>NOTE: Once a survey is moved from <strong>new</strong> designation to <strong>test</strong> you can no longer make changes. If you just want to see how it will look, not test functionality, please use the <strong>preview</strong> option available in when <strong>editing</strong> or <strong>creating</strong> a survey.</li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="status">Publishing a Survey</h2>
      <ul>
        <li>Once you have created/edited a survey, and are ready to make it available online you must activate it. Go to the Management Interface, click <strong><a href="manage.php?where=status" onclick="window.open(this.href,'_blank');return false;">Change the Status of a Survey</a></strong>. Find the survey you want to activate. Make note of the survey ID(SID) and the name(SURVEYNAME). </li>
        <li>NOTE: At this point you must <strong>activate</strong> the survey. This is a one way operation. After it has been activated, you can no longer edit or test this survey. All results gathered in testing mode (if any) will be cleared by activating it.</li>
        <li>Click on the <strong>Activate</strong> link for your survey. At this point your survey is active.</li>
        <li>To insert the survey into an existing page you must place a PHP tag in the HTML for the page.
          <ul>
            <li>NOTE: This code was also given to you on the <strong>Finish</strong> tab of the survey design.</li>
            <li>To access an active survey, goto
              <p><tt><?php echo($ESPCONFIG['autopub_url']."?name=SURVEYNAME"); ?></tt></p>
            </li>
            <li>To embed an active survey within another php document, add the following code:
              <p><tt>&lt;?php $sid=<em>SID</em>; $results=1; include('<?php echo($GLOBALS['ESPCONFIG']['handler']); ?>'); ?&gt;</tt></p>
            </li>
          </ul>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="themes">Themes</h2>
      <h3 style="padding-left:10px;"><a id="create">Creating Themes</a></h3>
      <ul>
        <li>To download a template to create your own css style sheet, click <a href="<?php echo($ESPCONFIG['base_url']);?>public/css/template.css" onclick="window.open(this.href,'_blank');return false;">here</a>.</li>
        <li>An example page outlining where css classes are applied is available <a href="<?php echo($ESPCONFIG['base_url']);?>examples/classes.html" onclick="window.open(this.href,'_blank');return false;">here</a>.</li>
      </ul>
      <!--creating themes info
		<li>All themes (css style sheets) are contained within the css
		directory which is, by default, located in the public directory
		of this package (and its path is definable in the phpESP.ini.php file).
		Please ensure that all your css files are
		contained within this directory. In this directory you will find
		the <strong>template.css</strong>. To create a new theme simply copy the
		the template.css file and edit the class definitions.<strong> Do not
		    alter the class names</strong>. Greater style flexibility is assured
		by not redefining html tags but rather by assigning classes to
		these tags. <a href="../examples/classes.html" target="#themes">
		    Click here</a> to see what these classes actually define.
		Once you've saved your new theme it will become available for
		selection from the dropdown list that is found on the <strong>general
		    tab</strong> page.
		</li></ul>
		-->
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;"><a id="assign">Assigning Themes</a></h3>
      <ul>
        <li>Themes can be assigned either when you create a new survey, or when you edit an existing survey. Select the <strong><a href="#general_tab">General Tab</a></strong>, navigate to the drop down box near the bottom of the page, and select the appropiate theme you wish to use.</li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="types">Response Types</h2>
      <h3 style="padding-left:10px;" id="yes_no">Yes/No</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for questions that will require a basic yes/no answer.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <input type="radio" name="bool" />
              Yes</dd>
            <dd>
              <input type="radio" name="bool" />
              No</dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="textbox">Text Box</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for questions that require a one line answer. Note: the size of the textbox can be set using the <strong>length</strong> field, he maximum length of the textbox can be set using the <strong>precision</strong> field.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <input type="text" size="30" />
            </dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="essay">Essay</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for questions in requiring an answer in essay format. Note: The col and row width of the essay area can be set be using the <strong>length</strong> and <strong>precision</strong> fields.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <textarea cols="40" rows="4"></textarea>
            </dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="radio">Radio Buttons</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use radio buttons for questions that have a set of possible answers, but only one can be selected.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <input type="radio" name="radio" />
              Option 1</dd>
            <dd>
              <input type="radio" name="radio" />
              Option 2</dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="check">Check Boxes</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use check boxes for questions that have a set of possible answers out of which multiple can be selected.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <input type="checkbox" />
              Option 1</dd>
            <dd>
              <input type="checkbox" />
              Option 2</dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="dropdown">Dropdown Box</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this to drop down a list of possible selections. For example, what state are you from? The drop down menu would then produce the list of states.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <select>
                <option></option>
                <option>Option 1</option>
                <option>Option 2</option>
              </select>
            </dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="rating">Rate (scale 1..N)</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for questions that require a rating. You can have a rating scale of 1 to N, set by the <strong>length</strong> field. Multiple options can also be added to the rating block as you see below. To show an extra column "N/A", set the <strong>precision</strong> field to 1.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <blockquote>
                <table border="0" cellspacing="1" cellpadding="0">
                  <tr>
                    <td></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;">1</td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;">2</td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;">3</td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;">4</td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;">5</td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;">N/A</td>
                  </tr>
                  <tr>
                    <td>Option 1</td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_124" value="1" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_124" value="2" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_124" value="3" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_124" value="4" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_124" value="5" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_124" value="N/A" /></td>
                  </tr>
                  <tr>
                    <td>Option 2</td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_125" value="1" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_125" value="2" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_125" value="3" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_125" value="4" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_125" value="5" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_125" value="N/A" /></td>
                  </tr>
                  <tr>
                    <td>Option 3</td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_126" value="1" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_126" value="2" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_126" value="3" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_126" value="4" /></td>
                    <td style="width: 40px; text-align: center; background-color:#eeeeee;"><input type="radio" name="118_126" value="5" /></td>
                    <td style="width: 40px; text-align: center; background-color:#dddddd;"><input type="radio" name="118_126" value="N/A" /></td>
                  </tr>
                </table>
              </blockquote>
            </dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="date">Date</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for responses that require uses to submit back a date. Configure the date format in the ini file.</dd>
          </dl>
        </li>
        <li>Example:
          <dl>
            <dd>
              <input type="text" name="date" size="10" />
              <em>(e.g. <?php echo strftime ($GLOBALS['ESPCONFIG']['date_format'],strtotime("09/21/2003")) ;?>)</em></dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="numeric">Numeric</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this for questions which you will only want a numeric answer. Note: All non-numeric responses are disgarded. The <strong>length</strong> determines the max number of digits that can be used, the <strong>precision</strong> field adds extra size for the numbers begind the comma. This is not very precise yet, so just use the <strong>length</strong> field for now.</dd>
          </dl>
        </li>
        <li>Example
          <dl>
            <dd>
              <input type="text" name="numeric" size="10" />
            </dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="session_text">Session text</h3>
      <ul>
        <li>Explanation:
          <dl>
            <dd>Use this to add some text to you survey (like a disclaimer before sending on submit). This is in fact no question a user can answer to.</dd>
          </dl>
        </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="results">Survey Results</h2>
      <h3 style="padding-left:10px;" id="analysis">Cross Analysis</h3>
      <ul>
        <li> To cross analyse results from a survey choose a question by selecting the appropriate radio button to the left of the question. You must then choose one or more of the question's choices by selecting the appropriate checkbox under the chosen question. This will display the entire results of this survey based on the criteria you have chosen. At present, Cross Analysis is limited to single questions.
          <p><a href="<?php echo($ESPCONFIG['image_url']);?>cross_analysis.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis.jpg" style="width: 500;" alt="Cross Analysis" /></a><br />
            <br />
          </p>
        </li>
        <li> This will produce the following result:<br />
          <br />
          <a href="<?php echo($ESPCONFIG['image_url']);?>cross_analysis_result.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis_result.jpg" style="width: 500;" alt="Cross Analysis Result" /></a><br />
          <br />
          The resulting display shows all the responses where question 1 choice was &quot;Yes&quot;. </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="tabulation">Cross Tabulation</h3>
      <ul>
        <li> Cross tabulation returns a result set based on a two question selection. This is achieved by choosing which question's options will form the rows or columns for the cross tabulated result set. Selecting a radio button in the red box to the right of the question indicates the row selection and selecting a radio button in the blue box to the right of the question indicates the column selection.
          <p> <a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate.jpg" style="width: 500;" alt="Cross Tabulate" /></a></p>
        </li>
        <li>
          <p>In the above example we have chosen to cross tabulate question1 and question 4 where question 1 is the row selection and question 4 is the column selection. This returns the following result set:<br />
            <br />
            <a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result1.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result1.jpg" style="width: 500;" alt="Cross Tabulate Result" /></a></p>
        </li>
        <li> Alternatively we can cross tabulate the same 2 questions but set question 4 as the row selection and question 1 as the column selection as shown below:<br />
          <br />
          <a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate2.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate2.jpg" style="width: 500;" alt="Cross Tabulate 2" /></a><br />
          <br />
        </li>
        <li> This produces the following result set:<br />
          <br />
          <a href="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result2.jpg" onclick="window.open(this.href,'_blank');return false;"> <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result2.jpg" style="width: 500;" alt="Cross Tabulate 2 Result" /></a> </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <h3 style="padding-left:10px;" id="csv">Exporting to CSV</h3>
      <ul>
        <li> By choosing to export to CSV, you are selected to dump all the survey result data into a text file seperate by comma's, which then has the ability to be opened with most spreadsheet programs. </li>
        <li> From the Management Interface, select <strong><a href="manage.php?where=export" onclick="window.open(this.href,'_blank');return false;">Export Data to CSV</a></strong> to view all the active or archived surveys. Simply click the <strong>download</strong> link listed behind the desired survey to obtain the CSV file. </li>
      </ul>
      <p style="padding-left:10px;"><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="stats">Survey Statistics</h2>
      <p>While completing surveys, various access statistics are collected for administrative review. These statistics, described
      in the table further below, allow you to better understand how your respondents interact with the surveys you create.
      Questions such as:</p>
      <ul>
      <li>Are my respondents having difficulty logging in?</li>
      <li>Are respondents moving through surveys, or are they abandoning them prematurely?</li>
      <li>What percentage of survey attempts have resulted in a complete survey?</li>
      </ul>
      <p>provide valuable insight into the effectiveness of your surveys.</p>
      <table>
      <tr><th>Statistic</th><th>Meaning</th></tr>
      <tr><td>Login Failures</td><td>The number of failed attempts to authenticate to take a particular survey.  This count only
      reflects the number of failures while attempting to access <em>a particular survey</em>.  Failed attempts to access the
      list of possible surveys are not recorded.</td></tr>
      <tr><td>Attempted</td><td>The number of unique attempts to start taking a survey, regardless of whether the attempt results
      in a completed or suspended survey.</td></tr>
      <tr><td>Abandoned</td><td>The number of surveys begun, but not suspended or completed.  This statistic can accumulate for
      a myriad of reasons, including:<ul>
      <li>Closing the browser and returning to the survey</li>
      <li>Refreshing the survey (via the browser refresh) before submitting</li>
      </ul></td></tr>
      <tr><td>Suspended</td><td>The number of surveys saved for later completion.</td></tr>
      <tr><td>Completed</td><td>The number of surveys completed. If a survey allows a respondent to reply multiple times,
      this statistic accumulates for each successful completion of the survey.</td></tr>
      </table>
      <p>If your survey is effective, the number of login failures and abandoned surveys is low, the number of attempts is
      nearly equal to the number of complete, and the number of suspended is small.</p>
      <p>As a general rule of thumb, you should investigate your survey implementation if any of these conditions occur:</p>
      <ul>
      <li>Excessive number of login failures.</li>
      <li>Large number of abandoned surveys.</li>
      <li>Large disparity in the number of attempted and completed surveys.</li>
      <li>High number of suspended, and low number of completed, surveys.</li>
      </ul>
      <div style="padding-left:10px;" id="refresh_statistics"><h3>Refreshing Statistics</h3>
      <p>As respondents complete surveys, the statistics will change.  You may update the statistics page to show the most recent
      values by clicking the "Refresh Statistics" button.
      </div>
      <div style="padding-left:10px;" id="reseting_statistics"><h3>Reseting Statistics</h3>
      <p>Occasionally, you may need to reset survey statistics.  For example, if you held a "trial run" survey phase before the
      actual deployment, you would need to delete all the responses and reset the statistics for the "actual run."</p>
      <p>To reset statistics for a survey, place a check in the "Reset" column on the row containing the survey you want to
      reset, then click the "Reset Statistics" button.  You may reset more than one survey by placing a check beside every
      survey you want to reset, then clicking the "Reset Statistics" button.  You will be prompted to confirm the reset
      action each time you click the "Reset Statistics" button; answer "Ok" to reset, or "Cancel" to skip the reset.</p>
      </div>
      <p><a href="#top">Back to Top</a></p>
      <hr style="width:100%;" />
      <h2 id="admin">Administration Tasks</h2>
      <h3 style="padding-left:10px;" id="batch">Batch Account Creation</h3>
      <ul>
        <li>
          <p>On either the Respondent or Designer Management Interfaces, click <strong>Bulk Upload</strong>. You will be presented with a form asking for <strong>File Type</strong> and <strong>File to Upload</strong>. From the <strong>File Type</strong> dropdown, select the file type you wish to upload. From the <strong>File to Upload</strong> browse your local filesystem to find the specific file you are uploading.</p>
        </li>
        <li><strong>Tab Delimited, Data File Format:</strong>
          <p>The tab delimited file should contain rows of text, terminated by a newline character(\n), with each field in the rows delimited by the tab character(\t). Each of the examples below should be a single row.</p>
          <ul>
            <li><strong>Respondent Format:</strong>
              <p>Username, Password and Group are required fields</p>
              <strong>Field Order:</strong>
              <p><tt>username\tpassword\tgroup\tfname\tlname\t email\texpiration\tdisabled\n</tt></p>
              <strong>Example Row:</strong>
              <p><tt>looser\tsecret\teditors\tJohn\tSmith\t smith@yahoo.com\t20011122\tN\n</tt></p>
            </li>
            <li><strong>Designer Format:</strong>
              <p>Username, Password and Group are required fields</p>
              <strong>Field Order:</strong>
              <p><tt>username\tpassword\tgroup\tfname\tlname\temail\t design\tstatus\texport\tgroupedit\tgroupadmin\tgrouprespondents\t expiration\tdisabled</tt></p>
              <strong>Example Row:</strong>
              <p><tt>looser\tsecret\teditors\tJohn\tSmith\tsmith@yahoo.com\t Y\tN\tY\tN\Y\tN\t20031122\tY\n</tt></p>
            </li>
            <li><strong>CSV, Data File Format:</strong>
              <p>Not Yet Implemented</p>
            </li>
            <li><strong>XML, Data File Format:</strong>
              <p>Not Yet Implemented</p>
            </li>
          </ul>
        </li>
      </ul>
      <p><a href="#top">Back to Top</a></p></td>
  </tr>
</table>

