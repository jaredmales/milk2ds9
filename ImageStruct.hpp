



#ifndef ImageStruct_hpp
#define ImageStruct_hpp

#include <limits>
#include <fitsio.h>

#ifndef ULONGLONG_IMG
#define ULONGLONG_IMG (80)
#endif


#include <ImageStruct.h>

#define IMAGESTRUCT_UINT8                   1
#define IMAGESTRUCT_INT8                    2
#define IMAGESTRUCT_UINT16                  3
#define IMAGESTRUCT_INT16                   4
#define IMAGESTRUCT_UINT32                  5
#define IMAGESTRUCT_INT32                   6
#define IMAGESTRUCT_UINT64                  7
#define IMAGESTRUCT_INT64                   8
#define IMAGESTRUCT_FLOAT                   9
#define IMAGESTRUCT_DOUBLE                 10
#define IMAGESTRUCT_COMPLEX_FLOAT          11
#define IMAGESTRUCT_COMPLEX_DOUBLE         12
#define IMAGESTRUCT_EVENT_UI8_UI8_UI16_UI8 20

inline
int imageStructBitPix( int dtype )
{
   switch(dtype)
   {
      case IMAGESTRUCT_UINT8:
         return BYTE_IMG;
      case IMAGESTRUCT_INT8:
         return SBYTE_IMG;
      case IMAGESTRUCT_UINT16:
         return USHORT_IMG;
      case IMAGESTRUCT_INT16:
         return SHORT_IMG;
      case IMAGESTRUCT_UINT32:
         return ULONG_IMG;
      case IMAGESTRUCT_INT32:
         return LONG_IMG;
      case IMAGESTRUCT_UINT64:
         return ULONGLONG_IMG;
      case IMAGESTRUCT_INT64:
         return LONGLONG_IMG;
      case IMAGESTRUCT_FLOAT:
         return FLOAT_IMG;
      case IMAGESTRUCT_DOUBLE:
         return DOUBLE_IMG;
      default:
         std::cerr << "bitpix not implemented for type\n";
         return -1;
   }
}

template<int is_dt>
struct imageStructDataType;

template<>
struct imageStructDataType<IMAGESTRUCT_UINT8>
{
   typedef uint8_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = BYTE_IMG;
   static void setPointer( IMAGE & im, char * mapv) { im.array.UI8 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_INT8>
{
   typedef int8_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = SBYTE_IMG;
   static void setPointer( IMAGE & im, char * mapv) { im.array.SI8 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_UINT16>
{
   typedef uint16_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = USHORT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI16 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_INT16>
{
   typedef int16_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = SHORT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI16 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_UINT32>
{
   typedef uint32_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = ULONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI32 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_INT32>
{
   typedef int32_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = LONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI32 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_UINT64>
{
   typedef uint64_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = ULONGLONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.UI64 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_INT64>
{
   typedef int64_t type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = LONGLONG_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.SI64 = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_FLOAT>
{
   typedef float type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = FLOAT_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.F = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_DOUBLE>
{
   typedef double type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = DOUBLE_IMG;

   static void setPointer( IMAGE & im, char * mapv) { im.array.D = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_COMPLEX_FLOAT>
{
   typedef complex_float type;
   constexpr static size_t size = sizeof(type);
   constexpr static type max = std::numeric_limits<type>::max();
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.CF = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_COMPLEX_DOUBLE>
{
   typedef complex_double type;
   constexpr static size_t size = sizeof(type);
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.CD = (type *) mapv;}
};

template<>
struct imageStructDataType<IMAGESTRUCT_EVENT_UI8_UI8_UI16_UI8>
{
   typedef EVENT_UI8_UI8_UI16_UI8 type;
   constexpr static size_t size = sizeof(type);
   constexpr static int bitpix = -1;

   static void setPointer( IMAGE & im, char * mapv) { im.array.event1121 = (type *) mapv;}
};

#endif //ImageStruct_hpp
