If you add an update script, also add it in upgrades.txt.
This file is parsed for the web based update. The format is easy enough:

#version
filename(s) (one for each line, without the database type)

The current content:
#1.8.2
#1.8.2h
update-1.8.2h-1.8.2i.sql 
#1.8.2i
#1.8.2k
update-1.8.2k-2.0.0.sql
#2.0.0
#2.0.2
update-2.0.2-2.0.3.sql

This means (if we use mysql as example for the database type):
if you are on version 1.8.2 or 1.8.2h, execute:
   mysql_update-1.8.2h-1.8.2i.sql
   mysql_update-1.8.2k-2.0.0.sql
   mysql_update-2.0.2-2.0.3.sql
if you are on version 1.8.2i or 1.8.2k, execute:
   mysql_update-1.8.2k-2.0.0.sql
   mysql_update-2.0.2-2.0.3.sql
if you are on version 2.0.0 or 2.0.2, execute:
   mysql_update-2.0.2-2.0.3.sql

