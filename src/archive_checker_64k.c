#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#define BUF_SIZE 1048576
#define PATH_MAX 4096

int main(int argc, char** argv) {
  int retval;
  char tarname[PATH_MAX];
  struct archive *a;
  struct archive_entry *entry;
  char buff[BUF_SIZE];
  size_t file_count = 0;
  unsigned int flg = 0;
  if (argc == 1) {
    printf("ERROR: No TAR file specified! Usage: %s <TAR>\n", argv[0]);
    exit(1);
  }
  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  
  strncpy(tarname, argv[1], PATH_MAX);
  printf("|    %s\n", tarname);
  retval = archive_read_open_filename(a, tarname, 1048576);
  if (retval != ARCHIVE_OK) {
    printf("ERROR: Can't open TAR file!\n");
    exit(1);
  }
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    retval = archive_read_data(a, buff, BUF_SIZE);
    assert (retval >= 0 && retval < BUF_SIZE);
  
    if (retval > 0)
      ++file_count;
   }
  
  printf("%d files can be extracted.\n", file_count);
  retval = archive_read_free(a);
  if (retval != ARCHIVE_OK) {
    printf("ERROR: Can't close the archive properly!\n");
    exit(1);
  }
  exit(0);
}
