/*
 * Minimal printf implementation
 */
#include <stdarg.h>

static void putchar(char c) {
    extern void arch_putchar(char c);
    arch_putchar(c);
}

static void print_str(const char *s) {
    while (*s) putchar(*s++);
}

static void print_num(int val, int base) {
    char buf[32];
    int i = 0;
    int neg = 0;
    
    if (val == 0) {
        putchar('0');
        return;
    }
    
    if (val < 0 && base == 10) {
        neg = 1;
        val = -val;
    }
    
    while (val) {
        int d = val % base;
        buf[i++] = (d < 10) ? '0' + d : 'a' + d - 10;
        val /= base;
    }
    
    if (neg) buf[i++] = '-';
    
    while (i--) putchar(buf[i]);
}

static void print_hex(unsigned int val) {
    char buf[16];
    int i = 0;
    
    if (val == 0) {
        print_str("0x0");
        return;
    }
    
    print_str("0x");
    
    while (val) {
        int d = val & 0xF;
        buf[i++] = (d < 10) ? '0' + d : 'a' + d - 10;
        val >>= 4;
    }
    
    while (i--) putchar(buf[i]);
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char c;
    while ((c = *fmt++)) {
        if (c == '%') {
            char spec = *fmt++;
            switch (spec) {
                case 'c': putchar(va_arg(args, int)); break;
                case 's': print_str(va_arg(args, char*)); break;
                case 'd': print_num(va_arg(args, int), 10); break;
                case 'u': print_num(va_arg(args, unsigned int), 10); break;
                case 'x': print_hex(va_arg(args, unsigned int)); break;
                case 'p': print_hex((unsigned int)va_arg(args, void*)); break;
                case '%': putchar('%'); break;
                default: putchar('?'); break;
            }
        } else {
            putchar(c);
        }
    }
    
    va_end(args);
    return 0;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char c;
    int n = 0;
    
    while ((c = *fmt++)) {
        if (c == '%') {
            char spec = *fmt++;
            char tmp[32];
            int len = 0;
            switch (spec) {
                case 'c':
                    buf[n++] = va_arg(args, int);
                    break;
                case 's':
                    {
                        char *s = va_arg(args, char*);
                        while (*s) buf[n++] = *s++;
                    }
                    break;
                case 'd':
                case 'u':
                case 'x':
                    /* Simplified - just append placeholder */
                    buf[n++] = '?';
                    break;
                default:
                    buf[n++] = spec;
                    break;
            }
        } else {
            buf[n++] = c;
        }
    }
    buf[n] = '\0';
    va_end(args);
    return n;
}