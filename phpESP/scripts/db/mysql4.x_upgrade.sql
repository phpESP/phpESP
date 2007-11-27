ALTER TABLE `respondent` CHANGE `password` `password` CHAR( 64 ) NOT NULL;
ALTER TABLE `designer` CHANGE `password` `password` CHAR( 64 ) NOT NULL;
ALTER TABLE `response` ADD COLUMN ip CHAR(64);
ALTER TABLE `survey` ADD COLUMN auto_num enum('Y','N') DEFAULT 'Y' NOT NULL;
