-- Update Required Zone for Achievement Spring Fling 2419

UPDATE `achievement_criteria_requirement` SET `value1`='186' WHERE `criteria_id`='9199' AND `type`='6';
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN ('9121','9199','9200','9201','9202','9203','9204','9205') AND `type` != '6';
