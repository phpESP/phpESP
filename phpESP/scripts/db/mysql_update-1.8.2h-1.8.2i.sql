-- # $Id$
-- #
-- # Table update for phpESP
-- # Written by James Flemer
-- # For eGrad2000.com
-- # <jflemer@alum.rpi.edu>

-- # Use this script to update the phpESP tables from version 1.8.2h to
-- # version 1.8.2i.
-- # 
-- # To execute this script via the mysql CLI, run:
-- #   mysql -u root -p phpesp < mysql_update-1.8.2h-1.8.2i.sql
-- # If you used a database name other than "phpesp", use it in place
-- # of "phpesp" in the command line.
-- #

ALTER TABLE survey     ADD    COLUMN auto_num   ENUM('Y','N') NOT NULL DEFAULT 'Y';
