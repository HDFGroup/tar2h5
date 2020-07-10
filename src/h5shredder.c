#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include <hdf5.h>
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
  hid_t fapl, file, fspace, mspace, dcpl;
  hid_t image, image_offset, name, name_offset;
  hsize_t dims, maxdims = H5S_UNLIMITED;
  hsize_t extent = 0, offset_extent = 0, offset = 0;
  hsize_t nextent = 0, noffset_extent = 0, noffset = 0;
  hsize_t start, one = 1, block;
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
  
  assert((fapl = H5Pcreate(H5P_FILE_ACCESS)) >= 0);
  assert(H5Pset_libver_bounds(fapl, H5F_LIBVER_V18, H5F_LIBVER_V110) >= 0);
  snprintf(h5name, PATH_MAX, "%s.shredder.h5", tarname);
  printf("|    %s\n", h5name);
  assert((file = H5Fcreate(h5name, H5F_ACC_TRUNC, H5P_DEFAULT, fapl))
         >= 0);
  assert(H5Pclose(fapl) >= 0);
  
  dims = (hsize_t) 0;
  assert((fspace = H5Screate_simple(1, &dims, &maxdims)) >= 0);
  
  assert((dcpl = H5Pcreate(H5P_DATASET_CREATE)) >= 0);
  dims = 1<<20;
  assert(H5Pset_chunk(dcpl, 1, &dims) >= 0);
  
  assert((image = H5Dcreate(file, "image", H5T_NATIVE_UINT8, fspace,
                            H5P_DEFAULT, dcpl, H5P_DEFAULT)) >= 0);
  assert((image_offset = H5Dcreate(file, "image_offset", H5T_NATIVE_HSIZE,
                                   fspace, H5P_DEFAULT, dcpl, H5P_DEFAULT))
         >= 0);
  assert((name_offset = H5Dcreate(file, "name_offset", H5T_NATIVE_HSIZE,
                                  fspace, H5P_DEFAULT, dcpl, H5P_DEFAULT))
         >= 0);
  assert(H5Pset_deflate(dcpl, 6) >= 0);
  assert((name = H5Dcreate(file, "name", H5T_NATIVE_UINT8, fspace, H5P_DEFAULT,
                           dcpl, H5P_DEFAULT)) >= 0);
  assert(H5Pclose(dcpl) >= 0);
  assert(H5Sclose(fspace) >= 0);
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    for (;;) {
      retval = archive_read_data(a, buff, BUF_SIZE);
      assert (retval >= 0);
      if (retval == 0) {
        noffset = start = nextent;
        block = (hsize_t) strlen(archive_entry_pathname(entry)) + 1;
        nextent += block;
        assert((mspace = H5Screate_simple(1, &block, NULL)) >= 0);
        assert(H5Sselect_all(mspace) >= 0);
        assert(H5Dset_extent(name, &nextent) >= 0);
        assert((fspace = H5Dget_space(name)) >= 0);
        assert(H5Sselect_hyperslab(fspace, H5S_SELECT_SET, &start, NULL, &one,
                                   &block) >= 0);
        assert(H5Dwrite(name, H5T_NATIVE_UINT8, mspace, fspace, H5P_DEFAULT,
                        archive_entry_pathname(entry)) >= 0);
        assert(H5Sclose(fspace) >= 0);
        assert(H5Sclose(mspace) >= 0);
        
        start = noffset_extent;
        block = 1;
        noffset_extent += block;
        assert((mspace = H5Screate_simple(1, &block, NULL)) >= 0);
        assert(H5Sselect_all(mspace) >= 0);
        assert(H5Dset_extent(name_offset, &noffset_extent) >= 0);
        assert((fspace = H5Dget_space(name_offset)) >= 0);
        assert(H5Sselect_hyperslab(fspace, H5S_SELECT_SET, &start, NULL, &one,
                                   &block) >= 0);
        assert(H5Dwrite(name_offset, H5T_NATIVE_HSIZE, mspace, fspace,
                        H5P_DEFAULT, &noffset) >= 0);
        assert(H5Sclose(fspace) >= 0);
        assert(H5Sclose(mspace) >= 0);
        ++file_count;
        break;
      }
  
      if (retval > 0) {
        offset = start = extent;
        block = (hsize_t) retval;
        extent += block;
        assert((mspace = H5Screate_simple(1, &block, NULL)) >= 0);
        assert(H5Sselect_all(mspace) >= 0);
        assert(H5Dset_extent(image, &extent) >= 0);
        assert((fspace = H5Dget_space(image)) >= 0);
        assert(H5Sselect_hyperslab(fspace, H5S_SELECT_SET, &start, NULL, &one,
                                   &block) >= 0);
        assert(H5Dwrite(image, H5T_NATIVE_UINT8, mspace, fspace, H5P_DEFAULT,
                        buff) >= 0);
        assert(H5Sclose(fspace) >= 0);
        assert(H5Sclose(mspace) >= 0);
        
        start = offset_extent;
        block = 1;
        offset_extent += block;
        assert((mspace = H5Screate_simple(1, &block, NULL)) >= 0);
        assert(H5Sselect_all(mspace) >= 0);
        assert(H5Dset_extent(image_offset, &offset_extent) >= 0);
        assert((fspace = H5Dget_space(image_offset)) >= 0);
        assert(H5Sselect_hyperslab(fspace, H5S_SELECT_SET, &start, NULL, &one,
                                   &block) >= 0);
        assert(H5Dwrite(image_offset, H5T_NATIVE_HSIZE, mspace, fspace,
                        H5P_DEFAULT, &offset) >= 0);
        assert(H5Sclose(fspace) >= 0);
        assert(H5Sclose(mspace) >= 0);
      }
    }
  }
  
  printf("%d files extracted.\n", file_count);
  assert(H5Dclose(name_offset) >= 0);
  assert(H5Dclose(name) >= 0);
  assert(H5Dclose(image_offset) >= 0);
  assert(H5Dclose(image) >= 0);
  assert(H5Fclose(file) >= 0);
  
  retval = archive_read_free(a);
  if (retval != ARCHIVE_OK) {
    printf("ERROR: Can't close the archive properly!\n");
    exit(1);
  }
  exit(0);
}
