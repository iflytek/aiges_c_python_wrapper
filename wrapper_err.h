#ifndef _WRAPPER_ERROR_H
#define _WRAPPER_ERROR_H

namespace WRAPPER{
    enum CError{
        innerError=-1,
        NotImplementError=-1000,
        NotImplementInit=-1001,
        NotImplementExec=-1002,
        NotImplementFini=-1003,

        RltDataKeyInvalid=-1010,
        RltDataLenInvalid=-1011,
        RltDataStatusInvalid=-1012,
        RltDataTypeInvalid=-1013,
    };

};
#endif