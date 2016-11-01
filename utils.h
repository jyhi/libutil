/**
 * @file utils.h
 * @version 0.1
 * @author Junde Yhi <lmy441900@aosc.xyz>
 * @copyright (C) 2016 Junde Yhi
 * @copyright This program is licensed under WTFPLv2. See COPYING file for details.
 *
 * This file is the main header file for libutil.
 *
 * All declarations and implementations are in this single file. You only need to include it to use it.
 */

#ifndef _LIB_UTILS_H
#define _LIB_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* _OutputType: type of messages for _output */
enum _OutputType {
    Info = 0,       //< Info
    Warn_NoAck = 1, //< Warning without user interaction, generally non-urgent things
    Warn_Ack = 2,   //< Warning with user interaction (Y/N)
    Error = 3       //< Fatal error, and will cause the program abort.
};

/**
 * A safe malloc implementation, which kills the program when memory allocation fails.
 *
 * @param  size [in] Size of memory to allocate.
 * @return A pointer points to the allocated memory.
 *
 * @see _xrealloc _xfree
 */
void *_xmalloc (const size_t size);

/**
 * A safe realloc implementation, which kills the program when memory allocation fails.
 *
 * @param  ptr  [in] A pointer points to the memory to be re-allocated.
 * @param  size [in] Size of memory to allocate
 * @return A pointer points to the newly-allocated memory.
 *
 * @see _xmalloc _xfree
 */
void *_xrealloc (void *ptr, const size_t size);

/**
 * A safe free implementation, which frees the data and sets the pointer to NULL.
 *
 * @param ptr [in] A pointer points to the memory to be free'd
 *
 * @see _xmalloc _xrealloc
 */
#define _xfree(ptr) \
{ \
    free ((ptr)); \
    (ptr) = NULL; \
}

/**
 * Print messages in different types.
 *
 * This function comes with various kinds of wrappers. Usually using the easy-to-use wrappers are enough.
 *
 * @param file [in] The source file name. Usually __FILE__ here.
 * @param func [in] The function name. Usually __func__ here.
 * @param line [in] The line number. Usually __LINE__ here.
 * @param outputType [in] THe output type. See _OutputType.
 * @param fmt  [in] The format of strings to be displayed. See how to use printf.
 */
void __output (const char *file, const char *func, const int line, enum _OutputType outputType, const char *fmt, ...);

#define _output(output_type, ...) __output (__FILE__, __func__, __LINE__, output_type, ##__VA_ARGS__)

#define _info(...) _output (Info, ##__VA_ARGS__)
#define _warn_noack(...) _output (Warn_NoAck, ##__VA_ARGS__)
#define _warn_ack(...) _output (Warn_Ack, ##__VA_ARGS__)
#define _warn _warn_noack
#define _warning _warn
#define _error(...) _output (Error, ##__VA_ARGS__)


/********** Implementation starts from here **********/


void *_xmalloc (const size_t size)
{
    void *ptr = malloc (size);
    if (ptr) {
        return ptr;
    } else {
        fprintf (stderr, " ** libutils: FATAL: memory allocation failed!");
        abort ();
    }
}

void *_xrealloc (void *ptr, const size_t size)
{
    void *newBuf = realloc (ptr, size);
    if ((newBuf != NULL) && (newBuf != ptr)) {
        return newBuf;
    } else {
        fprintf (stderr, " ** libutils: FATAL: memory reallocation failed!");
        abort ();
    }
}

void __output (const char *file, const char *func, const int line, enum _OutputType outputType, const char *fmt, ...)
{
    va_list args;
    static const char *Head = " ** libutils:";
    char yn = 'N';
    va_start (args, fmt);

    switch (outputType) {
        case Info:
            printf ("%s In %s (%s:%d) INFO: ", Head, func, file, line);
            vprintf (fmt, args);
            printf ("\n");
            break;
        case Warn_NoAck:
            printf ("%s In %s (%s:%d) WARN: ", Head, func, file, line);
            vprintf (fmt, args);
            printf ("\n");
            break;
        case Warn_Ack:
            printf ("%s In %s (%s:%d) WARN: ", Head, func, file, line);
            vprintf (fmt, args);
            printf ("\n");
            // Enquire for acknowledge
            while (1) {
                printf (" -> Continue? [y/N]");
                yn = getc (stdin);
                switch (yn) {
                    case 'Y': // fall through
                    case 'y':
                        while (getc(stdin) != EOF) {} // Clean stdin
                        return;
                    case 'N': // fall through
                    case 'n': // fall through
                    case '\r':
                        exit (255); // User terminated
                    default:
                        printf ("    Please answer [y]es or [N]o.");
                        break; // Repeat
                }
            }
            break;
        case Error:
            fprintf (stderr, "%s In %s (%s:%d) FAIL: ", Head, func, file, line);
            vfprintf (stderr, fmt, args);
            printf ("\n");
            abort (); // XXX: Really?
            break;
    }
}

#endif /* end of include guard: _LIB_UTILS_H */
