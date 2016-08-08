//
//  bitmapspec.h
//  OpenCLTest3
//

#ifndef __BITMAPSPEC__
#define __BITMAPSPEC__

typedef char  MyBYTE;  // 1byte
typedef short MyWORD;  // 2byte
typedef int   MyDWORD; // 4byte
typedef int   MyLONG;  // 4byte

#pragma pack(push, 1)

typedef struct {
    MyWORD  bfType;
    MyDWORD bfSize;
    MyWORD  bfReserved1;
    MyWORD  bfReserved2;
    MyDWORD bfOffBits;
} BitmapFileHeader;

typedef struct {
    MyDWORD biSize;
    MyLONG biWidth;
    MyLONG biHeight;
    MyWORD biPlanes;
    MyWORD biBitCount;
    MyDWORD biCompression;
    MyDWORD biSizeImage;
    MyLONG biXPelsPerMeter;
    MyLONG biYPelsPerMeter;
    MyDWORD biClrUsed;
    MyDWORD biClrImportant;
} BitmapInfoHeader;

#pragma pack(pop)

#endif /* __BITMAPSPEC__ */
