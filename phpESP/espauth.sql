-- # $Id$
-- # Table structures for espauth
-- # users table 
CREATE TABLE users ( 
	username	CHAR(64) NOT NULL,
	firstname	CHAR(64) NOT NULL,
	lastname	CHAR(64) NOT NULL,
	password	CHAR(64) NOT NULL,
	realm		CHAR(64),
	acctype		INT UNSIGNED NULL,
	accstatus	INT UNSIGNED NULL,
	maxlogin	INT UNSIGNED NOT NULL,
	numlogin	INT UNSIGNED NULL,
	changed		TIMESTAMP(14) NOT NULL DEFAULT ' ',
	PRIMARY KEY (username)
);

-- # access table
CREATE TABLE access ( 
	id			INT UNSIGNED NOT NULL AUTO_INCREMENT,
	survey_id	INT UNSIGNED NOT NULL,
	realm		CHAR(64) NOT NULL,
	uselogin	ENUM('y','n') NOT NULL DEFAULT 'n',
	start_time	DATETIME,
	end_time	DATETIME,
	PRIMARY KEY (id)
);
