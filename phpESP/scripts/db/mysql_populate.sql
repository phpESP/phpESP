
DROP TABLE IF EXISTS realm;
CREATE TABLE realm (
	name		CHAR(16) NOT NULL,
	title		CHAR(64) NOT NULL,
	changed         TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY(name)
);

-- # table of respondents (people who enter data / take surveys)
DROP TABLE IF EXISTS respondent;
CREATE TABLE respondent (
	username	CHAR(64) NOT NULL,
	password	CHAR(64) NOT NULL,
	auth		CHAR(16) NOT NULL DEFAULT 'BASIC',
	realm		CHAR(16) NOT NULL,
	fname		CHAR(16),
	lname		CHAR(24),
	email		CHAR(64),
	disabled	ENUM('Y','N') NOT NULL DEFAULT 'N',
	changed         TIMESTAMP DEFAULT '0000-00-00 00:00:00',
	expiration	TIMESTAMP,
	PRIMARY KEY (username, realm)
);

-- # table of designers (people who create forms / surveys)
DROP TABLE IF EXISTS designer;
CREATE TABLE designer (
	username	CHAR(64) NOT NULL,
	password	CHAR(64) NOT NULL,
	auth		CHAR(16) NOT NULL DEFAULT 'BASIC',
	realm		CHAR(16) NOT NULL,
	fname		CHAR(16),
	lname		CHAR(24),
	email		CHAR(64),
	pdesign		ENUM('Y','N') NOT NULL DEFAULT 'Y',
	pstatus		ENUM('Y','N') NOT NULL DEFAULT 'N',
	pdata		ENUM('Y','N') NOT NULL DEFAULT 'N',
	pall		ENUM('Y','N') NOT NULL DEFAULT 'N',
	pgroup		ENUM('Y','N') NOT NULL DEFAULT 'N',
	puser		ENUM('Y','N') NOT NULL DEFAULT 'N',
	disabled	ENUM('Y','N') NOT NULL DEFAULT 'N',
	changed         TIMESTAMP DEFAULT '0000-00-00 00:00:00',
	expiration	TIMESTAMP,
	PRIMARY KEY(username, realm)
);

-- # create the _special_ superuser group
-- # members of this group have superuser status
INSERT INTO realm ( name, title )
	VALUES ( 'superuser', 'ESP System Administrators' ),
		( 'auto', 'Self added users' );

-- # default root account
INSERT INTO designer (username, password, fname, lname, realm, pdesign, pstatus, pdata, pall, pgroup, puser, disabled)
	VALUES ('root', PASSWORD('esp'), 'ESP', 'Superuser', 'superuser', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'N');

-- ...............................................................
-- ..................... SURVEYS/FORMS ...........................
-- ...............................................................

-- # table of different surveys available
DROP TABLE IF EXISTS survey;
CREATE TABLE survey (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	name		CHAR(64) NOT NULL,
	owner		CHAR(16) NOT NULL,
	realm		CHAR(64) NOT NULL,
	public		ENUM('Y','N') NOT NULL DEFAULT 'Y',
	status		INT UNSIGNED NOT NULL DEFAULT '0',
	title		CHAR(255) NOT NULL,
	email		CHAR(64),
	subtitle	TEXT,
	info		TEXT,
	theme		CHAR(64),
	thanks_page	CHAR(255),
	thank_head	CHAR(255),
	thank_body	TEXT,
	changed         TIMESTAMP DEFAULT '0000-00-00 00:00:00',
	auto_num	ENUM('Y','N') NOT NULL DEFAULT 'Y',
	PRIMARY KEY (id),
	UNIQUE(name)
);

-- # types of questions
DROP TABLE IF EXISTS question_type;
CREATE TABLE question_type (
	id		INT UNSIGNED NOT NULL AUTO_INCREMENT,
	type		CHAR(32) NOT NULL,
	has_choices	ENUM('Y','N') NOT NULL,
	response_table	CHAR(32) NOT NULL,
	PRIMARY KEY (id)
);

-- # table of the questions for all the surveys
DROP TABLE IF EXISTS question;
CREATE TABLE question (
	id		INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	name		CHAR(30) NOT NULL,
	type_id		INT UNSIGNED NOT NULL,
	result_id	INT UNSIGNED,
	length		INT NOT NULL DEFAULT 0,
	precise		INT NOT NULL DEFAULT 0,
	position	INT UNSIGNED NOT NULL,
	content		TEXT NOT NULL,
	required	ENUM('Y','N') NOT NULL DEFAULT 'N',
	deleted		ENUM('Y','N') NOT NULL DEFAULT 'N',
	public		ENUM('Y','N') NOT NULL DEFAULT 'Y',
	PRIMARY KEY (id),
	KEY `result_id` (`result_id`),
	KEY `survey_id` (`survey_id`)
);

-- # table of the choices (possible answers) of each question
DROP TABLE IF EXISTS question_choice;
CREATE TABLE question_choice (
	id		INT UNSIGNED NOT NULL AUTO_INCREMENT,
	question_id	INT UNSIGNED NOT NULL,
	content		TEXT NOT NULL,
	value		TEXT,
	PRIMARY KEY (id),
	KEY `question_id` (`question_id`)
);

-- # access control to adding data to a form / survey
DROP TABLE IF EXISTS access;
CREATE TABLE access (
	id		INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	realm		CHAR(16),
	maxlogin	INT UNSIGNED DEFAULT '0',
        resume		ENUM('Y','N') NOT NULL DEFAULT 'N',
        navigate	ENUM('Y','N') NOT NULL DEFAULT 'N',
	PRIMARY KEY(id),
	KEY `survey_id` (`survey_id`)
);

-- ...............................................................
-- ..................... RESPONSE DATA ...........................
-- ...............................................................

-- # this table holds info to distinguish one servey response from another
-- # (plus timestamp, and username if known)
DROP TABLE IF EXISTS response;
CREATE TABLE response (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	submitted	TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	complete	ENUM('Y','N') NOT NULL DEFAULT 'N',
	username	CHAR(64),
	ip		CHAR(64),
	PRIMARY KEY (id),
	KEY `survey_id` (`survey_id`)
);

-- # answers to boolean questions (yes/no)
DROP TABLE IF EXISTS response_bool;
CREATE TABLE response_bool (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	ENUM('Y','N') NOT NULL,
	PRIMARY KEY(response_id,question_id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`)
);

-- # answers to single answer questions (radio, boolean, rate) (chose one of n)
DROP TABLE IF EXISTS response_single;
CREATE TABLE response_single (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(response_id,question_id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`)
);

-- # answers to questions where multiple responses are allowed
-- # (checkbox, select multiple)
DROP TABLE IF EXISTS response_multiple;
CREATE TABLE response_multiple (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`),
	KEY `choice_id` (`choice_id`)
);

-- # answers to rank questions
DROP TABLE IF EXISTS response_rank;
CREATE TABLE response_rank (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	rank		INT NOT NULL,
	PRIMARY KEY(response_id,question_id,choice_id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`),
	KEY `choice_id` (`choice_id`)
);

-- # answers to any fill in the blank or essay question
DROP TABLE IF EXISTS response_text;
CREATE TABLE response_text (
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	response	TEXT,
	PRIMARY KEY (response_id,question_id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`)
);

-- # answers to any Other: ___ questions
DROP TABLE IF EXISTS response_other;
CREATE TABLE response_other (
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	response	TEXT,
	PRIMARY KEY (response_id, question_id, choice_id),
	KEY `response_id` (`response_id`),
	KEY `choice_id` (`choice_id`),
	KEY `question_id` (`question_id`)
);

-- # answers to any date questions
DROP TABLE IF EXISTS response_date;
CREATE TABLE response_date (
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	response	DATE,
	PRIMARY KEY (response_id,question_id),
	KEY `response_id` (`response_id`),
	KEY `question_id` (`question_id`)
);

-- # populate the types of questions
INSERT INTO question_type VALUES ('1','Yes/No','N','response_bool');
INSERT INTO question_type VALUES ('2','Text Box','N','response_text');
INSERT INTO question_type VALUES ('3','Essay Box','N','response_text');
INSERT INTO question_type VALUES ('4','Radio Buttons','Y','response_single');
INSERT INTO question_type VALUES ('5','Check Boxes','Y','response_multiple');
INSERT INTO question_type VALUES ('6','Dropdown Box','Y','response_single');
-- # INSERT INTO question_type VALUES ('7','Rating','N','response_rank');
INSERT INTO question_type VALUES ('8','Rate (scale 1..5)','Y','response_rank');
INSERT INTO question_type VALUES ('9','Date','N','response_date');
INSERT INTO question_type VALUES ('10','Numeric','N','response_text');
INSERT INTO question_type VALUES ('99','Page Break','N','');
INSERT INTO question_type VALUES ('100','Section Text','N','');

DROP TABLE IF EXISTS conditions;
CREATE TABLE conditions (
        id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	q1_id		INT UNSIGNED NOT NULL,
	q2_id 		INT UNSIGNED NOT NULL,
	cond		INT UNSIGNED NOT NULL,
	cond_value	TEXT,
	PRIMARY KEY (id)
);

