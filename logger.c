#include "logger.h"

FILE *open_write_file(const char *path) {
  FILE *logFile;

  logFile = fopen(path, "w");

  if (logFile == NULL) {
    fprintf(stdout, "ERROR. \n");
  }

  time_t rawtime;
  time(&rawtime);
  struct tm * timeinfo;

  timeinfo = localtime(&rawtime);
  fprintf (logFile, "The current local date and time: %s\n", asctime (timeinfo));
}

//Close log
void close_logFile() {
  fprintf(logFile, "The Server is being shutdown.");
  fclose(logFile);
}
