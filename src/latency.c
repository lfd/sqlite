#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include "timing.h"

void measure_query(char *dbfile, char *basepath, char *query, int iterations) {
    struct stat s;
    char snum[3];
    char *err_msg = 0;
    tstamp_t *tstamps = sqlite3_malloc64(sizeof(tstamp_t)*iterations);
    sqlite3 *db;

    int rc = sqlite3_open(dbfile, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(db));
        sqlite3_close(db);

	exit(-1);
    }

    char *file = (char*)sqlite3_malloc64(strlen("query.sql") + strlen(query) + strlen(basepath) + 2);
    sprintf(file, "%s/query%s.sql", basepath, query);

    int fd = open(file, O_RDONLY);
    int status = fstat(fd, &s);
    char *sql = (char *)mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    tstamps[0] = get_tstamp();
    for (int count = 1; count < iterations; count++) {
      rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

	exit(-1);
      }
      tstamps[count]  = get_tstamp();
    }

    for (int count = 1; count < iterations; count++) {
      printf("%s\t%d\t%lu\t%lu\n", query, count, diff(tstamps[0], tstamps[count]),
	     diff(tstamps[count-1], tstamps[count]));
    }

    free(tstamps);
    sqlite3_close(db);
}


int main(int argc, char **argv) {
    if (argc < 5) {
      printf("Usage: %s <database> <querypath> iterations q1 q2 ... qn\n", argv[0]);
      exit(-1);
    }

    for (int query = 4; query < argc; query++) {
      measure_query(argv[1], argv[2], argv[query], strtol(argv[3], NULL, 10));
    }
}
