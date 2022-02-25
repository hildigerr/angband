#!/usr/bin/awk -f

BEGIN { FS = ","; RS = "\n{"; x = -1 }
{
  gsub(/\x27\x2c\x27\x2c/, "COMMA\x2c", $0);
  gsub(/\x27\x22\x27\x2c/, "QUOTE\x2c", $0);
  gsub(/\x22/, "", $0);
  print "\nN:" x++ ":" $1;

  gsub(/\s/, "", $0);
  gsub(/L/, "", $2);
  gsub(/0x[0]+/, "0x", $2);
  gsub(/\x27/, "", $4);
  gsub(/\/\*[0-9]+\*\//, "", $5);
  gsub(/\x7b/, "", $14);
  gsub(/\x7d/, "", $15);
  gsub(/L\x7d/, "", $18);

  gsub(/TV_MISC/, "1", $3);
  gsub(/TV_CHEST/, "2", $3);
  gsub(/TV_SPIKE/, "3", $3);
  gsub(/TV_MIN_WEAR/, "10", $3);
  gsub(/TV_SHOT/, "10", $3);
  gsub(/TV_BOLT/, "11", $3);
  gsub(/TV_ARROW/, "12", $3);
  gsub(/TV_LITE/, "15", $3);
  gsub(/TV_BOW/, "20", $3);
  gsub(/TV_HAFTED/, "21", $3);
  gsub(/TV_POLEARM/, "22", $3);
  gsub(/TV_SWORD/, "23", $3);
  gsub(/TV_DIGGING/, "25", $3);
  gsub(/TV_BOOTS/, "30", $3);
  gsub(/TV_GLOVES/, "31", $3);
  gsub(/TV_CLOAK/, "32", $3);
  gsub(/TV_HELM/, "33", $3);
  gsub(/TV_SHIELD/, "34", $3);
  gsub(/TV_HARD_ARMOR/, "35", $3);
  gsub(/TV_SOFT_ARMOR/, "36", $3);
  gsub(/TV_MAX_ENCHANT/, "39", $3);
  gsub(/TV_AMULET/, "40", $3);
  gsub(/TV_RING/, "45", $3);
  gsub(/TV_STAFF/, "55", $3);
  gsub(/TV_WAND/, "65", $3);
  gsub(/TV_ROD/, "66", $3);
  gsub(/TV_SCROLL1/, "70", $3);
  gsub(/TV_SCROLL2/, "71", $3);
  gsub(/TV_POTION1/, "75", $3);
  gsub(/TV_POTION2/, "76", $3);
  gsub(/TV_FLASK/, "77", $3);
  gsub(/TV_FOOD/, "80", $3);
  gsub(/TV_MAGIC_BOOK/, "90", $3);
  gsub(/TV_PRAYER_BOOK/, "91", $3);
  gsub(/TV_GOLD/, "100", $3);
  gsub(/TV_INVIS_TRAP/, "101", $3);
  gsub(/TV_VIS_TRAP/, "102", $3);
  gsub(/TV_RUBBLE/, "103", $3);
  gsub(/V_OPEN_DOOR/, "104", $3);
  gsub(/TV_CLOSED_DOOR/, "105", $3);
  gsub(/TV_UP_STAIR/, "107", $3);
  gsub(/TV_DOWN_STAIR/, "108", $3);
  gsub(/TV_SECRET_DOOR/, "109", $3);
  gsub(/TV_STORE_DOOR/, "110", $3);

  print "I:" $4 ":" $3 ":" $7 ":" $5 ":" $8 ":" $9 ":" $16 ":" $6;
  print "A:" $16 "/" $17;
  print "P:" $12 ":" $14 "d" $15 ":" $10 ":" $11 ":" $13;
  print "F:" $2 ":" $18;
}
