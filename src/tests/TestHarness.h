#ifndef __TESTHARNESS_H__
#define __TESTHARNESS_H__
#ifdef USE_CPPUTEST
#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <vector>
#include <stdio.h>	// For vsnprintf()
#include <stdarg.h>

/* The code below was taken from http://stackoverflow.com/questions/69738/c-how-to-get-fprintf-results-as-a-stdstring-w-o-sprintf#69911
 * If we compile this part, it means we are using cpputest, this implies a c++ compiler will be in use, so we can use C++ here without issue
 * Note: Strutil::vformat is only used in FAILF() macro, that in turn is only used during unit test, so all this code is not going to be in the released library
 */
namespace Strutil {
inline std::string vformat(const char* fmt, ...) {
    // Allocate a buffer on the stack that's big enough for us almost
    // all the time.  Be prepared to allocate dynamically if it doesn't fit.
    size_t size = 1024;
    char stackbuf[size];
    std::vector<char> dynamicbuf;
    char *buf = &stackbuf[0];
    va_list ap;

    while (1) {
        // Try to vsnprintf into our buffer.
        int needed = vsnprintf(buf, size, fmt, ap);
        // NB. C99 (which modern Linux and OS X follow) says vsnprintf
        // failure returns the length it would have needed.  But older
        // glibc and current Windows return -1 for failure, i.e., not
        // telling us how much was needed.

        if (needed <= (int)size && needed >= 0) {
            // It fit fine so we're done.
        	va_end(ap);
            return std::string(buf, (size_t) needed);
        }

        // vsnprintf reported that it wanted to write more characters
        // than we allotted.  So try again using a dynamic buffer.  This
        // doesn't happen very often if we chose our initial size well.
        size = (needed > 0) ? (needed+1) : (size*2);
        dynamicbuf.resize(size);
        buf = &dynamicbuf[0];
    }
}	// vformat()
}	// namespace Strutil

#define FAILF(message, ...) do { FAIL( \
                                      std::string( \
                                                  Strutil::vformat("%s():%d: ", __func__, __LINE__) + \
												  Strutil::vformat(message, ## __VA_ARGS__) \
												 ).c_str() \
                                     ); } while(0);
#define NOTIFYPASS()
#else
#include <stdio.h>	// For printf()
#include <stdlib.h>	// For exit()
#define TEST_GROUP(x) struct testgroup_ ## x
#define TEST(group, func) void func()
#define FAILF(...) \
        do { fprintf(stderr, "%s:%d:%s(): ",__FILE__, __LINE__, __func__);\
             fprintf(stderr, __VA_ARGS__);\
             fputc('\n', stderr); exit(1);\
        } while (0)
#define NOTIFYPASS() do { fprintf(stderr, "%s: tests passed\n", __func__); } while(0);
#endif

#endif // __TESTHARNESS_H__
