#ifndef _CLI_H_
#define _CLI_H_

#include "shell.h"
#include "system/sysdecl.hpp"


//===========================================================================
// External declarations.
//===========================================================================

#ifdef __cplusplus
extern "C" {
#endif


    void shellStart( BaseSequentialStream *psd);

#ifdef __cplusplus
}
#endif


#endif // _CLI_H_


