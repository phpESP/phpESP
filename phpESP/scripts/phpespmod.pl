#!/usr/bin/perl
#
# phpespmod.pl  - create a phpESP survey from delimited text
#   Super Quick Hack by Jim Brown, jpb@sixshooter.v6.thrupoint.net
#   Released under BSD License.
#   05/07/05
#
# Modified by Bishop Bettini <php@ideacode.com> to:
#   * 2008-01-22  Support latest survey table schema
#   * 2008-01-22  Set more obvious defaults
#   * 2008-01-24  Allow question name to be set in input file
#
# This script creates SQL statements for creating a new phpESP survey.
#
### WARNING ###
#
# This script is quasi-supported by the phpESP project: USE AT YOUR OWN RISK.
#
# We recommend throughly reading the documentation in this file as well as visiting the doc URL:
#    http://sixshooter.v6.thrupoint.net/phpESP_proto/README.txt
# 
# Always validate the SQL generated against your expectations!
#
### WARNING ###
#               
# The input is a text file prepared with pipe ('|') delimited format:
#   - the question (or item)
#   - any other text or explanation about the question (not currently used, set to 'none')
#   - the question type (yes,text,essay,radio,check,dropdown,rate,date,numeric,break,section)
#   - mutiple button labels delimited by ':' colons i.e. "first one:second one:third time pays for all"
#   - (optional) question name; if not present, quesX is used, where "X" is an incrementing number
#                 
#
# Sample input lines:
#
#  # Page 1 (comment line)
#  What is your name?|none|text
#  Which cheese do you like best?|none|radio|cheddar:monterey jack:swiss
#  Which drinks do you like? (Check all that apply.)|none|check|water:soda:wine:beer:rot gut whiskey
#  What kind of vechicle do you drive?|none|dropdown|volvo:mercedes benz:alfa romeo:volkswagon:skateboard
#  break|none|break|none
#  # Page 2
#  Please rate your experince with:|none|rate|computing:sports:social situations:inlaws:outlaws
#  Your age:|none|numeric
#  Today's date:|none|date
#  Are you satisfied with this survey?|none|yes
#  If no, what changes would you make?|none|essay
#
#
# The script requires several command line variables as follows:
#
#   perl phpmodesp.pl survey_id_num  survey_name question_start_num
# 
# See comments below.
#  
# Notes:
#   Required questions are not supported by this code, but could be implemented with the 
#   'any other text' field (see above) and a couple of changes below.
#
#   See the README file for further details.
#
#=====================================================================================================


sub usage()
{
   print "\nusage:  perl phpmodesp.pl survey_id_num  survey_name question_start_num\n\n";
   exit 1;
}


print "-- # Start of Program\n";

#
# Get mandatory options from command line.
#

if($#ARGV != 2)
{
   &usage;
}
else
{

  $q_survey_id   = $ARGV[0];  # Start with this survey number.  Must be 1 greater than last database entry. Check database survey table.
  $s_name        = $ARGV[1];  # Must be a unique name.  Check database survey table.
  $q_id          = $ARGV[2];  # Start with this question number.  Must be 1 greater than last database entry.  Check database question table.
}

# Use the following SQL statements to determine correct values
# for the above variables.
#
#   SELECT COUNT(*) FROM question;   (Use value+1 for q_survey_id parameter.)
#
#   SELECT COUNT(*) FROM survey;     (Use value+1 for q_id parameter.)
#
#   SELECT name FROM survey;         (Use a new name not in this list.)
#



# Hardcoded banner,title, and thank_you values.  Change to a perl require file if needed.

$s_title          = "FIX_ME";
$s_subtitle       = "FIX_ME";
$s_info           = "FIX_ME";
$s_email          = "FIX_ME";

$s_css            = "FIX_ME";
$s_thanks_head    = "FIX_ME";
$s_thanks_body    = "FIX_ME";

 
#
# Print out some info to STDERR
#
 
 print STDERR << "END_CMDLINE";
 
 q_survey_id   = [$q_survey_id]
 s_name        = [$s_name]
 q_id          = [$q_id]
 
END_CMDLINE
 
 
 print STDERR << "END_REQUIRE";
 s_title    = [$s_title]
 s_subtitle = [$s_subtitle]
 s_info     = [$s_info]
 s_thanks_head = [$s_thanks_head]
 s_thanks_body = [$s_thanks_body]
 
END_REQUIRE
 
#
# shift command line arguments off the ARGV vector
#

shift @ARGV;
shift @ARGV;
shift @ARGV;


#-----------------------------------------------------------------------------------------------------



#
# Question variables.
#
$q_name        = "";
$q_type_id     = 0;
$q_result_id   = "NULL";
$q_length      = 0;
$q_precise     = 0;
$q_position    = 0;
$q_content     = "";
$q_required    = "N";
$q_deleted     = "N";
$q_public      = "Y";


$db_survey_insert = 

"INSERT INTO survey (id,name,owner,realm,public,status,title,email,subtitle,info,theme,thanks_page,thank_head,thank_body,changed) VALUES (
$q_survey_id,
\"$s_name\",
\"root\",
\"superuser\",
\"Y\",
0,
\"$s_title\",
\"$s_email\",
\"$s_subtitle\",
\"$s_info\",
\"$s_css\",
\"\",
\"$s_thanks_head\",
\"$s_thanks_body\",
NOW() );";



#
# Print survey record to STDOUT.
#

print $db_survey_insert;

#
# Now read the incoming text file and make SQL statements for questions and choices.
#

while(<>)
{

  $inputline = $_;
  chop $inputline;

print "\n\n-- # [$inputline]\n";

  next if $inputline eq "";

  next if $inputline =~ /^\s+$/;

  next if $inputline =~ /^\s*#/;


  # reset preceision, length

  $q_precise = 0;
  $q_length  = 0;



  $quest_name = sprintf( "%s%d","quest",$q_id);

  ($q_question,$q_adl_text,$q_question_type,$buttons) = split('\|', $inputline);
  ($quest_name = sprintf( "%s%d","quest",$q_id)) unless $quest_name;


  if($q_question_type =~/yes/i)      { $q_type_id = 1; }
  if($q_question_type =~/text/i)     { $q_type_id = 2; $q_length = 50;}  #arbitrarily set everyone now
  if($q_question_type =~/essay/i)    { $q_type_id = 3; $q_precise = 10; $q_length = 50; }  # precise is top-bottom length, length is side-side width
  if($q_question_type =~/radio/i)    { $q_type_id = 4; }
  if($q_question_type =~/check/i)    { $q_type_id = 5; }
  if($q_question_type =~/dropdown/i) { $q_type_id = 6; }
  if($q_question_type =~/rate/i)     { $q_type_id = 8; $q_length = 5;} # hardcoded by survey?
  if($q_question_type =~/date/i)     { $q_type_id = 9; }
  if($q_question_type =~/numeric/i)  { $q_type_id = 10;$q_length = 10; } #hardcoded by me
  if($q_question_type =~/break/i)    { $q_type_id = 99; $q_question = "break";  }
  if($q_question_type =~/section/i)  { $q_type_id = 100; }



# use q_qid for question number in the SQL comment '-- $q_id'  below

  $db_insert_question = 
"INSERT INTO question VALUES ( 
   NULL,                -- $q_id
   $q_survey_id,
   \"$quest_name\",
   $q_type_id,
   $q_result_id,
   $q_length,
   $q_precise,
   $q_id,
   \"$q_question\",
   \"$q_required\",
   \"$q_deleted\",
   \"$q_public\" );";


print "$db_insert_question\n";


# decode buttons on  input line
#  $_ = $buttons;

  @b_buttons = split ':', $buttons;;


  if(!( $q_question_type =~/break/i or  $q_question_type =~ /section/i or $q_question_type =~ /yes/ ))
  {
    foreach $i (@b_buttons)
    {

      $db_insert_choice = 
"INSERT INTO question_choice VALUES (
  NULL,
  $q_id,
  \"$i\",
  NULL );";


      print "$db_insert_choice\n";

    }

  }

  # bump question id

  $q_id++;

}

