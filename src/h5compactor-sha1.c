#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include <hdf5.h>
#include <openssl/sha.h>
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
  
  char h5name[PATH_MAX];
  hid_t fapl, file, space, dcpl, lcpl, dset;
  hsize_t dims;
  
  unsigned char md[SHA_DIGEST_LENGTH];
  unsigned char hash[2*SHA_DIGEST_LENGTH];
  int i;
  if (argc == 1) {
    printf("ERROR: No TAR file specified! Usage: %s <TAR>\n", argv[0]);
    exit(1);
  }
  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  
  strncpy(tarname, argv[1], PATH_MAX);
  printf("|    %s\n", tarname);
  retval = archive_read_open_filename(a, tarname, BUF_SIZE);
  if (retval != ARCHIVE_OK) {
    printf("ERROR: Can't open TAR file!\n");
    exit(1);
  }
  
  assert((lcpl = H5Pcreate(H5P_LINK_CREATE)) >= 0);
  assert(H5Pset_create_intermediate_group(lcpl, 1) >= 0);
  
  assert((dcpl = H5Pcreate(H5P_DATASET_CREATE)) >= 0);
  assert(H5Pset_layout(dcpl, H5D_COMPACT) >= 0);
  
  assert((fapl = H5Pcreate(H5P_FILE_ACCESS)) >= 0);
  assert(H5Pset_libver_bounds(fapl, H5F_LIBVER_V18, H5F_LIBVER_V110) >= 0);
  
  snprintf(h5name, PATH_MAX, "%s.compactor-sha1.h5", tarname);
  printf("|    %s\n", h5name);
  assert((file = H5Fcreate(h5name, H5F_ACC_TRUNC, H5P_DEFAULT, fapl))
         >= 0);
  
  assert(H5Pclose(fapl) >= 0);
  assert(H5Eset_auto(H5E_DEFAULT, NULL, NULL) >= 0);
  
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    retval = archive_read_data(a, buff, BUF_SIZE);
    assert (retval >= 0 && retval < BUF_SIZE);
  
    if (retval > 0) {
      SHA1(buff, retval, md);
  
      for (i=0; i < SHA_DIGEST_LENGTH; ++i) {
        sprintf((char*)&(hash[i*2]), "%02x%c", md[i],
                i < (SHA_DIGEST_LENGTH-1) ? ' ' : '\0');
      }
  
      dims = (hsize_t) retval;
      assert((space = H5Screate_simple(1, &dims, NULL)) >= 0);
      if ((dset = H5Dcreate(file, hash, H5T_NATIVE_UINT8, space,
                            H5P_DEFAULT, dcpl, H5P_DEFAULT)) >= 0) {
        assert(H5Dwrite(dset, H5T_NATIVE_UINT8, H5S_ALL, H5S_ALL,
                        H5P_DEFAULT, buff)>= 0);
        assert(H5Dclose(dset) >= 0);
        ++file_count;
      }
      assert(H5Sclose(space) >= 0);
    }
   }
  
  printf("%d files extracted.\n", file_count);
  assert(H5Fclose(file) >= 0);
  assert(H5Pclose(dcpl) >= 0);
  assert(H5Pclose(lcpl) >= 0);
  
  retval = archive_read_free(a);
  if (retval != ARCHIVE_OK) {
    printf("ERROR: Can't close the archive properly!\n");
    exit(1);
  }
  exit(0);
}
