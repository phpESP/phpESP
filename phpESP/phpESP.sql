-- # $Id$
-- #
-- # Table structures for phpESP
-- # Written by James Flemer
-- # For eGrad2000.com
-- # <jflemer@acm.jhu.edu>
-- # <jflemer@egrad2000.com>
-- #

-- # types of questions
CREATE TABLE question_type (
	id				INT UNSIGNED NOT NULL AUTO_INCREMENT,
	type			CHAR(64) NOT NULL,
	has_choices		ENUM('Y','N') NOT NULL,
	response_table	CHAR(64) NOT NULL,
	PRIMARY KEY (id)
);

-- # types of results
CREATE TABLE result_type (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	type		CHAR(64) NOT NULL,
	PRIMARY KEY (id)
);

-- # table of different surveys available
CREATE TABLE survey (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	created		TIMESTAMP(14) NOT NULL DEFAULT '',
	owner		CHAR(64),
	public		ENUM('Y','N') NOT NULL DEFAULT 'N',
	status		INT UNSIGNED NOT NULL DEFAULT '0',
	name		CHAR(64) NOT NULL,
	title		CHAR(255) NOT NULL,
	email		CHAR(64),
	subtitle	TEXT,
	info		TEXT,
	theme		CHAR(64),
	thanks_page	CHAR(255),
	thank_head	CHAR(255),
	thank_body	TEXT,
	PRIMARY KEY (id),
	UNIQUE(name)
);

-- # table of users
CREATE TABLE user ( 
	username	CHAR(64) NOT NULL,
	password	CHAR(64) NOT NULL,
	firstname	CHAR(64),
	lastname	CHAR(64),
	email		CHAR(64),
	realm		CHAR(64),
	disabled	ENUM('Y','N') NOT NULL DEFAULT 'N',
	created		TIMESTAMP(14) NOT NULL DEFAULT '',
	changed		TIMESTAMP(14) NOT NULL DEFAULT '',
	expiration	TIMESTAMP(14),
	PRIMARY KEY (username)
);

-- # table of managers
CREATE TABLE manager (
	username	CHAR(64) NOT NULL,
	password	CHAR(64) NOT NULL,
	firstname	CHAR(64),
	lastname	CHAR(64),
	email		CHAR(64),
	realm		CHAR(64),
	screate		ENUM('Y','N') NOT NULL DEFAULT 'N',
	sactivate	ENUM('Y','N') NOT NULL DEFAULT 'N',
	send		ENUM('Y','N') NOT NULL DEFAULT 'N',
	sdelete		ENUM('Y','N') NOT NULL DEFAULT 'N',
	seeall		ENUM('Y','N') NOT NULL DEFAULT 'N',
	users		ENUM('Y','N') NOT NULL DEFAULT 'N',
	managers	ENUM('Y','N') NOT NULL DEFAULT 'N',
	superuser	ENUM('Y','N') NOT NULL DEFAULT 'N',
	PRIMARY KEY(username)
);

INSERT INTO manager VALUES ('root','esp','ESP','Superuser','','',
  'Y','Y','Y','Y','Y','Y','Y','Y');

-- # access table
CREATE TABLE survey_access ( 
	survey_id	INT UNSIGNED NOT NULL,
	realm		CHAR(64) NOT NULL,
	uselogin	ENUM('Y','N') NOT NULL DEFAULT 'N',
	start_time	TIMESTAMP(14),
	end_time	TIMESTAMP(14),
	PRIMARY KEY (survey_id)
);

-- # access table
CREATE TABLE survey_group ( 
	survey_id	INT UNSIGNED NOT NULL,
	sgroup		CHAR(64) NOT NULL,
	PRIMARY KEY(survey_id,sgroup)
);


-- # table of the questions for all the surveys
CREATE TABLE question (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	type_id		INT UNSIGNED NOT NULL,
	result_id	INT UNSIGNED,
	parent_id	INT UNSIGNED,
	position	INT UNSIGNED NOT NULL,
	content		TEXT NOT NULL,
	required	ENUM('Y','N') NOT NULL DEFAULT 'N',
	deleted		ENUM('Y','N') NOT NULL DEFAULT 'N',
	private		ENUM('Y','N') NOT NULL DEFAULT 'N',
	PRIMARY KEY (id)
);

-- # table of the choices (possible answers) of each question
CREATE TABLE question_choice (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	question_id	INT UNSIGNED NOT NULL,
	content		TEXT NOT NULL,
	value		TEXT,
	PRIMARY KEY (id)
);

-- # answer_* SUBMITTED BY USER (below this line)

-- # answers to boolean questions (yes/no)
CREATE TABLE response_bool (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	ENUM('Y','N') NOT NULL,
	PRIMARY KEY(response_id,question_id)
);

-- # answers to single answer questions (radio, boolean, rate) (chose one of n)
CREATE TABLE response_single (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(response_id,question_id)
);

-- # answers to questions where multiple responses are allowed
-- # (checkbox, select multiple)
CREATE TABLE response_multiple (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(id)
);

-- # answers to rank questions
CREATE TABLE response_rank (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	rank		INT NOT NULL,
	PRIMARY KEY(response_id,question_id,choice_id)
);

-- # answers to any fill in the blank or essay question
CREATE TABLE response_text (
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	response	TEXT,
	PRIMARY KEY (response_id,question_id)
);

-- # answers to any Other: ___ questions
CREATE TABLE response_other (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	response	TEXT,
	PRIMARY KEY (id)
);

-- # this table holds info to distinguish one servey response from another
-- # (plus timestamp, and username if known)
CREATE TABLE response (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	submitted	TIMESTAMP(14) NOT NULL DEFAULT '',
	complete	ENUM('Y','N') NOT NULL DEFAULT 'N',
	username	CHAR(64),
	PRIMARY KEY (id)
);

-- # populate the types of questions
INSERT INTO question_type VALUES ('1','Yes/No','N','response_bool');
INSERT INTO question_type VALUES ('2','Single Line Text Entry','N','response_text');
INSERT INTO question_type VALUES ('3','Essay (Multiline)','N','response_text');
INSERT INTO question_type VALUES ('4','Radio Buttons','Y','response_single');
INSERT INTO question_type VALUES ('5','Check Boxes','Y','response_multiple');
INSERT INTO question_type VALUES ('6','Dropdown Box','Y','response_single');
-- INSERT INTO question_type VALUES ('7','Rating','N','response_rank');
INSERT INTO question_type VALUES ('8','Rate (scale 1..5)','Y','response_rank');
INSERT INTO question_type VALUES ('99','Page Break','N','');


-- # populate the type of results
INSERT INTO result_type VALUES ('1','Percentages');
INSERT INTO result_type VALUES ('2','Ordered');
INSERT INTO result_type VALUES ('3','Count');
INSERT INTO result_type VALUES ('4','List');
INSERT INTO result_type VALUES ('5','Average Rank');
