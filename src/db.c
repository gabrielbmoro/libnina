#include "db.h"

static struct NINARegion *nina_regions = NULL;
static unsigned int nina_regions_length = 0;

static int getFileSize(char *filePath)
{
  FILE *arq;
  int ch, lines = 0;
  arq = fopen(filePath, "r");

  do {
    ch = fgetc(arq);
    if (ch == '\n')
      lines++;
  } while (ch != EOF);

  if (ch != '\n' && lines != 0) {
    lines++;
  }
  fclose(arq);
  return lines;
}

void LIBNINA_RegionsFree (void)
{
  if (nina_regions != NULL){
    free(nina_regions->source_filename);
    hdestroy_r(&nina_regions->htab);
    free(nina_regions);
  }
  nina_regions = NULL;
}

void LIBNINA_LoadRegionsFile (void)
{
  char fileNameTmp[300];

  //Free existing information
  LIBNINA_RegionsFree();

  //Get filename of the regions file
  char *filePath = getenv("NINA_CONFIG");
  if (filePath == NULL) return;

  //Count the number of lines
  int amountOfLines = getFileSize(filePath);

  //Open the regions file
  FILE *arq = fopen(filePath, "r");

  int i;
  //For each line of the file
  for (i = 0; i < amountOfLines; i++){
    //Read the line (this should be improved)
    long freqTmp;
    int start_lineTmp;
    int check = fscanf(arq, "%d,%ld,%s\n", &start_lineTmp, &freqTmp, fileNameTmp);
    if (check != 3){
      LOG(printf("LIBNINA: Ignore line %d of file %s\n", i+1, filePath));
      continue;
    }else{
      LOG(printf("[%d] [%ld] [%s]\n", start_lineTmp, freqTmp, fileNameTmp));
    }

    //Search for an existing region source file
    struct NINARegion *region = NULL;
    unsigned int j;
    for (j = 0; j < nina_regions_length; j++){
      LOG(printf("Search for %s in %s\n", fileNameTmp, nina_regions[j].source_filename));
      if (strstr(nina_regions[j].source_filename, fileNameTmp) != NULL){
	region = &nina_regions[j];
	LOG(printf("Found %s in %d (%s)\n", fileNameTmp, j, nina_regions[j].source_filename));
	break;
      }
    }
    // The region has not been found, create one
    if (region == NULL){
      LOG(printf("Region %s not found, create one\n", fileNameTmp));
      nina_regions_length++;
      nina_regions = realloc(nina_regions,
			     nina_regions_length * sizeof(struct NINARegion));
      region = &nina_regions[nina_regions_length-1];

      //A new region is born, define initial state
      region->source_filename = strdup(fileNameTmp);
      bzero(&region->htab, sizeof(struct hsearch_data));
      int check = hcreate_r(INIT_HASH_SIZE, &(region->htab));
      if (!check){
	perror(__func__);
	exit(1);
      }
    }

    //Region has been identified
    LOG(printf("Add %d line %ld freq to %s region\n", start_lineTmp, freqTmp, region->source_filename));

    //Add data to the hash table
    char aux[STR_HASH_KEY_SIZE];
    bzero(aux, STR_HASH_KEY_SIZE);
    snprintf(aux, STR_HASH_KEY_SIZE, "%d", start_lineTmp);
    ENTRY e, *ep = NULL;
    e.key = aux;
    e.data = NULL;

    LOG(printf("Search for key [%s] in %s\n", e.key, region->source_filename));
    hsearch_r (e, FIND, &ep, &region->htab);
    if (ep == NULL) {
      //This is okay, add it
      e.key = strdup(aux);
      long *d = malloc(sizeof(long));
      *d = freqTmp;
      e.data = d;
      LOG(printf("ENTER [%s]=%ld in %p\n", e.key, *(long*)e.data, &(region->htab)));
      if (!hsearch_r (e, ENTER, &ep, &region->htab)) {
	perror(__func__);
	exit(1);
      }
      //Check good insertion
      e.key = aux;
      e.data = NULL;
      if (hsearch_r (e, FIND, &ep, &region->htab)){
	LOG(printf("After ENTER, FIND returned [%s]=%ld\n", ep->key, *(long*)ep->data));
	if (*(long*)ep->data == freqTmp){
	  LOG(printf("Entered value equals the one that has been found, which is great.\n"));
	}else{
	  LOG(printf("Expected value after insertion is wrong. ERROR!\n"));
	  exit(1);
	}
      }else{
	LOG(printf("NOT FOUND after ENTER. ERROR!"));
	exit(1);
      }
    }else{
      //An entry has been found (PROBLEM!)
      LOG(printf("Another freq for the same region/line \n"));
      exit(1);
    }
  }
  LOG(printf("Final nina regions: %d\n", nina_regions_length));
  fclose(arq);
}

long LIBNINA_GetFrequency (char *source_filename, int start_line)
{
  LOG(printf("%s [%s] %d\n", __FUNCTION__, source_filename, start_line));
  //Search for an existing region source file
  struct NINARegion *region = NULL;
  unsigned int j;
  for (j = 0; j < nina_regions_length; j++){
    LOG(printf("%s Search for [%s] in %s\n", __FUNCTION__, source_filename, nina_regions[j].source_filename));
    if (strstr(source_filename, nina_regions[j].source_filename)){
      region = &nina_regions[j];
      LOG(printf("%s Found [%s] in %d (%s)\n", __FUNCTION__, source_filename, j, nina_regions[j].source_filename));
      break;
    }
  }
  // The region has not been found, ignore
  if (region == NULL){
    LOG(printf("Region with name [%s] not found.\n", source_filename));
    exit(1);
  }else{
    LOG(printf("Region %s found.\n", region->source_filename));
  }

  // Region has been found, find the frequency for that line number
  char aux[STR_HASH_KEY_SIZE];
  bzero(aux, STR_HASH_KEY_SIZE);
  snprintf(aux, STR_HASH_KEY_SIZE, "%d", start_line);
  ENTRY e, *ep = NULL;
  e.key = aux;
  e.data = NULL;

  hsearch_r (e, FIND, &ep, &region->htab);
  // The line number has not been found, ignore
  if (ep == NULL) {
    LOG(printf("%s The line number %d (%s) has not been found in source file %s.\n", __FUNCTION__, start_line, aux, region->source_filename));
    return -2;
  }
  LOG(printf("%s %s %d -> %ld\n", __FUNCTION__, source_filename, start_line, *(long*)ep->data));
  return *(long*)ep->data;
}

