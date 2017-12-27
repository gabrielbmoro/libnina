
/*
  From: https://www2.informatik.hu-berlin.de/~weber/slipOff/hashmap_c.html
*/

#include <stdio.h>
#include <stdlib.h>

/* this should be prime */
#define TABLE_STARTSIZE 1021

#define ACTIVE 1

typedef struct hEntry {
  long * data;
  int flags;
  unsigned long key;
} hEntry;

typedef struct s_hashmap{
  hEntry* table;
  long size, count;
} s_hashmap;

s_hashmap * hashmapCreate(int startsize);

void hashmapInsert(s_hashmap* hash, long * data, unsigned long key);

long* hashmapRemove(s_hashmap* hash, unsigned long key);

long * hashmapGet(s_hashmap* hash, unsigned long key);

long hashmapCount(s_hashmap* hash);

void hashmapDelete(s_hashmap* hash);
