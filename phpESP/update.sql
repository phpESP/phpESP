-- # $Id$
-- #
-- # Upgrade script to make database changes between
-- # release-1.0 and release-1.1. Please read the
-- # UPGRADE file before attempting to upgrade.
-- #
-- # Written by James Flemer
-- # <jflemer@acm.jhu.edu>
-- # <jflemer@egrad2000.com>
-- #

-- # NOTE: There was a bug in the Rank question type.
-- #       A blank answer and a N/A answer were entered
-- #       into the database the same way. This is part
-- #       of the fix.
ALTER TABLE answers_rank ALTER COLUMN rank INT NOT NULL;

DELETE FROM question_types WHERE id='7';
INSERT INTO question_types VALUES ('8','Rate (scale 1..5)',1,'answers_rank');

UPDATE result_types SET name='Ordered' WHERE id='2';
INSERT INTO result_types VALUES ('5','Average Rank');

-- # UPGRADE FROM v1.1 to v1.2beta1
ALTER TABLE answers_other ADD COLUMN choice_id INT UNSIGNED NOT NULL;
