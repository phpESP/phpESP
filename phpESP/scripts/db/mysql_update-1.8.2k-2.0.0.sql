-- # $Id$
-- #
-- # Table update for phpESP
-- # Written by Franky Van Liedekerke

-- # Use this script to update the phpESP tables from version 1.8.2k to
-- # version 1.8.2l.
-- #
-- # To execute this script via the mysql CLI, run:
-- #   mysql -u root -p phpesp < mysql_update-1.8.2k-1.8.2l.sql
-- # If you used a database name other than "phpesp", use it in place
-- # of "phpesp" in the command line.
-- #


CREATE TABLE conditions (
        id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
        survey_id       INT UNSIGNED NOT NULL,
        q1_id           INT UNSIGNED NOT NULL,
        q2_id           INT UNSIGNED NOT NULL,
        cond            INT UNSIGNED NOT NULL,
        cond_value      TEXT,
        PRIMARY KEY (id)
);

-- # To be sure, I add these again
ALTER TABLE respondent MODIFY COLUMN password CHAR(64) NOT NULL;
ALTER TABLE designer   MODIFY COLUMN password CHAR(64) NOT NULL;
ALTER TABLE response ADD COLUMN ip CHAR(64) AFTER username;
