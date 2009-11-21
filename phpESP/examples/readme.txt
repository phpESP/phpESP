Here you find some examples on how to use phpESP with own layout (as it should be used).
Some rules to take into account:
- public/phpESP.first.php must be included before any output happens
- if you include public/handler.php inside a function, declare the $ESPCONFIG array as a global variable:

<?php
function survey() {
   global $ESPCONFIG;
   include("/path/phpESP/public/handler.php");
}
survey();
?>  
- make sure you use all the variables as mentioned in the examples
