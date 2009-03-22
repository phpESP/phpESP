-- # $Id: mysql_create.sql 547 2003-02-27 19:04:51Z jimmerman $
-- #
-- # Database and User creation phpESP
-- # Written by James Flemer
-- # <jflemer@alum.rpi.edu>

-- # Use this script to create the phpESP database and user. Please
-- # change the password below before running this script. This should
-- # be executed _before_ the mysql_populate.sql script. If you are
-- # upgrading an existing phpESP database, you should not use this
-- # script.
-- # 
-- # To execute this script via the mysql CLI, run:
-- #   mysql -u root -p < mysql_create.sql
-- #

-- # Create a database called 'phpesp'.
CREATE DATABASE phpesp;

-- # Switch to the mysql database.
USE mysql;

-- # Create a user called 'phpesp', with a password 'phpesp'. The
-- # 'localhost' limits this user to connecting to the database only via
-- # localhost. You may change the password by changing the
-- # PASSWORD('...') clause.
INSERT INTO user (host, user, password) VALUES ( 'localhost',
  'phpesp', PASSWORD('phpesp'));

-- # Grant the 'phpesp' user (from 'localhost') privileges to select,
-- # insert, update, and delete on the database 'phpesp'.
INSERT INTO db (host, db, user, select_priv, insert_priv,
  update_priv, delete_priv) VALUES ( 'localhost', 'phpesp',
  'phpesp', 'Y', 'Y', 'Y', 'Y');

-- # Tell mysql to re-read the privileges tables so the previous two
-- # operations take effect.
FLUSH PRIVILEGES;
