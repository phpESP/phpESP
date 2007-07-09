-- # $Id$
-- #
-- # Table update for phpESP
-- # Written by James Flemer
-- # For eGrad2000.com
-- # <jflemer@alum.rpi.edu>

-- # Use this script to update the phpESP tables from version 1.5 to
-- # version 1.6.
-- # 
-- # To execute this script via the mysql CLI, run:
-- #   mysql -u root -p phpesp < mysql_update-1.5-1.6.sql
-- # If you used a database name other than "phpesp", use it in place
-- # of "phpesp" in the command line.
-- #

-- # Expand the username fields to 64 characters.
ALTER TABLE respondent MODIFY COLUMN username CHAR(64) NOT NULL;
ALTER TABLE designer   MODIFY COLUMN username CHAR(64) NOT NULL;
ALTER TABLE response   MODIFY COLUMN username CHAR(64);
ALTER TABLE response   ADD COLUMN ip CHAR(64);

-- # Add fields for resume and navigate options.
ALTER TABLE access     ADD    COLUMN resume   ENUM('Y','N') NOT NULL DEFAULT 'N';
ALTER TABLE access     ADD    COLUMN navigate ENUM('Y','N') NOT NULL DEFAULT 'N';

-- # Create the group for Self added users.
INSERT INTO realm (name, title) VALUES ('auto', 'Self added users');
