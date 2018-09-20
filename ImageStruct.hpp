



#ifndef ImageStruct_hpp
#define ImageStruct_hpp

#include <limits>
#include <fitsio.h>

#ifndef ULONGLONG_IMG
#define ULONGLONG_IMG (80)
#endif


#include <ImageStruct.h>


inline
int imageStructBitPix( int dtype )
{
   switch(dtype)
   {
      case _DATATYPE_UINT8:
         return BYTE_IMG;
      case _DATATYPE_INT8:
         return SBYTE_IMG;
      case _DATATYPE_UINT16:
         return USHORT_IMG;
      case _DATATYPE_INT16:
         return SHORT_IMG;
      case _DATATYPE_UINT32:
         return ULONG_IMG;
      case _DATATYPE_INT32:
         return LONG_IMG;
      case _DATATYPE_UINT64:
         return ULONGLONG_IMG;
      case _DATATYPE_INT64:
         return LONGLONG_IMG;
      case _DATATYPE_FLOAT:
         return FLOAT_IMG;
      case _DATATYPE_DOUBLE:
         return DOUBLE_IMG;
      default:
         std::cerr << "bitpix not implemented for type\n";
         return -1;
   }
}

template<int is_dt>
struct imageStructDataType;

template<>
struct imageStructDataType<_DATATYPE_UINT8>
{
   typedef uint8_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = BYTE_IMG;
   static void setPointer( IMAGE & im, char * mapv) { im.array.UI8 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_INT8>
{
   typedef int8_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = SBYTE_IMG;
   static void setPointer( IMAGE & im, char * mapv) { im.array.SI8 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_UINT16>
{
   typedef uint16_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = USHORT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI16 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_INT16>
{
   typedef int16_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = SHORT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI16 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_UINT32>
{
   typedef uint32_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = ULONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI32 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_INT32>
{
   typedef int32_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = LONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI32 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_UINT64 >
{
   typedef uint64_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = ULONGLONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI64 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_INT64>
{
   typedef int64_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = LONGLONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI64 = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_FLOAT>
{
   typedef float type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = FLOAT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.F = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_DOUBLE>
{
   typedef double type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = DOUBLE_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.D = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_COMPLEX_FLOAT>
{
   typedef complex_float type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.CF = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_COMPLEX_DOUBLE>
{
   typedef complex_double type;
   constexpr static size_t size = sizeof(type);
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.CD = (type *) mapv;}
};

template<>
struct imageStructDataType<_DATATYPE_EVENT_UI8_UI8_UI16_UI8>
{
   typedef EVENT_UI8_UI8_UI16_UI8 type;
   constexpr static size_t size = sizeof(type);
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.event1121 = (type *) mapv;}
};


inline
int openShMem( IMAGE & image,          ///< [out] A real-time image structure which contains the image data and meta-data.
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
         case _DATATYPE_UINT8:
            imageStructDataType<_DATATYPE_UINT8>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_UINT8>::size;
            bitpix = imageStructDataType<_DATATYPE_UINT8>::bitpix;
            break;
         case _DATATYPE_INT8:
            imageStructDataType<_DATATYPE_INT8>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_INT8>::size;
            bitpix = imageStructDataType<_DATATYPE_INT8>::bitpix;
            break;
         case _DATATYPE_UINT16:
            imageStructDataType<_DATATYPE_UINT16>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_UINT16>::size;
            bitpix = imageStructDataType<_DATATYPE_UINT16>::bitpix;
            break;
         case _DATATYPE_INT16:
            imageStructDataType<_DATATYPE_INT16>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_INT16>::size;
            bitpix = imageStructDataType<_DATATYPE_INT16>::bitpix;
            break;
         case _DATATYPE_UINT32:
            imageStructDataType<_DATATYPE_UINT32>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_UINT32>::size;
            bitpix = imageStructDataType<_DATATYPE_UINT32>::bitpix;
            break;
         case _DATATYPE_INT32:
            imageStructDataType<_DATATYPE_INT32>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_INT32>::size;
            bitpix = imageStructDataType<_DATATYPE_INT32>::bitpix;
            break;
         case _DATATYPE_UINT64:
            imageStructDataType<_DATATYPE_UINT64>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_UINT64>::size;
            bitpix = imageStructDataType<_DATATYPE_UINT64>::bitpix;
            break;
         case _DATATYPE_INT64:
            imageStructDataType<_DATATYPE_INT64>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_INT64>::size;
            bitpix = imageStructDataType<_DATATYPE_INT64>::bitpix;
            break;
         case _DATATYPE_FLOAT:
            imageStructDataType<_DATATYPE_FLOAT>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_FLOAT>::size;
            bitpix = imageStructDataType<_DATATYPE_FLOAT>::bitpix;
            break;
         case _DATATYPE_DOUBLE:
            imageStructDataType<_DATATYPE_DOUBLE>::setPointer( image, mapv);
            type_size = imageStructDataType<_DATATYPE_DOUBLE>::size;
            bitpix = imageStructDataType<_DATATYPE_DOUBLE>::bitpix;
            break;
         default:
            std::cerr << "Unknown or unsupported data type\n";
            return -1;
      }

   }

   return 0;
}

inline
int closeShMem( IMAGE & image /**< [in] A real-time image structure which contains the image data and meta-data.*/)
{
   return munmap( image.md, image.memsize);
}

#endif //ImageStruct_hpp
