<html>
<head>
<title>Cross Tabulation Help</title>
</head>
<body>
<blockquote>
<?php
	 # $Id$


        if (!defined('ESP_BASE'))
                define('ESP_BASE', dirname(__FILE__) . '/../');

        require_once(ESP_BASE . '/admin/phpESP.ini.php');
?>     
  <p><strong>Cross Tabulation:</strong><br>
    <font size="-1">Cross tabulation returns a result set based on a two question 
    selection. This is achieved by choosing which question's options will form 
    the rows or columns for the cross tabulated result set. Selecting a radio 
    button in the red box to the right of the question indicates the row selection 
    and selecting a radio button in the blue box to the right of the question 
    indicates the column selection. </font></p>
  <p><img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate.jpg" width="700" height="273"></p>
  <p><font size="-1">In the above example we have chosen to cross tabulate question1 and question 
    4 where question 1 is the row selection and question 4 is the column selection. 
    This returns the following result set:</font> <br>
    <br>
    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result1.jpg" width="700" height="187"><br>
    <font size="-1">Alternatevily we can cross tabulate the same 2 questions but set question 
    4 as the row selection and question 1 as the column selection as shown below:</font><br>
    <br>
    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate2.jpg" width="700" height="271"><br>
    <br>
    <font size="-1">This produces the following result set:</font><br>
    <br>
    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_tabulate_result2.jpg" width="700" height="274"> </p>
</blockquote>
</body>
</html>
