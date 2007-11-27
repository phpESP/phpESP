--
-- Table structure for table `access`
--

CREATE TABLE access (
  id integer NOT NULL,
  survey_id id integer  NOT NULL default '0',
  realm char(16) default NULL,
  maxlogin id integer  default '0',
  resume VARCHAR(1)  NOT NULL default 'N',
  navigate VARCHAR(1)  NOT NULL default 'N',
  PRIMARY KEY  (id)
) ;

--
-- Table structure for table `designer`
--

CREATE TABLE designer (
  username char(64) NOT NULL default '',
  password char(64) NOT NULL default '',
  auth char(16) NOT NULL default 'BASIC',
  realm char(16) NOT NULL default '',
  fname char(16) default NULL,
  lname char(24) default NULL,
  email char(64) default NULL,
  pdesign VARCHAR(1)  NOT NULL default 'Y',
  pstatus VARCHAR(1)  NOT NULL default 'N',
  pdata VARCHAR(1)  NOT NULL default 'N',
  pall VARCHAR(1)  NOT NULL default 'N',
  pgroup VARCHAR(1)  NOT NULL default 'N',
  puser VARCHAR(1)  NOT NULL default 'N',
  disabled VARCHAR(1)  NOT NULL default 'N',
  changed timestamp(14) default NULL,
  expiration timestamp(14) NOT NULL default '0',
  PRIMARY KEY  (username,realm)
) ;

--
-- Dumping data for table `designer`
--

INSERT INTO designer VALUES ('root','6915dba446289209cb5d2d799778a6d2','BASIC','superuser','ESP','Superuser',NULL,'Y','Y','Y','Y','Y','Y','N',date('now'),0);

--
-- Table structure for table `question`
--

CREATE TABLE question (
  id integer NOT NULL,
  survey_id id integer  NOT NULL default '0',
  name varchar(30) NOT NULL default '',
  type_id id integer  NOT NULL default '0',
  result_id id integer  default NULL,
  length int(11) NOT NULL default '0',
  precise int(11) NOT NULL default '0',
  position id integer  NOT NULL default '0',
  content text NOT NULL,
  required VARCHAR(1)  NOT NULL default 'N',
  deleted VARCHAR(1)  NOT NULL default 'N',
  public VARCHAR(1)  NOT NULL default 'Y',
  PRIMARY KEY  (id)
) ;

--
-- Table structure for table `question_choice`
--

CREATE TABLE question_choice (
  id integer NOT NULL,
  question_id id integer  NOT NULL default '0',
  content text NOT NULL,
  value text,
  PRIMARY KEY  (id)
) ;

--
-- Table structure for table `question_type`
--

CREATE TABLE question_type (
  id integer NOT NULL,
  type char(32) NOT NULL default '',
  has_choices VARCHAR(1)  NOT NULL default 'Y',
  response_table char(32) NOT NULL default '',
  PRIMARY KEY  (id)
) ;

--
-- Dumping data for table `question_type`
--

INSERT INTO question_type VALUES (1,'Yes/No','N','response_bool');
INSERT INTO question_type VALUES (2,'Text Box','N','response_text');
INSERT INTO question_type VALUES (3,'Essay Box','N','response_text');
INSERT INTO question_type VALUES (4,'Radio Buttons','Y','response_single');
INSERT INTO question_type VALUES (5,'Check Boxes','Y','response_multiple');
INSERT INTO question_type VALUES (6,'Dropdown Box','Y','response_single');
INSERT INTO question_type VALUES (8,'Rate (scale 1..5)','Y','response_rank');
INSERT INTO question_type VALUES (9,'Date','N','response_date');
INSERT INTO question_type VALUES (10,'Numeric','N','response_text');
INSERT INTO question_type VALUES (99,'Page Break','N','');
INSERT INTO question_type VALUES (100,'Section Text','N','');

--
-- Table structure for table `realm`
--

CREATE TABLE realm (
  name char(16) NOT NULL default '',
  title char(64) NOT NULL default '',
  changed timestamp(14) default NULL,
  PRIMARY KEY  (name)
) ;

--
-- Dumping data for table `realm`
--

INSERT INTO realm VALUES ('superuser','ESP System Administrators',date('now'));
INSERT INTO realm VALUES ('auto','Self added users',date('now'));

--
-- Table structure for table `respondent`
--

CREATE TABLE respondent (
  username char(64) NOT NULL default '',
  password char(64) NOT NULL default '',
  auth char(16) NOT NULL default 'BASIC',
  realm char(16) NOT NULL default '',
  fname char(16) default NULL,
  lname char(24) default NULL,
  email char(64) default NULL,
  disabled VARCHAR(1)  NOT NULL default 'N',
  changed timestamp(14) default NULL,
  expiration timestamp(14) NOT NULL default '0',
  PRIMARY KEY  (username,realm)
) ;

--
-- Table structure for table `response`
--

CREATE TABLE response (
  id integer NOT NULL,
  survey_id id integer  NOT NULL default '0',
  submitted timestamp(14) NOT NULL,
  complete VARCHAR(1)  NOT NULL default 'N',
  username char(64) default NULL,
  PRIMARY KEY  (id)
) ;

--
-- Table structure for table `response_bool`
--

CREATE TABLE response_bool (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  choice_id VARCHAR(1)  NOT NULL default 'Y',
  PRIMARY KEY  (response_id,question_id)
) ;

--
-- Table structure for table `response_date`
--

CREATE TABLE response_date (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  response date default NULL,
  PRIMARY KEY  (response_id,question_id)
) ;

--
-- Table structure for table `response_multiple`
--

CREATE TABLE response_multiple (
  id integer NOT NULL,
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  choice_id id integer  NOT NULL default '0',
  PRIMARY KEY  (id)
) ;

--
-- Table structure for table `response_other`
--

CREATE TABLE response_other (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  choice_id id integer  NOT NULL default '0',
  response text,
  PRIMARY KEY  (response_id,question_id,choice_id)
) ;

--
-- Table structure for table `response_rank`
--

CREATE TABLE response_rank (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  choice_id id integer  NOT NULL default '0',
  rank int(11) NOT NULL default '0',
  PRIMARY KEY  (response_id,question_id,choice_id)
) ;

--
-- Table structure for table `response_single`
--

CREATE TABLE response_single (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  choice_id id integer  NOT NULL default '0',
  PRIMARY KEY  (response_id,question_id)
) ;


--
-- Table structure for table `response_text`
--

CREATE TABLE response_text (
  response_id id integer  NOT NULL default '0',
  question_id id integer  NOT NULL default '0',
  response text,
  PRIMARY KEY  (response_id,question_id)
) ;

--
-- Table structure for table `survey`
--

CREATE TABLE survey (
  id integer NOT NULL,
  name varchar(64) NOT NULL default '',
  owner varchar(16) NOT NULL default '',
  realm varchar(64) NOT NULL default '',
  public VARCHAR(1)  NOT NULL default 'Y',
  status id integer  NOT NULL default '0',
  title varchar(255) NOT NULL default '',
  email varchar(64) default NULL,
  subtitle text,
  info text,
  theme varchar(64) default NULL,
  thanks_page varchar(255) default NULL,
  thank_head varchar(255) default NULL,
  thank_body text,
  changed timestamp(14) default NULL,
  unique(name),
  PRIMARY KEY  (id)
) ;
