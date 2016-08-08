//
//  ClHlper.hpp
//  OpenCLTest3
//

#ifndef __CL_HELPER_H__
#define __CL_HELPER_H__

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif //__APPLE__

/*------------------------------------------------------------------------------------------------------------
 * OpenCL helper class
 */
class ClHelper {
public:
    ClHelper();
    virtual ~ClHelper();
    
    void preloadProgram(const char* filname);
    cl_context getContext() const {return mContext;}
    cl_device_id getDevice() const {return mDevaiceId;}
    cl_command_queue getCommandQueue() const {return mCommandQueue;}
    cl_program getProgram() const {return mProgram;}
    cl_kernel getKernel() const {return mKernel;}
    
    static void printError(const cl_int err);
    
private:
    void showBuildingLog(const cl_program program, const cl_device_id device) const;
    char* loadProgramSource(const char *filename);
    
private:
    cl_context       mContext;      // コンテキスト
    cl_device_id     mDevaiceId;    // デバイスID
    cl_command_queue mCommandQueue; // コマンドキュー
    cl_program       mProgram;      // プログラム
    cl_kernel        mKernel;       // カーネル
};

#endif /* __CL_HELPER_H__ */
