
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/file.h>


#define DS9INTERFACE_NO_EIGEN
#include "mx/improc/ds9Interface.hpp"


#include "ImageStruct.hpp"

int openShmem( IMAGE & image,          ///< [out] A real-time image structure which contains the image data and meta-data.
               size_t & type_size,     ///< [out] The size of the data type in the images.
               int & bitpix,             ///< [out] The FITS standard bitpix value for the type in the images.
               sem_t* & sem,           ///< [out] The semaphore to be initialized.
               int semaphoreNumber,  ///< [in] The semaphore number to watch.
               const std::string & shmem_key ///< [in] The filename of the shared memory buffer.
             )
{
   int SM_fd;
   struct stat file_stat;
   IMAGE_METADATA *map;

   SM_fd = open(shmem_key.c_str(), O_RDWR);

   char *mapv;

   if(SM_fd==-1)
   {
      return -1;
   }
   else
   {
      fstat(SM_fd, &file_stat);

      map = (IMAGE_METADATA*) mmap(0, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, SM_fd, 0);

      if (map == MAP_FAILED)
      {
        ::close(SM_fd);
        perror("Error mmapping the file");
        return -1;
      }

      std::cerr << "Attached to " << shmem_key << "\n";

      image.memsize = file_stat.st_size;
      image.shmfd = SM_fd;
      image.md = map;
      image.md->shared = 1;

      mapv = (char*) map;
      mapv += sizeof(IMAGE_METADATA);

      //Make the semaphore file name and try to open it.
      char sname[200];
      sprintf(sname, "%s_sem%02ld", image.md[0].name, (long int) semaphoreNumber);
      if ((sem = sem_open(sname, 0, 0644, 0))== SEM_FAILED)
      {
         std::cerr << "Semaphore not open\n";
         return -1;
      }

      switch(image.md[0].atype)
      {
         case IMAGESTRUCT_UINT8:
            imageStructDataType<IMAGESTRUCT_UINT8>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_UINT8>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_UINT8>::bitpix;
            break;
         case IMAGESTRUCT_INT8:
            imageStructDataType<IMAGESTRUCT_INT8>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_INT8>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_INT8>::bitpix;
            break;
         case IMAGESTRUCT_UINT16:
            imageStructDataType<IMAGESTRUCT_UINT16>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_UINT16>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_UINT16>::bitpix;
            break;
         case IMAGESTRUCT_INT16:
            imageStructDataType<IMAGESTRUCT_INT16>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_INT16>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_INT16>::bitpix;
            break;
         case IMAGESTRUCT_UINT32:
            imageStructDataType<IMAGESTRUCT_UINT32>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_UINT32>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_UINT32>::bitpix;
            break;
         case IMAGESTRUCT_INT32:
            imageStructDataType<IMAGESTRUCT_INT32>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_INT32>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_INT32>::bitpix;
            break;
         case IMAGESTRUCT_UINT64:
            imageStructDataType<IMAGESTRUCT_UINT64>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_UINT64>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_UINT64>::bitpix;
            break;
         case IMAGESTRUCT_INT64:
            imageStructDataType<IMAGESTRUCT_INT64>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_INT64>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_INT64>::bitpix;
            break;
         case IMAGESTRUCT_FLOAT:
            imageStructDataType<IMAGESTRUCT_FLOAT>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_FLOAT>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_FLOAT>::bitpix;
            break;
         case IMAGESTRUCT_DOUBLE:
            imageStructDataType<IMAGESTRUCT_DOUBLE>::setPointer( image, mapv);
            type_size = imageStructDataType<IMAGESTRUCT_DOUBLE>::size;
            bitpix = imageStructDataType<IMAGESTRUCT_DOUBLE>::bitpix;
            break;
         default:
            std::cerr << "Unknown or unsupported data type\n";
            return -1;
      }

   }

   return 0;
}


void usage( const char * argv0,
            const char * err = 0
          )
{
   if(err)
   {
      std::cerr << '\n' << argv0 << ": " << err << "\n\n";
   }

   std::cerr << argv0 << ":\n";
   std::cerr << "Send images from a MILK shared memory buffer to the ds9 image viewer. Sends image to ds9 whenever the semaphore posts.  ";
   std::cerr << "Once started, runs until killed.\n\n";
   std::cerr << "Usage: " << argv0 << " " << "[-h] [-f frameno] [-p pauseTime] [-s semaphoreNumber] [-t ds9Title] [-w waitTime] /path/to/filename\n\n";
   std::cerr << "Required Argument:\n";
   std::cerr << "     /path/to/filename   the full path to the shared memory file.\n\n";
   std::cerr << "Options:\n";
   std::cerr << "     -h                 print this message and exit. \n";
   std::cerr << "     -f frameNo         specify the frame in which to display.\n";
   std::cerr << "                        Default is 1.\n";
   std::cerr << "     -p pauseTime       specify the time, in usec, to pause \n";
   std::cerr << "                        before re-checking the semaphore.\n";
   std::cerr << "                        Default is 100 usec.\n";
   std::cerr << "     -s semaphoreNumber specify the semaphore number to monitor\n";
   std::cerr << "     -t ds9Title        specify the title of the DS9 window to\n";
   std::cerr << "                        use.  Default is the filename.\n";
   std::cerr << "     -w waitTime        specify the time, in usec, to wait\n";
   std::cerr << "                        after sending an image to DS9.  Default\n";
   std::cerr << "                        is 1000 usec.\n";

}

/*

have to provide shmem_key as argument

-t ds9-title

-w time to wait after sending to ds9, in microseconds.  Default = 1000.
-p time to wait before re-checking if the semaphore has posted, in microseconds. Default = 100.
-s semaphore number to use
-f frame number

*/

int main( int argc,
          char ** argv )
{

   std::string shmem_key;

   std::string ds9Title;
   int semaphoreNumber {0}; ///< Number of the semaphore to monitor for new image data.  This determines the filename.

   int waitTime {1000};
   int pauseTime {100};
   int frameNo {1};

   bool help {false};

   opterr = 0;

   int c;
   while ((c = getopt (argc, argv, "f:hp:s:t:w:")) != -1)
   {
      if(c != 'h')
      if (optarg[0] == '-')
      {
         optopt = c;
         c = '?';
      }
      switch (c)
      {
         case 'f':
            frameNo = atoi(optarg);
            break;
         case 'h':
            help = true;
            break;
         case 'p':
           waitTime = atoi(optarg);
           break;
         case 's':
            semaphoreNumber = atoi(optarg);
            break;
         case 't':
           ds9Title = optarg;
           break;
         case 'w':
           waitTime = atoi(optarg);
           break;
         case '?':
            char err[256];
            if (optopt == 'f' || optopt == 'p' || optopt == 's' || optopt == 't' || optopt == 'w')
               snprintf(err, 256, "Option -%c requires an argument.", optopt);
            else if (isprint (optopt))
               snprintf(err, 256, "Unknown option `-%c'.", optopt);
            else
               snprintf(err, 256, "Unknown option character `\\x%x'.\n", optopt);

            usage(argv[0], err);
            return 1;
         default:
            usage(argv[0]);
            abort ();
      }
   }

   if(help)
   {
      usage(argv[0]);
      return 0;
   }


   if( optind != argc-1)
   {
      usage(argv[0], "must specify shared memory file name as only non-option argument.");
      return -1;
   }

   shmem_key = argv[optind];


   IMAGE image;

   size_t type_size; ///< The size, in bytes, of the image data type

   sem_t * sem {nullptr}; ///< The semaphore to monitor for new image data

   int bitpix;

   bool opened = false;
   while(!opened)
   {
      if( openShmem( image, type_size, bitpix, sem, semaphoreNumber, shmem_key) == 0) opened = true;
      usleep(1000);
   }

   mx::improc::ds9Interface ds9(shmem_key);

   int curr_image;
   size_t snx, sny;

   while(1)
   {
      if(sem_trywait(sem) == 0)
      {
         if(image.md[0].size[2] > 0)
         {
            curr_image = image.md[0].cnt1 - 1;
            if(curr_image < 0) curr_image = image.md[0].size[2] - 1;
         }
         else curr_image = 0;

         snx = image.md[0].size[0];
         sny = image.md[0].size[1];

         ds9.display( (void *) (image.array.SI8 + curr_image*snx*sny*type_size),
                           bitpix, type_size, snx, sny, 1, frameNo);
         usleep(waitTime);
      }
      else
      {
         usleep(pauseTime);
      }
   }

   return 0;
}
