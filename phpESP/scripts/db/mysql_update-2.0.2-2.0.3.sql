-- # $Id$
-- #
-- # Table update for phpESP, v2.0.2 to v2.0.3
-- # Written by Bishop Bettini <bishop@ideacode.com>

-- # Use this script to update the phpESP tables from version 2.0.2
-- # to version 2.0.3
-- #
-- # To execute this script via the mysql CLI, run:
-- #   mysql -u root -p phpesp < mysql_update-2.0.2-2.0.3.sql
-- # If you used a database name other than "phpesp", use it in place
-- # of "phpesp" in the command line.
-- #

-- ideacode Issue 919 (Track statistics related to survey completion)
DROP TABLE IF NOT EXISTS survey_statistics;
CREATE TABLE survey_statistics (
    survey_id INT UNSIGNED NOT NULL,
    loginfail INT UNSIGNED NOT NULL DEFAULT 0,
    attempted INT UNSIGNED NOT NULL DEFAULT 0,
    abandoned INT UNSIGNED NOT NULL DEFAULT 0,
    suspended INT UNSIGNED NOT NULL DEFAULT 0,
    completed INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (survey_id)
);

-- ideacode Issue 926 (Add opening and closing dates to survey)
ALTER TABLE survey ADD COLUMN open_date DATETIME NULL AFTER status;
ALTER TABLE survey ADD COLUMN close_date DATETIME NULL AFTER open_date;
