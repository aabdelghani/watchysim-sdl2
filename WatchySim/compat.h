#ifndef WATCHYSIM_COMPAT_H
#define WATCHYSIM_COMPAT_H

#include <cstdio>
#include <cstdlib>
#include <cstddef>

#ifndef _MSC_VER

inline char *_itoa(long value, char *buffer, int base) {
    if (base == 10) {
        std::snprintf(buffer, 32, "%ld", value);
    } else if (base == 16) {
        std::snprintf(buffer, 32, "%lx", (unsigned long)value);
    } else if (base == 8) {
        std::snprintf(buffer, 32, "%lo", (unsigned long)value);
    } else if (base == 2) {
        unsigned long v = (unsigned long)value;
        if (v == 0) { buffer[0] = '0'; buffer[1] = '\0'; return buffer; }
        char tmp[sizeof(unsigned long) * 8 + 1];
        int j = 0;
        while (v) { tmp[j++] = '0' + (char)(v & 1); v >>= 1; }
        int i = 0;
        while (j) buffer[i++] = tmp[--j];
        buffer[i] = '\0';
    } else {
        std::snprintf(buffer, 32, "%ld", value);
    }
    return buffer;
}

inline char *_ltoa(long value, char *buffer, int base) {
    return _itoa(value, buffer, base);
}

inline char *_ultoa(unsigned long value, char *buffer, int base) {
    return _itoa((long)value, buffer, base);
}

template <std::size_t N>
inline int _itoa_s(long value, char (&buffer)[N], int base) {
    _itoa(value, buffer, base);
    return 0;
}

#endif

#endif
