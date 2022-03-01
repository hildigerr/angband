/* monsters.c: monster definitions

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "angband.h"


monster_race r_list[MAX_R_IDX] = {

{"Filthy street urchin"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,40,4,1,11,'p',{1,4},{72,148,0,0},0,1,'m'},

{"Filthy street urchin"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_GROUP),(NONE8),(NONE8)
			    ,0,40,4,1,11,'p',{1,4},{72,148,0,0},0,2,'m'},

{"Scrawny cat"              ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
                            ,(NONE8)
			    ,0,10,30,1,11,'f',{1,2},{49,0,0,0},0,3,'n'},

{"Scruffy little dog"       ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
                            ,(NONE8)
			    ,0,5,20,1,11,'C',{1,3},{24,0,0,0},0,3,'n'},

{"Farmer Maggot"            ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_90),(NONE8)
                            ,(MF2_UNIQUE|MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_GOOD),(NONE8),(NONE8)
			    ,0,3,40,10,11,'h',{25,15},{283,283,0,0},0,4,'m'},

{"Blubbering idiot"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,0,6,1,11,'p',{1,2},{79,0,0,0},0,1,'m'},

{"Boil-covered wretch"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,0,6,1,11,'p',{1,2},{79,0,0,0},0,1,'m'},

{"Village idiot"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,0,6,1,12,'p',{4,4},{79,0,0,0},0,1,'m'},

{"Pitiful looking beggar"   ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,40,10,1,11,'p',{1,4},{72,0,0,0},0,1,'m'},

{"Mangy looking leper"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,50,10,1,11,'p',{1,1},{72,0,0,0},0,1,'m'},

{"Squint eyed rogue"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|
			      MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,0,99,10,8,11,'p',{2,8},{5,149,0,0},0,1,'m'},

{"Singing, happy drunk"	    ,(MF1_CARRY_GOLD|MF1_HAS_60|
			      MF1_MV_ATT_NORM|MF1_MV_40|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,0,10,1,11,'p',{2,3},{72,0,0,0},0,1,'m'},

{"Aimless looking merchant" ,(MF1_CARRY_GOLD|MF1_HAS_60|
			      MF1_MV_ATT_NORM|MF1_MV_40|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,255,10,1,11,'p',{3,3},{2,0,0,0},0,1,'m'},

{"Mean looking mercenary"   ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|
			      MF1_MV_ATT_NORM|MF1_MV_40|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,0,250,10,20,11,'p',{5,8},{9,0,0,0},0,1,'m'},

{"Battle scarred veteran"   ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|
			      MF1_MV_ATT_NORM|MF1_MV_40|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(NONE8),(NONE8),(NONE8)
			    ,0,250,10,30,11,'p',{7,8},{15,0,0,0},0,1,'m'},

{"Grey mold"		   ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_IM_POIS|MF2_CHARM_SLEEP|
						   MF2_ANIMAL|MF2_MINDLESS),(NONE8)
			   ,(NONE8),3,0,2,1,11,'m',{1,2},{3,3,0,0},1,1,'n'},

{"Grey mushroom patch"	   ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_IM_POIS|MF2_CHARM_SLEEP|
						   MF2_ANIMAL|MF2_MINDLESS),(NONE8)
			   ,(NONE8),1,0,2,1,11,',',{1,2},{91,0,0,0},1,1,'n'},

{"Giant yellow centipede"   ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,2,30,8,12,11,'c',{2,6},{26,60,0,0},1,1,'n'},

{"Giant white centipede"    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),2,40,7,10,11,'c',{3,5},{25,59,0,0},1,1,'n'},

{"White icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_75),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),2,10,12,7,11,'i',{3,5},{63,0,0,0},1,1,'n'},

{"Clear icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_75|MF1_MV_INVIS),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,1,10,12,6,11,'i',{2,5},{63,0,0,0},1,1,'n'},

{"Giant white mouse"	    ,(MF1_MV_ATT_NORM|MF1_MV_40|MF1_MULTIPLY),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,1,20,8,4,11,'r',{1,3},{25,0,0,0},1,1,'n'},

{"Large brown snake"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,3,99,4,35,10,'R',{4,6},{26,73,0,0},1,1,'n'},

{"Large white snake"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,2,99,4,30,11,'R',{3,6},{24,0,0,0},1,1,'n'},

{"Small kobold"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|
			     MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,5,10,20,16,11,'k',{2,7},{4,0,0,0},1,1,'n'},

{"Kobold"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|
			     MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,5,10,20,16,11,'k',{3,7},{5,0,0,0},1,1,'n'},

{"White worm mass"	    ,(MF1_MULTIPLY|MF1_MV_75|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_POIS|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,2,10,7,1,10,'w',{4,4},{173,0,0,0},1,1,'p'},

{"Floating eye"		    ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_ANIMAL|MF2_HURT_LITE)
			    ,(NONE8),(NONE8)
			    ,1,10,2,6,11,'e',{3,6},{146,0,0,0},1,1,'n'},

{"Rock lizard"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,2,15,20,4,11,'R',{3,4},{24,0,0,0},1,1,'n'},

{"Jackal"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,1,10,10,3,11,'C',{1,4},{24,0,0,0},1,1,'n'},

{"Soldier ant"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,3,10,10,3,11,'a',{2,5},{25,0,0,0},1,1,'n'},

{"Fruit bat"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,1,10,20,3,12,'b',{1,6},{24,0,0,0},1,1,'n'},

{"Shrieker mushroom patch"  ,(MF1_MV_ONLY_ATT),(NONE8)
                            ,(MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_MINDLESS),(NONE8),(NONE8)
			    ,1,0,2,1,11,',',{1,1},{203,0,0,0},2,1,'n'},

{"Blubbering icky thing"    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|
			      MF1_PICK_UP|MF1_THRO_CREAT|MF1_MV_40|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_ANIMAL|MF2_IM_POIS),(NONE8),(NONE8)
			    ,8,10,14,4,11,'i',{5,6},{174,210,0,0},2,1,'n'},

{"Metallic green centipede" ,(MF1_MV_40|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,3,10,5,4,12,'c',{4,4},{68,0,0,0},2,1,'n'},

{"Novice warrior"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(NONE8),(NONE8),(NONE8)
			    ,6,5,20,16,11,'p',{9,4},{6,5,0,0},2,1,'m'},

{"Novice rogue"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,6,5,20,12,11,'p',{8,4},{5,148,0,0},2,1,'m'},

{"Novice priest"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0xCL|MS1_CAUSE_1|MS1_FEAR)
			    ,(NONE8),(MS2_HEAL),(NONE8)
			    ,7,10,20,10,11,'p',{7,4},{4,0,0,0},2,1,'m'},

{"Novice mage"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0xCL|MS1_CONF|MS1_ARROW_1|MS1_BLINK)
			    ,(NONE8),(NONE8),(NONE8)
			    ,7,5,20,6,11,'p',{6,4},{3,0,0,0},2,1,'m'},

{"Yellow mushroom patch"   ,(MF1_MV_ONLY_ATT),(NONE8)
                           ,(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_POIS|MF2_MINDLESS),(NONE8)
			   ,(NONE8),2,0,2,1,11,',',{1,1},{100,0,0,0},2,1,'n'},

{"White jelly"		    ,(MF1_MV_ONLY_ATT),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_POIS|MF2_MINDLESS),
                            (NONE8)
			    ,(NONE8),10,99,2,1,12,'j',{8,8},{168,0,0,0},2,1,'n'},

{"Giant green frog"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),6,30,12,8,11,'R',{2,8},{26,0,0,0},2,1,'n'},

{"Giant black ant"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),8,80,8,20,11,'a',{3,6},{27,0,0,0},2,1,'n'},

{"Salamander"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_IM_FIRE|MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,10,80,8,20,11,'R',{4,6},{105,0,0,0},2,1,'n'},

{"White harpy"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_EVIL),(NONE8)
			  ,(NONE8),5,10,16,17,11,'H',{2,5},{49,49,25,0},2,1,'f'},

{"Blue yeek"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),4,10,18,14,11,'y',{2,6},{4,0,0,0},2,1,'n'},

{"Grip, Farmer Maggot's dog" ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_UNIQUE|MF2_MAX_HP|
			      MF2_CHARM_SLEEP|MF2_ANIMAL),(NONE8),(NONE8)
			     ,30,0,30,30,12,'C',{5,5},{27,0,0,0},2,1,'n'},

{"Fang, Farmer Maggot's dog" ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_UNIQUE|MF2_MAX_HP|
			      MF2_CHARM_SLEEP|MF2_ANIMAL),(NONE8),(NONE8)
			     ,30,0,30,30,12,'C',{5,5},{27,0,0,0},2,1,'n'},

{"Green worm mass"	    ,(MF1_MULTIPLY|MF1_MV_75|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_ACID|MF2_MINDLESS),(NONE8)
			    ,(NONE8),3,10,7,3,10,'w',{6,4},{140,0,0,0},2,1,'p'},

{"Large black snake"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),9,75,5,38,10,'R',{4,8},{27,74,0,0},2,1,'n'},

{"Cave spider"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_GROUP),(NONE8)
			    ,(NONE8),7,80,8,16,12,'S',{2,6},{27,0,0,0},2,1,'n'},

{"Wild cat"                 ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8)
                            ,(NONE8),8,0,40,12,12,'f',{3,5},{51,51,0,0},2,2,'n'},

{"Smeagol"		    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|MF1_MV_75|
			      MF1_MV_ATT_NORM|MF1_MV_INVIS),(NONE8),(MF2_EVIL|MF2_UNIQUE)
			    ,(NONE8),(NONE8)
			    ,16,5,20,12,13,'h',{11,4},{3,148,0,0},3,2,'m'},

{"Green ooze"		    ,(MF1_HAS_90|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_MV_75|MF1_MV_ATT_NORM)
                            ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_ACID|MF2_IM_POIS|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,4,80,8,16,12,'j',{3,4},{140,0,0,0},3,2,'n'},

{"Poltergeist"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_HAS_90|MF1_PICK_UP|
			     MF1_MV_INVIS|MF1_THRO_WALL|MF1_MV_40|MF1_MV_75|
			     MF1_MV_ATT_NORM),(MS1_BLINK|0xFL),(MF2_CHARM_SLEEP|
			     MF2_HURT_LITE|MF2_EVIL|MF2_NO_INFRA|MF2_UNDEAD|MF2_IM_COLD|
			     MF2_IM_POIS),(NONE8),(NONE8)
			    ,8,10,8,15,13,'G',{2,5},{93,0,0,0},3,1,'n'},

{"Metallic blue centipede"  ,(MF1_MV_40|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),7,15,6,6,12,'c',{4,5},{69,0,0,0},3,1,'n'},

{"Giant white louse"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75)
			    ,(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,1,10,6,5,12,'l',{1,1},{24,0,0,0},3,1,'n'},

{"Black naga"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_MV_ATT_NORM|
			      MF1_MV_20),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,20,120,16,40,11,'n',{6,8},{75,0,0,0},3,1,'f'},

{"Spotted mushroom patch"   ,(MF1_MV_ONLY_ATT),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_POIS|MF2_MINDLESS),(NONE8)
			    ,(NONE8),3,0,2,1,11,',',{1,1},{175,0,0,0},3,1,'n'},

{"Silver jelly"		    ,(MF1_MV_ONLY_ATT),(0xFL|MS1_MANA_DRAIN),
			     (MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_POIS|MF2_MINDLESS)
                            ,(NONE8)
			  ,(NONE8),12,99,2,1,12,'j',{10,8},{213,213,0,0},3,2,'n'},

{"Yellow jelly"		    ,(MF1_MV_ONLY_ATT),(0xFL|MS1_MANA_DRAIN),
			     (MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_POIS|MF2_MINDLESS),
                             (NONE8)
			    ,(NONE8),12,99,2,1,12,'j',{10,8},{169,0,0,0},3,1,'n'},

{"Scruffy looking hobbit"   ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,4,10,16,8,11,'h',{3,5},{3,148,0,0},3,1,'m'},

{"Giant white ant"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,7,80,8,16,11,'a',{3,6},{27,0,0,0},3,1,'n'},

{"Yellow mold"		   ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_IM_POIS|MF2_CHARM_SLEEP|
			   MF2_ANIMAL|MF2_MINDLESS),(NONE8),(NONE8)
			   ,9,99,2,10,11,'m',{8,8},{3,0,0,0},3,1,'n'},

{"Metallic red centipede"   ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),12,20,8,9,12,'c',{4,8},{69,0,0,0},3,1,'n'},

{"Yellow worm mass"	    ,(MF1_MULTIPLY|MF1_MV_75|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_MINDLESS),(NONE8),(NONE8)
			    ,4,10,7,4,10,'w',{4,8},{182,0,0,0},3,2,'p'},

{"Clear worm mass"	    ,(MF1_MULTIPLY|MF1_MV_INVIS|MF1_MV_75|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_POIS|MF2_MINDLESS),(NONE8),(NONE8)
			    ,4,10,7,1,10,'w',{4,4},{173,0,0,0},3,2,'p'},

{"Radiation eye"	    ,(MF1_MV_ONLY_ATT),(0xBL|MS1_MANA_DRAIN)
			    ,(MF2_ANIMAL|MF2_HURT_LITE),(NONE8),(NONE8)
			    ,6,10,2,6,11,'e',{3,6},{88,0,0,0},3,1,'n'},

{"Cave lizard"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,8,80,8,16,11,'R',{3,6},{28,0,0,0},4,1,'n'},

{"Novice ranger"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0x9L|MS1_ARROW_1)
			    ,(NONE8),(NONE8),(NONE8)
			    ,18,5,20,6,11,'p',{6,8},{4,4,0,0},4,1,'m'},

{"Novice paladin"           ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0x9L|MS1_CAUSE_1|MS1_FEAR),(NONE8)
			    ,(NONE8),(NONE8)
			    ,20,5,20,16,11,'p',{6,8},{6,6,0,0},4,2,'m'},

{"Blue jelly"		    ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|
                             MF2_HURT_LITE|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,14,99,2,1,11,'j',{12,8},{125,0,0,0},4,1,'n'},

{"Creeping copper coins"    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,9,10,3,24,10,'$',{7,8},{3,170,0,0},4,2,'p'},

{"Giant white rat"	    ,(MF1_MULTIPLY|MF1_MV_20|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,1,30,8,7,11,'r',{2,2},{153,0,0,0},4,1,'n'},

{"Blue worm mass"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS),(NONE8)
			    ,(NONE8),5,10,7,12,10,'w',{5,8},{129,0,0,0},4,1,'p'},

{"Large grey snake"	    ,(MF1_MV_20|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),14,50,6,41,10,'R',{6,8},{28,75,0,0},4,1,'n'},

{"Bullroarer the Hobbit"    ,(MF1_CARRY_OBJ|MF1_HAS_2D2|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_UNIQUE|MF2_GOOD|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,90,10,16,8,12,'h',{8,8},{5,149,148,0},5,3,'m'},

{"Novice mage"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0xCL|MS1_CONF|MS1_ARROW_1|MS1_BLINK|
			      MS1_BLIND),(MF2_GROUP),(NONE8),(NONE8)
			    ,7,20,20,6,11,'p',{6,4},{3,0,0,0},5,1,'m'},

{"Green naga"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_PICK_UP|
			     MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_EVIL|MF2_IM_ACID),(NONE8)
			 ,(NONE8),30,120,18,40,11,'n',{9,8},{75,118,0,0},5,1,'f'},

{"Blue ooze"		    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_MV_75|MF1_MV_ATT_NORM)
                            ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_COLD|MF2_MINDLESS), (NONE8)
			    ,(NONE8),7,80,8,16,11,'j',{3,4},{129,0,0,0},3,1,'n'},

{"Green glutton ghost"	    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|
			      MF1_THRO_WALL|MF1_MV_INVIS|MF1_MV_ATT_NORM|MF1_MV_40|MF1_MV_75)
			   ,(NONE8),(MF2_CHARM_SLEEP|MF2_EVIL|MF2_NO_INFRA|MF2_UNDEAD),(NONE8)
			   ,(NONE8),15,10,10,20,13,'G',{3,4},{211,0,0,0},5,1,'n'},

{"Green jelly"		    ,(MF1_MV_ONLY_ATT),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_ACID|MF2_MINDLESS),
                             (NONE8)
			    ,(NONE8),18,99,2,1,12,'j',{22,8},{136,0,0,0},5,1,'n'},

{"Large kobold"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_IM_POIS),(NONE8),(NONE8)
			    ,25,30,20,32,11,'k',{13,9},{9,0,0,0},5,1,'n'},

{"Skeleton kobold"	    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|
			     MF2_IM_POIS|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,12,40,20,26,11,'s',{5,8},{5,0,0,0},5,1,'n'},

{"Grey icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),10,15,14,12,11,'i',{4,8},{66,0,0,0},5,1,'n'},

{"Disenchanter eye"	    ,(MF1_MV_ONLY_ATT),(MS1_MANA_DRAIN|0x9L)
			    ,(MF2_ANIMAL|MF2_HURT_LITE),(NONE8),(NONE8)
			    ,20,10,2,10,10,'e',{7,8},{207,0,0,0},5,2,'n'},

{"Red worm mass"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_IM_FIRE|MF2_MINDLESS),(NONE8),(NONE8)
			    ,6,10,7,12,10,'w',{5,8},{111,0,0,0},5,1,'p'},

{"Copperhead snake"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,15,1,6,20,11,'R',{4,6},{158,0,0,0},5,1,'n'},

{"Purple mushroom patch"    ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,15,0,2,1,11,',',{1,1},{183,183,183,0},6,2,'n'},

{"Novice priest"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0xCL|MS1_CAUSE_1|MS1_FEAR),(MF2_GROUP)
			    ,(MS2_HEAL),(NONE8)
			    ,7,5,20,10,11,'p',{7,4},{4,0,0,0},6,2,'m'},

{"Novice warrior"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(NONE8),(MF2_GROUP),(NONE8),(NONE8)
			    ,6,5,20,16,11,'p',{9,4},{6,5,0,0},6,2,'m'},

{"Novice rogue"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_GROUP),(NONE8)
			    ,(NONE8)
			    ,6,5,20,12,11,'p',{8,4},{5,148,0,0},6,2,'m'},

{"Brown mold"		    ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,20,99,2,12,11,'m',{15,8},{89,0,0,0},6,1,'n'},

{"Giant brown bat"	    ,(MF1_MV_40|MF1_MV_20|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,10,30,10,15,13,'b',{3,8},{26,0,0,0},6,1,'n'},

{"Novice archer"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_1D2)
			    ,(0x3L),(NONE8),(NONE8),(MS3_ARROW_3)
			    ,20,5,20,10,12,'p',{6,8},{3,3,0,0},6,2,'m'},

{"Creeping silver coins"    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,18,10,4,30,10,'$',{12,8},{5,171,0,0},6,2,'p'},

{"Snaga"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_GROUP|MF2_HURT_LITE),(NONE8)
			    ,(NONE8),15,30,20,32,11,'o',{8,8},{7,0,0,0},6,1,'m'},

{"Rattlesnake"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,20,1,6,24,11,'R',{6,7},{159,0,0,0},6,1,'n'},

{"Cave orc"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_GROUP|MF2_HURT_LITE),(NONE8)
			    ,(NONE8),20,30,20,32,11,'o',{11,9},{7,0,0,0},7,1,'m'},

{"Wood spider"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_GROUP|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,15,80,8,16,12,'S',{3,6},{26,165,0,0},7,3,'n'},

{"Manes"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_DEMON|MF2_EVIL|MF2_GROUP|MF2_IM_FIRE),(NONE8)
			    ,(NONE8),16,30,20,32,11,'I',{8,8},{7,0,0,0},7,2,'n'},

{"Bloodshot eye"	    ,(MF1_MV_ONLY_ATT),(0x7L|MS1_MANA_DRAIN)
			    ,(MF2_ANIMAL|MF2_HURT_LITE),(NONE8),(NONE8)
			    ,15,10,2,6,11,'e',{5,8},{143,0,0,0},7,3,'n'},

{"Red naga"		    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_MV_ATT_NORM|
			      MF1_MV_20|MF1_PICK_UP),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,40,120,20,40,11,'n',{11,8},{76,82,0,0},7,2,'f'},

{"Red jelly"		    ,(MF1_MV_ONLY_ATT),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_HURT_LITE|MF2_MINDLESS),(NONE8),
                             (NONE8),26,99,2,1,11,'j',{26,8},{87,0,0,0},7,1,'n'},

{"Giant red frog"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8),16,50,12,16,11,'R',{5,8},{83,0,0,0},7,1,'n'},

{"Green icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_IM_ACID),(NONE8)
			   ,(NONE8),18,20,14,12,11,'i',{5,8},{137,0,0,0},7,2,'n'},

{"Zombie kobold"	    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),
			     (MF2_CHARM_SLEEP|MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS|
			     MF2_IM_POIS),(NONE8),(NONE8)
			     ,14,30,20,14,11,'z',{6,8},{1,1,0,0},7,1,'n'},

{"Lost soul"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_HAS_90|
			      MF1_PICK_UP|MF1_MV_INVIS|MF1_THRO_WALL|MF1_MV_ATT_NORM|
			      MF1_MV_20|MF1_MV_40),(0xFL|MS1_TELEPORT|MS1_MANA_DRAIN)
			    ,(MF2_CHARM_SLEEP|MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,18,10,12,10,11,'G',{2,8},{11,185,0,0},7,2,'n'},

{"Dark elf"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_90|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0xAL|MS1_CONF),(MF2_EVIL|MF2_HURT_LITE)
			    ,(MS2_DARKNESS)
			    ,(NONE8),25,20,20,16,11,'h',{7,10},{5,5,0,0},7,2,'m'},

{"Night lizard"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,35,30,20,16,11,'R',{4,8},{29,29,0,0},7,2,'n'},

{"Mughash the Kobold Lord"  ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,100,20,20,20,11,'k',{12,12},{9,9,9,0},7,3,'m'},

{"Wormtongue, Agent of Saruman",
			     (MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(0x5L|MS1_BO_COLD|MS1_SLOW)
			    ,(MF2_EVIL|MF2_MAX_HP|MF2_UNIQUE|MF2_SPECIAL)
			    ,(MS2_TRAP_CREATE|MS2_HEAL|MS2_BA_POIS),(NONE8)
			    ,150,20,20,30,11,'p',{25,10},{4,4,148,0},8,1,'m'},

{"Lagduf, the Snaga"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_EVIL|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD),(NONE8)
			    ,(NONE8)
			    ,80,30,20,32,11,'o',{16,12},{9,9,8,8},8,2,'m'},

{"Brown yeek"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,11,10,18,18,11,'y',{4,8},{5,0,0,0},8,1,'n'},

{"Novice ranger"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0x9L|MS1_ARROW_1),(MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,18,5,20,6,11,'p',{6,8},{4,4,0,0},8,1,'m'},

{"Giant salamander"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(0x9L|MS1_BR_FIRE)
			    ,(MF2_ANIMAL|MF2_IM_FIRE),(NONE8)
			    ,(NONE8),50,1,6,40,11,'R',{6,7},{106,0,0,0},8,1,'n'},

{"Green mold"		    ,(MF1_MV_ONLY_ATT),(NONE8),
			     (MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_IM_ACID|MF2_IM_POIS|MF2_MINDLESS),(NONE8),(NONE8)
			    ,28,75,2,14,11,'m',{21,8},{94,0,0,0},8,2,'n'},

{"Skeleton orc"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),
			     (NONE8),(MF2_CHARM_SLEEP|MF2_ORC|MF2_UNDEAD|MF2_EVIL|MF2_IM_POIS|
			     MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS),(NONE8),(NONE8)
			    ,26,40,20,36,11,'s',{10,8},{14,0,0,0},8,1,'n'},

{"Seedy looking human"	    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,22,20,20,26,11,'p',{8,9},{17,0,0,0},8,1,'m'},

{"Lemure"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_DEMON|MF2_EVIL|MF2_GROUP|MF2_IM_FIRE)
			    ,(NONE8),(NONE8)
			    ,16,30,20,32,11,'I',{13,9},{7,0,0,0},8,3,'n'},

{"Hill orc"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_GROUP),(NONE8),(NONE8)
			    ,25,30,20,32,11,'o',{13,9},{9,0,0,0},8,1,'m'},

{"Bandit"		    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,26,10,20,24,11,'p',{8,8},{13,148,0,0},8,2,'m'},

{"Yeti"			    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_IM_COLD)
			    ,(NONE8),(NONE8)
			    ,30,10,20,24,11,'Y',{11,9},{51,51,27,0},9,3,'n'},

{"Bloodshot icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(0xBL|MS1_MANA_DRAIN)
			    ,(MF2_ANIMAL|MF2_IM_POIS),(NONE8),(NONE8)
			    ,24,20,14,18,11,'i',{7,8},{65,139,0,0},9,3,'n'},

{"Giant grey rat"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_20),(NONE8)
                            ,(MF2_IM_POIS|MF2_ANIMAL),(NONE8),(NONE8)
			    ,2,20,8,12,11,'r',{2,3},{154,0,0,0},9,1,'n'},

{"Black harpy"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_EVIL|MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,19,10,16,22,12,'H',{3,8},{50,50,26,0},9,1,'f'},

{"Orc shaman"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x8L|MS1_ARROW_1|MS1_CAUSE_1|MS1_BLINK)
			    ,(MF2_EVIL|MF2_ORC|MF2_HURT_LITE)
			    ,(NONE8),(NONE8)
			    ,30,20,20,15,11,'o',{9,8},{5,5,0,0},9,1,'n'},

{"Baby blue dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_ELEC),(MF2_IM_ELEC|MF2_EVIL|MF2_DRAGON|
			     MF2_MAX_HP),(NONE8),(NONE8)
			    ,35,70,20,30,11,'d',{10,10},{51,51,28,0},9,2,'n'},

{"Baby white dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_COLD),(MF2_IM_COLD|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,35,70,20,30,11,'d',{10,10},{51,51,28,0},9,2,'n'},

{"Baby green dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_POIS),(MF2_IM_POIS|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,35,70,20,30,11,'d',{10,10},{51,51,28,0},9,2,'n'},

{"Baby black dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_ACID),(MF2_IM_ACID|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,35,70,20,30,11,'d',{10,10},{51,51,28,0},9,2,'n'},

{"Baby red dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_FIRE),(MF2_IM_FIRE|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,35,70,20,30,11,'d',{10,11},{51,51,28,0},9,2,'n'},

{"Giant red ant"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,22,60,12,34,11,'a',{4,8},{27,85,0,0},9,2,'n'},

{"Brodda, the Easterling"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
			    ,100,20,20,25,11,'p',{30,7},{10,10,10,10},9,2,'m'},

{"King cobra"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,28,1,8,30,11,'R',{8,10},{144,161,0,0},9,2,'n'},

{"Giant spider"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_IM_POIS),(NONE8)
			    ,(NONE8)
			    ,35,80,8,16,11,'S',{10,10},{32,156,156,32},10,2,'n'},

{"Dark elven mage"	    ,(MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BLIND|MS1_ARROW_1|MS1_CONF)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_HURT_LITE)
			    ,(MS2_BA_POIS|MS2_DARKNESS),(NONE8)
			    ,50,20,20,16,12,'h',{7,10},{5,5,0,0},10,1,'m'},

{"Orfax, Son of Boldor"    ,(MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x4L|MS1_S_MONSTER|MS1_TELE_TO|MS1_BLINK|MS1_CONF|MS1_SLOW)
			    ,(MF2_INTELLIGENT|MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_GOOD|MF2_MAX_HP)
			    ,(MS2_HEAL),(NONE8)
			   ,80,10,18,20,12,'y',{12,10},{8,7,268,268},10,3,'m'},

{"Dark elven warrior"	    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_1D2|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE),(MF2_EVIL|MF2_HURT_LITE)
			    ,(NONE8),(NONE8)
			    ,50,20,20,16,11,'h',{10,11},{7,7,0,0},10,1,'m'},

{"Clear mushroom patch"	    ,(MF1_MULTIPLY|MF1_MV_ONLY_ATT|MF1_MV_INVIS)
			    ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,3,0,4,1,12,',',{1,1},{70,0,0,0},10,2,'n'},

{"Grishnakh, the Hill Orc"  ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,160,20,20,20,11,'o',{15,15},{10,9,10,9},10,3,'m'},

{"Giant white tick"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_IM_POIS),(NONE8)
			 ,(NONE8),27,20,12,40,10,'t',{12,8},{160,0,0,0},10,2,'n'},

{"Hairy mold"		   ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_MINDLESS|MF2_IM_POIS),
                          (NONE8)
			 ,(NONE8),32,70,2,15,11,'m',{15,8},{151,0,0,0},10,2,'n'},

{"Disenchanter mold"	    ,(MF1_MV_ONLY_ATT),(MS1_MANA_DRAIN|0xBL)
			    ,(MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_MINDLESS|MF2_IM_POIS),(NONE8),(NONE8)
			    ,40,70,2,20,11,'m',{16,8},{206,0,0,0},10,2,'n'},

{"Pseudo dragon"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60)
			    ,(0xBL|MS1_FEAR|MS1_CONF),(MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(MS3_BR_LITE|MS3_BR_DARK)
			    ,150,40,20,30,11,'d',{22,9},{51,51,28,0},10,2,'n'},

{"Tengu"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BLINK|MS1_TELE_TO),(MF2_DEMON|MF2_EVIL|MF2_IM_FIRE),(NONE8)
			   ,(NONE8),40,30,20,32,12,'I',{16,9},{7,0,0,0},10,1,'n'},

{"Creeping gold coins"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_90|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_ANIMAL|MF2_IM_POIS|MF2_NO_INFRA|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,32,10,5,36,10,'$',{18,8},{14,172,0,0},10,3,'p'},

{"Wolf"			  ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_GROUP),(NONE8)
			  ,(NONE8),30,20,30,30,12,'C',{6,6},{29,0,0,0},10,1,'n'},

{"Giant fruit fly"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,4,10,8,14,12,'F',{2,2},{25,0,0,0},10,6,'n'},

{"Panther"                  ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
                            ,25,0,40,30,12,'f',{10,8},{54,54,0,0},10,2,'n'},

{"Brigand"		    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,35,10,20,32,11,'p',{9,8},{13,149,0,0},10,2,'m'},

{"Baby multi-hued dragon"   ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0xBL|MS1_BR_FIRE|MS1_BR_COLD|MS1_BR_POIS|MS1_BR_ACID|
			     MS1_BR_ELEC)
			    ,(MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_POIS|MF2_IM_ACID|MF2_IM_ELEC|
			     MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,45,70,20,30,11,'d',{10,13},{51,51,28,0},11,2,'n'},

{"Hippogriff"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,30,10,12,14,11,'H',{20,9},{14,35,0,0},11,1,'n'},

{"Orc zombie"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_CHARM_SLEEP|MF2_EVIL|MF2_UNDEAD|MF2_ORC|MF2_IM_COLD|MF2_NO_INFRA|
			     MF2_IM_POIS|MF2_MINDLESS),(NONE8),(NONE8)
			    ,30,25,20,24,11,'z',{11,8},{3,3,3,0},11,1,'n'},

{"Gnome mage"		    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_MV_ATT_NORM|
			     MF1_THRO_DR)
			    ,(0x4L|MS1_BLINK|MS1_BO_COLD|MS1_S_MONSTER),(MF2_EVIL)
			    ,(MS2_DARKNESS),(NONE8)
			    ,38,10,18,20,11,'h',{7,8},{4,0,0,0},11,2,'m'},

{"Black mamba"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,40,1,10,32,12,'R',{10,8},{163,0,0,0},12,3,'n'},

{"White wolf"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_GROUP|
			      MF2_IM_COLD),(NONE8),(NONE8)
			    ,30,20,30,30,12,'C',{7,7},{26,27,0,0},12,1,'n'},

{"Grape jelly"		    ,(MF1_MV_ONLY_ATT),(MS1_MANA_DRAIN|0xBL)
			    ,(MF2_HURT_LITE|MF2_CHARM_SLEEP|MF2_IM_POIS|MF2_MINDLESS),(NONE8)
			  ,(NONE8),60,99,2,1,11,'j',{52,8},{186,0,0,0},12,3,'n'},

{"Nether worm mass"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8)
			    ,(MF2_ANIMAL|MF2_HURT_LITE|MF2_MINDLESS),(NONE8),(NONE8)
			    ,6,3,10,15,10,'w',{5,8},{186,0,0,0},12,3,'p'},

{"Golfimbul, the Hill Orc Chief"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_IM_COLD|MF2_IM_FIRE|
			     MF2_IM_ELEC|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,230,20,20,60,11,'o',{30,8},{10,10,9,9},12,3,'m'},

{"Master yeek"		    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0x4L|MS1_BLINK|MS1_TELEPORT|MS1_S_MONSTER|
			     MS1_BLIND|MS1_SLOW),(MF2_ANIMAL|MF2_EVIL),(MS2_BA_POIS),(NONE8)
			    ,28,10,18,24,11,'y',{12,9},{7,0,0,0},12,2,'n'},

{"Priest"		    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_MV_ATT_NORM|
			    MF1_THRO_DR),(0x3L|MS1_CAUSE_2|MS1_S_MONSTER|MS1_FEAR)
			    ,(MF2_EVIL|MF2_INTELLIGENT)
			    ,(MS2_HEAL),(NONE8)
			    ,36,40,20,22,11,'p',{12,8},{12,12,0,0},12,1,'m'},

{"Dark elven priest"	    ,(MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BLIND|MS1_CAUSE_2|MS1_CONF)
			    ,(MF2_EVIL|MF2_INTELLIGENT|MF2_HURT_LITE)
			    ,(MS2_HEAL|MS2_DARKNESS),(NONE8)
			    ,50,30,20,30,12,'h',{7,10},{8,9,0,0},12,1,'m'},

{"Air spirit"		    ,(MF1_THRO_DR|MF1_MV_INVIS|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8)
			    ,(MF2_EVIL|MF2_NO_INFRA|MF2_IM_POIS|MF2_MINDLESS|
			      MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,40,20,12,40,13,'E',{8,8},{2,0,0,0},12,2,'n'},

{"Skeleton human"	    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),
			     (MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS|
			     MF2_IM_POIS),(NONE8),(NONE8)
			    ,38,30,20,30,11,'s',{10,8},{7,0,0,0},12,1,'n'},

{"Zombie human"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),
			     (MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS|
			     MF2_IM_POIS),(NONE8),(NONE8)
			    ,34,20,20,24,11,'z',{12,8},{3,3,0,0},12,1,'n'},

{"Tiger"                    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
                            ,40,0,40,40,12,'f',{12,10},{54,54,29,0},12,2,'n'},

{"Moaning spirit"	    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|
			      MF1_THRO_WALL|MF1_MV_INVIS|MF1_MV_ATT_NORM|MF1_MV_20)
			    ,(0xFL|MS1_TELEPORT|MS1_FEAR)
			    ,(MF2_CHARM_SLEEP|MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,44,10,14,20,12,'G',{5,8},{99,178,0,0},12,2,'n'},

{"Swordsman"		    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(NONE8),(NONE8)
			    ,(NONE8),(NONE8)
			    ,40,20,20,34,11,'p',{12,8},{18,18,0,0},12,1,'m'},

{"Stegocentipede"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,40,30,12,30,12,'c',{13,8},{34,34,62,0},12,2,'n'},

{"Spotted jelly"	    ,(MF1_THRO_DR|MF1_MV_ONLY_ATT|MF1_PICK_UP),(NONE8)
			    ,(MF2_IM_ACID|MF2_IM_POIS|MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_NO_INFRA|
                             MF2_HURT_LITE|MF2_MINDLESS),(NONE8),(NONE8)
			    ,33,1,12,18,12,'j',{13,8},{115,138,138,0},12,3,'n'},

{"Drider"		    ,(MF1_MV_ATT_NORM),(0x8L|MS1_CAUSE_1|MS1_CONF)
			    ,(MF2_EVIL|MF2_IM_POIS),(MS2_DARKNESS),(NONE8)
			    ,55,80,8,30,11,'S',{10,13},{10,10,156,0},13,2,'n'},

{"Killer brown beetle"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,38,30,10,40,11,'K',{13,8},{41,0,0,0},13,2,'n'},

{"Boldor, King of the Yeeks",(MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_1D2|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0x3L|MS1_BLINK|MS1_TELEPORT|MS1_S_MONSTER|
			     MS1_BLIND|MS1_SLOW),(MF2_MAX_HP|
			     MF2_INTELLIGENT|MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_GOOD),(MS2_HEAL)
			  ,(NONE8),200,10,18,24,12,'y',{20,9},{8,8,7,0},13,3,'m'},

{"Ogre"			    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_GROUP|MF2_GIANT),(NONE8),(NONE8)
			   ,50,30,20,33,11,'O',{13,9},{16,0,0,0},13,2,'n'},

{"Creeping mithril coins"   ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_90|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_ANIMAL|MF2_IM_POIS|MF2_NO_INFRA),(NONE8),(NONE8)
			   ,45,10,5,50,11,'$',{20,8},{14,172,0,0},13,4,'p'},

{"Illusionist"		    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|
			      MF1_MV_ATT_NORM),(0x3L|MS1_BLINK|MS1_TELEPORT|MS1_BLIND|
			      MS1_CONF|MS1_SLOW|MS1_HOLD),(MF2_EVIL|MF2_INTELLIGENT)
			    ,(MS2_HASTE|MS2_DARKNESS),(NONE8)
			    ,50,10,20,10,11,'p',{12,8},{11,0,0,0},13,2,'m'},

{"Druid"		    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_1D2|
			     MF1_THRO_DR),(0x3L|MS1_BLINK|MS1_HOLD|MS1_BLIND|
			     MS1_SLOW|MS1_BO_FIRE),(MF2_EVIL|MF2_INTELLIGENT)
			    ,(MS2_HASTE|MS2_BO_ELEC),(NONE8)
			    ,50,10,20,10,11,'p',{12,12},{13,13,0,0},13,2,'m'},

{"Black orc"		    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_ORC|MF2_GROUP|MF2_HURT_LITE)
			    ,(NONE8),(NONE8)
			    ,45,20,20,36,11,'o',{12,10},{17,17,0,0},13,2,'m'},

{"Ochre jelly"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_PICK_UP),(NONE8)
			    ,(MF2_IM_ACID|MF2_IM_POIS|MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_NO_INFRA|
                             MF2_MINDLESS),(NONE8),(NONE8)
			    ,40,1,12,18,12,'j',{13,8},{115,138,138,0},13,3,'n'},

{"Giant flea"		    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,4,10,8,25,12,'F',{2,2},{25,0,0,0},14,1,'n'},

{"Ufthak of Cirith Ungol"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_IM_COLD|MF2_MAX_HP|
			     MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
			    ,200,20,20,50,11,'o',{40,8},{17,17,17,17},14,3,'m'},

{"Giant white dragon fly"   ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_COLD|0xAL)
			    ,(MF2_ANIMAL|MF2_IM_COLD),(NONE8),(NONE8)
			    ,60,50,20,20,11,'F',{5,8},{122,0,0,0},14,3,'n'},

{"Blue icky thing"	    ,(MF1_MV_ATT_NORM|MF1_MV_40|MF1_THRO_DR|MF1_MULTIPLY)
			    ,(0x8L|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_ANIMAL|MF2_IM_POIS|MF2_EVIL),(NONE8),(NONE8)
			    ,20,20,15,20,10,'i',{10,6},{174,210,3,3},14,4,'n'},

{"Hill giant"		    ,(MF1_MV_ATT_NORM|MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_GIANT)
			    ,(NONE8),(NONE8)
			    ,60,50,20,45,11,'P',{16,10},{19,19,0,0},14,1,'n'},

{"Flesh golem"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_IM_ELEC|MF2_CHARM_SLEEP|
                             MF2_MINDLESS),(NONE8),(NONE8)
			    ,50,10,12,30,11,'g',{12,8},{5,5,0,0},14,1,'n'},

{"Warg"			    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_EVIL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,40,40,20,20,12,'C',{8,8},{31,0,0,0},14,2,'n'},

{"Giant black louse"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),(NONE8)
			    ,(NONE8),(NONE8)
			    ,3,10,6,7,12,'l',{1,2},{25,0,0,0},14,1,'n'},

{"Lurker"                   ,(MF1_MV_ONLY_ATT|MF1_MV_INVIS),(NONE8)
                            ,(MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,80,10,30,25,11,'.',{20,10},{7,7,0,0},14,3,'n'},

{"Wererat"		    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_60|MF1_THRO_DR)
			    ,(0x9L|MS1_CAUSE_2|MS1_BLINK|MS1_BO_COLD)
			    ,(MF2_EVIL|MF2_ANIMAL),(MS2_BA_POIS),(NONE8)
			    ,45,10,10,10,11,'r',{20,8},{54,54,36,0},15,2,'n'},

{"Black ogre"		    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR
			      |MF1_MV_ATT_NORM|MF1_MV_20)
			    ,(NONE8),(MF2_EVIL|MF2_GROUP|MF2_GIANT),(NONE8),(NONE8)
			    ,75,30,20,33,11,'O',{20,9},{16,16,0,0},15,2,'n'},

{"Magic mushroom patch"	    ,(MF1_MV_ONLY_ATT),(0x1L|MS1_BLINK|MS1_FEAR|MS1_SLOW)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP|MF2_MINDLESS),(MS2_DARKNESS),(NONE8)
			    ,10,0,40,10,13,',',{1,1},{0,0,0,0},15,2,'n'},

{"Guardian naga"	    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|
			     MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_EVIL),(NONE8)
			     ,(NONE8)
			    ,80,120,20,65,11,'n',{24,11},{77,31,31,0},15,2,'f'},

{"Light hound"		   ,(MF1_MV_ATT_NORM),(0x5L),(MF2_ANIMAL|MF2_GROUP),(NONE8)
			    ,(MS3_BR_LITE)
			    ,50,0,30,30,11,'Z',{6,6},{29,0,0,0},15,1,'n'},

{"Dark hound"		  ,(MF1_MV_ATT_NORM),(0x5L),(MF2_ANIMAL|MF2_GROUP),(NONE8)
			    ,(MS3_BR_DARK)
			    ,50,0,30,30,11,'Z',{6,6},{29,0,0,0},15,1,'n'},

{"Half-orc"		    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_ORC|MF2_GROUP),(NONE8),(NONE8)
			    ,50,20,20,40,11,'o',{16,10},{17,17,0,0},15,3,'m'},

{"Giant tarantula"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,70,80,8,32,12,'S',{10,15},{156,156,156,0},15,3,'n'},

{"Giant clear centipede"    ,(MF1_MV_INVIS|MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,30,30,12,30,11,'c',{5,8},{34,62,0,0},15,2,'n'},

{"Mirkwood spider"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_GROUP|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,25,80,15,25,12,'S',{9,8},{31,156,156,0},15,2,'n'},

{"Frost giant"		    ,(MF1_MV_ATT_NORM|MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_IM_COLD|MF2_GIANT),(NONE8),(NONE8)
			    ,75,50,20,50,11,'P',{17,10},{120,16,0,0},15,1,'n'},

{"Griffon"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,70,10,12,15,11,'H',{30,8},{17,36,0,0},15,1,'n'},

{"Homonculous"		    ,(MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_DEMON|MF2_EVIL|MF2_IM_FIRE),(NONE8),(NONE8)
			    ,40,30,20,32,11,'I',{8,8},{145,9,0,0},15,3,'n'},

{"Gnome mage"		    ,(MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_60|MF1_MV_ATT_NORM|MF1_THRO_DR)
			    ,(0x4L|MS1_BLINK|MS1_BO_COLD|MS1_S_MONSTER),(MF2_EVIL|MF2_GROUP)
			    ,(MS2_DARKNESS),(NONE8)
			    ,40,20,20,20,11,'h',{7,8},{4,0,0,0},15,2,'m'},

{"Ethereal hound"	    ,(MF1_MV_ATT_NORM|MF1_MV_INVIS),(NONE8),(MF2_ANIMAL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,50,0,30,30,11,'Z',{10,6},{29,29,29,0},15,2,'n'},

{"Clay golem"		    ,(MF1_MV_ATT_NORM),(NONE8),
			     (MF2_HURT_ROCK|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_COLD|MF2_MINDLESS|
			      MF2_IM_POIS|MF2_NO_INFRA|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,50,10,12,30,11,'g',{14,8},{7,7,0,0},15,2,'n'},

{"Umber hulk"		    ,(MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_EVIL|MF2_ANIMAL|MF2_BREAK_WALL|MF2_HURT_ROCK|MF2_IM_POIS|
			     MF2_NO_INFRA),(NONE8),(NONE8)
			    ,75,10,20,50,11,'U',{20,10},{92,5,5,36},16,1,'n'},

{"Orc captain"		    ,(MF1_HAS_90|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_ORC),(NONE8),(NONE8)
			    ,40,20,20,59,11,'o',{20,10},{17,17,17,0},16,3,'m'},

{"Gelatinous cube"	    ,(MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_HAS_4D2|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90),(NONE8)
			    ,(MF2_IM_ACID|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|MF2_IM_COLD|
			      MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_NO_INFRA|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,80,1,12,18,11,'j',{45,8},{115,115,115,0},16,4,'n'},

{"Giant green dragon fly"   ,(MF1_MV_ATT_NORM|MF1_MV_75),(MS1_BR_POIS|0xAL),
			     (MF2_IM_POIS|MF2_ANIMAL),(NONE8),(NONE8)
			    ,70,50,12,20,11,'F',{3,8},{156,0,0,0},16,2,'n'},

{"Fire giant"		    ,(MF1_MV_ATT_NORM|MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_IM_FIRE|MF2_GIANT),(NONE8),(NONE8)
			    ,54,50,20,60,11,'P',{20,8},{102,102,0,0},16,2,'n'},

{"Hummerhorn"		    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_75),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,4,10,8,14,12,'F',{2,2},{234,0,0,0},16,5,'n'},

{"Ulfast, Son of Ulfang"     ,(MF1_HAS_90|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_UNIQUE|MF2_GOOD|MF2_MAX_HP)
			     ,(NONE8),(NONE8)
			     ,200,40,20,40,11,'p',{20,17},{18,18,18,18},16,3,'m'},

{"Quasit"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_MV_INVIS|
			      MF1_CARRY_OBJ|MF1_HAS_1D2),(0xAL|MS1_BLINK|MS1_TELE_TO|MS1_TELEPORT|MS1_FEAR
			     |MS1_CONF|MS1_BLIND)
			    ,(MF2_INTELLIGENT|MF2_DEMON|MF2_IM_FIRE|MF2_EVIL)
			    ,(MS2_TELE_LEVEL),(NONE8)
			    ,50,20,20,30,11,'I',{6,8},{176,51,51,0},16,2,'n'},

{"Imp"			     ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_MV_INVIS|
			      MF1_CARRY_OBJ|MF1_HAS_1D2),(0xAL|MS1_BLINK|MS1_TELE_TO|MS1_TELEPORT|MS1_FEAR
			     |MS1_CONF|MS1_BLIND),
			     (MF2_DEMON|MF2_IM_FIRE|MF2_EVIL|MF2_NO_INFRA|MF2_INTELLIGENT)
			    ,(MS2_TELE_LEVEL),(NONE8)
			    ,55,20,20,30,11,'I',{6,8},{152,152,0,0},17,2,'n'},

{"Forest troll"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|MF1_CARRY_GOLD|
			     MF1_CARRY_OBJ),(NONE8),(MF2_TROLL|MF2_EVIL|MF2_HURT_LITE|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,70,40,20,50,11,'T',{20,10},{3,3,29,0},17,1,'m'},

{"Nar, the Dwarf"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_HAS_1D2),(0x6L|MS1_CAUSE_2|MS1_BLIND|MS1_CONF)
			    ,(MF2_CHARM_SLEEP|MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_GOOD|
			     MF2_MAX_HP|MF2_UNIQUE),(MS2_MIND_BLAST|MS2_HEAL),(NONE8)
			    ,250,25,25,70,11,'h',{45,10},{18,18,18,18},17,2,'m'},

{"2-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_1D2)
			    ,(0xBL|MS1_FEAR),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,80,20,20,60,11,'R',{100,3},{36,36,0,0},17,2,'n'},

{"Water spirit"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),
			     (MF2_EVIL|MF2_IM_POIS|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,58,40,12,28,12,'E',{9,8},{13,13,0,0},17,1,'n'},

{"Giant brown scorpion"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,62,20,12,44,11,'S',{11,8},{34,86,0,0},17,1,'n'},

{"Earth spirit"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_WALL|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_EVIL|MF2_HURT_ROCK|MF2_IM_POIS|MF2_NO_INFRA|
			     MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,64,50,10,40,12,'E',{13,8},{7,7,0,0},17,2,'n'},

{"Fire spirit"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),
			     (NONE8),(MF2_EVIL|MF2_IM_POIS|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,75,20,16,30,12,'E',{10,9},{101,101,0,0},18,2,'n'},

{"Fire hound"		  ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_FIRE)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_FIRE),(NONE8)
			    ,(NONE8)
			    ,70,0,30,30,11,'Z',{10,6},{105,105,105,0},18,1,'n'},

{"Cold hound"		  ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_COLD)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_COLD),(NONE8)
			    ,(NONE8)
			    ,70,0,30,30,11,'Z',{10,6},{122,54,29,0},18,1,'n'},

{"Energy hound"		   ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_ELEC)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_ELEC),(NONE8)
			    ,(NONE8)
			    ,70,0,30,30,11,'Z',{10,6},{131,131,131,0},18,1,'n'},

{"Mimic"                    ,(MF1_MV_ONLY_ATT),(0x6L|MS1_BO_COLD|MS1_BLIND|MS1_FEAR|
			     MS1_CONF|MS1_CAUSE_2)
                            ,(MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,60,0,25,30,11,'!',{10,10},{152,12,12,0},18,3,'n'},

{"Blink dog"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(0x4L|MS1_BLINK|MS1_TELE_TO)
			    ,(MF2_ANIMAL|MF2_GROUP),(NONE8),(NONE8)
			    ,50,10,20,20,12,'C',{8,8},{31,0,0,0},18,2,'n'},

{"Uruk-Hai"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,68,20,20,50,11,'o',{10,8},{18,18,0,0},18,1,'m'},

{"Shagrat, the Orc Captain", (MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			     ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			     ,(NONE8),(NONE8)
			     ,400,20,20,60,11,'o',{40,10},{20,20,18,18},18,2,'m'},

{"Gorbag, the Orc Captain", (MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			     ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			     ,(NONE8),(NONE8)
			     ,400,20,20,60,11,'o',{40,10},{20,20,18,18},18,3,'m'},

{"Shambling mound"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_90)
			    ,(NONE8),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_EVIL|MF2_MINDLESS),(NONE8)
                            ,(NONE8)
			    ,75,40,20,16,11,',',{20,6},{203,7,7,0},18,2,'n'},

{"Stone giant"		    ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_CARRY_GOLD|
			     MF1_CARRY_OBJ|MF1_HAS_60),(NONE8),(MF2_EVIL|MF2_GIANT)
			    ,(NONE8),(NONE8)
			    ,90,50,20,75,11,'P',{24,8},{20,20,0,0},18,1,'n'},

{"Giant black dragon fly"   ,(MF1_MV_ATT_NORM|MF1_MV_75),(MS1_BR_ACID|0x9L),
			     (MF2_IM_ACID|MF2_ANIMAL),(NONE8),(NONE8)
			    ,68,50,12,20,12,'F',{3,8},{0,0,0,0},18,2,'n'},

{"Stone golem"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_HURT_ROCK|MF2_IM_FIRE|
			     MF2_IM_COLD|MF2_IM_ELEC|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_MINDLESS|
			     MF2_NO_INFRA),(NONE8),(NONE8)
			    ,100,10,12,75,10,'g',{28,8},{9,9,0,0},19,2,'n'},

{"Red mold"		    ,(MF1_MV_ONLY_ATT),(NONE8)
                            ,(MF2_IM_FIRE|MF2_IM_POIS|MF2_ANIMAL|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,64,70,2,16,11,'m',{17,8},{108,0,0,0},19,1,'n'},

{"Giant gold dragon fly"    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x9L),
			     (MF2_IM_FIRE|MF2_ANIMAL),(MS2_BR_SOUN),(NONE8)
			    ,78,50,12,20,12,'F',{3,8},{26,0,0,0},18,2,'n'},

{"Bolg, Son of Azog"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,800,20,20,50,12,'o',{50,10},{19,19,19,19},20,4,'m'},

{"Phase spider"		    ,(MF1_MV_ATT_NORM),(0x5L|MS1_BLINK|MS1_TELE_TO)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_POIS),(NONE8),(NONE8)
			    ,60,80,15,25,12,'S',{6,8},{31,156,156,0},20,2,'n'},

{"3-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x9L|MS1_FEAR),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,350,20,20,65,12,'R',{100,5},{36,36,36,0},20,2,'n'},

{"Earth hound"		    ,(MF1_MV_ATT_NORM),(0xAL)
			    ,(MF2_ANIMAL|MF2_GROUP),(MS2_BR_SHAR)
			    ,(NONE8)
			    ,200,0,30,30,11,'Z',{15,8},{31,31,58,58},20,1,'n'},

{"Air hound"		    ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_POIS)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_POIS),(NONE8)
			    ,(NONE8)
			    ,200,0,30,30,11,'Z',{15,8},{31,31,58,58},20,1,'n'},

{"Sabre-tooth tiger"        ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
                            ,120,0,40,50,12,'f',{20,14},{56,56,32,32},20,2,'n'},

{"Water hound"		    ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_ACID)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_IM_ACID),(NONE8)
			    ,(NONE8)
			    ,200,0,30,30,11,'Z',{15,8},{113,113,58,58},20,2,'n'},

{"Chimera"		    ,(MF1_MV_ATT_NORM),(0xAL|MS1_BR_FIRE),(MF2_IM_FIRE)
			    ,(NONE8),(NONE8)
			    ,200,10,12,15,11,'H',{13,8},{32,105,105,0},20,1,'n'},

{"Quylthulg"		    ,(MF1_MV_INVIS),(0x4L|MS1_BLINK|MS1_S_MONSTER),(MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,250,0,10,1,11,'Q',{6,8},{0,0,0,0},20,1,'n'},

{"Sasquatch"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(NONE8),(MF2_ANIMAL|MF2_IM_COLD)
			    ,(NONE8),(NONE8)
			    ,180,10,15,40,12,'Y',{20,19},{56,56,37,0},20,3,'n'},

{"Werewolf"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_PICK_UP|MF1_THRO_DR)
			    ,(NONE8),(MF2_ANIMAL|MF2_EVIL),(NONE8),(NONE8)
			    ,150,70,15,30,11,'C',{20,22},{29,29,32,0},20,1,'n'},

{"Dark elven lord"	    ,(MF1_CARRY_OBJ|MF1_HAS_2D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BLIND|MS1_BO_COLD|MS1_BO_FIRE|MS1_CONF)
			    ,(MF2_EVIL|MF2_HURT_LITE),(MS2_HASTE|MS2_DARKNESS),(NONE8)
			    ,500,30,20,40,12,'h',{18,15},{20,18,0,0},20,2,'m'},

{"Cloud giant"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_90)
			    ,(NONE8),(MF2_EVIL|MF2_GIANT|MF2_IM_ELEC),(NONE8),(NONE8)
			    ,125,50,20,60,11,'P',{24,10},{130,130,0,0},20,1,'n'},

{"Ugluk, the Uruk-Hai"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			     ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			     ,(NONE8),(NONE8)
			    ,550,20,20,90,11,'o',{40,16},{18,18,18,18},20,4,'m'},

{"Lugdush, the Uruk-Hai"    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_IM_COLD|MF2_IM_FIRE|
			     MF2_MAX_HP|MF2_GOOD|MF2_UNIQUE|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,550,20,20,95,11,'o',{40,18},{20,20,18,18},21,3,'m'},

{"Blue dragon bat"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_ELEC|0x4L)
			    ,(MF2_ANIMAL|MF2_IM_ELEC),(NONE8),(NONE8)
			    ,54,50,12,26,13,'b',{4,4},{131,0,0,0},21,1,'n'},

{"Mimic"                    ,(MF1_MV_ONLY_ATT),(0x5L|MS1_BO_FIRE|MS1_CAUSE_2|MS1_FEAR|
			     MS1_BLIND|MS1_CONF|MS1_S_MONSTER),(MF2_NO_INFRA|MF2_MINDLESS|
			     MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,60,0,30,40,11,'?',{10,14},{152,132,12,12},21,3,'n'},

{"Fire vortex"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_FIRE|0x6L)
			    ,(MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,100,0,100,30,11,'v',{9,9},{239,0,0,0},21,1,'n'},

{"Water vortex"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_ACID|0x6L)
			    ,(MF2_IM_ACID|MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,100,0,100,30,11,'v',{9,9},{240,0,0,0},21,1,'n'},

{"Cold vortex"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_COLD|0x6L)
			    ,(MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,100,0,100,30,11,'v',{9,9},{241,0,0,0},21,1,'n'},

{"Energy vortex"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_ELEC|0x6L)
			    ,(MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,130,0,100,30,11,'v',{12,12},{242,0,0,0},21,1,'n'},

{"Mummified orc"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR)
			    ,(NONE8),(MF2_EVIL|MF2_ORC|MF2_UNDEAD|MF2_IM_COLD|MF2_NO_INFRA|MF2_MINDLESS|
			     MF2_IM_POIS|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,56,75,20,28,11,'M',{15,8},{13,13,0,0},21,1,'n'},

{"Killer stag beetle"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,80,30,12,55,11,'K',{20,8},{41,10,0,0},22,1,'n'},

{"Iron golem"		    ,(MF1_MV_ATT_NORM),(MS1_SLOW|0x7L)
			    ,(MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_IM_POIS|MF2_MINDLESS|
			      MF2_NO_INFRA|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,160,10,12,80,11,'g',{80,12},{10,0,0,0},22,2,'n'},

{"Giant yellow scorpion"    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8)
			    ,(NONE8)
			    ,60,20,12,38,11,'S',{12,8},{31,167,0,0},22,1,'n'},

{"Black ooze"		    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_PICK_UP|MF1_MULTIPLY|
			      MF1_THRO_DR|MF1_THRO_CREAT|MF1_MV_ATT_NORM|MF1_MV_40)
			    ,(0xBL|MS1_MANA_DRAIN),(MF2_IM_POIS|MF2_ANIMAL|MF2_MINDLESS),(NONE8)
			    ,(NONE8)
			    ,7,1,10,6,9,'j',{6,8},{138,0,0,0},23,1,'n'},

{"Hardened warrior"	    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,60,40,20,40,11,'p',{15,11},{18,18,0,0},23,1,'m'},

{"Azog, King of the Uruk-Hai",(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
			   ,(NONE8),(MF2_ORC|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			   ,(NONE8),(NONE8)
			   ,1111,20,20,80,12,'o',{60,15},{23,23,23,0},23,5,'m'},

{"Master rogue"		    ,(MF1_HAS_2D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ
			     |MF1_THRO_DR|MF1_PICK_UP|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_EVIL),(NONE8),(NONE8)
			   ,110,40,20,30,12,'p',{15,9},{16,16,231,0},23,2,'m'},

{"Red dragon bat"	    ,(MF1_MV_ATT_NORM|MF1_MV_40),(MS1_BR_FIRE|0x4L)
			    ,(MF2_IM_FIRE|MF2_ANIMAL),(NONE8),(NONE8)
			    ,60,50,12,28,13,'b',{3,8},{105,0,0,0},23,1,'n'},

{"Killer blue beetle"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL),(NONE8)
			  ,(NONE8),85,30,14,55,11,'K',{20,8},{44,0,0,0},23,1,'n'},

{"Giant bronze dragon fly"  ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x9L),
			     (MF2_CHARM_SLEEP|MF2_ANIMAL),(MS2_BR_CONF),(NONE8)
			    ,70,50,12,20,12,'F',{3,8},{0,0,0,0},18,1,'n'},

{"Forest wight"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_CARRY_GOLD|
			      MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90)
			    ,(0xAL|MS1_FEAR|MS1_MANA_DRAIN)
			    ,(MF2_EVIL|MF2_NO_INFRA|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|
			      MF2_HURT_LITE|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,140,30,20,30,11,'W',{12,8},{5,5,187,0},24,1,'n'},

{"Ibun, Son of Mim"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(0x8L|MS1_SLOW|MS1_BO_FIRE),(MF2_UNIQUE|MF2_IM_COLD|MF2_IM_FIRE|
			     MF2_GOOD|MF2_CHARM_SLEEP|MF2_MAX_HP),(MS2_HEAL),(NONE8)
			    ,300,10,20,80,11,'h',{55,15},{19,19,19,204},24,2,'m'},

{"Khim, Son of Mim"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(0x8L|MS1_SLOW|MS1_BO_FIRE),(MF2_UNIQUE|MF2_IM_COLD|MF2_IM_FIRE|
			     MF2_GOOD|MF2_CHARM_SLEEP|MF2_MAX_HP),(MS2_HEAL),(NONE8)
			    ,300,10,20,80,11,'h',{55,15},{19,19,19,204},24,2,'m'},

{"4-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_4D2)
			    ,(0x7L|MS1_FEAR),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,350,20,20,70,12,'R',{100,6},{36,36,36,36},24,2,'n'},

{"Mummified human"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_90)
			    ,(NONE8),(MF2_EVIL|MF2_NO_INFRA|MF2_UNDEAD|MF2_IM_COLD|MF2_MINDLESS|
			     MF2_IM_POIS|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,70,60,20,34,11,'M',{17,9},{13,13,0,0},24,1,'n'},

{"Vampire bat"		    ,(MF1_MV_ATT_NORM|MF1_MV_40),(NONE8)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|
			     MF2_CHARM_SLEEP|MF2_NO_INFRA),(NONE8),(NONE8)
			    ,150,50,12,40,12,'b',{9,10},{236,236,0,0},24,2,'n'},

{"Sangahyando of Umbar"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_HAS_90|MF1_THRO_DR)
			    ,(0x4L|MS1_SLOW),(MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_UNIQUE|
			     MF2_IM_ELEC|MF2_GOOD|MF2_MAX_HP),(MS2_FORGET),(NONE8)
			,400,25,25,80,11,'p',{80,10},{22,22,22,22},24,2,'m'},

{"Angamaite of Umbar"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_HAS_90|MF1_THRO_DR)
			    ,(0x4L|MS1_SLOW),(MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|
			     MF2_GOOD|MF2_MAX_HP|MF2_UNIQUE),(MS2_FORGET),(NONE8)
			,400,25,25,80,11,'p',{80,10},{22,22,22,22},24,2,'m'},

{"Banshee"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_MV_40|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_HAS_1D2|MF1_THRO_WALL|MF1_MV_INVIS|MF1_PICK_UP)
			    ,(0xFL|MS1_TELEPORT|MS1_MANA_DRAIN)
			    ,(MF2_NO_INFRA|MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|
			     MF2_CHARM_SLEEP|MF2_IM_POIS),(NONE8),(NONE8)
			    ,60,10,20,24,12,'G',{6,8},{99,188,0,0},24,2,'f'},

{"Pukelman"		    ,(MF1_MV_ATT_NORM),(MS1_BO_ACID|MS1_SLOW|MS1_CONF|0x4L)
			    ,(MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_IM_POIS|MF2_MINDLESS|
			      MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_HURT_ROCK),(NONE8),(NONE8)
			    ,600,10,12,80,11,'g',{80,12},{10,19,0,0},25,3,'n'},

{"Dark elven druid"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR)
			    ,(0x6L|MS1_S_MONSTER|MS1_CONF),(MF2_IM_POIS|
			     MF2_CHARM_SLEEP|MF2_EVIL|MF2_HURT_LITE)
			    ,(MS2_HEAL|MS2_S_SPIDER|MS2_DARKNESS),(NONE8)
			    ,500,10,15,75,12,'h',{20,20},{6,6,20,0},25,3,'m'},

{"Stone troll"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|MF1_CARRY_GOLD|
			     MF1_CARRY_OBJ),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_HURT_LITE|MF2_HURT_ROCK|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,85,50,20,40,11,'T',{23,10},{5,5,41,0},25,1,'m'},

{"Troll priest"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_HAS_90),(0x5L|MS1_CAUSE_1|MS1_BLINK|MS1_FEAR|MS1_ARROW_1)
			    ,(MF2_TROLL|MF2_EVIL|MF2_HURT_LITE|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_DARKNESS),(NONE8)
			    ,100,30,20,50,11,'T',{23,13},{7,7,41,0},25,1,'m'},

{"Wereworm"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_IM_ACID|MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,300,20,15,70,11,'w',{100,11},{32,139,224,156},
								    25,3,'n'},

{"Carrion crawler"          ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_IM_POIS)
                            ,(NONE8),(NONE8)
                            ,60,10,15,40,11,'c',{20,12},{253,253,0,0},25,2,'n'},

{"Killer red beetle"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,85,30,14,50,11,'K',{20,8},{84,0,0,0},25,2,'n'},

{"Giant grey ant lion"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT),(NONE8),(MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,90,40,10,40,11,'a',{19,8},{39,0,0,0},26,1,'n'},

{"Ulwarth, Son of Ulfang"      ,(MF1_HAS_90|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,500,40,20,40,11,'p',{80,11},{22,22,22,0},26,4,'m'},

{"Displacer beast"          ,(MF1_MV_ATT_NORM|MF1_MV_INVIS),(NONE8),(MF2_ANIMAL)
                            ,(NONE8),(NONE8)
			    ,100,20,35,100,11,'f',{25,10},{37,9,9,9},26,2,'n'},

{"Giant fire tick"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_IM_FIRE)
			    ,(NONE8),(NONE8)
			    ,90,20,14,54,11,'t',{16,8},{109,0,0,0},26,1,'n'},

{"Cave ogre"		    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR
			      |MF1_MV_ATT_NORM)
			    ,(NONE8),(MF2_EVIL|MF2_GROUP|MF2_GIANT),(NONE8),(NONE8)
			    ,42,30,20,33,11,'O',{30,9},{20,20,0,0},26,1,'n'},

{"White wraith"		    ,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|
			     MF1_MV_ATT_NORM),(0x8L|MS1_FEAR|MS1_CAUSE_2)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_NO_INFRA|MF2_IM_COLD|MF2_IM_POIS|
			      MF2_CHARM_SLEEP|MF2_HURT_LITE),(MS2_DARKNESS),(NONE8)
			    ,175,10,20,40,11,'W',{15,8},{5,5,189,0},26,1,'n'},

{"Monadic Deva"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_2D2)
			    ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_IM_POIS|MF2_IM_ACID|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_FORGET),(NONE8)
			    ,220,255,30,60,11,'A',{25,12}
			    ,{17,17,17,17},26,6,'n'},

{"Mim, Betrayer of Turin"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
			    ,(0x6L|MS1_FEAR|MS1_BO_ACID),(MF2_EVIL|MF2_MAX_HP|MF2_IM_COLD|
			     MF2_IM_FIRE|MF2_IM_POIS|MF2_IM_ACID|MF2_IM_ELEC|MF2_UNIQUE|
			     MF2_GOOD|MF2_CHARM_SLEEP),(MS2_BA_ACID|MS2_HEAL),(NONE8)
			    ,1000,20,20,80,12,'h',{100,11},{20,20,20,204},27,4,'m'},

{"Killer fire beetle"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_IM_FIRE|MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,95,30,14,45,11,'K',{13,8},{41,110,0,0},27,1,'n'},

{"Creeping adamantite coins",(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_90|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_ANIMAL|MF2_IM_POIS|MF2_NO_INFRA),(NONE8),(NONE8)
			   ,45,10,5,50,12,'$',{20,25},{161,172,10,10},27,4,'n'},

{"Algroth"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|MF1_CARRY_GOLD|
			     MF1_CARRY_OBJ),(NONE8),(MF2_TROLL|MF2_EVIL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,150,40,20,60,11,'T',{21,12},{238,238,29,0},27,1,'n'},

{"Vibration hound"	      ,(MF1_MV_ATT_NORM),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(MS2_BR_SOUN)
			    ,(NONE8)
			    ,250,0,30,30,11,'Z',{25,10},{36,36,58,58},27,3,'n'},

{"Nexus hound"		  ,(MF1_MV_ATT_NORM),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(MS2_BR_NETH)
			    ,(NONE8)
			    ,250,0,30,30,11,'Z',{25,10},{37,37,58,58},27,3,'n'},

{"Ogre mage"		,(MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x4L|MS1_HOLD|MS1_BA_COLD|MS1_S_MONSTER)
			    ,(MF2_EVIL|MF2_GIANT),(MS2_HEAL|MS2_TRAP_CREATE),(NONE8)
			  ,300,30,20,40,11,'O',{30,12},{20,20,20,20},27,2,'n'},

{"Lokkak, the Ogre Chieftain",(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_2D2|MF1_THRO_DR)
			    ,(NONE8),(MF2_GIANT|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_GOOD|MF2_UNIQUE)
			    ,(NONE8),(NONE8)
			,1500,20,20,100,12,'O',{90,16},{235,235,235,0},27,2,'m'},

{"Vampire"		    ,(MF1_HAS_1D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|
			      MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x9L|MS1_HOLD|MS1_FEAR|MS1_TELE_TO|MS1_CAUSE_2)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_NO_INFRA|MF2_IM_COLD|MF2_IM_POIS|
			     MF2_CHARM_SLEEP|MF2_HURT_LITE)
			    ,(MS2_MIND_BLAST|MS2_FORGET|MS2_DARKNESS),(NONE8)
			    ,175,10,20,45,11,'V',{25,12},{5,5,190,0},27,1,'n'},

{"Gorgimera"		    ,(MF1_MV_ATT_NORM),(0x8L|MS1_BR_FIRE),(MF2_IM_FIRE)
			    ,(NONE8),(NONE8)
			  ,200,10,12,55,11,'H',{25,20},{105,105,32,223},27,2,'n'},

{"Colbran"		    ,(MF1_MV_ATT_NORM),(0x3L)
			    ,(MF2_IM_ELEC|MF2_IM_POIS|MF2_NO_INFRA|MF2_CHARM_SLEEP)
			    ,(MS2_BO_ELEC),(NONE8)
			    ,900,10,12,80,12,'g',{80,12},{130,130,0,0},27,2,'n'},

{"Spirit naga"		    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|
			     MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_MV_INVIS)
			    ,(0x4L|MS1_BLIND)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP)
			    ,(MS2_MIND_BLAST|MS2_DARKNESS|MS2_HEAL),(NONE8)
			    ,60,120,20,75,11,'n',{30,15},{77,77,31,31},28,2,'f'},

{"5-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_1D2)
			    ,(0x5L|MS1_FEAR),(MF2_ANIMAL|MF2_IM_POIS)
			    ,(MS2_BA_POIS),(NONE8)
		       ,350,20,20,80,12,'R',{100,8},{163,163,163,163},28,2,'n'},

{"Black knight"		   ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_CARRY_GOLD)
			    ,(0x8L|MS1_CAUSE_3|MS1_BLIND|MS1_FEAR),(MF2_EVIL)
			    ,(MS2_DARKNESS),(NONE8)
			    ,240,10,20,70,12,'p',{30,10},{23,23,23,0},28,1,'m'},

{"Uldor the Accursed"	  ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			    ,600,40,20,70,11,'p',{50,20},{22,22,22,18},28,4,'n'},

{"Mage"			    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_TELEPORT|MS1_TELE_TO|MS1_BLIND|MS1_BO_COLD|MS1_BO_FIRE
			     |MS1_CONF|MS1_S_MONSTER),(MF2_EVIL|MF2_INTELLIGENT)
			    ,(MS2_BO_ELEC|MS2_HASTE),(NONE8)
			    ,150,10,20,40,11,'p',{15,8},{14,14,0,0},28,1,'m'},

{"Mind flayer"		    ,(MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_HAS_60|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x8L|MS1_HOLD|MS1_FEAR|MS1_BLIND)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_MIND_BLAST|MS2_BRAIN_SMASH|MS2_FORGET),(NONE8)
			    ,200,10,20,60,11,'p',{18,8},{225,225,0,0},28,1,'n'},

{"Draebor, the Imp"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_MV_INVIS|
			     MF1_CARRY_OBJ|MF1_HAS_4D2),(0x5L|MS1_BLINK|MS1_TELE_TO|MS1_TELEPORT|MS1_FEAR
			    |MS1_CONF|MS1_BLIND)
			    ,(MF2_DEMON|MF2_IM_FIRE|MF2_MAX_HP|MF2_EVIL|MF2_UNIQUE
			      |MF2_GOOD|MF2_INTELLIGENT)
			    ,(MS2_TELE_LEVEL|MS2_TELE_AWAY),(NONE8)
			    ,750,20,20,50,12,'I',{40,13},{152,152,17,0},28,5,'n'},

{"Basilisk"		    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR)
			    ,(NONE8),(MF2_ANIMAL|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,300,30,15,90,12,'R',{20,30},{146,39,39,39},28,3,'n'},

{"Ice troll"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_CARRY_GOLD|
			     MF1_HAS_60),(NONE8),
			    (MF2_EVIL|MF2_TROLL|MF2_IM_COLD|MF2_HURT_LITE|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,160,50,20,56,11,'T',{24,10},{4,4,123,4},28,1,'m'},

{"Giant purple worm"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_IM_ACID|MF2_ANIMAL)
			    ,(NONE8),(NONE8)
			    ,400,30,14,65,11,'w',{65,8},{7,113,166,0},29,3,'n'},

{"Movanic Deva"             ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
                              MF1_HAS_2D2)
                            ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_CONF)
                            ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_INTELLIGENT)
                            ,(MS2_HEAL|MS2_HASTE),(NONE8)
                            ,400,255,30,68,11,'A',{25,16}
                            ,{18,18,18,18},29,6,'n'},

{"Catoblepas"		    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_2D2)
			    ,(NONE8)
			    ,(MF2_IM_POIS|MF2_ANIMAL),(NONE8),(NONE8)
			    ,400,40,15,55,11,'q',{30,10}
			    ,{221,222,228,39},29,2,'n'},

{"Mimic"                    ,(MF1_MV_ONLY_ATT),(0x4L|MS1_FEAR|MS1_CONF|MS1_BLIND|
			     MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID|MS1_CAUSE_2|
			     MS1_S_MONSTER),(MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_MINDLESS),
                             (MS2_BO_ELEC|MS2_FORGET),(NONE8)
			,200,100,30,60,12,']',{10,35},{152,152,152,152},29,3,'n'},

{"Young blue dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ),(0xBL|MS1_FEAR|MS1_BR_ELEC)
			    ,(MF2_IM_ELEC|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,300,70,20,50,11,'d',{33,8},{52,52,29,0},29,1,'n'},

{"Young white dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ),(0xBL|MS1_FEAR|MS1_BR_COLD)
			    ,(MF2_IM_COLD|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP),(NONE8),(NONE8)
			    ,275,70,20,50,11,'d',{32,8},{52,52,29,0},29,1,'n'},

{"Young green dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ),(0xBL|MS1_FEAR|MS1_BR_POIS)
			    ,(MF2_IM_POIS|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP),(NONE8),(NONE8)
			    ,290,70,20,60,11,'d',{32,8},{52,52,29,0},29,1,'n'},

{"Young bronze dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_60|MF1_HAS_90|
			     MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR),
			     (0xBL|MS1_FEAR)
			    ,(MF2_DRAGON|MF2_MAX_HP|MF2_CHARM_SLEEP)
			    ,(MS2_BR_CONF),(NONE8)
			    ,310,150,20,63,11,'d',{34,8},{52,52,29,0},29,3,'n'},

{"Mithril golem"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_2D2),(NONE8)
			    ,(MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|MF2_MINDLESS
			      |MF2_NO_INFRA|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			  ,500,10,12,100,11,'g',{80,15},{20,20,23,23},30,4,'n'},

{"Shadow drake"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_MV_INVIS|MF1_THRO_DR|MF1_HAS_2D2|
			    MF1_CARRY_OBJ|MF1_PICK_UP),(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_IM_COLD|MF2_DRAGON)
			    ,(MS2_HASTE|MS2_DARKNESS),(NONE8)
			  ,700,30,25,50,11,'d',{20,10},{122,122,122,0},30,2,'n'},

{"Skeleton troll"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(NONE8)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_TROLL|MF2_NO_INFRA|MF2_IM_COLD|MF2_CHARM_SLEEP|
			     MF2_IM_POIS|MF2_MINDLESS),(NONE8),(NONE8)
			    ,225,20,20,55,11,'s',{20,10},{5,5,41,0},30,1,'n'},

{"Manticore"		    ,(MF1_MV_ATT_NORM),(0x5L),(MF2_EVIL|MF2_MAX_HP),(MS2_ARROW_2)
			    ,(NONE8)
			    ,300,10,12,15,12,'H',{25,10},{17,17,17,17},30,2,'n'},

{"Giant static ant"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_IM_ELEC)
			    ,(NONE8),(NONE8)
			    ,80,60,10,50,11,'a',{8,8},{134,0,0,0},30,2,'n'},

{"Giant army ant"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT),(NONE8)
			    ,(MF2_ANIMAL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,90,40,10,40,12,'a',{19,6},{39,0,0,0},30,3,'n'},

{"Grave wight"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_DR|MF1_HAS_1D2|MF1_CARRY_OBJ)
			    ,(0x8L|MS1_CAUSE_3|MS1_FEAR)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_NO_INFRA|MF2_IM_COLD|MF2_IM_POIS|
			      MF2_HURT_LITE|MF2_CHARM_SLEEP),(MS2_DARKNESS),(NONE8)
			    ,325,30,20,50,11,'W',{12,10},{6,6,191,0},30,1,'n'},

{"Killer slicer beetle"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,200,30,14,60,11,'K',{22,10},{48,48,0,0},30,2,'n'},

{"Ghost"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD|
			     MF1_HAS_60|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			    ,(0xFL|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|
			     MF2_IM_POIS),(NONE8),(NONE8)
			    ,350,10,20,30,12,'G',{13,8},{99,192,184,0},31,1,'n'},

{"Death watch beetle"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,190,30,16,60,11,'K',{25,12},{47,67,0,0},31,3,'n'},

{"Ogre shaman"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_90|MF1_CARRY_OBJ)
			    ,(0x5L|MS1_TELEPORT|MS1_HOLD|MS1_CAUSE_2|
			     MS1_FEAR|MS1_S_MONSTER|MS1_BO_FIRE),(MF2_EVIL|MF2_GIANT)
			    ,(MS2_TRAP_CREATE),(NONE8)
			    ,250,30,20,55,11,'O',{14,10},{19,19,19,0},32,2,'n'},

{"Nexus quylthulg"	    ,(MF1_MV_INVIS),(0x1L|MS1_BLINK),(MF2_CHARM_SLEEP|MF2_MINDLESS)
			    ,(MS2_TELE_AWAY),(NONE8)
			    ,300,0,10,1,11,'Q',{10,12},{0,0,0,0},32,1,'n'},

{"Shelob, Spider of Darkness",(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_2D2|MF1_HAS_1D2)
			   ,(0x2L|MS1_FEAR|MS1_BLIND|MS1_CONF|MS1_SLOW|MS1_CAUSE_3)
			     ,(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_HURT_LITE|MF2_CHARM_SLEEP|
			     MF2_MAX_HP|MF2_GOOD|MF2_INTELLIGENT),(MS2_RAZOR|MS2_HEAL|MS2_S_SPIDER|
			     MS2_TRAP_CREATE),(NONE8)
			  ,1200,80,8,80,11,'S',{120,10},{38,167,85,167},32,3,'f'},

{"Ninja"		   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_1D2)
			    ,(NONE8),(MF2_EVIL|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,300,10,20,60,12,'p',{13,12},{152,80,80,0},32,2,'m'},

{"Memory moss"              ,(MF1_MV_ONLY_ATT),(0x6L),(MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_MINDLESS)
                            ,(MS2_FORGET),(NONE8)
			    ,150,5,30,1,11,',',{1,2},{89,89,0,0},32,3,'n'},

{"Storm giant"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_1D2)
			    ,(0x8L|MS1_FEAR|MS1_CONF|MS1_BLINK|MS1_TELE_TO)
			    ,(MF2_EVIL|MF2_GIANT|MF2_IM_ELEC|MF2_IM_COLD|MF2_MAX_HP)
			    ,(MS2_BO_ELEC|MS2_BA_ELEC),(NONE8)
			  ,1500,40,20,60,11,'P',{24,16},{215,215,215,0},32,1,'n'},

{"Cave troll"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|
			      MF1_CARRY_OBJ|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_HURT_LITE|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,350,50,20,50,11,'T',{24,12},{18,7,7,7},33,1,'m'},

{"Half-troll"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_90)
			    ,(NONE8),(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,300,50,20,50,11,'T',{25,14},{53,53,53,36},33,2,'m'},

{"Mystic"		    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_MV_INVIS)
			    ,(0x6L),(MF2_IM_POIS|MF2_IM_ACID|MF2_MAX_HP|MF2_CHARM_SLEEP)
			    ,(MS2_HEAL|MS2_S_SPIDER),(NONE8)
			 ,500,5,30,50,12,'p',{10,35},{266,266,266,266},33,3,'m'},

{"Barrow wight"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_60),(0x8L|MS1_CAUSE_2|
			      MS1_HOLD|MS1_FEAR)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_NO_INFRA|MF2_IM_COLD|MF2_IM_POIS|
			      MF2_HURT_LITE|MF2_CHARM_SLEEP|MF2_GROUP),(MS2_DARKNESS),(NONE8)
			    ,375,10,20,40,11,'W',{15,10},{7,7,193,0},33,3,'n'},

{"Giant skeleton troll"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|MF2_MINDLESS|
			      MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP),(NONE8),(NONE8)
			    ,325,20,20,50,11,'s',{45,10},{8,8,28,28},33,1,'n'},

{"Chaos drake"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_CARRY_OBJ)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW),(MF2_EVIL|MF2_IM_FIRE|
			     MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON),(MS2_BR_DISE)
			    ,(NONE8)
			  ,700,30,25,100,11,'d',{50,10},{54,54,36,0},33,3,'n'},

{"Law drake"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_CARRY_OBJ)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW),(MF2_EVIL|MF2_IM_COLD|
			     MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON),(MS2_BR_SHAR|
			     MS2_BR_SOUN),(NONE8)
			  ,700,30,25,100,11,'d',{50,11},{54,54,36,0},33,3,'n'},

{"Balance drake"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_CARRY_OBJ)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW),(MF2_EVIL|MF2_IM_FIRE|
			     MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON)
			    ,(MS2_BR_DISE|MS2_BR_SOUN|MS2_BR_SHAR),(NONE8)
			  ,700,30,25,100,11,'d',{50,12},{54,54,36,0},33,3,'n'},

{"Ethereal drake"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_MV_INVIS|
			     MF1_THRO_WALL)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(MS3_BR_LITE|MS3_BR_DARK)
			  ,700,15,25,100,11,'d',{50,9},{54,54,36,0},33,3,'n'},

{"Bert the Stone Troll"	  ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_HAS_1D2|
			      MF1_CARRY_OBJ),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_HURT_ROCK|
			      MF2_HURT_LITE|MF2_GOOD|MF2_IM_COLD),(NONE8),(NONE8)
			   ,2000,50,20,70,11,'T',{55,20},{23,38,33,0},33,7,'m'},

{"Bill the Stone Troll"	  ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_HAS_1D2|
			      MF1_CARRY_OBJ),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_HURT_ROCK|
			      MF2_HURT_LITE|MF2_GOOD|MF2_IM_COLD),(NONE8),(NONE8)
			  ,2000,50,20,70,11,'T',{55,20},{23,38,33,0},33,7,'m'},

{"Tom the Stone Troll"	 ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_HAS_1D2|
			      MF1_CARRY_OBJ),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|MF2_HURT_ROCK|
			      MF2_HURT_LITE|MF2_GOOD|MF2_IM_COLD),(NONE8),(NONE8)
			  ,2000,50,20,70,11,'T',{55,20},{23,38,33,0},33,7,'m'},

{"Shade"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_CARRY_OBJ|MF1_HAS_2D2|
			     MF1_HAS_90|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			    ,(0xFL|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|
			     MF2_IM_POIS),(MS2_FORGET),(NONE8)
			  ,350,10,20,30,12,'G',{14,20},{99,192,184,0},33,3,'n'},

{"Spectre"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_CARRY_OBJ|MF1_HAS_2D2|
			     MF1_HAS_90|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			    ,(0xFL|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|
			     MF2_IM_POIS),(MS2_FORGET),(NONE8)
			  ,350,10,20,30,12,'G',{14,20},{99,192,237,0},33,3,'n'},

{"Water troll"		     ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|
			      MF1_CARRY_OBJ|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_IM_COLD|MF2_HURT_LITE|
			      MF2_MAX_HP|MF2_GROUP),(NONE8),(NONE8)
			    ,420,50,20,50,11,'T',{26,14},{8,8,11,11},33,1,'m'},

{"Fire elemental"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT)
			    ,(0x6L|MS1_BO_FIRE)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,350,50,12,50,11,'E',{30,8},{103,103,0,0},33,2,'n'},

{"Astral Deva"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_BO_FIRE)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_INTELLIGENT|
			      MF2_IM_ACID|MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_HEAL|MS2_HASTE|MS2_MIND_BLAST|MS2_S_SUMMON),(NONE8)
			    ,400,255,30,68,12,'A',{25,18}
			    ,{21,20,21,20},33,6,'n'},

{"Water elemental"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT)
			    ,(0x6L|MS1_BO_COLD)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,325,50,12,40,11,'E',{25,8},{9,9,9,0},33,2,'n'},

{"Invisible stalker"	    ,(MF1_MV_ATT_NORM|MF1_MV_40|MF1_MV_INVIS|MF1_THRO_DR)
			    ,(NONE8),(MF2_IM_ELEC|MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|
			     MF2_NO_INFRA),(NONE8),(NONE8)
			    ,300,20,20,46,13,'E',{19,12},{5,5,5,0},34,3,'n'},

{"Carrion crawler"	    ,(MF1_MV_ATT_NORM|MF1_MV_20),(NONE8),(MF2_ANIMAL|MF2_IM_POIS|
			     MF2_GROUP),(NONE8),(NONE8)
			    ,100,10,15,40,11,'c',{20,12},{253,253,0,0},34,2,'n'},

{"Master thief"		    ,(MF1_HAS_2D2|MF1_HAS_90|MF1_CARRY_GOLD|MF1_CARRY_OBJ
			     |MF1_THRO_DR|MF1_PICK_UP|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_EVIL),(NONE8),(NONE8)
			  ,350,40,20,30,13,'p',{18,10},{16,17,231,232},34,2,'m'},

{"Ulfang the Black"	    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL|MF2_UNIQUE|MF2_GOOD)
			    ,(NONE8),(NONE8)
			  ,1200,40,20,90,12,'p',{80,13},{23,23,23,23},34,5,'m'},

{"Lich"			    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_1D2|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD)
			    ,(0x4L|MS1_BLINK|MS1_TELE_TO|MS1_CAUSE_3|
			      MS1_HOLD|MS1_BLIND|MS1_MANA_DRAIN|
			      MS1_SLOW|MS1_FEAR)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|MF2_NO_INFRA|
			      MF2_MAX_HP|MF2_HURT_LITE|MF2_CHARM_SLEEP|MF2_INTELLIGENT)
			    ,(MS2_BRAIN_SMASH|MS2_TELE_AWAY),(NONE8)
			 ,800,60,20,60,11,'L',{25,12},{179,179,194,214},34,3,'n'},

{"Master vampire"	   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_CARRY_GOLD)
			    ,(0x6L|MS1_TELE_TO|MS1_CAUSE_3|MS1_HOLD|
			      MS1_FEAR|MS1_CONF)
			    ,(MF2_CHARM_SLEEP|MF2_HURT_LITE|MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|
			     MF2_MAX_HP|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(MS2_DARKNESS|MS2_MIND_BLAST|MS2_FORGET|MS2_BO_NETH),(NONE8)
			    ,750,10,20,60,11,'V',{28,12},{5,5,195,0},34,3,'n'},

{"Giant red scorpion"	    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			    ,275,40,12,50,12,'S',{18,20},{29,165,0,0},34,4,'n'},

{"Earth elemental"	    ,(MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_ATT_NORM)
			    ,(0x8L|MS1_BO_ACID)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_MINDLESS|
			      MF2_CHARM_SLEEP|MF2_HURT_ROCK|MF2_EVIL|MF2_NO_INFRA)
			    ,(NONE8),(NONE8)
			    ,375,90,10,60,10,'E',{30,10},{22,22,22,0},34,2,'n'},

{"Air elemental"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT),(0x8L)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_COLD|MF2_MINDLESS|
			      MF2_IM_ELEC|MF2_IM_ACID|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(MS2_BO_ELEC),(NONE8)
			    ,390,50,12,50,12,'E',{30,5},{9,89,9,0},34,2,'n'},

{"Hell hound"		    ,(MF1_MV_ATT_NORM|MF1_MV_20),(0x5L|MS1_BR_FIRE),
			     (MF2_ANIMAL|MF2_EVIL|MF2_MAX_HP|MF2_IM_FIRE),(NONE8),(NONE8)
			    ,600,0,25,80,12,'C',{25,16},{107,107,107,0},35,3,'n'},

{"Eog golem"		    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_2D2),(NONE8)
			    ,(MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|MF2_MINDLESS
			      |MF2_NO_INFRA|MF2_CHARM_SLEEP),(NONE8),(NONE8)
		     ,1200,10,12,125,10,'g',{100,20},{218,218,235,235},35,4,'n'},

{"Olog-Hai"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|
			      MF1_CARRY_OBJ|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,400,50,20,50,11,'T',{30,14},{10,10,33,33},35,1,'n'},

{"Dagashi"		   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_1D2)
			    ,(NONE8),(MF2_EVIL|MF2_CHARM_SLEEP),(NONE8),(NONE8)
			  ,500,10,20,70,12,'p',{13,25},{152,80,80,152},35,4,'m'},

{"Gravity hound"	    ,(MF1_MV_ATT_NORM),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP),(NONE8)
			    ,(MS3_BR_GRAV)
			    ,500,0,30,30,11,'Z',{35,10},{39,39,39,58},35,2,'n'},

{"Acidic cytoplasm"	     ,(MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_HAS_4D2|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90),(NONE8)
			    ,(MF2_IM_ACID|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|MF2_IM_COLD|
			      MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_NO_INFRA|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,36,1,12,18,12,'j',{50,8},{115,115,115,115},35,5,'n'},

{"Inertia hound"	    ,(MF1_MV_ATT_NORM),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(NONE8)
			    ,(MS3_BR_SLOW)
			    ,500,0,30,30,11,'Z',{35,10},{39,39,39,58},35,2,'n'},

{"Impact hound"		   ,(MF1_MV_ATT_NORM),(0x8L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(NONE8)
			    ,(MS3_BR_WALL)
			    ,500,0,30,30,11,'Z',{35,10},{39,39,39,58},35,2,'n'},

{"Dread"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_CARRY_OBJ|MF1_HAS_2D2|
			     MF1_HAS_60|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			    ,(0xFL|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND|MS1_CONF)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|
			     MF2_IM_POIS),(MS2_BO_NETH),(NONE8)
			    ,600,10,20,30,12,'G',{25,20},{235,235,80,0},35,2,'n'},

{"Ooze elemental"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR)
			    ,(0x5L|MS1_BO_ACID)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_IM_ACID|
		      MF2_CHARM_SLEEP|MF2_EVIL|MF2_NO_INFRA|MF2_MINDLESS),(MS2_BA_ACID),(NONE8)
			  ,300,90,10,80,11,'E',{13,10},{115,115,115,0},35,3,'n'},

{"Smoke elemental"	    ,(MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BO_FIRE)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_MINDLESS|
			      MF2_CHARM_SLEEP|MF2_EVIL),(MS2_DARKNESS),(NONE8)
			    ,375,90,10,80,12,'E',{15,10},{36,36,0,0},35,3,'n'},

{"Young black dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_60|MF1_HAS_90|
			     MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR),
			     (MS1_BR_ACID|0xBL|MS1_FEAR)
			    ,(MF2_EVIL|MF2_IM_ACID|MF2_DRAGON|MF2_MAX_HP),(NONE8),(NONE8)
			    ,620,50,20,60,11,'d',{32,8},{53,53,29,0},35,1,'n'},

{"Mumak"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL),(NONE8),(NONE8)
			 ,2100,100,20,55,11,'q',{90,10},{227,227,233,0},35,3,'n'},

{"Giant red ant lion"	    ,(MF1_MV_ATT_NORM|MF1_THRO_CREAT),(NONE8)
			    ,(MF2_ANIMAL|MF2_IM_FIRE|MF2_MAX_HP|MF2_GROUP),(NONE8),(NONE8)
			    ,350,40,14,49,11,'a',{25,8},{107,107,0,0},35,1,'n'},

{"Mature white dragon"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			      MF1_HAS_2D2),(0xAL|MS1_BR_COLD|MS1_FEAR)
			    ,(MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_EVIL|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1000,70,20,65,11,'d',{50,8},{54,54,37,0},35,1,'n'},

{"Xorn"			    ,(MF1_MV_ATT_NORM|MF1_THRO_WALL|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_POIS|MF2_NO_INFRA|
			     MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_HURT_ROCK|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,650,10,20,80,11,'X',{20,8},{5,5,5,5},36,2,'n'},

{"Shadow"		    ,(MF1_MV_ATT_NORM|MF1_THRO_WALL|MF1_MV_INVIS|MF1_CARRY_OBJ|
			     MF1_HAS_1D2),(0x8L|MS1_TELE_TO|MS1_SLOW),(MF2_UNDEAD|MF2_EVIL|
			     MF2_IM_COLD|MF2_IM_POIS|MF2_NO_INFRA|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,400,20,30,30,12,'G',{10,20},{200,200,184,252},
								  36,3,'n'},

{"Phantom"		    ,(MF1_MV_ATT_NORM|MF1_THRO_WALL|MF1_MV_INVIS|MF1_CARRY_OBJ|
			     MF1_HAS_1D2),(0x5L),(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_IM_POIS|
			     MF2_NO_INFRA|MF2_CHARM_SLEEP),(MS2_FORGET),(NONE8)
			    ,400,20,30,30,12,'G',{20,25},{200,200,184,252},
								  36,3,'n'},

{"Grey wraith"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ)
			    ,(0x7L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR)
			   ,(MF2_UNDEAD|MF2_EVIL|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|
			     MF2_IM_POIS),(MS2_DARKNESS),(NONE8)
			    ,700,10,20,50,11,'W',{24,8},{9,9,196,0},36,1,'n'},

{"Young multi-hued dragon"  ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			      MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90)
			    ,(0x5L|MS1_BR_POIS|MS1_BR_ELEC|MS1_BR_ACID|MS1_BR_COLD|
			      MS1_BR_FIRE|MS1_FEAR)
			    ,(MF2_IM_COLD|MF2_IM_ACID|MF2_IM_POIS|MF2_IM_ELEC|
			      MF2_IM_FIRE|MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1320,50,20,60,11,'d',{40,8},{55,55,38,0},36,1,'n'},

{"Colossus"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_MINDLESS|
			     MF2_NO_INFRA|MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC)
			    ,(NONE8),(NONE8)
		     ,850,10,12,150,10,'g',{200,15},{212,212,235,235},36,4,'n'},

{"Young gold dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_60|MF1_HAS_90|
			     MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR),
			     (0xBL|MS1_FEAR)
			    ,(MF2_DRAGON|MF2_MAX_HP)
			    ,(MS2_BR_SOUN),(NONE8)
			    ,950,150,20,63,11,'d',{38,8},{54,54,37,0},36,2,'n'},

{"Rogrog the Black Troll"   ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_HAS_2D2|
			      MF1_CARRY_OBJ),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|
			      MF2_GOOD|MF2_IM_COLD),(NONE8),(NONE8)
			  ,5000,50,20,70,12,'T',{55,28},{235,38,33,229},36,5,'m'},

{"Mature blue dragon"	    ,(MF1_HAS_2D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_BR_ELEC|MS1_FEAR)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1200,70,20,75,11,'d',{49,8},{54,54,38,0},36,1,'n'},

{"Mature green dragon"	    ,(MF1_HAS_2D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_BR_POIS|MS1_FEAR)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1100,70,20,70,11,'d',{49,8},{52,52,29,0},36,1,'n'},

{"Mature bronze dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_FEAR|MS1_CONF)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_BR_CONF),(NONE8)
			    ,1300,150,20,70,11,'d',{55,8},{54,54,38,0},36,2,'n'},

{"Young red dragon"	    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_HAS_60|MF1_HAS_90|
			     MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_THRO_DR),
			     (MS1_BR_FIRE|0xBL|MS1_FEAR)
			    ,(MF2_EVIL|MF2_IM_FIRE|MF2_DRAGON|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,640,50,20,63,11,'d',{36,8},{54,54,37,0},36,1,'n'},

{"Trapper"                  ,(MF1_MV_ONLY_ATT|MF1_MV_INVIS),(NONE8)
                            ,(MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,580,10,30,75,12,'.',{50,12},{20,20,265,265},36,3,'n'},

{"Bodak"		    ,(MF1_PICK_UP|MF1_MV_ATT_NORM|MF1_THRO_DR)
			    ,(0x4L|MS1_BA_FIRE|MS1_BO_FIRE|MS1_S_DEMON)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_EVIL)
			    ,(NONE8),(NONE8)
			  ,750,90,10,68,11,'I',{35,10},{103,103,224,0},36,2,'n'},

{"Ice elemental"	    ,(MF1_PICK_UP|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BA_COLD)
			    ,(MF2_IM_POIS|MF2_IM_COLD|MF2_IM_ELEC|MF2_MINDLESS|
			     MF2_CHARM_SLEEP|MF2_EVIL|MF2_NO_INFRA),(MS2_BO_ICEE),(NONE8)
			   ,650,90,10,60,11,'E',{35,10},{121,22,121,0},36,2,'n'},

{"Necromancer"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(0x3L|MS1_TELEPORT|MS1_TELE_TO|MS1_CAUSE_3|MS1_HOLD|
			   MS1_S_UNDEAD|MS1_FEAR|MS1_BLIND),(MF2_EVIL|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(MS2_BO_NETH|MS2_HASTE),(NONE8)
			    ,630,10,20,50,11,'p',{25,11},{15,15,0,0},36,2,'m'},

{"Lorgan, Chief of the Easterlings"
			    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
			    ,(0x4L|MS1_TELE_TO),(MF2_EVIL|MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_IM_ACID|
			     MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|MF2_GOOD|
			     MF2_UNIQUE),(MS2_S_SUMMON),(NONE8)
		       ,1200,10,25,100,12,'p',{50,35},{235,235,20,20},36,2,'m'},

{"Demonologist"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2)
			    ,(0x2L|MS1_TELEPORT|MS1_HOLD|
			     MS1_S_DEMON),(MF2_EVIL|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(NONE8),(NONE8)
			    ,700,10,20,50,12,'p',{25,11},{15,15,14,0},36,2,'m'},

{"Mummified troll"	    ,(MF1_HAS_60|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			     MF1_THRO_DR|MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_UNDEAD|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_IM_POIS|MF2_MINDLESS|
			      MF2_TROLL|MF2_EVIL|MF2_NO_INFRA|MF2_MAX_HP),(NONE8),(NONE8)
			    ,420,50,20,50,11,'M',{19,10},{15,15,0,0},37,1,'n'},

{"The Queen Ant"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_2D2|MF1_THRO_DR)
			    ,(0x2L),(MF2_ANIMAL|MF2_MAX_HP|MF2_UNIQUE|MF2_CHARM_SLEEP|
			     MF2_GOOD),(NONE8),(MS3_S_ANT)
			 ,1000,10,30,100,12,'a',{120,12},{39,39,37,37},37,2,'f'},

{"Will o' the wisp"	    ,(MF1_MV_ATT_NORM|MF1_MV_40|MF1_MV_INVIS|MF1_THRO_DR|MF1_THRO_WALL)
			    ,(0x2L|MS1_BLINK|MS1_CAUSE_2|MS1_CONF|MS1_TELEPORT)
			    ,(MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_POIS|
			     MF2_IM_ACID|MF2_IM_ELEC|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(NONE8),(NONE8)
			    ,500,0,30,150,13,'E',{20,10},{8,8,8,8},37,4,'n'},

{"Magma elemental"	    ,(MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_ATT_NORM)
			    ,(0x7L|MS1_BA_FIRE)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_ELEC|MF2_MINDLESS|
			      MF2_CHARM_SLEEP|MF2_EVIL),(MS2_BO_PLAS),(NONE8)
			    ,950,90,10,70,11,'E',{35,10},{102,22,102,0},37,2,'n'},

{"Black pudding"	    ,(MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_HAS_1D2|
			      MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90),(NONE8)
			    ,(MF2_IM_ACID|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|MF2_IM_COLD|
			      MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_NO_INFRA|MF2_GROUP|MF2_MINDLESS)
			    ,(NONE8),(NONE8)
			    ,36,1,12,18,11,'j',{50,8},{115,115,115,115},37,5,'n'},

{"Iridescent beetle"	    ,(MF1_MV_ATT_NORM),(NONE8)
			    ,(MF2_ANIMAL|MF2_IM_ELEC|MF2_MAX_HP),(NONE8),(NONE8)
			    ,850,30,16,60,11,'K',{32,8},{45,10,146,0},37,2,'n'},

{"Nexus vortex"		    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L)
			    ,(MF2_CHARM_SLEEP|MF2_MINDLESS),(MS2_BR_NETH),(NONE8)
			    ,800,0,100,40,12,'v',{32,10},{244,0,0,0},37,1,'n'},

{"Plasma vortex"	    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L)
			    ,(MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(MS3_BR_PLAS)
			    ,800,0,100,40,12,'v',{32,10},{243,0,0,0},37,1,'n'},

{"Mature red dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_BR_FIRE|MS1_FEAR|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1400,30,20,80,11,'d',{60,8},{52,56,39,0},37,1,'n'},

{"Mature gold dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_FEAR|MS1_CONF)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_BR_SOUN),(NONE8)
			    ,1500,150,20,80,11,'d',{70,8},{52,56,39,0},37,2,'n'},

{"Crystal drake"	    ,(MF1_MV_ATT_NORM|MF1_MV_INVIS|MF1_THRO_DR|MF1_HAS_4D2|
			     MF1_CARRY_OBJ)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW)
			    ,(MF2_EVIL|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON)
			    ,(MS2_BR_SHAR),(NONE8)
			   ,1500,30,25,100,12,'d',{50,10},{52,52,35,0},37,2,'n'},

{"Mature black dragon"	    ,(MF1_HAS_2D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_BR_ACID|MS1_FEAR)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_ACID|MF2_CHARM_SLEEP|MF2_MAX_HP),(NONE8)
			    ,(NONE8)
			    ,1350,30,20,55,11,'d',{58,8},{54,54,38,0},37,1,'n'},

{"Mature multi-hued dragon" ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|
			      MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90|MF1_HAS_2D2)
			    ,(0x5L|MS1_BR_POIS|MS1_BR_ELEC|MS1_BR_ACID|MS1_BR_COLD|
			      MS1_BR_FIRE|MS1_FEAR|MS1_CONF|MS1_BLIND)
			    ,(MF2_IM_COLD|MF2_IM_ACID|MF2_IM_POIS|MF2_IM_ELEC|
			      MF2_IM_FIRE|MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1700,50,20,65,11,'d',{81,8},{56,56,39,0},38,2,'n'},

{"Death knight"		    ,(MF1_HAS_2D2|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_CAUSE_3|MS1_BLIND|MS1_FEAR)
			    ,(MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(MS2_BO_NETH|MS2_S_SUMMON),(NONE8)
			  ,1000,10,20,100,12,'p',{30,20},{235,23,23,0},38,1,'n'},

{"Castamir the Usurper"	    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_PICK_UP|
			      MF1_MV_ATT_NORM),(0x2L|MS1_BO_FIRE|MS1_BO_COLD)
			    ,(MF2_EVIL|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD|MF2_INTELLIGENT)
			    ,(MS2_TRAP_CREATE|MS2_BO_ICEE|MS2_HEAL|MS2_BO_ELEC),(NONE8)
			    ,1600,40,20,90,12,'p',{80,11},{23,23,23,23},38,5,'m'},

{"Time vortex"		    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L)
			    ,(MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(MS3_BR_TIME)
			    ,900,0,100,40,13,'v',{32,10},{244,0,0,0},38,4,'n'},

{"Shimmering vortex"	    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L)
			    ,(MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(MS3_BR_LITE)
			    ,200,0,100,30,14,'v',{6,12},{203,203,0,0},38,4,'n'},

{"Ancient blue dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x9L|MS1_BR_ELEC|MS1_FEAR|MS1_BLIND
			      |MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1500,80,20,80,12,'D',{88,8},{54,54,37,0},38,1,'n'},

{"Ancient bronze dragon"    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x6L|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP),(MS2_BR_CONF),(NONE8)
			    ,1700,200,20,100,12,'D',{92,8},{54,54,38,0},38,2,'n'},

{"Beholder"		    ,(MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID|
			      MS1_MANA_DRAIN|MS1_BLIND|MS1_CONF|MS1_FEAR|MS1_SLOW)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_IM_POIS)
			    ,(MS2_FORGET|MS2_MIND_BLAST|MS2_DARKNESS),(NONE8)
			,6000,10,30,80,12,'e',{80,20},{223,224,225,226},38,4,'n'},

{"Emperor wight"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x6L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|
			      MF2_IM_POIS|MF2_NO_INFRA|MF2_HURT_LITE)
			    ,(MS2_BO_NETH),(NONE8)
			    ,1600,10,20,40,12,'W',{48,8},{10,10,199,0},38,2,'n'},

{"Planetar"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0xBL|MS1_CONF|MS1_BO_MANA)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_INTELLIGENT|
			      MF2_IM_ACID|MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_HEAL|MS2_HASTE|MS2_S_SUMMON|MS2_TELE_AWAY|MS2_BO_PLAS|MS2_S_ANGEL)
			    ,(NONE8)
			    ,1800,255,30,68,12,'A',{50,10}
			    ,{22,23,23,22},38,6,'n'},

{"Vargo, Tyrant of Fire"    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT)
			    ,(0x4L|MS1_BA_FIRE)
			   ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|MF2_UNIQUE)
			   ,(MS2_BO_PLAS),(NONE8)
			,3000,50,12,50,12,'E',{60,25},{103,103,103,103},38,3,'n'},

{"Black wraith"		    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x7L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR|MS1_BLIND)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|
			      MF2_IM_POIS|MF2_HURT_LITE|MF2_NO_INFRA)
			    ,(MS2_BO_NETH),(NONE8)
			    ,1700,10,20,55,12,'W',{50,10},{10,10,199,0},38,2,'n'},

{"Erinyes"		    ,(MF1_MV_ATT_NORM|MF1_HAS_60|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x7L|MS1_BLIND|MS1_CONF|MS1_BO_FIRE)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			    ,1000,80,20,50,11,'&',{30,8},{17,87,0,0},38,2,'n'},

{"Nether wraith"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM|
			      MF1_MV_INVIS|MF1_THRO_WALL)
			    ,(0x6L|MS1_CAUSE_3|MS1_FEAR|MS1_BLIND)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|
			      MF2_HURT_LITE|MF2_NO_INFRA|MF2_IM_POIS)
			    ,(MS2_BO_NETH|MS2_MIND_BLAST|MS2_DARKNESS),(NONE8)
			    ,1700,10,20,55,12,'W',{60,8},{10,10,202,0},39,2,'n'},

{"Eldrak"		    ,(MF1_MV_ATT_NORM|MF1_PICK_UP|MF1_THRO_DR|MF1_HAS_60|
			      MF1_CARRY_OBJ|MF1_CARRY_GOLD),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,800,50,20,80,11,'T',{30,25},{17,17,17,0},39,3,'n'},

{"Ettin"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2),(NONE8)
			    ,(MF2_TROLL|MF2_EVIL|MF2_IM_POIS|MF2_MAX_HP|MF2_CHARM_SLEEP)
			    ,(NONE8),(NONE8)
			    ,1000,30,20,100,11,'T',{50,30},{19,19,19,0},39,3,'n'},

{"Waldern, King of Water"   ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT)
			    ,(0x4L|MS1_BA_COLD)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_NO_INFRA)
			    ,(MS2_BO_WATE|MS2_BA_WATE|MS2_BO_ICEE),(NONE8)
			    ,3250,50,12,40,12,'E',{80,25},{23,23,23,23},39,3,'n'},

{"Kavlax the Many-Headed"   ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x4L|MS1_BR_FIRE|MS1_BR_COLD|MS1_BR_ELEC|MS1_BR_ACID)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD|
			     MF2_IM_ACID|MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_ELEC)
			    ,(MS2_BR_CONF|MS2_BR_SOUN|MS2_BR_SHAR|MS2_BR_NETH)
			    ,(MS3_BR_GRAV)
		       ,3000,30,20,85,12,'d',{130,10},{56,39,39,39},39,3,'m'},

{"Ancient white dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x9L|MS1_BR_COLD|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,2500,80,20,90,12,'D',{88,8},{55,55,39,0},39,1,'n'},

{"Ancient green dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x9L|MS1_BR_POIS|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,2400,80,20,85,12,'D',{90,8},{54,54,38,0},39,1,'n'},

{"7-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x5L|MS1_FEAR|MS1_BR_POIS)
			    ,(MF2_ANIMAL|MF2_IM_POIS),(MS2_BA_POIS),(NONE8)
		     ,2000,20,20,90,12,'R',{100,10},{162,162,162,144},39,2,'n'},

{"Night mare"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_CARRY_GOLD)
			    ,(NONE8),(MF2_UNDEAD|MF2_IM_POIS|MF2_IM_COLD|MF2_EVIL|
			     MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			,2900,0,30,85,12,'q',{150,10},{236,20,20,216},39,3,'n'},

{"Vampire lord"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_4D2|MF1_CARRY_GOLD|
			     MF1_CARRY_OBJ|MF1_HAS_60)
			    ,(0x7L|MS1_CAUSE_3|MS1_MANA_DRAIN|MS1_FEAR|MS1_HOLD|
			      MS1_BLIND)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_HURT_LITE|
			      MF2_IM_COLD|MF2_HURT_LITE|MF2_NO_INFRA|MF2_IM_POIS)
			    ,(MS2_BRAIN_SMASH|MS2_BO_NETH|MS2_RAZOR|MS2_DARKNESS),(NONE8)
			    ,1800,10,20,70,12,'V',{62,25},{5,5,5,198},39,3,'n'},

{"Ancient black dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x9L|MS1_BR_ACID|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_ACID|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,2500,70,20,90,12,'D',{90,8},{55,55,38,0},39,1,'n'},

{"Disenchanter worms"	    ,(MF1_MULTIPLY|MF1_MV_ATT_NORM|MF1_MV_40),(NONE8),
			     (MF2_ANIMAL|MF2_HURT_LITE|MF2_MINDLESS),(NONE8),(NONE8)
			    ,30,10,7,5,10,'w',{10,8},{208,0,0,0},40,3,'p'},

{"Rotting quylthulg"	    ,(MF1_MV_INVIS)
			    ,(0x2L|MS1_S_UNDEAD|MS1_BLINK|MS1_TELEPORT), (MF2_ANIMAL|MF2_EVIL|MF2_MAX_HP|
			     MF2_CHARM_SLEEP|MF2_MINDLESS),(NONE8),(NONE8)
			    ,1500,0,20,1,12,'Q',{20,8},{0,0,0,0},40,1,'n'},

{"Spirit troll"		    ,(MF1_MV_INVIS|MF1_MV_ATT_NORM|MF1_THRO_WALL|MF1_THRO_DR|
			     MF1_CARRY_OBJ|MF1_CARRY_GOLD|MF1_HAS_90)
			    ,(NONE8),(MF2_EVIL|MF2_TROLL|MF2_IM_POIS|MF2_IM_COLD|MF2_MAX_HP|
			     MF2_CHARM_SLEEP|MF2_IM_ELEC),(NONE8),(NONE8)
			,900,5,20,90,11,'T',{40,25},{19,18,18,0},40,3,'n'},

{"Lesser titan"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x3L|MS1_FEAR|MS1_TELE_TO)
			    ,(MF2_EVIL|MF2_GIANT|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(MS2_S_SUMMON|MS2_HEAL),(NONE8)
		       ,3500,15,30,80,12,'P',{35,30},{216,216,216,216},40,3,'n'},

{"9-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x4L|MS1_FEAR|MS1_BO_FIRE|MS1_BR_FIRE)
			    ,(MF2_ANIMAL|MF2_IM_FIRE),(NONE8),(NONE8)
		       ,3000,20,20,95,12,'R',{100,12},{106,106,106,106},40,2,'n'},

{"Enchantress"		    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_S_DRAGON|MS1_BLIND)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD),(NONE8),(NONE8)
			    ,2100,10,20,60,13,'p',{40,13},{15,15,16,0},40,4,'f'},

{"Archpriest"		    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_HOLD|MS1_BLIND|MS1_CONF|
			      MS1_S_MONSTER|MS1_S_UNDEAD|MS1_CAUSE_3)
			    ,(MF2_INTELLIGENT|MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(MS2_HEAL),(NONE8)
			    ,1800,10,20,60,12,'p',{40,13},{17,17,18,0},40,2,'m'},

{"Sorceror"		    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_BLINK|MS1_TELE_TO|MS1_BLIND|MS1_CONF|
			      MS1_S_MONSTER|MS1_S_UNDEAD|MS1_CAUSE_3|MS1_S_DRAGON|
			      MS1_BA_FIRE|MS1_BA_COLD|MS1_BO_ACID)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP),(MS2_TRAP_CREATE),(NONE8)
			    ,2150,10,20,60,13,'p',{40,13},{16,16,16,0},40,2,'m'},

{"Xaren"		     ,(MF1_MV_ATT_NORM|MF1_THRO_WALL|MF1_THRO_DR|MF1_PICK_UP)
			    ,(NONE8),(MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_POIS|MF2_NO_INFRA|
			     MF2_IM_ELEC|MF2_CHARM_SLEEP|MF2_HURT_ROCK|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1200,10,20,80,12,'X',{40,8},{17,17,17,17},40,1,'n'},

{"Giant roc"                ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_IM_ELEC)
                            ,(NONE8),(NONE8)
			    ,1000,10,20,70,11,'B',{80,13},{78,78,284,0},40,3,'n'},

{"Uvatha the Horseman"	    ,(MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(NONE8)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_GOOD|MF2_NO_INFRA|MF2_IM_POIS)
			    ,(NONE8),(NONE8)
			 ,7000,10,90,60,12,'W',{35,35},{235,23,23,199},40,3,'n'},

{"Minotaur"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_EVIL),(NONE8),(NONE8)
			    ,2100,10,13,25,13,'H',{100,10}
			    ,{227,227,228,228},40,2,'n'},

{"Medusa, the Gorgon"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_HAS_1D2|MF1_CARRY_OBJ)
			    ,(0x2L|MS1_HOLD|MS1_CAUSE_3|MS1_BO_FIRE|MS1_FEAR)
			    ,(MF2_EVIL|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_UNIQUE|MF2_INTELLIGENT|
			     MF2_GOOD|MF2_IM_POIS|MF2_MAX_HP|MF2_IM_ACID)
			    ,(MS2_BO_PLAS|MS2_BA_ACID),(MS3_S_REPTILE)
			,9000,5,30,100,12,'n',{40,60},{246,267,235,235},40,3,'f'},

{"Death drake"		    ,(MF1_MV_ATT_NORM|MF1_MV_INVIS|MF1_THRO_DR|MF1_HAS_2D2|MF1_HAS_4D2|
			     MF1_CARRY_OBJ|MF1_PICK_UP|MF1_THRO_WALL)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW)
			    ,(MF2_EVIL|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON)
			    ,(MS2_BR_LIFE),(NONE8)
			 ,3500,30,25,100,12,'D',{105,10},{56,56,236,0},40,2,'n'},

{"Ancient red dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x6L|MS1_BR_FIRE|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			 ,2750,70,20,100,12,'D',{105,10},{56,56,40,0},40,1,'n'},

{"Ancient gold dragon"	    ,(MF1_HAS_4D2|MF1_CARRY_GOLD|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x6L|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP),(MS2_BR_SOUN),(NONE8)
			 ,4000,200,20,100,12,'D',{150,10},{56,56,40,0},40,2,'n'},

{"Great crystal drake"	    ,(MF1_MV_ATT_NORM|MF1_MV_INVIS|MF1_THRO_DR|MF1_HAS_4D2|MF1_HAS_2D2|
			     MF1_CARRY_OBJ)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_SLOW)
			    ,(MF2_EVIL|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON)
			    ,(MS2_BR_SHAR),(NONE8)
			   ,3500,30,25,100,11,'D',{50,30},{55,55,39,0},40,2,'n'},

{"Vrock"		    ,(MF1_MV_ATT_NORM|MF1_HAS_60|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x8L|MS1_BLIND|MS1_CONF)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,1000,80,20,50,11,'&',{20,20},{17,78,78,0},40,2,'n'},

{"Death quasit"		    ,(MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_90|MF1_CARRY_OBJ|MF1_MV_INVIS|
			      MF1_THRO_WALL|MF1_MV_ATT_NORM)
			  ,(0xAL|MS1_FEAR|MS1_CONF|MS1_BLIND|MS1_CAUSE_3|MS1_S_DEMON)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DEMON|MF2_IM_FIRE
			      |MF2_INTELLIGENT),(MS2_FORGET),(NONE8)
			    ,1000,0,20,80,13,'I',{55,8},{177,58,58,0},40,3,'n'},

{"Adunaphel the Quiet"	   ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_MV_INVIS
			     |MF1_THRO_WALL)
			    ,(0x3L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR|MS1_BLIND
			      |MS1_S_MONSTER|MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_GOOD|MF2_NO_INFRA|MF2_IM_POIS)
			    ,(MS2_FORGET|MS2_BO_NETH),(NONE8)
			  ,8000,10,90,60,12,'W',{35,35},{23,23,199,0},41,3,'f'},

{"Dark elven sorceror"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_BLINK|MS1_TELE_TO|MS1_BLIND|MS1_CONF|
			      MS1_S_MONSTER|MS1_S_UNDEAD|MS1_CAUSE_3|MS1_S_DEMON|
			      MS1_BA_FIRE|MS1_BA_COLD|MS1_BO_ACID)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_INTELLIGENT|MF2_MAX_HP|MF2_HURT_LITE)
			    ,(MS2_HEAL|MS2_DARKNESS),(NONE8)
			    ,3000,10,20,70,13,'h',{40,20},{16,16,16,0},41,2,'m'},

{"Master lich"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x3L|MS1_FEAR|MS1_CONF|MS1_BLIND|MS1_HOLD|
			      MS1_CAUSE_3|MS1_MANA_DRAIN|MS1_TELE_TO|MS1_BLINK|MS1_S_UNDEAD)
			    ,(MF2_UNDEAD|MF2_IM_POIS|MF2_IM_COLD|MF2_EVIL|MF2_MAX_HP|
			      MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_INTELLIGENT)
			    ,(MS2_BRAIN_SMASH|MS2_RAZOR),(NONE8)
		       ,10000,50,20,80,12,'L',{42,42},{181,201,214,181},41,2,'n'},

{"Hezrou"		    ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x9L|MS1_S_DEMON|MS1_BO_FIRE)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|MF2_GROUP)
			    ,(NONE8),(NONE8)
			    ,1500,80,20,40,11,'&',{35,15},{17,17,0,0},41,3,'n'},

{"Akhorahil the Blind"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR|MS1_BLIND
			      |MS1_S_MONSTER|MS1_BO_FIRE|MS1_BO_COLD)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_GOOD|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(MS2_BO_NETH|MS2_DARKNESS),(NONE8)
			  ,12000,10,90,70,12,'W',{35,50},{23,23,199,99},41,3,'m'},

{"Gorlim, Betrayer of Barahir",(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x2L|MS1_CAUSE_3|MS1_BO_MANA)
			    ,(MF2_GOOD|MF2_UNIQUE|MF2_MAX_HP|MF2_INTELLIGENT|MF2_CHARM_SLEEP|
			     MF2_IM_POIS|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_COLD|MF2_IM_ELEC)
			    ,(MS2_BO_WATE),(NONE8)
		       ,7000,40,20,120,12,'p',{80,20},{218,218,230,230},41,3,'m'},

{"Solar"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_TELE_TO
			      |MS1_CAUSE_2|MS1_BO_MANA)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_ELEC|
			      MF2_GOOD|MF2_INTELLIGENT)
			    ,(MS2_S_ANGEL|MS2_RAZOR),(NONE8)
			    ,15000,255,30,140,13,'A',{120,30}
			    ,{217,217,218,218},41,6,'n'},

{"Glabrezu"		    ,(MF1_MV_ATT_NORM|MF1_HAS_90|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x9L|MS1_S_DEMON|MS1_BO_FIRE)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,1750,80,20,40,11,'&',{40,15},{17,17,0,0},41,2,'n'},

{"Ren the Unclean"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_MV_INVIS)
			    ,(0x3L|MS1_CAUSE_3|MS1_HOLD|MS1_FEAR|MS1_BLIND
			      |MS1_S_MONSTER|MS1_BA_FIRE|MS1_BO_FIRE)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_IM_FIRE|MF2_GOOD|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(MS2_BO_NETH),(NONE8)
			  ,13000,10,90,70,12,'W',{35,50},{23,23,199,99},41,3,'m'},

{"Nalfeshnee"		    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x9L|MS1_BLIND|MS1_CONF|MS1_BR_FIRE|MS1_S_DEMON)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,2000,80,20,50,11,'&',{45,15},{17,17,17,0},42,2,'n'},

{"Undead beholder"	    ,(MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_S_UNDEAD|MS1_SLOW|MS1_MANA_DRAIN|MS1_BO_MANA)
			    ,(MF2_ANIMAL|MF2_UNDEAD|MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_IM_POIS
			     |MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_ACID|MF2_IM_COLD|MF2_NO_INFRA)
			    ,(MS2_FORGET|MS2_MIND_BLAST|MS2_RAZOR|MS2_BRAIN_SMASH),(NONE8)
		     ,4000,10,30,100,12,'e',{90,30},{223,224,225,226},42,4,'n'},

{"Dread"		    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_CARRY_OBJ|
			     MF1_HAS_60|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			    ,(0xFL|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND|MS1_CONF)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_GROUP|
			     MF2_IM_POIS),(MS2_BO_NETH),(NONE8)
			    ,600,10,20,30,12,'G',{25,20},{235,235,80,0},35,1,'n'},

{"Mumak"		    ,(MF1_MV_ATT_NORM),(NONE8),(MF2_ANIMAL|MF2_GROUP)
			    ,(NONE8),(NONE8)
			 ,2100,100,20,55,11,'q',{90,10},{227,227,233,0},43,2,'n'},

{"Ancient multi-hued dragon",(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|
			      MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x5L|MS1_BR_POIS|MS1_BR_ELEC|MS1_BR_ACID|MS1_BR_COLD|
			      MS1_BR_FIRE|MS1_FEAR|MS1_CONF|MS1_BLIND)
			    ,(MF2_IM_COLD|MF2_IM_ACID|MF2_IM_POIS|MF2_IM_ELEC|
			      MF2_IM_FIRE|MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			  ,13000,70,20,100,12,'D',{52,40},{57,57,42,0},43,1,'n'},

{"Ethereal dragon"	   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90|MF1_HAS_4D2|
			    MF1_HAS_2D2|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_MV_INVIS|MF1_THRO_WALL)
			   ,(0x5L|MS1_CONF|MS1_BLIND)
			   ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP)
			   ,(MS2_BR_CONF),(MS3_BR_LITE|MS3_BR_DARK)
			,11000,15,25,100,12,'D',{52,40},{57,57,42,0},43,2,'n'},

{"Ji Indur Dawndeath"	   ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM|MF1_MV_INVIS)
			    ,(0x3L|MS1_BA_FIRE|MS1_CAUSE_3|MS1_HOLD|
			      MS1_FEAR|MS1_BLIND|MS1_S_UNDEAD)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_IM_FIRE|MF2_GOOD|MF2_IM_POIS|MF2_NO_INFRA)
			    ,(MS2_BA_NETH)
			    ,(NONE8)
			  ,12000,10,90,70,12,'W',{35,50},{23,23,199,0},43,4,'m'},

{"Marilith"		    ,(MF1_MV_ATT_NORM|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x9L|MS1_CAUSE_2|MS1_BLIND|MS1_S_DEMON)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			    ,5000,80,20,75,12,'&',{40,30},{19,19,19,19},43,2,'f'},

{"Quaker, Master of Earth"  ,(MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_ATT_NORM),(0x6L|MS1_BO_ACID)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ELEC|
			      MF2_CHARM_SLEEP|MF2_HURT_ROCK|MF2_EVIL|MF2_MAX_HP|MF2_NO_INFRA|
			      MF2_DESTRUCT|MF2_UNIQUE),(MS2_BA_ACID),(NONE8)
			,6000,90,10,97,11,'E',{90,20},{212,235,235,235},43,4,'m'},

{"Balor"		    ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_1D2|
			      MF1_CARRY_OBJ|MF1_THRO_DR)
			   ,(0x4L|MS1_CONF|MS1_BLIND|MS1_S_DEMON|MS1_BR_FIRE)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP)
			    ,(NONE8),(NONE8)
			,8000,80,20,50,12,'&',{60,30},{101,22,101,23},44,3,'n'},

{"Ariel, Queen of Air"	    ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_THRO_CREAT),(0x5L|MS1_BA_COLD)
			    ,(MF2_EVIL|MF2_IM_POIS|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_COLD|
			      MF2_IM_ELEC|MF2_IM_ACID|MF2_IM_POIS|MF2_MAX_HP|MF2_UNIQUE|
			      MF2_NO_INFRA),(MS2_BA_ELEC|MS2_BO_ELEC),(NONE8)
			,8000,50,12,50,13,'E',{60,45},{22,89,22,89},44,4,'f'},

{"11-headed hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x4L|MS1_FEAR|MS1_BO_FIRE|MS1_BA_FIRE|MS1_BR_FIRE)
			    ,(MF2_ANIMAL|MF2_IM_FIRE),(MS2_BO_PLAS),(NONE8)
		      ,6000,20,20,100,12,'R',{100,18},{107,107,107,107},44,2,'n'},

{"Patriarch"		    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_HOLD|MS1_BLIND|MS1_S_UNDEAD)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(MS2_HEAL|MS2_BRAIN_SMASH|MS2_RAZOR|MS2_S_SUMMON),(NONE8)
			,5000,10,20,60,12,'p',{40,20},{17,17,18,0},44,2,'m'},

{"Dreadmaster"		   ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_1D2|
			     MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS)
			   ,(0x9L|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND
			     |MS1_CONF|MS1_S_UNDEAD)
			   ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP|
			     MF2_INTELLIGENT|MF2_IM_POIS)
			   ,(MS2_BO_NETH|MS2_RAZOR|MS2_TELE_LEVEL),(NONE8)
			,8000,10,20,100,12,'G',{60,20},{235,235,80,80},44,2,'n'},

{"Drolem"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(0x6L|MS1_BLIND|MS1_CONF|
			     MS1_SLOW|MS1_BR_POIS)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_IM_FIRE|
			     MF2_IM_POIS|MF2_IM_ELEC|MF2_MAX_HP|MF2_NO_INFRA)
			    ,(MS2_ARROW_2),(NONE8)
		      ,12000,30,25,130,12,'g',{100,30},{48,48,238,238},44,3,'n'},

{"Scatha the Worm"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BR_COLD|MS1_CAUSE_3|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
			,17000,70,20,130,12,'D',{150,12},{56,56,56,276},44,2,'m'},

{"Dwar, Dog Lord of Waw"    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BA_FIRE|MS1_CAUSE_3|MS1_HOLD|
			      MS1_FEAR|MS1_BLIND|MS1_S_UNDEAD)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_IM_FIRE|MF2_GOOD|MF2_INTELLIGENT|MF2_IM_POIS|
			     MF2_NO_INFRA),(MS2_BA_NETH|MS2_S_SUMMON|MS2_S_HOUND),(NONE8)
			 ,13000,10,90,90,12,'W',{40,50},{23,23,199,99},44,3,'m'},

{"Smaug the Golden"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BR_FIRE|MS1_CAUSE_3|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
			 ,19000,70,20,100,12,'D',{150,13},{56,56,56,276},45,2,'m'},

{"Dracolich"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_2D2|MF1_HAS_4D2|
			     MF1_CARRY_OBJ|MF1_PICK_UP)
			    ,(0x6L|MS1_FEAR|MS1_CONF|MS1_BO_MANA|MS1_BR_COLD)
			    ,(MF2_EVIL|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_UNDEAD|
			    MF2_MAX_HP|MF2_DRAGON|MF2_IM_POIS|MF2_NO_INFRA),(NONE8),(NONE8)
			,18000,30,25,120,12,'D',{70,50},{57,57,236,236},46,2,'n'},

{"Greater titan"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x3L|MS1_TELE_TO)
			    ,(MF2_EVIL|MF2_GIANT|MF2_MAX_HP|MF2_INTELLIGENT)
			    ,(MS2_S_SUMMON|MS2_HEAL),(NONE8)
		      ,13500,15,30,125,12,'P',{75,50},{269,269,269,269},46,3,'n'},

{"Dracolisk"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_4D2|MF1_CARRY_OBJ)
			    ,(0x6L|MS1_HOLD|MS1_FEAR|MS1_BR_FIRE)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_DRAGON|MF2_IM_FIRE|
			     MF2_IM_ACID),(MS2_BR_NETH),(NONE8)
			,14000,30,25,120,12,'H',{70,50},{39,39,48,146},46,2,'n'},

{"Death mold"		    ,(MF1_MV_ONLY_ATT),(NONE8),(MF2_IM_FIRE|MF2_IM_POIS|MF2_MINDLESS|
			     MF2_IM_COLD|MF2_IM_ACID|MF2_IM_ELEC|MF2_ANIMAL|MF2_EVIL)
			    ,(NONE8),(NONE8)
		      ,1000,0,200,60,14,'m',{200,10},{257,257,257,202},47,1,'n'},

{"Itangast the Fire Drake"  ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BR_FIRE|MS1_CAUSE_3|MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
		       ,20000,70,20,100,12,'D',{150,15},{56,56,276,277},47,4,'m'},

{"Glaurung, Father of the Dragons"
			    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x5L|MS1_BR_FIRE|MS1_CAUSE_3|MS1_CONF|MS1_S_DRAGON)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
		      ,25000,70,20,120,12,'D',{110,25},{272,272,279,279},48,2,'m'},

{"Master mystic"	   ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_MV_INVIS)
			   ,(0x3L),(MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_POIS|MF2_IM_ELEC|
			    MF2_IM_ACID|MF2_MAX_HP|MF2_CHARM_SLEEP)
			   ,(MS2_HEAL|MS2_S_SPIDER),(NONE8)
			,6000,5,30,60,13,'p',{20,55},{266,266,264,265},50,3,'m'},

{"Muar, the Balrog",	     (MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
			      MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
			    ,(0x4L|MS1_FEAR|MS1_S_UNDEAD|MS1_BR_FIRE|MS1_CONF|MS1_S_DEMON)
			    ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(NONE8)
		       ,30000,80,20,100,12,'&',{50,60},{104,78,214,0},50,3,'m'},

{"Nightwing"		    ,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x4L|MS1_FEAR|MS1_S_UNDEAD|MS1_BLIND|MS1_BO_MANA)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|MF2_CHARM_SLEEP|
			     MF2_INTELLIGENT|MF2_GOOD|MF2_NO_INFRA)
			    ,(MS2_BRAIN_SMASH|MS2_RAZOR|MS2_BA_NETH|MS2_BO_NETH)
			    ,(NONE8)
		      ,6000,10,20,120,12,'W',{60,30},{172,172,230,230},50,4,'n'},

{"Nether hound"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(MS2_BR_LIFE)
			    ,(NONE8)
		       ,5000,0,30,100,12,'Z',{60,10},{39,39,39,58},51,2,'n'},

{"Time hound"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(0x8L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(NONE8)
			    ,(MS3_BR_TIME)
			    ,5000,0,30,100,13,'Z',{60,10},{39,39,39,58},51,4,'n'},

{"Plasma hound"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP|MF2_IM_FIRE),(NONE8)
			    ,(MS3_BR_PLAS)
			    ,5000,0,30,100,12,'Z',{60,10},{39,39,39,58},51,2,'n'},

{"Demonic quylthulg"	    ,(MF1_MV_INVIS),(0x2L|MS1_S_DEMON|MS1_BLINK|MS1_TELEPORT)
			    ,(MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_ANIMAL|MF2_EVIL|MF2_MINDLESS)
                            ,(NONE8),(NONE8)
			    ,3000,0,20,1,12,'Q',{60,8},{0,0,0,0},51,1,'n'},

{"Great storm wyrm"	    ,(MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x6L|MS1_BR_ELEC|MS1_FEAR|MS1_BLIND
			      |MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_ELEC|MF2_CHARM_SLEEP
			     |MF2_MAX_HP|MF2_GOOD),(NONE8),(NONE8)
			  ,17000,80,30,150,12,'D',{50,60},{57,57,57,277},51,2,'n'},

{"Baphomet the Minotaur Lord" ,(MF1_HAS_4D2|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_MV_ATT_NORM)
			    ,(0x6L|MS1_SLOW|MS1_BO_MANA),(MF2_EVIL|MF2_CHARM_SLEEP|MF2_UNIQUE|
			     MF2_MAX_HP|MF2_GOOD|MF2_IM_POIS|MF2_IM_FIRE),(MS2_BO_PLAS|
			     MS2_ARROW_2|MS2_BA_ELEC),(MS3_BR_WALL)
		      ,18000,30,30,120,13,'H',{70,50},{282,282,212,212},51,4,'m'},

{"Harowen the Black Hand"   ,(MF1_HAS_4D2|MF1_HAS_1D2|MF1_CARRY_OBJ|MF1_MV_ATT_NORM)
			    ,(0x6L),(MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_UNIQUE|
			     MF2_GOOD|MF2_IM_POIS),(MS2_TRAP_CREATE),(NONE8)
			,20000,0,40,90,14,'p',{50,50},{258,259,260,261},52,3,'m'},

{"Hoarmurath of Dir",	(MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_THRO_DR|MF1_MV_ATT_NORM)
			    ,(0x3L|MS1_BA_COLD|MS1_CAUSE_3|MS1_HOLD
			      |MS1_BO_COLD|MS1_FEAR|MS1_BLIND|MS1_S_UNDEAD)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			      |MF2_HURT_LITE|MF2_GOOD|MF2_INTELLIGENT|MF2_IM_POIS|
			     MF2_NO_INFRA),(MS2_RAZOR|MS2_BA_NETH|MS2_MIND_BLAST),(NONE8)
		       ,40000,10,90,100,12,'W',{50,50},{212,23,199,99},52,3,'m'},

{"Grand master mystic"	  ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_MV_INVIS)
			  ,(0x2L),(MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_POIS|MF2_IM_ELEC|
			   MF2_IM_ACID|MF2_MAX_HP|MF2_CHARM_SLEEP)
			  ,(MS2_HEAL|MS2_S_HOUND|MS2_S_SPIDER|MS3_S_REPTILE),(NONE8)
		      ,15000,5,30,80,13,'p',{40,55},{263,266,264,265},53,3,'m'},

{"Khamul the Easterling", (MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|
			  MF1_MV_ATT_NORM)
			,(0x2L|MS1_BO_MANA|MS1_BA_FIRE|MS1_CAUSE_3|MS1_HOLD|
			 MS1_FEAR|MS1_BLIND|MS1_S_UNDEAD|MS1_BA_COLD)
		       ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP|MF2_UNIQUE
			 |MF2_HURT_LITE|MF2_IM_FIRE|MF2_IM_POIS|MF2_IM_ACID|MF2_GOOD
			 |MF2_INTELLIGENT|MF2_NO_INFRA)
			,(MS2_BA_NETH|MS2_TELE_LEVEL|MS2_RAZOR),(NONE8)
		      ,50000,10,90,100,12,'W',{70,50},{212,23,199,199},53,3,'m'},

{"Ethereal hound"	   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_MV_INVIS|MF1_THRO_WALL)
			   ,(0x5L),(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP)
			   ,(MS2_BR_LIFE),(NONE8)
		       ,5000,0,30,100,12,'Z',{60,15},{39,39,39,58},54,3,'n'},

{"Great ice wyrm"	   ,(MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x6L|MS1_BR_COLD|MS1_FEAR|MS1_BLIND
			      |MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_COLD|MF2_CHARM_SLEEP
			     |MF2_MAX_HP|MF2_GOOD),(NONE8),(NONE8)
			,20000,80,30,170,12,'D',{50,60},{57,57,271,277},54,2,'n'},

{"The Phoenix"          ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_2D2)
                        ,(0x3L|MS1_BO_FIRE|MS1_BA_FIRE|MS1_BR_FIRE)
			,(MF2_ANIMAL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_POIS|
			 MF2_IM_ACID|MF2_GOOD|MF2_UNIQUE|MF2_MAX_HP),(MS2_BO_PLAS)
			,(MS3_BR_LITE|MS3_BR_PLAS)
 		      ,40000,0,60,130,12,'B',{36,100},{251,251,220,220},54,3,'n'},

{"Nightcrawler"		,(MF1_MV_ATT_NORM|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR)
			,(0x4L|MS1_FEAR|MS1_S_UNDEAD|MS1_BLIND|MS1_BO_MANA)
			,(MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|MF2_IM_FIRE|MF2_CHARM_SLEEP|
			 MF2_INTELLIGENT|MF2_GOOD|MF2_NO_INFRA),(MS2_BRAIN_SMASH|MS2_BA_NETH|
			 MS2_BO_NETH|MS2_BR_LIFE),(NONE8)
		      ,8000,10,20,160,12,'W',{80,60},{254,254,255,255},54,4,'n'},

{"Hand druj"		,(MF1_MV_ONLY_ATT)
			,(0x1L|MS1_FEAR|MS1_BLIND|MS1_CONF|MS1_CAUSE_3)
			,(MF2_EVIL|MF2_NO_INFRA|MF2_IM_COLD|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_UNDEAD
			  |MF2_INTELLIGENT|MF2_IM_POIS|MF2_MINDLESS)
			,(MS2_DARKNESS|MS2_FORGET|MS2_TELE_AWAY),(NONE8)
			,12000,10,20,110,13,'s',{30,20},{0,0,0,0},55,4,'n'},

{"Eye druj"		,(MF1_MV_ONLY_ATT)
			,(0x1L|MS1_S_UNDEAD|MS1_BO_MANA)
			,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_MAX_HP|MF2_IM_COLD
			  |MF2_IM_POIS|MF2_IM_FIRE|MF2_INTELLIGENT|MF2_MINDLESS)
			,(MS2_BO_NETH|MS2_BA_NETH),(NONE8)
			,24000,10,20,90,13,'s',{40,25},{246,246,0,0},55,4,'n'},

{"Skull druj"		,(MF1_MV_ONLY_ATT)
			,(0x1L|MS1_S_UNDEAD|MS1_SLOW)
			,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_MAX_HP|MF2_IM_COLD
			  |MF2_IM_POIS|MF2_IM_FIRE|MF2_INTELLIGENT|MF2_MINDLESS)
			,(MS2_MIND_BLAST|MS2_TRAP_CREATE|MS2_BO_NETH|MS2_BO_PLAS
			  |MS2_BRAIN_SMASH|MS2_RAZOR|MS2_BA_WATE),(NONE8)
			,25000,10,20,120,13,'s',{50,27},{247,236,248,249},55,
									4,'n'},

{"Chaos vortex"		     ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L)
			    ,(MF2_CHARM_SLEEP|MF2_MINDLESS),(MS2_BR_CHAO),(NONE8)
			    ,4000,0,100,80,14,'v',{32,20},{0,0,0,0},55,1,'n'},

{"Aether vortex"	    ,(MF1_MV_ATT_NORM|MF1_MV_75),(0x6L|MS1_BR_FIRE|MS1_BR_COLD|
			    MS1_BR_POIS|MS1_BR_ACID|MS1_BR_ELEC)
			    ,(MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_POIS|
			      MF2_IM_ELEC|MF2_MINDLESS)
			    ,(MS2_BR_SHAR|MS2_BR_SOUN|MS2_BR_CHAO|MS2_BR_CONF
			      |MS2_BR_LIFE|MS2_BR_NETH)
			    ,(MS3_BR_TIME|MS3_BR_WALL|MS3_BR_SLOW|MS3_BR_LITE
			      |MS3_BR_DARK|MS3_BR_PLAS|MS3_BR_GRAV)
		       ,4500,0,100,40,13,'v',{32,20},{242,239,240,241},55,2,'n'},

{"The Lernean Hydra"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_GOLD|MF1_HAS_4D2|MF1_HAS_2D2|
			     MF1_HAS_1D2|MF1_THRO_CREAT)
			    ,(0x3L|MS1_FEAR|MS1_BA_FIRE|MS1_BO_FIRE|
			     MS1_BR_FIRE|MS1_BR_POIS)
			    ,(MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_POIS|MF2_UNIQUE|MF2_ANIMAL|
			     MF2_INTELLIGENT|MF2_MAX_HP)
			    ,(MS2_BO_PLAS|MS2_BA_POIS),(MS3_S_REPTILE)
		    ,20000,20,20,140,12,'R',{100,43},{250,250,251,251},55,2,'n'},

{"Thuringwethil"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
			     MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90)
			    ,(0x3L|MS1_CAUSE_3|MS1_MANA_DRAIN|MS1_FEAR|MS1_HOLD|
			     MS1_BLIND)
			    ,(MF2_UNDEAD|MF2_EVIL|MF2_MAX_HP|MF2_CHARM_SLEEP|MF2_INTELLIGENT|
			     MF2_IM_COLD|MF2_HURT_LITE|MF2_NO_INFRA|MF2_UNIQUE|MF2_GOOD|
			    MF2_IM_POIS),(MS2_BA_NETH|MS2_RAZOR|MS2_BRAIN_SMASH),(NONE8)
		      ,23000,10,20,145,13,'V',{100,40},{48,216,216,198},55,4,'f'},

{"Great hell wyrm"	    ,(MF1_HAS_1D2|MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x6L|MS1_BR_FIRE|MS1_FEAR|MS1_BLIND
			      |MS1_CONF)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD)
			    ,(NONE8),(NONE8)
			,23000,40,40,170,12,'D',{90,60},{57,57,271,277},55,2,'n'},

{"Draconic quylthulg"	     ,(MF1_MV_INVIS)
			    ,(0x2L|MS1_S_DRAGON|MS1_BLINK|MS1_TELEPORT),(MF2_MAX_HP|MF2_ANIMAL|MF2_EVIL|MF2_MINDLESS
			     |MF2_CHARM_SLEEP),(NONE8),(NONE8)
			    ,5500,0,20,1,12,'Q',{90,8},{0,0,0,0},55,3,'n'},

{"Fundin Bluecloak"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_MV_ATT_NORM|MF1_THRO_DR)
			    ,(0x4L|MS1_CAUSE_3|MS1_FEAR|MS1_BLIND|MS1_CONF)
			    ,(MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD|MF2_IM_POIS|MF2_IM_COLD|
			     MF2_IM_FIRE|MF2_IM_ACID|MF2_IM_ELEC|MF2_UNIQUE),(MS2_FORGET|
			     MS2_RAZOR|MS2_HEAL|MS2_S_SUMMON|MS2_BRAIN_SMASH),(NONE8)
		     ,20000,10,25,195,13,'h',{100,48},{212,218,218,218},56,2,'m'},

{"Uriel, Angel of Fire"	    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x2L|MS1_BLIND|MS1_TELE_TO|MS1_BR_FIRE
			      |MS1_BA_FIRE|MS1_BO_FIRE|MS1_BO_MANA)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_ELEC|
			      MF2_GOOD|MF2_INTELLIGENT|MF2_MAX_HP|MF2_UNIQUE)
			    ,(MS2_S_ANGEL),(NONE8)
			    ,25000,10,40,160,13,'A',{220,25}
			    ,{220,103,212,212},56,3,'m'},


{"Azriel, Angel of Death"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x2L|MS1_BLIND|MS1_TELE_TO|MS1_BO_MANA)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_ELEC|
			      MF2_GOOD|MF2_INTELLIGENT|MF2_MAX_HP|MF2_UNIQUE)
			    ,(MS2_S_ANGEL|MS2_BR_LIFE|MS2_BO_NETH|MS2_BA_NETH)
			    ,(NONE8)
			    ,30000,10,40,170,13,'A',{240,25}
			    ,{202,260,212,212},57,3,'m'},

{"Ancalagon the Black"	    ,(MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_2D2|MF1_THRO_DR|
			      MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_BR_ACID|MS1_FEAR|MS1_BLIND|MS1_CONF|MS1_S_DRAGON)
			    ,(MF2_EVIL|MF2_DRAGON|MF2_IM_FIRE|MF2_IM_ACID|
			      MF2_UNIQUE|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD)
			    ,(NONE8),(MS3_S_ANCIENTD)
		      ,30000,70,20,125,12,'D',{110,70},{273,274,275,281},58,3,'m'},

{"Nightwalker"		    ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x4L|MS1_FEAR|MS1_S_UNDEAD|MS1_BLIND|MS1_BO_MANA)
			    ,(MF2_EVIL|MF2_UNDEAD|MF2_IM_COLD|MF2_IM_POIS|MF2_IM_ELEC|MF2_GOOD
			     |MF2_IM_FIRE|MF2_CHARM_SLEEP|MF2_INTELLIGENT|MF2_NO_INFRA)
			    ,(MS2_BRAIN_SMASH|MS2_BA_NETH|MS2_BO_NETH),(NONE8)
		      ,15000,10,20,175,13,'W',{50,65},{256,256,257,257},59,4,'n'},

{"Gabriel, the Messenger"   ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_PICK_UP|MF1_CARRY_OBJ|
			      MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x2L|MS1_BLIND|MS1_TELE_TO|MS1_BO_MANA)
			    ,(MF2_IM_POIS|MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_ACID|MF2_IM_ELEC|
			      MF2_GOOD|MF2_INTELLIGENT|MF2_MAX_HP|MF2_UNIQUE)
			    ,(MS2_S_ANGEL),(NONE8)
			    ,35000,10,40,180,13,'A',{140,55}
			    ,{230,103,212,212},59,3,'m'},

{"Saruman of Many Colours"  ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
			      MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
			    ,(0x2L|MS1_FEAR|MS1_BLIND|MS1_S_UNDEAD|MS1_S_DEMON|MS1_S_DRAGON
			     |MS1_CONF|MS1_TELEPORT|MS1_BA_FIRE|MS1_BA_COLD)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_COLD
			      |MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD|MF2_IM_POIS|MF2_INTELLIGENT)
			     ,(MS2_RAZOR|MS2_BA_WATE|MS2_BA_ACID|MS2_TELE_AWAY|MS2_FORGET|
			       MS2_BO_ICEE|MS2_MIND_BLAST|MS2_TRAP_CREATE|MS2_HEAL|MS2_HASTE)
			     ,(NONE8)
			     ,35000,0,100,100,12
			     ,'p',{100,50},{230,230,23,23},60,1,'m'},

{"Dreadlord"		  ,(MF1_MV_ATT_NORM|MF1_MV_20|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_2D2|
			   MF1_HAS_60|MF1_THRO_WALL|MF1_PICK_UP|MF1_MV_INVIS|MF1_HAS_1D2)
			   ,(0x4L|MS1_HOLD|MS1_MANA_DRAIN|MS1_BLIND|
			    MS1_S_UNDEAD|MS1_CONF)
			   ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP|
			    MF2_IM_POIS),(MS2_BA_NETH),(NONE8)
		      ,20000,10,20,150,12,'G',{100,27},{235,235,81,81},62,2,'n'},

{"The Cat Lord"           ,(MF1_MV_ATT_NORM|MF1_MV_INVIS|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_4D2)
                          ,(0x3L|MS1_TELE_TO),(MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD|MF2_UNIQUE|
			   MF2_IM_FIRE|MF2_IM_COLD|MF2_IM_POIS),(NONE8),(NONE8)
		      ,30000,0,100,200,13,'f',{80,60},{269,181,260,265},64,3,'m'},

{"Jabberwock"		    ,(MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|MF1_MV_ATT_NORM)
			    ,(0x5L)
			    ,(MF2_ANIMAL|MF2_MAX_HP),(MS2_BR_CHAO|MS2_RAZOR)
			    ,(NONE8)
			   ,19000,255,35,125,13,'J',{80,40},{212,212,212,212},
			     65,4,'n'},

{"Chaos hound"		    ,(MF1_MV_ATT_NORM),(0x5L)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP),(MS2_BR_CHAO)
			    ,(NONE8)
			  ,10000,0,30,100,12,'Z',{60,30},{39,39,39,58},65,1,'n'},

{"Great Wyrm of Chaos"	    ,(MF1_HAS_1D2|MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x3L|MS1_FEAR|MS1_BLIND|MS1_CONF|
			     MS1_S_DRAGON),(MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD)
			    ,(MS2_BR_CHAO|MS2_BR_DISE),(NONE8)
		       ,29000,20,40,170,12,'D',{65,70},{273,273,274,280},67,2,'n'},

{"Great Wyrm of Law"	    ,(MF1_HAS_1D2|MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x3L|MS1_FEAR|MS1_BLIND|MS1_CONF|
			     MS1_S_DRAGON),(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD)
			    ,(MS2_BR_SHAR|MS2_BR_SOUN),(NONE8)
		      ,29000,255,40,170,12,'D',{70,65},{273,273,274,280},67,2,'n'},

{"Great Wyrm of Balance"    ,(MF1_HAS_1D2|MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_90|MF1_HAS_60|
			      MF1_MV_ATT_NORM),(0x3L|MS1_FEAR|MS1_BLIND|MS1_S_DRAGON
			      |MS1_CONF)
			    ,(MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP|MF2_GOOD)
			    ,(MS2_BR_SHAR|MS2_BR_SOUN|MS2_BR_CHAO|MS2_BR_DISE),
			     (MS3_S_ANCIENTD)
		      ,31000,255,40,170,12,'D',{70,70},{273,273,274,280},67,4,'n'},

{"Tselakus, the Dreadlord" ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_2D2|
			   MF1_THRO_WALL|MF1_MV_INVIS|MF1_HAS_1D2)
			   ,(0x3L|MS1_HOLD|MS1_BLIND|MS1_CONF)
			   ,(MF2_UNDEAD|MF2_EVIL|MF2_IM_COLD|MF2_NO_INFRA|MF2_CHARM_SLEEP|MF2_MAX_HP|
			    MF2_IM_POIS|MF2_GOOD|MF2_UNIQUE)
			   ,(MS2_BA_NETH),(MS3_S_GUNDEAD|MS3_DARK_STORM|MS3_S_WRAITH)
		      ,35000,10,20,150,13,'G',{100,67},{81,81,212,212},68,2,'m'},

{"Tiamat, Celestial Dragon of Evil",(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|
			      MF1_THRO_DR|MF1_HAS_60|MF1_HAS_90|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x2L|MS1_BR_POIS|MS1_BR_ELEC|MS1_BR_ACID|MS1_BR_COLD|
			      MS1_BR_FIRE|MS1_FEAR|MS1_CONF|MS1_BLIND)
			    ,(MF2_IM_COLD|MF2_IM_ACID|MF2_IM_POIS|MF2_IM_ELEC|
			      MF2_IM_FIRE|MF2_EVIL|MF2_DRAGON|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_SPECIAL),(NONE8),(MS3_S_ANCIENTD)
		   ,45000,70,20,125,13,'D',{100,100},{274,275,275,281},70,4,'f'},

{"Black reaver"		    ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_HAS_2D2)
			    ,(0x3L|MS1_CONF|MS1_BLIND|MS1_HOLD|
			      MS1_CAUSE_3|MS1_MANA_DRAIN|MS1_TELE_TO|MS1_S_UNDEAD)
			    ,(MF2_UNDEAD|MF2_IM_POIS|MF2_IM_COLD|MF2_EVIL|MF2_MAX_HP|MF2_GOOD|
			      MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_INTELLIGENT|MF2_BREAK_WALL)
			    ,(MS2_BRAIN_SMASH|MS2_RAZOR|MS2_BA_NETH),(MS3_MANA_STORM)
		      ,23000,50,20,170,12,'L',{60,60},{230,230,81,81},71,3,'n'},

{"Master quylthulg"	    ,(MF1_MV_INVIS)
			,(0x2L|MS1_S_MONSTER|MS1_S_UNDEAD|MS1_S_DRAGON),(MF2_CHARM_SLEEP|MF2_MINDLESS|
			  MF2_ANIMAL|MF2_MAX_HP|MF2_EVIL),(MS2_S_SUMMON),(MS3_S_GUNDEAD|MS3_S_ANCIENTD)
		       ,12000,0,20,1,12,'Q',{100,20},{0,0,0,0},71,3,'n'},

{"Greater draconic quylthulg" ,(MF1_MV_INVIS)
			    ,(0x2L|MS1_BLINK|MS1_TELE_TO),(MF2_ANIMAL|MF2_EVIL|MF2_MAX_HP|MF2_MINDLESS|
			     MF2_CHARM_SLEEP) ,(NONE8),(MS3_S_ANCIENTD)
			,10500,0,20,1,12,'Q',{100,14},{0,0,0,0},71,3,'n'},

{"Greater rotting quylthulg",(MF1_MV_INVIS)
			    ,(0x2L|MS1_BLINK|MS1_TELE_TO),(MF2_ANIMAL|MF2_EVIL|MF2_MINDLESS|
			     MF2_CHARM_SLEEP|MF2_MAX_HP),(NONE8),(MS3_S_GUNDEAD)
			,10500,0,20,1,12,'Q',{100,14},{0,0,0,0},71,3,'n'},

{"Vecna, the Emperor Lich"  ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_CARRY_OBJ|MF1_HAS_4D2|MF1_HAS_2D2)
			    ,(0x2L|MS1_FEAR|MS1_CONF|MS1_BLIND|MS1_HOLD|
			      MS1_CAUSE_3|MS1_BO_MANA|MS1_TELE_TO|MS1_BLINK|MS1_S_UNDEAD)
			    ,(MF2_UNDEAD|MF2_IM_POIS|MF2_IM_COLD|MF2_EVIL|MF2_MAX_HP|
			      MF2_CHARM_SLEEP|MF2_NO_INFRA|MF2_UNIQUE|MF2_GOOD|MF2_INTELLIGENT)
			   ,(MS2_BA_NETH|MS2_TRAP_CREATE|MS2_RAZOR|MS2_S_SUMMON|MS2_BRAIN_SMASH)
			    ,(MS3_MANA_STORM)
		      ,30000,50,20,85,13,'L',{90,50},{181,201,214,181},72,2,'m'},

{"Omarax the Eye Tyrant"    ,(MF1_MV_ATT_NORM)
			    ,(0x2L|MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID|
			      MS1_MANA_DRAIN|MS1_BLIND|MS1_CONF|MS1_FEAR|MS1_SLOW)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_CHARM_SLEEP|MF2_MAX_HP|
			      MF2_IM_POIS|MF2_UNIQUE|MF2_INTELLIGENT)
			    ,(MS2_FORGET|MS2_MIND_BLAST|MS2_DARKNESS),(MS3_DARK_STORM)
		       ,16000,10,30,80,13,'e',{80,80},{223,224,225,226},73,4,'m'},

{"Ungoliant, the Unlight"   ,(MF1_MV_ATT_NORM|MF1_CARRY_OBJ|MF1_HAS_4D2)
			    ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_CONF|MS1_SLOW|MS1_BR_POIS)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_HURT_LITE|MF2_MAX_HP|
			      MF2_CHARM_SLEEP|MF2_GOOD|MF2_INTELLIGENT|MF2_IM_POIS)
			    ,(MS2_HEAL|MS2_S_SPIDER|MS2_DARKNESS)
			    ,(MS3_DARK_STORM|MS3_BR_DARK)
		     ,35000,80,8,160,12,'S',{130,100},{162,162,167,167},75,1,'f'},

{"Aether hound"		    ,(MF1_MV_ATT_NORM),(0x5L|MS1_BR_FIRE|MS1_BR_COLD|MS1_BR_POIS|
					    MS1_BR_ACID|MS1_BR_ELEC)
			    ,(MF2_ANIMAL|MF2_GROUP|MF2_CHARM_SLEEP|MF2_IM_FIRE|
			      MF2_IM_COLD|MF2_IM_ELEC|MF2_IM_POIS|MF2_IM_ACID)
			    ,(MS2_BR_CHAO|MS2_BR_SHAR|MS2_BR_SOUN|MS2_BR_CONF
			      |MS2_BR_DISE|MS2_BR_LIFE|MS2_BR_NETH)
			    ,(MS3_BR_WALL|MS3_BR_GRAV|MS3_BR_SLOW|
			      MS3_BR_PLAS|MS3_BR_TIME|MS3_BR_LITE|
			      MS3_BR_DARK)
			,10000,0,30,100,12,'Z',{60,30},{39,39,39,58},75,2,'n'},

{"The Mouth of Sauron"	    ,(MF1_MV_ATT_NORM|MF1_MV_INVIS|MF1_THRO_DR|MF1_HAS_4D2|MF1_HAS_1D2|
			     MF1_CARRY_OBJ),(0x2L|MS1_TELE_TO|MS1_CAUSE_3|MS1_BA_FIRE|
			     MS1_HOLD),(MF2_EVIL|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD|MF2_CHARM_SLEEP
			     |MF2_INTELLIGENT|MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_ELEC),
			     (MS2_TRAP_CREATE|MS2_BA_WATE|MS2_BO_PLAS|MS2_BA_NETH)
			    ,(MS3_MANA_STORM|MS3_DARK_STORM)
		    ,38000,10,60,100,13,'p',{10,100},{230,230,214,214},78,3,'m'},

{"The Emperor Quylthulg"    ,(MF1_MV_INVIS|MF1_CARRY_OBJ|MF1_HAS_4D2)
			    ,(0x2L),(MF2_ANIMAL|MF2_EVIL|MF2_MAX_HP|MF2_UNIQUE|MF2_CHARM_SLEEP)
			    ,(MS2_BRAIN_SMASH),(MS3_S_GUNDEAD|MS3_S_ANCIENTD)
		     ,20000,0,30,1,13,'Q',{50,100},{0,0,0,0},78,3,'n'},

{"Qlzqqlzuup, the Lord of Flesh", (MF1_MV_INVIS|MF1_CARRY_OBJ|MF1_HAS_4D2)
                            ,(0x1L|MS1_S_UNDEAD|MS1_S_DEMON|MS1_S_DRAGON|MS1_S_MONSTER)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_MAX_HP|MF2_CHARM_SLEEP)
			    ,(MS2_S_SUMMON|MS2_S_ANGEL|MS2_S_SPIDER|MS2_S_HOUND)
			    ,(MS3_S_REPTILE|MS3_S_ANT|MS3_S_GUNDEAD|MS3_S_ANCIENTD|MS3_S_UNIQUE|
			      MS3_S_WRAITH)
		      ,20000,0,30,1,13,'Q',{50,100},{0,0,0,0},78,3,'n'},

{"Murazor, the Witch-King of Angmar",
                       (MF1_HAS_4D2|MF1_HAS_2D2|MF1_CARRY_OBJ|MF1_HAS_1D2|MF1_THRO_DR|MF1_MV_ATT_NORM)
                            ,(0x2L|MS1_BO_MANA|MS1_CAUSE_3|MS1_HOLD|
			      MS1_FEAR|MS1_BLIND)
			      ,(MF2_EVIL|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_MAX_HP
				|MF2_IM_POIS|MF2_HURT_LITE|MF2_UNIQUE|MF2_GOOD|MF2_NO_INFRA
				|MF2_INTELLIGENT)
			      ,(MS2_BA_NETH|MS2_BRAIN_SMASH|MS2_TELE_AWAY|MS2_S_SUMMON)
			      ,(MS3_S_WRAITH|MS3_S_ANCIENTD|MS3_S_GUNDEAD)
                     ,42000,10,90,120,13,'W',{120,50},{212,23,199,199},80,3,'n'},

{"Pazuzu, Lord of Air"     ,(MF1_MV_ATT_NORM|MF1_THRO_DR|MF1_MV_INVIS|MF1_CARRY_OBJ|MF1_HAS_4D2)
                           ,(0x3L|MS1_BO_MANA),(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|
			    MF2_MAX_HP|MF2_IM_COLD|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_ACID|
			    MF2_IM_POIS|MF2_GOOD|MF2_UNIQUE),(MS2_BO_ELEC|MS2_BA_ELEC|
			    MS2_MIND_BLAST),(NONE8)
		      ,30000,10,40,125,14,'B',{100,55},{284,284,284,284},82,2,'m'},

{"Hell hound"              ,(MF1_MV_ATT_NORM|MF1_MV_20),(0x5L|MS1_BR_FIRE),
                            (MF2_ANIMAL|MF2_EVIL|MF2_MAX_HP|MF2_IM_FIRE|MF2_GROUP),(NONE8),(NONE8)
                           ,600,30,25,80,12,'C',{30,16},{107,107,107,0},83,4,'n'},
 
{"Cantoras, the Skeletal Lord",
			     (MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|
			      MF1_THRO_DR|MF1_HAS_2D2|MF1_HAS_1D2)
			    ,(0x1L|MS1_TELE_TO|MS1_BO_MANA|MS1_FEAR|MS1_SLOW)
                            ,(MF2_EVIL|MF2_UNIQUE|MF2_UNDEAD|MF2_CHARM_SLEEP|MF2_IM_COLD|MF2_NO_INFRA
			|MF2_MINDLESS|MF2_IM_POIS|MF2_IM_FIRE|MF2_MAX_HP|MF2_INTELLIGENT|MF2_SPECIAL)
			    ,(MS2_BA_WATE|MS2_RAZOR|MS2_BRAIN_SMASH|MS2_BO_ICEE|
			      MS2_BA_NETH),(MS3_S_GUNDEAD)
			    ,45000,80,20,120,14,'s',{150,45},{246,172,172,0},
								    84,2,'m'},

{"The Tarrasque"	    ,(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_THRO_DR|MF1_CARRY_OBJ)
			    ,(0x2L|MS1_BR_COLD)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|MF2_UNIQUE|MF2_GOOD)
			    ,(MS2_BR_DISE),(NONE8)
		     ,35000,20,50,185,13,'R',{85,95},{212,212,214,214},84,2,'n'},

{"Lungorthin, the Balrog of White Fire",
			     (MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
                              MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
                            ,(0x4L|MS1_FEAR|MS1_BLIND|MS1_S_DEMON|
                      	      MS1_BR_FIRE|MS1_CONF)
                            ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_MAX_HP|
			      MF2_UNIQUE|MF2_GOOD),(NONE8),(MS3_S_GUNDEAD)
		     ,37000,80,20,125,13,'&',{80,95},{104,104,78,214},85,2,'m'},

{"Draugluin, Sire of All Werewolves",(MF1_MV_ATT_NORM|MF1_MV_20|MF1_PICK_UP|MF1_THRO_DR)
			    ,(0x3L|MS1_FEAR)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_IM_POIS|MF2_MAX_HP|MF2_GOOD)
			    ,(MS2_S_SUMMON|MS2_S_HOUND),(NONE8)
		     ,40000,90,80,90,13,'C',{100,70},{58,58,160,160},87,2,'m'},

{"Feagwath the Undead Sorceror",(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
                              MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
                            ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_S_DEMON
			      |MS1_TELEPORT|MS1_BA_FIRE|MS1_BO_MANA)
                            ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_COLD
			      |MF2_MAX_HP|MF2_UNIQUE|MF2_SPECIAL|MF2_IM_POIS|MF2_INTELLIGENT
			      |MF2_NO_INFRA|MF2_UNDEAD)
			     ,(MS2_BRAIN_SMASH|MS2_RAZOR|MS2_S_SUMMON),(MS3_MANA_STORM|
			      MS3_S_GUNDEAD)
			     ,45000,0,100,100,13
			     ,'L',{120,50},{230,230,23,23},90,3,'m'},

{"Carcharoth, the Jaws of Thirst",(MF1_MV_ATT_NORM|MF1_MV_20|MF1_PICK_UP|MF1_THRO_DR)
			    ,(0x4L|MS1_BR_FIRE|MS1_FEAR)
			    ,(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_IM_POIS|MF2_IM_FIRE|
			      MF2_MAX_HP|MF2_GOOD|MF2_CHARM_SLEEP)
			    ,(MS2_HEAL|MS2_BRAIN_SMASH|MS2_S_HOUND),(NONE8)
		     ,40000,10,80,110,13,'C',{150,50},{58,58,163,163},92,2,'m'},

{"Cerberus, Guardian of Hades",(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_CARRY_OBJ|MF1_THRO_DR)
			    ,(0x3L|MS1_BR_FIRE),(MF2_ANIMAL|MF2_EVIL|MF2_UNIQUE|MF2_IM_FIRE|
			     MF2_MAX_HP|MF2_GOOD|MF2_CHARM_SLEEP)
			    ,(MS2_BR_LIFE|MS2_S_HOUND),(MS3_DARK_STORM)
		  ,40000,10,50,160,13,'C',{100,100},{220,220,220,220},94,1,'n'},

{"Gothmog, the High Captain of Balrogs",
			     (MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
                              MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
                            ,(0x3L|MS1_FEAR|MS1_BLIND|MS1_S_DEMON|
                              MS1_BR_FIRE|MS1_CONF)
                            ,(MF2_EVIL|MF2_DEMON|MF2_CHARM_SLEEP|MF2_IM_FIRE|
			      MF2_IM_ELEC|MF2_MAX_HP|MF2_UNIQUE|MF2_SPECIAL)
			    ,(NONE8),(MS3_S_GUNDEAD)
	             ,43000,0,100,140,13,'&',{100,80},{220,220,78,214},95,1,'m'},

{"Sauron, the Sorcerer",     (MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
                              MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR)
                            ,(0x2L|MS1_FEAR|MS1_BLIND|MS1_S_DEMON
                             |MS1_CONF|MS1_TELEPORT|MS1_BO_MANA|MS1_BA_FIRE)
                            ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_COLD
			      |MF2_MAX_HP|MF2_UNIQUE|MF2_SPECIAL|MF2_IM_POIS|MF2_QUESTOR|
			      MF2_INTELLIGENT)
			    ,(MS2_BRAIN_SMASH|MS2_BA_NETH|MS2_BO_ICEE|MS2_BO_PLAS|
			      MS2_BA_WATE|MS2_TELE_LEVEL|MS2_FORGET|MS2_DARKNESS|MS2_S_SUMMON|
			      MS2_RAZOR),(MS3_MANA_STORM|MS3_S_ANCIENTD|MS3_S_GUNDEAD)
			     ,50000,0,100,160,13
			     ,'p',{99,105},{270,270,214,214},99,1,'m'},

{"Morgoth, Lord of Darkness",(MF1_MV_ATT_NORM|MF1_HAS_4D2|MF1_HAS_2D2|MF1_HAS_1D2|
                              MF1_CARRY_OBJ|MF1_HAS_60|MF1_HAS_90|MF1_THRO_DR|MF1_WINNER)
                            ,(0x3L|MS1_BO_MANA)
                            ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_IM_FIRE|MF2_IM_ELEC|MF2_IM_COLD|
			     MF2_IM_POIS|MF2_IM_ACID|MF2_MAX_HP|MF2_UNIQUE|MF2_SPECIAL|
			     MF2_BREAK_WALL|MF2_DESTRUCT|MF2_INTELLIGENT)
			    ,(MS2_S_SUMMON|MS2_BRAIN_SMASH|MS2_BA_NETH),(MS3_S_WRAITH|
			      MS3_MANA_STORM|MS3_S_ANCIENTD|MS3_S_GUNDEAD|MS3_S_UNIQUE)
			    ,60000,0,100,150,14
		            ,'P',{180,95},{262,262,245,214},100,10,'m'},

{NULL                       ,(NONE8)
                            ,(NONE8)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_UNDEAD|MF2_UNIQUE|MF2_GOOD
			     |MF2_IM_POIS|MF2_IM_COLD|MF2_NO_INFRA),(NONE8)
                            ,(NONE8)
			    ,0,0,100,0,11
		            ,'@',{5,5},{0,0,0,0},32000,255,'n'}
};


