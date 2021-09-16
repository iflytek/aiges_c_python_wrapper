/** 
 * @file    ifly_mspp_error.h
 * @brief   iFLY MSPP Errors Codes header file
 * 
 * This file contains the iFly MSPP error codes.
 * Copyright (C)    1999 - 2018 by IFLYTEK. Co.,LTD.
 *                  All rights reserved.
 * 
 * @author  MSPP Dept. iFLYTEK.
 * @version 2.0.0.2
 * @date    2018/10/11
 * 
 * @see        
 * 
 * @par History:
 * <table border=1>
 *  <tr> <th>version    <th>data    <th>author    <th>notes </tr>
 *  <tr> <td>2.0.0.2    <td>2018/10/11    <td>yfxie5    <td>Create this file  </tr>
 * </table>
 */

#ifndef _ifly_mspp_error_h__
#define _ifly_mspp_error_h__

enum
{
	/* Generic error defines */
	MSPP_SUCCESS                          =  0,
	MSPP_ERROR_FAIL                       = -1,
	MSPP_ERROR_EXCEPTION                  = -2,

	/* Common errors */

	MSPP_ERROR_GENERAL                    = 10000, 
	MSPP_ERROR_OUT_OF_MEMORY              = 10001, 
	MSPP_ERROR_FILE_NOT_FOUND             = 10002, 
	MSPP_ERROR_NOT_SUPPORT                = 10003, 
	MSPP_ERROR_NOT_IMPLEMENT              = 10004, 
	MSPP_ERROR_ACCESS                     = 10005, 
	MSPP_ERROR_INVALID_PARA               = 10006, 
	MSPP_ERROR_INVALID_PARA_VALUE         = 10007, 
	MSPP_ERROR_INVALID_HANDLE             = 10008, 
	MSPP_ERROR_INVALID_DATA               = 10009, 
	MSPP_ERROR_NO_LICENSE                 = 10010, 
	MSPP_ERROR_NOT_INIT                   = 10011, 
	MSPP_ERROR_NULL_HANDLE                = 10012, 
	MSPP_ERROR_OVERFLOW                   = 10013, 
	MSPP_ERROR_TIME_OUT                   = 10014, 
	MSPP_ERROR_OPEN_FILE                  = 10015, 
	MSPP_ERROR_NOT_FOUND                  = 10016, 
	MSPP_ERROR_NO_ENOUGH_BUFFER           = 10017, 
	MSPP_ERROR_NO_DATA                    = 10018, 
	MSPP_ERROR_NEED_MORE_DATA             = 10019, 
	MSPP_ERROR_RES_MISSING                = 10020, 
	MSPP_ERROR_SKIPPED                    = 10021, 
	MSPP_ERROR_ALREADY_EXIST              = 10022, 
	MSPP_ERROR_LOAD_MODULE                = 10023, 
	MSPP_ERROR_BUSY                       = 10024, 
	MSPP_ERROR_INVALID_CONFIG             = 10025, 
	MSPP_ERROR_VERSION_CHECK              = 10026, 
	MSPP_ERROR_CANCELED                   = 10027, 
	MSPP_ERROR_INVALID_MEDIA_TYPE         = 10028, 
	MSPP_ERROR_NULL_PTR                   = 10029, 
	MSPP_ERROR_NOT_BEGIN                  = 10030, 
	MSPP_ERROR_NO_TASK                    = 10031, 
	MSPP_ERROR_NOT_AUDIO_END              = 10032,
	MSPP_ERROR_CREAT_INST                 = 10033,
	MSPP_ERROR_DESTORY_INST               = 10034,
	MSPP_ERROR_AUDIO_WRITE                = 10035,
	MSPP_ERROR_GET_RESULT                 = 10036,
	MSPP_ERROR_GET_FEA                    = 10037,
	MSPP_ERROR_UPDATE_FEA				  = 10038,

};
#endif // ifly_mspp_error_h__
