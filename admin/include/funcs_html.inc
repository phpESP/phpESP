<?php

# $Id$

// Written by James Flemer
// For eGrad2000.com
// <jflemer@acm.jhu.edu>
// <jflemer@eGrad2000.com>

// void		mkwarn(char *warning);
// void		mkerror(char *error);

// void		mkradio(char *name, char *value);
// void		mkcheckbox(char *name, char *value);
// void		mktext(char *name);
// void		mktextarea(char *name, int rows, int cols, bool wordwrap);
// void		mkselect(char *name, char *options[]);

// void		mkrespercent(int[char*] counts, int total, bool showTotals);
// void		mkresrank(int[char*] counts, int total, bool showTotals);
// void		mkrescount(int[char*] counts, int total, bool showTotals);
// void		mkreslist(int[char*] counts, int total, bool showTotals);
// void		mkresavg(int[char*] counts, int total, bool showTotals);

if(!defined('_FUNCS_HTML')) {
	define('_FUNCS_HTML',TRUE);

	// return HTML format for an warning message
	function mkwarn ($msg) {
		global $cf_warn_color;
		return("<font color=\"${cf_warn_color}\" size=\"+1\">${msg}</font>\n");
	}

	// return HTML format for an error message
	function mkerror ($msg) {
		global $cf_error_color;
		return("<font color=\"${cf_error_color}\" size=\"+1\">[ ${msg} ]</font>\n");
	}

	// return HTML format for a radio button
	function mkradio ($name, $value) {
		global $$name;	// needs register_globals = on
		$str = '<input type="radio" name="' . htmlspecialchars($name) .'" value="' . htmlspecialchars($value) .'"';
		if ( $$name == $value )
			$str .= ' checked';
		$str .= '>';
		return($str);
	}

	// return HTML format for a check box
	function mkcheckbox ($name, $value) {
		global $$name;	// needs register_globals = on
		$str = '<input type="checkbox" name="' . htmlspecialchars($name) .'[]" value="' . htmlspecialchars($value) .'"';
		if ( @in_array($value,$$name ) )
			$str .= ' checked';
		$str .= '>';
		return($str);
	}

	// return HTML format for a text entry line
	function mktext ($name) {
		global $$name;	// needs register_globals = on
		return('<input type="text" name="'. htmlspecialchars($name) .'" value="'. htmlspecialchars($$name) .'">');
	}

	// return HTML format for a text entry box
	function mktextarea ($name,$rows,$cols,$wrap) {
		global $$name;	// needs register_globals = on
		$str = '<textarea name="' . htmlspecialchars($name) .'"';
		if($rows > 0)
			$str .= ' rows="' . $rows . '"';
		if($cols > 0)
			$str .= ' cols="' . $cols . '"';
		if($wrap != '')
			$str .= ' wrap="' . strtolower($wrap) . '"';
		$str .= '>' . $$name . '</textarea>';
		return($str);
	}

	// return HTML format for a select box (dropdown)
	function mkselect ($name,$options) {
		global $$name;
		$str  = '<select name="' .$name .'">' . "\n";
		$str .= "<option></option>\n";
		while(list($cid, $content) = each($options)) {
			$checked = '';
			if($$name == $cid)
				$checked = ' selected';
			$str .= '<option value="' . $cid .'"'. $checked .">${content}</option>\n";
		}
		$str .= "</select>\n";
		return($str);
	}

	// build HTML showing PERCENTAGE results
	function mkrespercent($counts,$total,$use_totals) {
		global $IMAGE_PATH;
		$i=0;
?>
<table width="90%" border="0">
<?php
		while(list($content,$num) = each($counts)) {
			if($num>0) { $percent = $num/$total*100.0; }
			else { $percent = 0; }

			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($content); ?></td>
		<td align="left">
<?php		if($num)
				printf("&nbsp;<img src=\"${IMAGE_PATH}hbar_l.gif\" height=9 width=4><img src=\"${IMAGE_PATH}hbar.gif\" height=9 width=%d><img src=\"${IMAGE_PATH}hbar_r.gif\" height=9 width=4>&nbsp;%.2f%%",$percent*2, $percent);
?>
		</td>
		<td align="right">(<?php echo($num); ?>)</td>
	</tr>
<?php
			$i += $num;
		} // end while
		if($use_totals) {
			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><b>TOTAL</b></td>
		<td><b>&nbsp;<?php if($i && !($i > $total)) printf("<img src=\"${IMAGE_PATH}hbar_l.gif\" height=9 width=4><img src=\"${IMAGE_PATH}hbar.gif\" height=9 width=%d><img src=\"${IMAGE_PATH}hbar_r.gif\" height=9 width=4>&nbsp;%.2f%%",$i/$total*200.0,$i/$total*100.0); ?></b></td>
		<td align="right"><b><?php echo($total); ?></b></td>
	</tr>
<?		} ?>
</table>
<?php
	} // end function mkrespercent($counts,$total)

	// build HTML showing RANK results
	function mkresrank($counts,$total,$use_totals) {
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

			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td align="right"><b><?php echo(++$i); ?></b></td>
		<td><?php echo($content); ?></td>
		<td align="right" width="60"><?php if($p) printf("%.2f%%",$p); ?></td>
		<td align="right" width="60">(<?php echo($num); ?>)</td>
	</tr>
<?php
		} // end while
		if($use_totals) {
			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td colspan=2 align="left"><b>TOTAL</b></td>
		<td align="right"><b><?php printf("%.2f%%",$pt); ?></b></td>
		<td align="right"><b><?php echo($total); ?></b></td>
	</tr>
<?		} ?>
</table>
<?php
	} // end function mkresrank($counts,$total)

	// build HTML showing COUNT results
	function mkrescount($counts,$total,$use_totals) {
		$i=0;
?>
<table width="90%" border="0">
<?php
		while(list($content,$num) = each($counts)) {
			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><?php echo($content); ?></td>
		<td align="right" width="60"><?php echo($num); ?></td>
		<td align="right" width="60">(<?php if($num) printf("%.2f",$num/$total*100.0); ?>%)</td>
	</tr>
<?php
			$i += $num;
		} // end while
		if($use_totals) {
			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td><b>TOTAL</b></td>
		<td align="right"><b><?php echo($total); ?></b></td>
		<td align="right"><b>(<?php if($i) printf("%.2f",$i/$total*100.0); ?>%)</b></td>
	</tr>
<?		} ?>
</table>
<?php
	} // end function mkrescount($counts,$total)

	// build HTML showing LIST results
	function mkreslist($counts,$total,$use_totals) {
		if($total==0)	return;
?>
<table width="90%" border="0" cellpadding="1">
	<tr><th align="left">#</th><th>Response</th></tr>
<?php
	while(list($text,$num) = each($counts)) {
		if($bg != "#eeeeee")	$bg = "#eeeeee";
		else                	$bg = "#ffffff";
		echo("<tr bgcolor=\"$bg\"><th align=\"left\">$num</th><td>$text</td></tr>\n");
	}
?>
</table>
<?php
	} // end function mkreslist($counts,$total)

	// build HTML showing AVG results
	function mkresavg($counts,$total,$use_totals) {
		global $IMAGE_PATH;
		$i=0;
?>
<table border="0">
	<tr>
		<td></td>
		<td align="center" colspan="7">Average rank</td>
	</tr>
	<tr>
		<td></td>
		<td align="right" width="44">1</td>
		<td align="right" width="40">2</td>
		<td align="right" width="40">3</td>
		<td align="right" width="40">4</td>
		<td align="right" width="36">5</td>
		<td width="20"></td>
		<td></td>
	</tr>
<?php
		while(list($content,$avg) = each($counts)) {
			if($avg>5) { $avg = 5; }
			if($avg<0) { $avg = 0; }

			if($bg != "#eeeeee")	$bg = "#eeeeee";
			else                	$bg = "#ffffff";
?>
	<tr bgcolor="<?php echo($bg); ?>">
		<td align="right"><?php echo($content); ?></td>
		<td align="left" width="220" colspan="6">
<?php		if($avg)
				printf("&nbsp;<img src=\"${IMAGE_PATH}hbar_l.gif\" height=9 width=4><img src=\"${IMAGE_PATH}hbar.gif\" height=9
				width=%d><img src=\"${IMAGE_PATH}hbar_r.gif\" height=9 width=4>",$avg*40);
?>
		</td>
		<td align="right" width="60">(<?php printf("%.2f",$avg); ?>)</td>
	</tr>
<?php
			$i += $num;
		} // end while
?>
</table>
<?php
	} // end function mkresavg($counts,$total)

}	// end _FUNCS_HTML

?>
