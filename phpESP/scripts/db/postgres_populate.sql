BEGIN;

--
-- Sequences for table ACCESS
--

CREATE SEQUENCE access_id_seq;

-- MySQL dump 9.10
--
-- Host: bfs.itlab.musc.edu    Database: esp_test
-- ------------------------------------------------------
-- Server version	4.0.16-log

--
-- Table structure for table `access`
--

CREATE TABLE access (
  id INT4 DEFAULT nextval('access_id_seq'),
  survey_id INT4  NOT NULL DEFAULT '0',
  realm varchar(16) DEFAULT NULL,
  maxlogin INT4  DEFAULT '0',
  resume varchar (1) NOT NULL DEFAULT 'N',
  navigate varchar (1) NOT NULL DEFAULT 'N',
  PRIMARY KEY (id),
  CHECK (survey_id>=0),
  CHECK (maxlogin>=0)

);

--
-- Table structure for table `designer`
--

CREATE TABLE designer (
  username varchar(64) NOT NULL DEFAULT '',
  password varchar(64) NOT NULL DEFAULT '',
  auth varchar(16) NOT NULL DEFAULT 'BASIC',
  realm varchar(16) NOT NULL DEFAULT '',
  fname varchar(16) DEFAULT NULL,
  lname varchar(24) DEFAULT NULL,
  email varchar(64) DEFAULT NULL,
  pdesign varchar(1) NOT NULL DEFAULT 'Y',
  pstatus varchar(1) NOT NULL DEFAULT 'N',
  pdata varchar(1) NOT NULL DEFAULT 'N',
  pall varchar(1) NOT NULL DEFAULT 'N',
  pgroup varchar(1) NOT NULL DEFAULT 'N',
  puser varchar(1) NOT NULL DEFAULT 'N',
  disabled varchar(1) NOT NULL DEFAULT 'N',
  changed TIMESTAMP NOT NULL,
  expiration varchar (19) NULL,
  PRIMARY KEY (username,realm)

);

--
-- Dumping data for table `designer`
--

INSERT INTO designer VALUES ('root',md5('esp'),'BASIC','superuser','ESP','Superuser',NULL,'Y','Y','Y','Y','Y','Y','N',now(),'0');


--
-- Table structure for table `question`
--



--
-- Sequences for table QUESTION
--

CREATE SEQUENCE question_id_seq;

CREATE TABLE question (
  id INT4 DEFAULT nextval('question_id_seq'),
  survey_id INT4  NOT NULL DEFAULT '0',
  name varchar(30) NOT NULL DEFAULT '',
  type_id INT4  NOT NULL DEFAULT '0',
  result_id INT4  DEFAULT NULL,
  length INT4 NOT NULL DEFAULT '0',
  precise INT4 NOT NULL DEFAULT '0',
  position INT4  NOT NULL DEFAULT '0',
  content TEXT DEFAULT '' NOT NULL,
  required varchar(1) NOT NULL DEFAULT 'N',
  deleted varchar(1) NOT NULL DEFAULT 'N',
  public varchar(1) NOT NULL DEFAULT 'Y',
  PRIMARY KEY (id),
  CHECK (survey_id>=0),
  CHECK (type_id>=0),
  CHECK (result_id>=0),
  CHECK (position>=0)

);

--
-- Table structure for table `question_choice`
--



--
-- Sequences for table QUESTION_CHOICE
--

CREATE SEQUENCE question_choice_id_seq;

CREATE TABLE question_choice (
  id INT4 DEFAULT nextval('question_choice_id_seq'),
  question_id INT4  NOT NULL DEFAULT '0',
  content TEXT DEFAULT '' NOT NULL,
  value text,
  PRIMARY KEY (id),
  CHECK (question_id>=0)

);

--
-- Table structure for table `question_type`
--



--
-- Sequences for table QUESTION_TYPE
--

CREATE SEQUENCE question_type_id_seq;

CREATE TABLE question_type (
  id INT4 DEFAULT nextval('question_type_id_seq'),
  type varchar(32) NOT NULL DEFAULT '',
  has_choices varchar(1) NOT NULL DEFAULT 'Y',
  response_table varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

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
  name varchar(16) NOT NULL DEFAULT '',
  title varchar(64) NOT NULL DEFAULT '',
  changed TIMESTAMP NOT NULL,
  PRIMARY KEY (name)

);

--
-- Dumping data for table `realm`
--

INSERT INTO realm VALUES ('superuser','ESP System Administrators','20040212 205000');
INSERT INTO realm VALUES ('auto','Self added users','20040212 205000');

--
-- Table structure for table `respondent`
--

CREATE TABLE respondent (
  username varchar(64) NOT NULL DEFAULT '',
  password varchar(64) NOT NULL DEFAULT '',
  auth varchar(16) NOT NULL DEFAULT 'BASIC',
  realm varchar(16) NOT NULL DEFAULT '',
  fname varchar(16) DEFAULT NULL,
  lname varchar(24) DEFAULT NULL,
  email varchar(64) DEFAULT NULL,
  disabled varchar(1) NOT NULL DEFAULT 'N',
  changed TIMESTAMP NOT NULL,
  expiration varchar(19) NULL,
  PRIMARY KEY (username,realm)

);

--
-- Table structure for table `response`
--



--
-- Sequences for table RESPONSE
--

CREATE SEQUENCE response_id_seq;

CREATE TABLE response (
  id INT4 DEFAULT nextval('response_id_seq'),
  survey_id INT4  NOT NULL DEFAULT '0',
  submitted TIMESTAMP NOT NULL,
  complete varchar (1) NOT NULL DEFAULT 'N',
  username varchar(64) DEFAULT NULL,
  PRIMARY KEY (id),
  CHECK (survey_id>=0)

);

--
-- Table structure for table `response_bool`
--

CREATE TABLE response_bool (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  choice_id varchar (1) NOT NULL DEFAULT 'Y',
  PRIMARY KEY (response_id,question_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0)

);

--
-- Table structure for table `response_date`
--

CREATE TABLE response_date (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  response DATE DEFAULT NULL,
  PRIMARY KEY (response_id,question_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0)

);

--
-- Sequences for table RESPONSE_MULTIPLE
--

CREATE SEQUENCE response_multiple_id_seq;

CREATE TABLE response_multiple (
  id INT4 DEFAULT nextval('response_multiple_id_seq'),
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  choice_id INT4  NOT NULL DEFAULT '0',
  PRIMARY KEY (id),
  CHECK (response_id>=0),
  CHECK (question_id>=0),
  CHECK (choice_id>=0)

);

--
-- Table structure for table `response_other`
--

CREATE TABLE response_other (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  choice_id INT4  NOT NULL DEFAULT '0',
  response text,
  PRIMARY KEY (response_id,question_id,choice_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0),
  CHECK (choice_id>=0)

);

--
-- Table structure for table `response_rank`
--

CREATE TABLE response_rank (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  choice_id INT4  NOT NULL DEFAULT '0',
  rank INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (response_id,question_id,choice_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0),
  CHECK (choice_id>=0)

);

--
-- Table structure for table `response_single`
--

CREATE TABLE response_single (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  choice_id INT4  NOT NULL DEFAULT '0',
  PRIMARY KEY (response_id,question_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0),
  CHECK (choice_id>=0)

);

--
-- Table structure for table `response_text`
--

CREATE TABLE response_text (
  response_id INT4  NOT NULL DEFAULT '0',
  question_id INT4  NOT NULL DEFAULT '0',
  response text,
  PRIMARY KEY (response_id,question_id),
  CHECK (response_id>=0),
  CHECK (question_id>=0)

);

--
-- Table structure for table `survey`
--



--
-- Sequences for table SURVEY
--

CREATE SEQUENCE survey_id_seq;

CREATE TABLE survey (
  id INT4 DEFAULT nextval('survey_id_seq'),
  name varchar(64) NOT NULL DEFAULT '',
  owner varchar(16) NOT NULL DEFAULT '',
  realm varchar(64) NOT NULL DEFAULT '',
  public varchar (1) NOT NULL DEFAULT 'Y',
  -- status BIT VARYING(4)  NOT NULL DEFAULT B'0',
  status INT4  NOT NULL DEFAULT '0',
  title varchar(255) NOT NULL DEFAULT '',
  email varchar(64) DEFAULT NULL,
  subtitle text,
  info text,
  theme varchar(64) DEFAULT NULL,
  thanks_page varchar(255) DEFAULT NULL,
  thank_head varchar(255) DEFAULT NULL,
  thank_body text,
  changed TIMESTAMP NOT NULL,
  PRIMARY KEY (id)
);

--
-- Indexes for table SURVEY
--

CREATE UNIQUE INDEX name_survey_index ON survey (name);

--
-- Sequences for table QUESTION
--

SELECT SETVAL('question_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from question));

--
-- Sequences for table SURVEY
--

SELECT SETVAL('survey_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from survey));

--
-- Sequences for table RESPONSE
--

SELECT SETVAL('response_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from response));

--
-- Sequences for table ACCESS
--

SELECT SETVAL('access_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from access));

--
-- Sequences for table RESPONSE_MULTIPLE
--

SELECT SETVAL('response_multiple_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from response_multiple));

--
-- Sequences for table QUESTION_TYPE
--

SELECT SETVAL('question_type_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from question_type));

--
-- Sequences for table QUESTION_CHOICE
--

SELECT SETVAL('question_choice_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from question_choice));

--
-- Functions for table SURVEY
--
CREATE FUNCTION plpgsql_call_handler() RETURNS language_handler
    AS '$libdir/plpgsql', 'plpgsql_call_handler'
        LANGUAGE c;

CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql HANDLER plpgsql_call_handler;

CREATE FUNCTION bin_compare(integer, integer) RETURNS boolean AS '
    DECLARE 
	status ALIAS FOR $1; 
	code ALIAS FOR $2; 
    BEGIN 
        IF (status & code) >= 1 THEN
	   RETURN true;
	ELSE IF (status & code) = 0 THEN 
	    RETURN false; 
	END IF; 
	END IF;
    END; 
' LANGUAGE 'plpgsql';

COMMIT;
