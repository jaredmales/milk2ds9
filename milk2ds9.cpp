
#include <fcntl.h>
#include <signal.h>

#define DS9INTERFACE_NO_EIGEN
#include "mx/improc/ds9Interface.hpp"


#include <ImageStreamIO/ImageStruct.h>
#include <ImageStreamIO/ImageStreamIO.h>

bool timeToDie;

void sigHandler( int signum,
                 siginfo_t *siginf,
                 void *ucont
               )
{
   //Suppress those warnings . . .
   static_cast<void>(signum);
   static_cast<void>(siginf);
   static_cast<void>(ucont);
   
   timeToDie = true;
}

int setSigTermHandler()
{
   struct sigaction act;
   sigset_t set;

   act.sa_sigaction = sigHandler;
   act.sa_flags = SA_SIGINFO;
   sigemptyset(&set);
   act.sa_mask = set;

   errno = 0;
   if( sigaction(SIGTERM, &act, 0) < 0 )
   {
      std::cerr << " (" << "milk2ds9" << "): error setting SIGTERM handler: " << strerror(errno) << "\n";
      return -1;
   }

   errno = 0;
   if( sigaction(SIGQUIT, &act, 0) < 0 )
   {
      std::cerr << " (" << "milk2ds9" << "): error setting SIGQUIT handler: " << strerror(errno) << "\n";
      return -1;
   }

   errno = 0;
   if( sigaction(SIGINT, &act, 0) < 0 )
   {
      std::cerr << " (" << "milk2ds9" << "): error setting SIGINT handler: " << strerror(errno) << "\n";
      return -1;
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
   std::cerr << "                        Default is 1000 usec.\n";
   std::cerr << "     -s semaphoreNumber specify the semaphore number to monitor\n";
   std::cerr << "     -t ds9Title        specify the title of the DS9 window to\n";
   std::cerr << "                        use.  Default is the filename.\n";
   std::cerr << "     -w waitTime        specify the time, in usec, to wait\n";
   std::cerr << "                        after sending an image to DS9.  Default\n";
   std::cerr << "                        is 10000 usec.\n";

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

   timeToDie = false;
   
   std::string ds9Title;
   int semaphoreNumber {0}; ///< Number of the semaphore to monitor for new image data.  This determines the filename.

   int waitTime {10000};
   int pauseTime {1000};
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

   std::string shmem_key = argv[optind];
   
   if(ds9Title == "") ds9Title = shmem_key;

   IMAGE image;

   size_t type_size; ///< The size, in bytes, of the image data type

   sem_t * sem {nullptr}; ///< The semaphore to monitor for new image data

   int bitpix;

   if(setSigTermHandler() < 0) return -1;
   
   mx::improc::ds9Interface ds9(ds9Title);
   
   while(!timeToDie)
   {
      bool opened = false;
      
      int reported = 0;
      while(!opened && !timeToDie)
      {
         
         //b/c ImageStreamIO prints every single time, and latest version don't support stopping it yet, and that isn't thread-safe-able anyway
         //we do our own checks.  This is the same code in ImageStreamIO_openIm...
         int SM_fd;
         char SM_fname[200];
         ImageStreamIO_filename(SM_fname, sizeof(SM_fname), shmem_key.c_str());
         SM_fd = open(SM_fname, O_RDWR);
         if(SM_fd == -1)
         {
            if(!reported) std::cerr << "ImageStream " << shmem_key << " not found (yet).  Retrying . . . \n";
            reported = 1;
            sleep(1); //be patient
            continue;
         }
         
         //Found and opened,  close it and then use ImageStreamIO
         reported = 0;
         close(SM_fd);
         
         if( ImageStreamIO_openIm(&image, shmem_key.c_str()) == 0)
         {
            if(image.md[0].sem <= semaphoreNumber) 
            {
               std::cerr << "Creation not complete yet\n";
               ImageStreamIO_closeIm(&image);
               sleep(1); //We just need to wait for the server process to finish startup.
            }
            else
            {
               sem = image.semptr[semaphoreNumber];
               type_size = ImageStreamIO_typesize(image.md[0].datatype);
               bitpix = ImageStreamIO_bitpix(image.md[0].datatype);
               opened = true;
            }
         }
         else
         {
            sleep(1); //be patient
         }
      }

      int curr_image;
      size_t snx, sny, snz;
      size_t last_snx = image.md[0].size[0];
      size_t last_sny = image.md[0].size[1];
      size_t last_snz = image.md[0].size[2];

      uint64_t last_cnt0 = -1;
      
      while(!timeToDie)
      {
         errno = 0;
         if(image.md->cnt0 != last_cnt0)
         {
            if(image.md[0].size[2] > 0)
            {
               curr_image = image.md[0].cnt1 - 1;
               if(curr_image < 0) curr_image = image.md[0].size[2] - 1;
            }
            else curr_image = 0;

            snx = image.md[0].size[0];
            sny = image.md[0].size[1];
            snz = image.md[0].size[2];
         
            last_cnt0 = image.md->cnt0;
            
            if( snx != last_snx || sny != last_sny || snz != last_snz )
            {
               std::cerr << "\nSize change detected!\n\n";
               break;
            }
         
            
            
            ds9.display( (void *) (image.array.SI8 + curr_image*snx*sny*type_size), bitpix, type_size, snx, sny, 1, frameNo);
         
            usleep(waitTime);
         }
         else
         {
            if(image.md[0].sem <= 0) break; //Indicates that the server has cleaned up.
            
            usleep(pauseTime);
         }
      }

      if(opened) ImageStreamIO_closeIm(&image);
   }
   return 0;
}
