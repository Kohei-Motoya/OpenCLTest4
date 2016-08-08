//
//  ClHlper.cpp
//  OpenCLTest3
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__

#include "MyError.h"
#include "ClHelper.h"

/*------------------------------------------------------------------------------------------------------------
 * コンストラクタ
 */
ClHelper::ClHelper()
    :mContext((cl_context)0), mDevaiceId((cl_device_id)0),
    mCommandQueue((cl_command_queue)0), mProgram((cl_program)0),
    mKernel((cl_kernel)0)
{
    cl_int status;
    
    // create context for specified device
    mContext = clCreateContextFromType(NULL,
                               // CL_DEVICE_TYPE_CPU, // CPUを使用
                               CL_DEVICE_TYPE_GPU,    // GPUを使用
                               NULL,
                               NULL,
                               &status);
    
    if (status != CL_SUCCESS) {
        printError(status);
        throw MyError("failed to create context.", __FUNCTION__);
    }
    
    
    // get device id from context
    cl_device_id devices[10];
    
    status = clGetContextInfo(mContext,
                              CL_CONTEXT_DEVICES,
                              sizeof(devices),
                              devices,
                              NULL);
    if(status != CL_SUCCESS){
        printError(status);
        throw MyError("failed to get device id.", __FUNCTION__);
    }
    mDevaiceId = devices[0];
    
    // Create command queue
    mCommandQueue = clCreateCommandQueue(mContext,
                                         devices[0],
                                         0,
                                         &status);
    if(status != CL_SUCCESS){
        printError(status);
        throw MyError("failed to create command queue.", __FUNCTION__);
    }
}

/*------------------------------------------------------------------------------------------------------------
 * デストラクタ
 */
ClHelper::~ClHelper()
{
    if (mContext != (cl_context)0) {
        clReleaseContext(mContext);
        mContext = (cl_context)0;
    }
    
    mDevaiceId = (cl_device_id)0;
    
    if (mCommandQueue != (cl_command_queue)0) {
        clReleaseCommandQueue(mCommandQueue);
        mCommandQueue = (cl_command_queue)0;
    }
    
    if (mProgram != (cl_program)0) {
        clReleaseProgram(mProgram);
        mProgram = (cl_program)0;
    }
    
    if (mKernel != (cl_kernel)0) {
        clReleaseKernel(mKernel);
        mKernel = (cl_kernel)0;
    }
}

/*------------------------------------------------------------------------------------------------------------
 * 指定したソースコードファイルからプログラムオブジェクとを作成する
 * filename:カーネルのソースファイル
 */
void ClHelper::preloadProgram(const char* filename)
{
    cl_int status;
    char *source = 0;
    
    // ファイルからプログラムを読み込む
    try {
        source = loadProgramSource(filename);
    } catch (MyError err) {
        fprintf(stderr, "Error: %s\n", err.cstr());
        throw MyError("failed to load compute program from file.", __FUNCTION__);
    }
    
    // プログラムオブジェクトを作成する
    mProgram = clCreateProgramWithSource(mContext,
                                         1,
                                         (const char **)&source,
                                         NULL,
                                         &status);
    if (mProgram ==(cl_program)0) {
        printError(status);
        delete [] source;
        throw MyError("failed to create program object", __FUNCTION__);
    }
    
    // プログラムをビルドする
    cl_device_id devices[1];
    devices[0] = mDevaiceId;
    
    status = clBuildProgram(mProgram, 1, devices, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        printError(status);
        showBuildingLog(mProgram, devices[0]);
        delete [] source;
        throw MyError("failed to build program object.", __FUNCTION__);
    }
    
    delete [] source;
}

/*------------------------------------------------------------------------------------------------------------
 * エラーコードに対応するメッセージを出力する
 * err:エラーコード
 */
void ClHelper::printError(const cl_int err)
{
    switch (err) {
        case CL_BUILD_PROGRAM_FAILURE:
            fprintf(stderr, "Program build faild\n");
            break;
        case CL_COMPILER_NOT_AVAILABLE:
            fprintf(stderr, "OpenCL compiler is not available\n");
            break;
        case CL_DEVICE_NOT_FOUND:
            fprintf(stderr, "Device is not available\n");
            break;
        case CL_IMAGE_FORMAT_NOT_SUPPORTED  :
            fprintf(stderr, "Image format is not supported\n");
            break;
        case CL_IMAGE_FORMAT_MISMATCH:
            fprintf(stderr, "Image format missmatch\n");
            break;
        case CL_INVALID_ARG_INDEX:
            fprintf(stderr, "Invalid arg index\n");
            break;
        case CL_INVALID_ARG_SIZE:
            fprintf(stderr, "Invalid arg size\n");
            break;
        case CL_INVALID_ARG_VALUE:
            fprintf(stderr, "Invalid arg value\n");
            break;
        case CL_INVALID_BINARY:
            fprintf(stderr, "Invalid binary\n");
            break;
        case CL_INVALID_BUFFER_SIZE:
            fprintf(stderr, "Invalid buffer size\n");
            break;
        case CL_INVALID_BUILD_OPTIONS:
            fprintf(stderr, "Invalid build optionsn");
            break;
        case CL_INVALID_COMMAND_QUEUE:
            fprintf(stderr, "Invalid command queue\n");
            break;
        case CL_INVALID_CONTEXT:
            fprintf(stderr, "Invalid conrtext\n");
            break;
        case CL_INVALID_DEVICE:
            fprintf(stderr, "Invalid device\n");
            break;
        case CL_INVALID_DEVICE_TYPE:
            fprintf(stderr, "Invalid device type\n");
            break;
        case CL_INVALID_EVENT:
            fprintf(stderr, "Invalid event\n");
            break;
        case CL_INVALID_EVENT_WAIT_LIST:
            fprintf(stderr, "Invalid event wait list\n");
            break;
        case CL_INVALID_GL_OBJECT:
            fprintf(stderr, "Invalid OpenGL object\n");
            break;
        case CL_INVALID_GLOBAL_OFFSET:
            fprintf(stderr, "Invalid global offset\n");
            break;
        case CL_INVALID_HOST_PTR:
            fprintf(stderr, "Invalid host pointer\n");
            break;
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            fprintf(stderr, "Invalid image format descriptor\n");
            break;
        case CL_INVALID_IMAGE_SIZE:
            fprintf(stderr, "Invalid image size\n");
            break;
        case CL_INVALID_KERNEL:
            fprintf(stderr, "Invalid kernel\n");
            break;
        case CL_INVALID_KERNEL_ARGS:
            fprintf(stderr, "Invalid kernel args\n");
            break;
        case CL_INVALID_KERNEL_DEFINITION:
            fprintf(stderr, "Invalid kernel definition\n");
            break;
        case CL_INVALID_KERNEL_NAME:
            fprintf(stderr, "Invalid kernel name\n");
            break;
        case CL_INVALID_MEM_OBJECT:
            fprintf(stderr, "Invalid memory object\n");
            break;
        case CL_INVALID_MIP_LEVEL:
            fprintf(stderr, "Invalid MIP level\n");
            break;
        case CL_INVALID_OPERATION:
            fprintf(stderr, "Invalid operation\n");
            break;
        case CL_INVALID_PLATFORM:
            fprintf(stderr, "Invalid platform\n");
            break;
        case CL_INVALID_PROGRAM:
            fprintf(stderr, "Invalid program\n");
            break;
        case CL_INVALID_PROGRAM_EXECUTABLE:
            fprintf(stderr, "Invalid program executable\n");
            break;
        case CL_INVALID_QUEUE_PROPERTIES:
            fprintf(stderr, "Invalid queue properties\n");
            break;
        case CL_INVALID_SAMPLER:
            fprintf(stderr, "Invalid Sampler\n");
            break;
        case CL_INVALID_VALUE:
            fprintf(stderr, "Invalid value\n");
            break;
        case CL_INVALID_WORK_DIMENSION:
            fprintf(stderr, "Invalid work dimension\n");
            break;
        case CL_INVALID_WORK_GROUP_SIZE:
            fprintf(stderr, "Invalid work group size\n");
            break;
        case CL_INVALID_WORK_ITEM_SIZE:
            fprintf(stderr, "Invalid work item size\n");
            break;
        case CL_MAP_FAILURE:
            fprintf(stderr, "Memory mapping failed\n");
            break;
        case CL_MEM_COPY_OVERLAP:
            fprintf(stderr, "Copying overlapped memory address\n");
            break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            fprintf(stderr, "Memory object allocation failure\n");
            break;
        case CL_OUT_OF_HOST_MEMORY:
            fprintf(stderr, "Out of host memory\n");
            break;
        case CL_OUT_OF_RESOURCES:
            fprintf(stderr, "Out of resources\n");
            break;
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            fprintf(stderr, "Profiling info is not available\n");
            break;
        case CL_SUCCESS:
            fprintf(stderr, "Succeeded\n");
            break;
        default:
            fprintf(stderr, "Unknown error code: %d\n", err);
            break;
    }
}

/*------------------------------------------------------------------------------------------------------------
 * ビルドログを表示する
 * program: ビルドを行ったプログラムオブジェクと
 * device:  ビルドのターゲットに用いたデバイスID
 */
void ClHelper::showBuildingLog(const cl_program program,
                               const cl_device_id device) const
{
    cl_int status;
    size_t size_ret;
    
    char buffer[4096];
    status = clGetProgramBuildInfo(program,
                                   device,
                                   CL_PROGRAM_BUILD_LOG,
                                   sizeof(buffer) - 1,
                                   buffer,
                                   &size_ret);
    if(status != CL_SUCCESS){
        printf("clGetProgramBuildInfo faild.\n");
        printError(status);
    }else{
        buffer[size_ret] = '\0';
        printf(">>> build log <<<\n");
        printf("%s\n", buffer);
        printf(">>> end of build log <<<\n");
    }
}

/*------------------------------------------------------------------------------------------------------------
 * ファイルから読み込んだ内容を文字列として返す
 * （メモリをアロケートしてポインタを返すので、呼び出し側で削除する必要がある）
 */
char* ClHelper::loadProgramSource(const char* filename)
{
    struct stat statbuf;
    FILE *inFile;
    char *source;
    
    inFile = fopen(filename, "r");
    if (inFile == 0) {
        perror("fopen");
        fprintf(stderr, "file: %s\n", filename);
        throw MyError("kernel program file open failed.", __FUNCTION__);
    }
    if (stat(filename, &statbuf) != 0) {
        perror("stat");
        throw MyError("fuction stat() failed.", __FUNCTION__);
    }
    
    source = new char[statbuf.st_size + 1];
    fread(source, statbuf.st_size, 1, inFile);
    source[statbuf.st_size] = '\0';
    
    return source;
}
