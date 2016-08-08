//
//  Bitmap.cpp
//  OpenCLTest3
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "MyError.h"
#include "Bitmap.h"


/*------------------------------------------------------------------------------------------------------------
 * コンストラクタ
 */
Bitmap::Bitmap() : mWidth(0), mHeight(0), mData(0), mDataSize(0), mTotalPixels(0)
{
    assert(sizeof(MyBYTE)  == 1);
    assert(sizeof(MyWORD)  == 2);
    assert(sizeof(MyDWORD) == 4);
    assert(sizeof(MyLONG)  == 4);
}


/*------------------------------------------------------------------------------------------------------------
 * デストラクタ
 */
Bitmap::~Bitmap()
{
    if (mData != 0) {
        delete [] mData;
        mData = 0;
    }
    mDataSize = 0;
}

/*------------------------------------------------------------------------------------------------------------
 * ビットマップデータを作成する
 *
 * width, height: ビットマップの幅、高さ
 * dataR, dataG, dataB: 0から1.0fで表現した色値
 */

void Bitmap::create(const int width, const int height,
                    const float* dataR, const float* dataG, const float* dataB)
{
    initialize(width, height);
    fillData(dataR, dataG, dataB);
}

/*------------------------------------------------------------------------------------------------------------
 * ビットマップデータを作成する
 *
 * width, height: ビットマップの幅、高さ
 * dataR, dataG, dataB: 0から255で表現した色値
 */

void Bitmap::create(const int width, const int height,
                    const unsigned char* dataR,
                    const unsigned char* dataG,
                    const unsigned char* dataB)
{
    initialize(width, height);
    fillData(dataR, dataG, dataB);
}

/*------------------------------------------------------------------------------------------------------------
 * ビットマップデータを作成する
 *
 * width, height: ビットマップの幅、高さ
 * dataBgra: 1つのピクセル値を、BGRA 4バイトで表したイメージデータ。
 *           BGRAそれぞれの値は、0から255までの値を取る。
 */
void Bitmap::createFromBgra(const int width, const int height, const unsigned char* dataBgra)
{
    initialize(width, height);
    fillBgraData(dataBgra);
}

/*------------------------------------------------------------------------------------------------------------
 * ビットマップファイルを読み込む
 *
 * path: 読み込むビットマップファイル
 */
void Bitmap::readFile(const char *path)
{
    FILE *ifp = fopen(path, "rb");
    
    if (ifp == 0) {
        perror("fopen");
        fprintf(stderr, "file: %s\n", path);
        throw MyError("bitmap file open failed", __FUNCTION__);
    }
    
    int error;
    error = readFileHeader(ifp);
    if (error > 0) {
        throw MyError("failed to read bitmap file header", __FUNCTION__);
    }
    
    error = readInfoHeader(ifp);
    if (error > 0) {
        throw MyError("failed to read bitmap info header", __FUNCTION__);
    }
    
    mDataSize = mFileHeader.bfSize - mInfoHeader.biSize - sizeof(mFileHeader);
    if (mDataSize != 0) {
        delete [] mData;
    }
    mData = new unsigned char[mDataSize];
    
    error = readBitmapData(ifp);
    if (error > 0) {
        throw MyError("failed toread bitmap data", __FUNCTION__);
    }
    
    
    mWidth = mInfoHeader.biWidth;
    mHeight = mInfoHeader.biHeight;
    
    fclose(ifp);
}

/*------------------------------------------------------------------------------------------------------------
 * ビットマップファイルを書き出す
 *
 * path: 書き出すビットマップファイル
 */
void Bitmap::writeFile(const char *path) const
{
    FILE *ofp = fopen(path, "wb");
    if (ofp == 0) {
        perror("fopen");
        fprintf(stderr, "file: %s\n", path);
        throw MyError("bitmap file open failed", __FUNCTION__);
    }
    
    writeFileHeader(ofp);
    writeInfoHeader(ofp);
    writeBitmapData(ofp);
    
    fclose(ofp);
}

/*------------------------------------------------------------------------------------------------------------
 * 画像データをR、G、Bチャンネル（dataR, dataG, dataB）に分割して返す
 *
 */
void Bitmap::getRgbData(unsigned char* dataR, unsigned char* dataG, unsigned char* dataB) const
{
    int index = 0;
    
    if (mInfoHeader.biBitCount == 32) {
        // 32-bitビットマップファイル(BGRA)
        for (int i = 0; i < mDataSize; i+=4) {
            dataB[index] = mData[i + 0];
            dataG[index] = mData[i + 1];
            dataR[index] = mData[i + 2];
            index++;
        }
    } else if (mInfoHeader.biBitCount == 24) {
        // 24-bitビットマップファイル(BGR)
        for (int i = 0; i < mDataSize; i+=3) {
            dataB[index] = mData[i + 0];
            dataG[index] = mData[i + 1];
            dataR[index] = mData[i + 2];
            index++;
        }
    } else {
        throw MyError("Unsupported bitmap format", __FUNCTION__);
    }
}

/*------------------------------------------------------------------------------------------------------------
 * 画像データBGRAとして返す
 *
 */
void Bitmap::getBgraData(unsigned char* dataBgra) const
{
    if (mInfoHeader.biBitCount == 32) {
        // 32bitビットマップはBGRAの順に並んでいるので、そのままコピーする
        memcpy(dataBgra, mData, mDataSize);
    } else if (mInfoHeader.biBitCount == 24) {
        // 24bitビットマップファイルはAチャンネルにダミーの値をセットする
        int index = 0;
        for (int i = 0; i < mDataSize; i+=3) {
            dataBgra[index++] = mData[i + 0]; // B
            dataBgra[index++] = mData[i + 1]; // G
            dataBgra[index++] = mData[i + 2]; // R
            dataBgra[index++] = 255;          // A
            index++;
        }
    } else {
        throw MyError("Unsupported bitmap format", __FUNCTION__);
    }
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
void Bitmap::initialize(const int width, const int height)
{
    if (width <= 0) {
        throw MyError("width must be > 0\n", __FUNCTION__);
    }
    if (width % 4 != 0) {
        throw MyError("width must be a multiply of 4.", __FUNCTION__);
    }
    
    initFileHeader();
    initInfoHeader();
    
    mWidth = width;
    mHeight = height;
    
    const int absHeight = height > 0 ? height : height * -1;
    mTotalPixels = width * absHeight;
    mDataSize = mTotalPixels * 3;
    
    if (mData != 0) {
        delete [] mData;
        mData = 0;
    }
    
    mData = new unsigned char[mDataSize];
    
    mInfoHeader.biWidth = width;
    mInfoHeader.biHeight = height;
    mFileHeader.bfSize = (MyDWORD)(mDataSize + sizeof(mFileHeader) + sizeof(mInfoHeader));
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
void Bitmap::fillData(const float* dataR, const float* dataG, const float* dataB)
{
    int index = 0;
    
    for (int i = 0; i < mDataSize; i +=3) {
        mData[i + 0] = getPixelValue(dataB[index]);
        mData[i + 1] = getPixelValue(dataG[index]);
        mData[i + 2] = getPixelValue(dataR[index]);
        index++;
    }
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
void Bitmap::fillData(const unsigned char* dataR, const unsigned char* dataG, const unsigned char* dataB)
{
    int index = 0;
    
    for (int i = 0; i < mDataSize; i +=3) {
        mData[i + 0] = dataB[index];
        mData[i + 1] = dataG[index];
        mData[i + 2] = dataR[index];
        index++;
    }
}

/*------------------------------------------------------------------------------------------------------------
 * 受け取ったBGRAデータを、内部のデータとしてセットする。
 * BGRのみ取得し、Aチャンネルは捨てる
 */
void Bitmap::fillBgraData(const unsigned char* dataBgra)
{
    int index = 0;
    int bgraSize = (mDataSize * 4) / 3;
    
    for (int i = 0; i < bgraSize; i+=4) {
        mData[index++] = dataBgra[i + 0];     // B
        mData[index++] = dataBgra[i + 1];     // G
        mData[index++] = dataBgra[i + 2];     // R
     // mData[index++] = dataBgra[i + 3];     // A
    }
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::readFileHeader(FILE *fp)
{
    int error = 0;
    const size_t numobject = 1;
    
    size_t result = fread(&mFileHeader, sizeof(mFileHeader), numobject, fp);
    if (numobject != result) {
        error++;
    }
    
    return error;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::readInfoHeader(FILE *fp)
{
    int error = 0;
    const size_t numobject = 1;
    size_t result = fread(&mInfoHeader, sizeof(mInfoHeader), numobject, fp);
    
    if (numobject != result) {
        error++;
    }
    
    return error;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::readBitmapData(FILE* fp)
{
    if (mData == 0 || mDataSize == 0) {
        return 1;
    }
    
    size_t result = fread(mData, sizeof(*mData), mDataSize, fp);
    if (mDataSize != result) {
        return 1;
    }
    
    return  0;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::writeFileHeader(FILE *fp) const
{
    int error;
    const size_t numobject = 1;
    
    size_t times = fwrite(&mFileHeader, sizeof(mFileHeader), numobject, fp);
    if (numobject != times) {
        error++;
        return error;
    }
    
    return error;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::writeInfoHeader(FILE *fp) const
{
    int error;
    const size_t numobject = 1;
    
    size_t times = fwrite(&mInfoHeader, sizeof(mInfoHeader), numobject, fp);
    if (numobject != times) {
        error++;
        return error;
    }
    
    return error;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
int Bitmap::writeBitmapData(FILE *fp) const
{
    int error = 0;
    const size_t numobject = mDataSize;
    
    size_t times = fwrite(mData, sizeof(*mData), mDataSize, fp);
    if (numobject != times) {
        error++;
        return error;
    }
    
    return error;
    
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
void Bitmap::initFileHeader()
{
    mFileHeader.bfType = 0x4d42;
    mFileHeader.bfSize = 0;
    mFileHeader.bfSize = sizeof(mInfoHeader);
    mFileHeader.bfReserved1 = 0;
    mFileHeader.bfReserved2 = 0;
    mFileHeader.bfOffBits = 54;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
void Bitmap::initInfoHeader()
{
    mInfoHeader.biSize = sizeof(mInfoHeader);
    mInfoHeader.biWidth = 0;
    mInfoHeader.biHeight = 0;
    mInfoHeader.biPlanes = 1;
    mInfoHeader.biBitCount = 24;
    mInfoHeader.biCompression = 0;
    mInfoHeader.biSizeImage = 0;
    mInfoHeader.biXPelsPerMeter = 0;
    mInfoHeader.biYPelsPerMeter = 0;
    mInfoHeader.biClrUsed = 0;
    mInfoHeader.biClrImportant = 0;
}

/*------------------------------------------------------------------------------------------------------------
 *
 */
unsigned char Bitmap::getPixelValue(const float val) const
{
    int intval = (int)(val * 255.0f);
    
    if (intval < 0) {
        intval = 0;
    }else if (intval > 255) {
        intval = 255;
    }
    return (unsigned char)intval;
}