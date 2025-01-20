#ifndef BOXING_LOG_H
#define BOXING_LOG_H

/*****************************************************************************
**
**  Definition of the logging functions
**
**  Creation date:  2014/12/16
**  Created by:     Piql AS
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "boxing/platform/platform.h"

// Define DLOG_DISABLED to disable logging completely
#if !defined (DLOG_DISABLED)


#define DLOG_LEVEL_INFO    0
#define DLOG_LEVEL_WARNING 1
#define DLOG_LEVEL_ERROR   2
#define DLOG_LEVEL_FATAL   3
#define DLOG_LEVEL_ALWAYS  4

// Assign custom log function to static function pointers to override default log functions
extern void(*boxing_log_custom)(int log_level, const char * string);
extern void(*boxing_log_args_custom)(int log_level, const char * format, va_list args);

typedef void(*boxing_fatal_exception_cb_t)(const char * str);
extern boxing_fatal_exception_cb_t boxing_fatal_exception;

void boxing_log(int log_level, const char * string);
void boxing_log_args(int log_level, const char * format, ...);

#define DFATAL(expression, string) if ( !(expression) ) { boxing_log(DLOG_LEVEL_FATAL, string); if(boxing_fatal_exception) (*boxing_fatal_exception)(string); }

#define DLOG_ALWAYS(string) boxing_log(DLOG_LEVEL_ALWAYS, string);
#define DLOG_ALWAYS1(string, var1) boxing_log_args(DLOG_LEVEL_ALWAYS, string, var1);

#define DLOG_FATAL(string) boxing_log(DLOG_LEVEL_FATAL, string);
#define DLOG_FATAL1(string, var1) boxing_log_args(DLOG_LEVEL_FATAL, string, var1);
#define DLOG_FATAL2(string, var1, var2) boxing_log_args(DLOG_LEVEL_FATAL, string, var1, var2);
#define DLOG_FATAL3(string, var1, var2, var3) boxing_log_args(DLOG_LEVEL_FATAL, string, var1, var2, var3);
#define DLOG_FATAL4(string, var1, var2, var3, var4) boxing_log_args(DLOG_LEVEL_FATAL, string, var1, var2, var3, var4);

#define DLOG_ERROR(string) boxing_log(DLOG_LEVEL_ERROR, string);
#define DLOG_ERROR1(string, var1) boxing_log_args(DLOG_LEVEL_ERROR, string, var1);
#define DLOG_ERROR2(string, var1, var2) boxing_log_args(DLOG_LEVEL_ERROR, string, var1, var2);
#define DLOG_ERROR3(string, var1, var2, var3) boxing_log_args(DLOG_LEVEL_ERROR, string, var1, var2, var3);
#define DLOG_ERROR4(string, var1, var2, var3, var4) boxing_log_args(DLOG_LEVEL_ERROR, string, var1, var2, var3, var4);

#define DLOG_WARNING(string) boxing_log(DLOG_LEVEL_WARNING, string);
#define DLOG_WARNING1(string, var1) boxing_log_args(DLOG_LEVEL_WARNING, string, var1);
#define DLOG_WARNING2(string, var1, var2) boxing_log_args(DLOG_LEVEL_WARNING, string, var1, var2);
#define DLOG_WARNING3(string, var1, var2, var3) boxing_log_args(DLOG_LEVEL_WARNING, string, var1, var2, var3);
#define DLOG_WARNING4(string, var1, var2, var3, var4) boxing_log_args(DLOG_LEVEL_WARNING, string, var1, var2, var3, var4);

#define DLOG_INFO(string) boxing_log(DLOG_LEVEL_INFO, string);
#define DLOG_INFO1(string, var1) boxing_log_args(DLOG_LEVEL_INFO, string, var1);
#define DLOG_INFO2(string, var1, var2) boxing_log_args(DLOG_LEVEL_INFO, string, var1, var2);
#define DLOG_INFO3(string, var1, var2, var3) boxing_log_args(DLOG_LEVEL_INFO, string, var1, var2, var3);
#define DLOG_INFO4(string, var1, var2, var3, var4) boxing_log_args(DLOG_LEVEL_INFO, string, var1, var2, var3, var4);

#else

#define DFATAL(expression, string)

#define DLOG_ALWAYS(string)
#define DLOG_ALWAYS1(string, var1)

#define DLOG_FATAL(string)
#define DLOG_FATAL1(string, var1)
#define DLOG_FATAL2(string, var1, var2)
#define DLOG_FATAL3(string, var1, var2, var3)
#define DLOG_FATAL4(string, var1, var2, var3, var4)

#define DLOG_ERROR(string) 
#define DLOG_ERROR1(string, var1) 
#define DLOG_ERROR2(string, var1, var2) 
#define DLOG_ERROR3(string, var1, var2, var3) 
#define DLOG_ERROR4(string, var1, var2, var3, var4) 

#define DLOG_WARNING(string) 
#define DLOG_WARNING1(string, var1) 
#define DLOG_WARNING2(string, var1, var2) 
#define DLOG_WARNING3(string, var1, var2, var3) 
#define DLOG_WARNING4(string, var1, var2, var3, var4) 

#define DLOG_INFO(string) 
#define DLOG_INFO1(string, var1) 
#define DLOG_INFO2(string, var1, var2) 
#define DLOG_INFO3(string, var1, var2, var3) 
#define DLOG_INFO4(string, var1, var2, var3, var4) 

#endif // DLOG_DISABLED

#define boxing_throw(a) {DLOG_ERROR(a); assert(0);}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
