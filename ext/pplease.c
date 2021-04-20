/*
** 2013-05-15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This SQLite extension tries to please the user whenever this is
** desired. Based on the rot13 example in the sqlit source tree

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

/* Pleasify the world, because there is just no pleasing anyone anymore. */
static void pleasify(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  const unsigned char *zIn;
  int nIn, nOut;
  unsigned char *zOut;
  unsigned char *zToFree = 0;
  const char delimiters[] = ".!";
  char *token;

  assert( argc==1 );
  if( sqlite3_value_type(argv[0])==SQLITE_NULL ) return;

  zIn = (const unsigned char*)sqlite3_value_text(argv[0]);
  nIn = sqlite3_value_bytes(argv[0]);

  char *str = (char*)sqlite3_malloc64(nIn+1);
  strncpy(str, zIn, nIn+1);

  // Count number of of full stops '.' / exclamation marks '!' in string
  int i=0, count=0;
  for (; str[i] != '\0'; (str[i] == '.' || str[i] == '!') ? count++ : 0, i++);

  // "." -> ", please." => 8 extra chars per please
  nOut = nIn+8*count;
  zOut = zToFree = (unsigned char*)sqlite3_malloc64(nOut+1);
  zOut[0] = '\0';

  if (zOut==NULL){
    sqlite3_result_error_nomem(context);
    return;
  }

  token = strtok(str, delimiters);
  do {

      strncat(zOut, token, strlen(token));
      if (rand() % 100 >= 50) {
	strcat(zOut, ", please.");
      } else {
	strcat(zOut, ".");
      }
      token = strtok(NULL, delimiters);
  } while (token != NULL);

  sqlite3_result_text(context, (char*)zOut, nOut, SQLITE_TRANSIENT);
  sqlite3_free(zToFree);
}

/*
** Implement the please collating sequence so that if
**
**      x=y COLLATE please
**
** Then 
**
**      please(x)=please(y) COLLATE binary
*/
static int pleaseCollFunc(
  void *notUsed,
  int nKey1, const void *pKey1,
  int nKey2, const void *pKey2
){
  const char *zA = (const char*)pKey1;
  const char *zB = (const char*)pKey2;

  return(!strcmp(zA, zB));
}


#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_please_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  rc = sqlite3_create_function(db, "pplease", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS,
                   0, pleasify, 0, 0);
  if( rc==SQLITE_OK ){
    rc = sqlite3_create_collation(db, "pplease", SQLITE_UTF8, 0, pleaseCollFunc);
  }
  return rc;
}
