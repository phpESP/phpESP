<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* help.php -- Respondent help manual.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
* @_PATTERNS_@
*
* @_NOTES_@
*/
// hook into the phpESP environment
require_once('../phpESP.first.php');

// figure out a nice way to say who our support is (this is a HELP document!)
if (empty($GLOBALS['ESPCONFIG']['support_email_address'])) {
    $adminLink   = '';
    $yourAdminIs = '';
} else {
    $eaddr = $GLOBALS['ESPCONFIG']['support_email_address'];
    $adminLink   = "<a href='mailto:{$eaddr}'>{$eaddr}</a>";
    $yourAdminIs = "Your survey administrator is $adminLink.";
}
?>
<html>
<head>
<link rel="stylesheet" href="../css/default.css" title="Default" type="text/css" />
<style type='text/css'>
img { display: block; border: solid 4px #dcd5d0; margin: .25em 0; }
a[href="#top"] { font-size: small; }
</style>
</head>
<body>
<table class="help">
  <tr>
    <td colspan="2"><h1 id="top">Survey Help System</h1></td>
  </tr>
  <tr>
    <td class="menu">
        <a href="#overview">Overview</a>
        <ul>
          <li><a href="#login">Logging In</a></li>
          <li><a href="#signup">Signing Up</a></li>
          <li><a href="#loginhelp">Getting Help</a></li>
          <li><a href="#public">Public Surveys</a></li>
        </ul>
        <a href="#dashboard">Your Dashboard</a>
        <ul>
          <li><a href="#mysurveys">My Surveys</a></li>
          <li><a href="#myhistory">My History</a></li>
          <li><a href="#mytools">My Tools</a></li>
        </ul>
        <a href="#complete">Completing Surveys</a>
        <ul>
          <li><a href="#save">Saving for Later</a></li>
          <li><a href="#submit">Submitting</a></li>
        </ul>
        <!--
        <a href="#topic10">Topic 10</a>
        <ul>
          <li><a href="#topic1010">Topic 10.10</a></li>
          <li><a href="#topic1020">Topic 10.20</a></li>
          <li><a href="#topic1030">Topic 10.30</a></li>
          <li><a href="#topic1040">Topic 10.40</a></li>
          <li><a href="#topic1050">Topic 10.50</a></li>
          <li><a href="#topic1060">Topic 10.60</a></li>
        </ul>
        -->
      </div></td>
    <td rowspan="2">
      <div>
        <h2 id="overview">Overview</h2>
        <p>Welcome to our online survey system, powered by phpESP!  <em><?php echo $yourAdminIs; ?></em></p>
        <p>Let's get started.  When you first access the survey system, you'll be asked to log in:</p>
        <img src='login.png' alt='first page' title='The first page of the survey system: log in, sign up, take public surveys, or get help' />
        <p><a href="#top">Back to Top</a></p>
        <div>
          <h3 id="login">Logging In</h3>
          <p>If your survey administrator has given you an account, enter your User ID and Password in the corresponding fields
          within the box labeled "Login".  When done, click the Login button.</p>
          <p>Please remember that the User ID and Password are case sensitive, so pay special attention to your typing.  For
          your security, your password is not shown while you type it; instead, your browser obscures each letter with an
          asterisk (<code>*</code>) or other character.</p>
          <p>If your User ID and Password aren't recognized, one of the following errors will be displayed:</p>
          <ul>
          <li><span class='error'>[ Incorrect User ID or Password, or your account has been disabled/expired. ]</span>.</li>
          </ul>
          <p>If you receive an error, first try retyping your User ID and Password (check your caps lock key, as
          well).  If you are absolutely certain you are typing your information correctly, contact your survey administrator for
          assistance.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="signup">Signing Up</h3>
          <p>If allowed by your survey administrator, you may create a new account by clicking on the "Don't have an account? Sign up" link in the lower right hand corner of the login box.  You will be prompted for your name, email address, desired user ID and password.  If the requested user ID is available, your account will be created and you can log in using the given user ID and password.</p>
          <p>However, please note: your survey administrator may need to give you access to additional surveys after you create your new account.  If, after logging in, you don't see the surveys you expect, contact your survey administrator for further assistance.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="loginhelp">Getting Help</h3>
          <p>This help page is available both before and after logging in: the link labeled "Help" always brings you here.  The links in the menu to the left allow you to navigate within this page.  The "Back to Top" link after each section will take you to the top of the page, from which you can select additional menu links.</p>
          <p>Remember, you can always contact your survey administrator for further information. <?php echo $yourAdminIs; ?></p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="public">Public Surveys</h3>
          <p>If your survey administrator has created public surveys (which do not require you to log in to take) and wants them to be shown, then they are available in the box labeled "Public Surveys."  Simply click on any of the links to take that survey.</p>
          <p><em>If you have an account, we recommend you log in before taking any public surveys.</em>  Once you log in, you will have the opportunity to take the public surveys.  By logging in first, your responses to the public survey will be identified as yours, which will then appear in your survey history (more on your survey history, below).</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <h2 id="dashboard">Your Dashboard</h2>
        <p>After you log in, you'll see your dashboard:
        <img src='dashboard.png' alt='dashboard' title='The dashboard, your view into the online survey system' />
        <p><a href="#top">Back to Top</a></p>
        <div>
          <h3 id="mysurveys">My Surveys</h3>
          <p>These are the surveys that you can take.  These surveys can be private to you, or a group you're in, or they can be public surveys.</p>
          <p>If you have at least one survey available, you'll see a table listing the title, status, and last access data for all your available surveys.  If you do not have any surveys available, you'll see a message indicating such.</p>
          <p>The survey title is a link: follow those links to take each survey.  The survey status indicates how you have, so far, interacted with the survey:</p>
          <ul>
          <li><em>Not Started</em>.  You have not yet submitted a response to this survey.  You may have looked at the survey, but you haven't submitted any responses. You may start this survey at any time.</li>
          <li><em>Started, but Incomplete</em>.  You have started this survey, but left early and saved your responses.  You may return to this survey at any time. This status is only available if the survey administrator allows you to save your responses to return to later.</li>
          <li><em>Some Finished, some Incomplete</em>.  You have submitted at least one complete response, but have saved another for later.  This status is only available for surveys that allow you to respond multiple times.</li>
          <li><em>Finished</em>.  You have submitted at least one complete response to this survey.</li>
          </ul>
          <p>The last access column indicates when you last visited the survey.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="myhistory">My History</h3>
          <p>These are the surveys that you have had access to at one time, but the survey administrator has closed further access to them.  Because these surveys are closed, you can no longer submit responses to them.</p>
          <p>Like My Surveys, My History lists the survey title, status, and last access date.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="mytools">My Tools</h3>
          <p>In this area, you'll find links to get help as well as logout.  Your survey administrator may also have given you access to these tools:</p>
          <ul>
          <li><em>Change my profile</em>.  This tool allows you to change your personal information, including your name and email address.</li>
          <li><em>Change my password</em>.  This tool allows you to change your password.  To ensure that only you change your password, you must enter your current password, as well as your desired new password.</li>
          <li><em>E-mail support</em>.  This tool opens up an email composition window directly to the survey administrator.</li>
          </ul>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <h2 id="complete">Completing Surveys</h2>
        <p>Completing a survey is very easy: read the questions, top to bottom, and answer each.  Let's start with the basics:</p>
        <img src='samplesurvey1.png' alt='sample survey, first page' title='The first page of a sample survey' />
        <p>Every survey has a title.  In the image above, the title is "Sample Survey."  A survey may also have a sub-title and instructions, both of which are found below the title.</p>
        <p>A survey may be on a single page, or may span multiple pages.  If a survey spans multiple pages, your current page and the total number of pages will be shown.  You may always move forward to the next page, and the survey administrator may also allow you to move backward.
        <p>Some questions require an answer, while others do not.  If an answer is mandatory, the question will be marked with a red asterisk.  You must answer the question before moving to the next page or submitting your final answers.</p>
        <p>The most basic question types are radio buttons, text fields, and essay boxes, as seen in the image above.  Radio buttons allow you to choose one, out of many, options.  A text field allows you to type one line of text.  An essay box allows you to type many lines of text.  The survey administrator may limit the number of characters you can type into a text field or essay box.</p>
        <p>Let's look at more question types:</p>
        <img src='samplesurvey2.png' alt='sample survey, second page' title='The second (and last) page of a sample survey' />
        <p>Checkboxes allow you to choose multiple options from a list.  Click in the square beside the item and a "check" will appear, indicating your selection.  Click in the square again to remove the check.</p>
        <p>Drop-downs allow you to choose one item from a list of items; in this way, they are very much like radio buttons.  Click the down arrow on the right of the drop-down, then select one of the items from the list that appears.  Your selection will appear to the left of the down arrow.</p>
        <p>A rating scale (technically called a "Likert scale") is a special kind of radio button answer, where you indicate your degree of agreement with the presented statement.  You may choose one out of the five possible options.</p>
        <p>A date entry field looks like a text field, but requires that you enter a value that can be understood to be a date.  For example, "21/09/2003" for September 21, 2003.</p>
        <p>A numeric field also looks like a text field, but requires that you enter a number.</p>
        <p><a href="#top">Back to Top</a></p>
        <div>
          <h3 id="save">Saving for Later</h3>
          <p>If the survey administrator allows you to save a survey, you will see a "Save" button at the bottom of each page in your survey.  When you click the save button, the survey system remembers your responses so that you may return to the survey at a later time to complete it.</p>
          <p>When you return to the survey, the survey system will automatically fill in your previous answers and allow you to pick up from where you left off.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <div>
          <h3 id="submit">Submitting</h3>
          <p>When you have finished taking the survey, click the Submit Survey button.  Your responses will be saved and you will be taken to a thank you page.</p>
          <p><em>Please do not use your back button to return to the survey.</em></p>
          <p>From the thank you page, you may return to your dashboard.</p>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <!--
        <h2 id="topic10">Topic 10</h2>
        <div>
          <h3 id="topic1010">Topic 10.10</h3>
          <p><a href="#top">Back to Top</a></p>
        </div>
        <p><a href="#top">Back to Top</a></p>
        -->
  </td></tr>
</table>
</body>
</html>
