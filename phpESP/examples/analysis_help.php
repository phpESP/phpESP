<html>
<head>
<title>Cross Analysis Help</title>
</head>
<body>
<blockquote>
<?php
	 # $Id$

        if (!defined('ESP_BASE'))
                define('ESP_BASE', dirname(__FILE__) . '/../');

        require_once(ESP_BASE . '/admin/phpESP.ini.php');
?>     
  <p><strong>Cross Analysis:</strong><br>
    <font size="-1">To cross analyse results from a survey choose a question by 
    selecting the appropriate radio button to the left of the question. You must 
    then choose one or more of the question's choices by selecting the appropriate 
    checkbox under the chosen question. This will display the entire results of 
    this survey based on the criteria you have chosen. At present, Cross Analysis 
    is limited to single questions. </font>
  <p><img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis.jpg" width="700" height="262"><br>
    <br>
    <font size="-1">This will produce the following result:</font><br>
    <br>
    <img src="<?php echo($ESPCONFIG['image_url']);?>cross_analysis_result.jpg" width="700" height="284"><br>
    <br>
    <font size="-1">The resulting display shows all the responses where question 1 choice was 
    &quot;Yes&quot;.</font>
</blockquote>
</body>
</html>
