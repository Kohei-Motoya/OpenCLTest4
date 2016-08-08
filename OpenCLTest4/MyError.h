//
//  MyError.h
//  OpenCLTest3
//

#ifndef __MY_ERROR_H__
#define __MY_ERROR_H__

#include <string>

class MyError {
public:
    MyError(const std::string & msg,
            const std::string & function = "") {mMsg = msg + " " + function;}
    virtual ~MyError() {};
    
    const std::string message() const {return mMsg;}
    const char * cstr() const {return mMsg.c_str();}

private:
    std::string mMsg;
};

#endif /* __MY_ERROR_H__ */
