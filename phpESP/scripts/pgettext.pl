#!/usr/bin/perl
use strict;

# $Id$

# set up variables
my $verbose = 0;
my $strict = 0;
my $php = 0;
my $debug = 0;
my $width = 80;
my $output = 'messages.po';

my %msgs;
my @order = ();

sub usage {
	print "usage: $0 [-v|--verbose] [-s|--strict] [-o|--output file] file ...\n\n";
	exit 1;
}

sub debug {
	my $level = shift;
	if ($debug >= $level) {
		print STDERR shift;
		return 1;
	}
	return 0;
}

sub add_msg {
	my $msg = shift;
	my $file = shift;
	my $line = shift;
	
	debug(1, "$file:$line == $msg\n");

	if (exists($msgs{$msg})) {
		push(@{$msgs{$msg}}, [ $file, $line ]);
	} else {
		$msgs{$msg} = [ [ $file, $line ] ];
		push(@order, $msg);
	}
}

sub msg_cmp {
	# $args = [ $file, $line ]
	if ($a->[0] eq $b->[0]) {
		return ($a->[1] <=> $b->[1]);
	}
	return ($a->[0] cmp $b->[0]);
}

my $s_blank = "\n";
my $s_pre = "#: ";
my $s_fmt = "%s:%d";

{
	my $arg = "";
	ARG: while ($arg = shift @ARGV) {
		if ($arg =~ s/^-//) {
			while ($arg ne '') {
				if ($arg =~ s/^v//) {
						$verbose++;
						$| = 1;
				} elsif ($arg =~ s/^s//) {
						$strict++;
				} elsif ($arg =~ s/^o//) {
						$output = shift @ARGV
							or usage();
						if ($output eq '-') {
							$output = '&STDOUT';
						}
				} elsif ($arg =~ s/^p//) {
						$php++;
				} elsif ($arg =~ s/^g//) {
						$debug++;
						$| = 1;
				} elsif ($arg eq '-') {
					last ARG;
				} elsif ($arg =~ s/^-(.).*/$1/) {
					# long arg type
				} else {
					usage();
				}
			}
		} else {
			last ARG;
		}
	}
	if ($arg ne '-') {
		unshift @ARGV, $arg;
	}
}

if ($strict) {
	$s_blank = "#\n";
	$s_pre = "# ";
	$s_fmt = "File: %s, line: %d\n";
}

@ARGV = sort @ARGV;

while (my $file = shift @ARGV) {
	open IN, $file
		or die "Can't open $file.";
	if (!debug(1, "Scanning $file ...\n") && $verbose) {
		print "Scanning $file ...\n";
	}
	my $lnum = 0;
	my $snum = 0;
	my $line = '';
	LINE: while (my $str = <IN>) {
		$line .= $str;
		$lnum++;
		if (!debug(5,"$lnum:$line") && (($lnum % 5) == 0)) {
			debug(1, "\tline:$lnum\n");
		}

		while ($line ne '') {
			debug(5, "$lnum:$line");
			if (!($line =~ s/.*?(?=_\()//s )) {
				$line = '';
				$snum = 0;
				next LINE;
			} elsif ($snum == 0) {
				$snum = $lnum;
			}

			if ($line =~ s/^_\(\s*"(([^"]|\\")*)"\s*\)//s) {
				my $msg = $1;
				debug(5, "case: double quotes\n");
				$msg =~ s/\t/\\t/g;
				$msg =~ s/\n/\\n/g;
				add_msg($msg, $file, $snum);
				$snum = 0;
			} elsif ($line =~ s/^_\(\s*'(([^']|\\')*)'\s*\)//s) {
				my $msg = $1;
				debug(5, "case: single quotes\n");
				$msg =~ s/\\'/'/g;
				$msg =~ s/\\/\\\\/g;
				$msg =~ s/\t/\\t/g;
				$msg =~ s/\n/\\n/g;
				$msg =~ s/"/\\"/g;
				add_msg($msg, $file, $snum);
				$snum = 0;
			} else {
				# line-spanning string. get next line.
				debug(5, "case: multiline\n");
				next LINE;
			}
		}
	}
	close IN;
}

my $date = `date "+%Y-%m-%d %H:%M%z"`;
chop $date;

open MESSAGES, ">$output" or
	die "cannot create $output";

print MESSAGES <<EOF;
# SOME DESCRIPTIVE TITLE.
# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER
# This file is distributed under the same license as the PACKAGE package.
# FIRST AUTHOR <EMAIL\@ADDRESS>, YEAR.
#
#, fuzzy
msgid ""
msgstr ""
"Project-Id-Version: PACKAGE VERSION\\n"
"POT-Creation-Date: $date\\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"
"Last-Translator: FULL NAME <EMAIL\@ADDRESS>\\n"
"Language-Team: LANGUAGE <LL\@li.org>\\n"
"MIME-Version: 1.0\\n"
"Content-Type: text/plain; charset=CHARSET\\n"
"Content-Transfer-Encoding: 8bit\\n"
EOF

foreach my $msg (@order) {
	if ($msg =~ m/^\s*$/) {
		next;
	}
	print MESSAGES $s_blank;
	my @loc = map { sprintf($s_fmt, @{$_}); }
			sort msg_cmp @{$msgs{$msg}};
	my $str = '';
	foreach my $loc (@loc) {
		if ($loc =~ m/\n/) {
			print MESSAGES $s_pre, $loc;
			next;
		}
		
		if (length($s_pre) + length($str) + 
				1 + length($loc) >= $width) {
			print MESSAGES $s_pre, $str, "\n";
			$str = $loc;
		} elsif ($str eq '') {
			$str = $loc;
		} else {
			$str .= ' ' . $loc;
		}
	}
	if ($str ne '') {
		print MESSAGES $s_pre, $str, "\n";
	}
	if ($msg =~ s/\\n/\\n"\n"/g) {
		$msg = "\"\n\"" . $msg;
	}
	print MESSAGES "msgid \"$msg\"\nmsgstr \"\"\n";
}
close MESSAGES;
