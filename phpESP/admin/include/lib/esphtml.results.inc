<?php

# $Id$

// Written by James Flemer
// <jflemer@acm.jhu.edu>

// void		mkrespercent(int[char*] counts, int total, bool showTotals);
// void		mkresrank(int[char*] counts, int total, bool showTotals);
// void		mkrescount(int[char*] counts, int total, bool showTotals);
// void		mkreslist(int[char*] counts, int total, bool showTotals);
// void		mkresavg(int[char*] counts, int total, bool showTotals);

if(!defined('_LIB_ESPHTML_RESULTS')) {
	define('_LIB_ESPHTML_RESULTS',TRUE);

/* {{{ proto void mkrespercent(array weights, int total, int precision, bool show_totals)
  Builds HTML showing PERCENTAGE results. */
function mkrespercent($counts,$total,$precision,$showTotals) {
	$i=0;
?>
<table width="90%" border="0">
<?php
	while(list($content,$num) = each($counts)) {
		if($num>0) { $percent = $num/$total*100.0; }
		else { $percent = 0; }

		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($content); ?></td>
		<td align="left">
<?php	
		if($num)
			printf("&nbsp;<img src=\"". ESP_IMAGE_PATH . "hbar_l.gif\" height=9 width=4><img src=\"". ESP_IMAGE_PATH ."hbar.gif\" height=9 width=%d><img src=\"". ESP_IMAGE_PATH ."hbar_r.gif\" height=9 width=4>&nbsp;%.${precision}f%%",$percent*2, $percent);
?>
		</td>
		<td align="right">(<?php echo($num); ?>)</td>
	</tr>
<?php
		$i += $num;
	} // end while
	if($showTotals) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><b><?php echo(_('TOTAL')); ?></b></td>
		<td width="40%"><b>&nbsp;<?php if($i && !($i > $total)) printf("<img src=\"". ESP_IMAGE_PATH ."hbar_l.gif\" height=9 width=4><img src=\"". ESP_IMAGE_PATH ."hbar.gif\" height=9 width=%d><img src=\"". ESP_IMAGE_PATH ."hbar_r.gif\" height=9 width=4>&nbsp;%.${precision}f%%",$i/$total*200.0,$i/$total*100.0); ?></b></td>
		<td width="10%" align="right"><b><?php echo($total); ?></b></td>
	</tr>
<?	} ?>
</table>
<?php
}
/* }}} */

	
/* {{{ proto void mkresrank(array weights, int total, int precision, bool show_totals) 
   Builds HTML showing RANK results. */
function mkresrank($counts,$total,$precision,$showTotals) {
?>
<table border="0">
	<tr>
		<td align="right"><b>Rank</b></td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
<?php
	arsort($counts);
	$i=0; $pt=0;
	while(list($content,$num) = each($counts)) {
		if($num)
			$p = $num/$total*100.0;
		else
			$p = 0;
		$pt += $p;

		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td align="right"><b><?php echo(++$i); ?></b></td>
		<td><?php echo($content); ?></td>
		<td align="right" width="60"><?php if($p) printf("%.${precision}f%%",$p); ?></td>
		<td align="right" width="60">(<?php echo($num); ?>)</td>
	</tr>
<?php
	} // end while
	if($showTotals) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td colspan=2 align="left"><b><?php echo(_('TOTAL')); ?></b></td>
		<td align="right"><b><?php printf("%.${precision}f%%",$pt); ?></b></td>
		<td align="right"><b><?php echo($total); ?></b></td>
	</tr>
<?	} ?>
</table>
<?php
}
/* }}} */

/* {{{ proto void mkrescount(array weights, int total, int precision, bool show_totals) 
   Builds HTML showing COUNT results. */
function mkrescount($counts,$total,$precision,$showTotals) {
	$i=0;
?>
<table width="90%" border="0">
<?php
	while(list($content,$num) = each($counts)) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($content); ?></td>
		<td align="right" width="60"><?php echo($num); ?></td>
		<td align="right" width="60">(<?php if($num) printf("%.${precision}f",$num/$total*100.0); ?>%)</td>
	</tr>
<?php
		$i += $num;
	} // end while
	if($showTotals) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><b><?php echo(_('TOTAL')); ?></b></td>
		<td align="right"><b><?php echo($total); ?></b></td>
		<td align="right"><b>(<?php if($i) printf("%.${precision}f",$i/$total*100.0); ?>%)</b></td>
	</tr>
<?	} ?>
</table>
<?php
}
/* }}} */

/* {{{ proto void mkreslist(array weights, int total, int precision, bool show_totals) 
	Builds HTML showing LIST results. */
function mkreslist($counts,$total,$precision,$showTotals) {
	if($total==0)	return;
?>
<table width="90%" border="0" cellpadding="1">
	<tr><th align="left"><?php echo(_('#')); ?></th><th><?php echo(_('Response')); ?></th></tr>
<?php
	while(list($text,$num) = each($counts)) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
		echo("<tr bgcolor=\"$bg\"><th align=\"left\">$num</th><td>$text</td></tr>\n");
	}
?>
</table>
<?php
}
/* }}} */

/* {{{ proto void mkresavg(array weights, int total, int precision, bool show_totals) 
	Builds HTML showing AVG results. */
function mkresavg($counts,$total,$precision,$showTotals) {
	$i=0;
?>
<table border="0">
	<tr>
		<td></td>
		<td align="center" colspan="7"><?php echo(_('Average rank')); ?></td>
	</tr>
	<tr>
		<td></td>
		<td align="right" width="44"><?php echo(_('1')); ?></td>
		<td align="right" width="40"><?php echo(_('2')); ?></td>
		<td align="right" width="40"><?php echo(_('3')); ?></td>
		<td align="right" width="40"><?php echo(_('4')); ?></td>
		<td align="right" width="36"><?php echo(_('5')); ?></td>
		<td width="20"></td>
		<td></td>
	</tr>
<?php
	while(list($content,$avg) = each($counts)) {
		if($bg != ESP_COLOR_ALTBG1)
			$bg = ESP_COLOR_ALTBG1;
		else
			$bg = ESP_COLOR_ALTBG2;
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td align="right"><?php echo($content); ?></td>
		<td align="left" width="220" colspan="6">
<?php
		if($avg) {
			echo("&nbsp;<img src=\"". ESP_IMAGE_PATH ."hbar_l.gif\" height=9 width=4><img src=\"". ESP_IMAGE_PATH ."hbar.gif\" height=9");
			printf(" width=%d><img src=\"". ESP_IMAGE_PATH ."hbar_r.gif\" height=9 width=4>",$avg*40);
		}
?>
		</td>
		<td align="right" width="60">(<?php printf("%.${precision}f",$avg); ?>)</td>
	</tr>
<?php
		$i += $num;
	} // end while
?>
</table>
<?php
}
/* }}} */

} // end _LIB_ESPHTML_RESULTS
?>
