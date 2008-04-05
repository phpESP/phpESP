<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* survey_copy.inc -- Copy one survey into a new survey.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
*/

// {{{ survey_copy()                   Copy a survey into a new survey

function survey_copy($sid) {
    esp_require_once('/function/survey_aggregate');
    return (survey_aggregate($sid) === false ? false : true);
}

// }}}

?>
