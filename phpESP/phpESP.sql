-- # $Id$
-- #
-- # Table structures for phpESP
-- # Written by James Flemer
-- # For eGrad2000.com
-- # <jflemer@acm.jhu.edu>
-- # <jflemer@egrad2000.com>
-- #

-- # types of questions
CREATE TABLE question_types ( 
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	name		CHAR(64) NOT NULL,
	has_answers	TINYINT UNSIGNED NOT NULL,
	answer_table	CHAR(64) NOT NULL,
	PRIMARY KEY (id)
);

-- # types of results
CREATE TABLE result_types (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	name		CHAR(64) NOT NULL,
	PRIMARY KEY (id)
);

-- # table of different surveys available
CREATE TABLE surveys (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	created		TIMESTAMP(14) NOT NULL DEFAULT '',
	name		CHAR(64) NOT NULL,
	title		CHAR(255) NOT NULL,
	thanks_page	CHAR(255),
	email		CHAR(64),
	status		INT UNSIGNED NOT NULL DEFAULT '0',
	subtitle	TEXT,
	info		TEXT,
	thank_head	CHAR(255),
	thank_body	TEXT,
	PRIMARY KEY (id),
	UNIQUE(name)
);

-- # table of the questions for all the surveys
CREATE TABLE survey_questions (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	type_id		INT UNSIGNED NOT NULL,
	result_id	INT UNSIGNED,
	position	INT UNSIGNED,
	parent_id	INT UNSIGNED,
	required	CHAR(1) NOT NULL DEFAULT 'N',
	status		INT UNSIGNED NOT NULL DEFAULT '0',
	params		CHAR(64),
	content		TEXT NOT NULL,
	PRIMARY KEY (id)
);

-- # table of the choices (possible answers) of each question
CREATE TABLE question_choices (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	question_id	INT UNSIGNED NOT NULL,
	content		TEXT NOT NULL,
	value		TEXT,
	PRIMARY KEY (id)
);

-- # answer_* SUBMITTED BY USER (below this line)

-- # answers to boolean questions (yes/no)
CREATE TABLE answers_bool (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	ENUM("yes", "no") NOT NULL,
	PRIMARY KEY(response_id,question_id)
);

-- # answers to single answer questions (radio, boolean, rate) (chose one of n)
CREATE TABLE answers_single (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(response_id,question_id)
);

-- # answers to questions where multiple responses are allowed
-- # (checkbox, select multiple)
CREATE TABLE answers_multiple (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	PRIMARY KEY(id)
);

-- # answers to rank questions
CREATE TABLE answers_rank (
	response_id	INT UNSIGNED NOT NULL,
	question_id	INT UNSIGNED NOT NULL,
	choice_id	INT UNSIGNED NOT NULL,
	rank		INT NOT NULL,
	PRIMARY KEY(response_id,question_id,choice_id)
);

-- # answers to any fill in the blank or essay question
CREATE TABLE answers_text (
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	answer		TEXT,
	PRIMARY KEY (response_id,question_id)
);

-- # answers to any Other: ___ questions
CREATE TABLE answers_other (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	response_id	INT UNSIGNED NOT NULL,
	question_id INT UNSIGNED NOT NULL,
	answer		TEXT,
	PRIMARY KEY (id)
);

-- # this table holds info to distinguish one servey response from another
-- # (plus timestamp, and username if known)
CREATE TABLE survey_responses (
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	submitted	TIMESTAMP(14) NOT NULL DEFAULT '',
	complete	ENUM("yes", "no") NOT NULL DEFAULT 'no',
	username	CHAR(255),
	PRIMARY KEY (id)
);

-- # populate the types of questions
INSERT INTO question_types VALUES ('1','Yes/No',0,'answers_bool');
INSERT INTO question_types VALUES ('2','Single Line Text Entry',0,'answers_text');
INSERT INTO question_types VALUES ('3','Essay (Multiline)',0,'answers_text');
INSERT INTO question_types VALUES ('4','Radio Buttons',1,'answers_single');
INSERT INTO question_types VALUES ('5','Check Boxes',1,'answers_multiple');
INSERT INTO question_types VALUES ('6','Dropdown Box',1,'answers_single');
-- INSERT INTO question_types VALUES ('7','Rating',0,'answers_rank');
INSERT INTO question_types VALUES ('8','Rate (scale 1..5)',1,'answers_rank');
INSERT INTO question_types VALUES ('99','Page Break',0,'');


-- # populate the types of results
INSERT INTO result_types VALUES ('1','Percentages');
INSERT INTO result_types VALUES ('2','Ordered');
INSERT INTO result_types VALUES ('3','Count');
INSERT INTO result_types VALUES ('4','List');
INSERT INTO result_types VALUES ('5','Average Rank');
