/* Copyright 2001,2002,2003 Roger Dingledine, Matej Pfajfar.
 * Copyright 2004-2005 Roger Dingledine, Nick Mathewson */
/* See LICENSE for licensing information */
/* $Id$ */

/**
 * \file log.h
 *
 * \brief Headers for log.c
 **/

#ifndef __LOG_H
#define LOG_H_ID "$Id$"

#include "../common/compat.h"

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#define LOG_WARN LOG_WARNING
#if LOG_DEBUG < LOG_ERR
#error "Your syslog.h thinks high numbers are more important.  We aren't prepared to deal with that."
#endif
#else
/* XXXX Note: The code was originally written to refer to severities,
 * with 0 being the least severe; while syslog's logging code refers to
 * priorities, with 0 being the most important.  Thus, all our comparisons
 * needed to be reversed when we added syslog support.
 *
 * The upshot of this is that comments about log levels may be messed
 * up: for "maximum severity" read "most severe" and "numerically
 * *lowest* severity".
 */

/** Debug-level severity: for hyper-verbose messages of no interest to
 * anybody but developers. */
#define LOG_DEBUG   7
/** Info-level severity: for messages that appear frequently during normal
 * operation. */
#define LOG_INFO    6
/** Notice-level severity: for messages that appear infrequently
 * during normal operation; that the user will probably care about;
 * and that are not errors.
 */
#define LOG_NOTICE  5
/** Warn-level severity: for messages that only appear when something has gone
 * wrong. */
#define LOG_WARN    4
/** Error-level severity: for messages that only appear when something has gone
 * very wrong, and the Tor process can no longer proceed. */
#define LOG_ERR     3
#endif

/* Logging domains */
#define LD_GENERAL 0
#define LD_CRYPTO  1
#define LD_NET     2
#define LD_CONFIG  3
#define LD_FS      4
#define LD_PROTOCOL 5

typedef void (*log_callback)(int severity, int domain, const char *msg);

int parse_log_level(const char *level);
const char *log_level_to_string(int level);
void add_stream_log(int severityMin, int severityMax, const char *name, FILE *stream);
int add_file_log(int severityMin, int severityMax, const char *filename);
#ifdef HAVE_SYSLOG_H
int add_syslog_log(int loglevelMin, int loglevelMax);
#endif
int add_callback_log(int loglevelMin, int loglevelMax, log_callback cb);
int get_min_log_level(void);
void switch_logs_debug(void);
void close_logs(void);
void reset_logs(void);
void add_temp_log(void);
void close_temp_logs(void);
void mark_logs_temp(void);
void configure_libevent_logging(void);
void suppress_libevent_log_msg(const char *msg);
void change_callback_log_severity(int loglevelMin, int loglevelMax,
                                  log_callback cb);

/* Outputs a message to stdout */
void _log(int severity, int domain, const char *format, ...) CHECK_PRINTF(3,4);

#ifdef __GNUC__
void _log_fn(int severity, int domain,
             const char *funcname, const char *format, ...)
  CHECK_PRINTF(4,5);
/** Log a message at level <b>severity</b>, using a pretty-printed version
 * of the current function name. */
#ifdef OLD_LOG_INTERFACE
#define log_fn(severity, args...)                               \
  _log_fn(severity, LD_GENERAL, __PRETTY_FUNCTION__, args)
#define log(severity, args...)                                  \
  _log(severity, LD_GENERAL, args)
#else
#define log_fn(severity, domain, args...)               \
  _log_fn(severity, domain, __PRETTY_FUNCTION__, args)
#define log _log
#endif
#define debug(domain, args...)                          \
  _log_fn(LOG_DEBUG, domain, __PRETTY_FUNCTION__, args)
#define info(domain, args...)                           \
  _log_fn(LOG_INFO, domain, __PRETTY_FUNCTION__, args)
#define notice(domain, args...)                         \
  _log_fn(LOG_INFO, domain, __PRETTY_FUNCTION__, args)
#define warn(domain, args...)                           \
  _log_fn(LOG_INFO, domain, __PRETTY_FUNCTION__, args)
#define err(args...)                                    \
  _log_fn(LOG_ERR, LD_GENERAL, __PRETTY_FUNCTION__, args)
#else

void _log_fn(int severity, int domain, const char *format, ...);
void _debug(int domain, const char *format, ...);
void _info(int domain, const char *format, ...);
void _notice(int domain, const char *format, ...);
void _warn(int domain, const char *format, ...);
void _err(const char *format, ...);

#define log _log /* hack it so we don't conflict with log() as much */

#if defined(_MSC_VER) && _MSC_VER < 1300
/* MSVC 6 and earlier don't have __FUNCTION__, or even __LINE__. */
#define log_fn _log_fn
#define debug _debug
#define info _info
#define notice _notice
#define warn _warn
#define err _err
#else
/* We don't have GCC's varargs macros, so use a global variable to pass the
 * function name to log_fn */
extern const char *_log_fn_function_name;
/* We abuse the comma operator here, since we can't use the standard
 * do {...} while (0) trick to wrap this macro, since the macro can't take
 * arguments. */
#define log_fn (_log_fn_function_name=__FUNCTION__),_log_fn
#define debug (_log_fn_function_name=__FUNCTION__),_debug
#define info (_log_fn_function_name=__FUNCTION__),_info
#define notice (_log_fn_function_name=__FUNCTION__),_notice
#define warn (_log_fn_function_name=__FUNCTION__),_warn
#define err (_log_fn_function_name=__FUNCTION__),_err
#endif

#endif /* !GNUC */

# define __LOG_H
#endif

