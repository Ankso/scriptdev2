-- Eye of Acherus ("Death comes from on high") Script Texts

UPDATE `script_texts` SET `content_loc3` = "Das Auge von Acherus bewegt sich auf sein Ziel zu." WHERE `entry` = '-1666451';
UPDATE `script_texts` SET `content_loc3` = "Das Auge von Acherus unterliegt Eurer Kontrolle." WHERE `entry` = '-1666452';

-- Scarlet Courier

DELETE FROM `script_texts` WHERE `entry` IN ('-1609531','-1609532');
INSERT INTO `script_texts`
    (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`)
VALUES
    (-1609531, 'Hrm, what a strange tree. I must investigate.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Scarlet Courier SAY_TREE1'),
    (-1609532, 'What\'s this!? This isn\'t a tree at all! Guards! Guards!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Scarlet Courier SAY_TREE2');
