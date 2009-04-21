<?php
/* $Id$ */
/* vim: set tabstop=4 shiftwidth=4 expandtab: */
/**
* statistics.inc -- Implements the "statistics" where on the management interface.
* Original Author: Bishop Bettini <bishop@ideacode.com>
*
* This management module shows the survey statistics for all surveys the current user can access.
* The user may select any set of statistics to reset.  The look and feel is controlled within the
* view_survey_statistics CSS div.
*
* @_TODO_@
* - Sort by table headers (always leaving summary at the bottom)
* - Filter by table values
* - Export values to CSV/Excel
* - Support automatic, periodic refresh via javascript
*
*/

// get the survey data (name, status, etc)
esp_require_once('/lib/espsurvey');
survey_get_for_current_user($surveys);

// handle any button press events
// NOTE: we handle these after getting survey data to ensure that actions happen only on allowed surveys
$doReset = (isset($_REQUEST['doReset']) && isset($_REQUEST['reset']) && 0 < count($_REQUEST['reset']) ? true : false);
if ($doReset) {
    esp_require_once('/lib/espsurveystat');
    foreach ($_REQUEST['reset'] as $sid) {
        if (array_key_exists($sid, $surveys)) {
            survey_stat_reset($sid);
        } else {
            // bad access
            $GLOBALS['errmsg'] = mkerror(_('Request to reset survey that is outside allowed access'));
        }
    }
}

// get the statistics
esp_require_once('/lib/espsurveystat');
$stats = stat_fetch_with_summary();

// show them
echo '<h2>' . _('View Survey Statistics') . '</h2>';
echo "<a href='{$GLOBALS['ESPCONFIG']['ME']}?where=help#stats' target='_blank'>"._('Help')."</a>";
echo '<input type="hidden" name="where" value="statistics" />';
echo '<div id="view_survey_statistics">';
if (0 < count($stats)) {
    echo '<table><caption>' . _('Various access statistics for your active and done surveys.') . '</caption>';
    echo '<thead><tr><th>' .
         _('Survey') .
         '</th><th>' .
         _('Login Failures') .
         '</th><th>' .
         _('Attempted') .
         '</th><th>' .
         _('Abandoned') .
         '</th><th>' .
         _('Suspended') .
         '</th><th>' .
         _('Completed') .
         '</th><th>' .
         _('Reset?') .
         '</th></tr></thead><tbody>';
    foreach ($stats as $sid => $stat) {
        echo '<tr><td>' .
             (empty($sid) ? _('Statistics Total') : $surveys[$sid]['name']) .
             '</td><td>' .
             $stat['loginfail'] .
             '</td><td>' .
             $stat['attempted'] .
             '</td><td>' .
             $stat['abandoned'] .
             '</td><td>' .
             $stat['suspended'] .
             '</td><td>' .
             $stat['completed'] .
             '</td><td>' .
             (empty($sid) ? '' : mkcheckbox('reset', $sid)) .
             '</td></tr>';
    }
    echo '</tbody></table>';
    $confirm = sprintf(
                   'return confirm("%s");',
                   _('You are about to make permanent and irrevocable changes. Click OK if you are certain this is what you want to do.')
               );
    echo mksubmit('doRefresh', _('Refresh')) .
         '&nbsp;' .
         mksubmit('doReset', _('Reset Statistics'), array ('onclick' => $confirm, 'onkeypress' => $confirm));
} else {
    echo _('You have no active or done surveys. Check back later.');
}
echo '</div>';
echo("<a href=\"". $GLOBALS['ESPCONFIG']['ME'] ."?where=manage\">" . _('Go back to Management Interface') . "</a>\n");
?>
