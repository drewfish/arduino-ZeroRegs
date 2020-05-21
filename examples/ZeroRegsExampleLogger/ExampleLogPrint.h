#ifndef EXAMPLE_LOG_STREAM_H
#define EXAMPLE_LOG_STREAM_H

#include <Print.h>


class ExampleLogPrint : public Print {
    Print &wrapped;
    bool prefixWritten;

    // copied(simplified) from platformio packages/framework-arduinosam/cores/samd/Print.cpp
    size_t writeNumber(unsigned long n) {
        char buf[8 * sizeof(long) + 1];
        char *str = &buf[sizeof(buf) - 1];
        *str = '\0';
        do {
            char c = n % 10;
            n /= 10;
            *--str = c + '0';
        } while(n);
        return wrapped.write(str);
    }

    size_t writePrefix() {
        size_t s = 0;
        s += wrapped.write('[');
        s += writeNumber(millis());
        s += wrapped.write(']');
        s += wrapped.write(" level=info msg=\"");
        return s;
    }

    size_t writeSuffix() {
        return wrapped.write('\"');
    }

public:
    ExampleLogPrint(Print &_wrapped) : wrapped(_wrapped), prefixWritten(false) {}

    // virtual function from Print
    size_t write(uint8_t c) {
        size_t s = 0;
        if (! prefixWritten) {
            s += writePrefix();
            prefixWritten = true;
        }
        if (c == '"') {
            // escape the quote we're about to write
            s += wrapped.write('\\');
        }
        if (c == '\r') {
            s += writeSuffix();
        }
        if (c == '\n') {
            // this will cause us to reshow the prefix on the next line
            prefixWritten = false;
        }
        s += wrapped.write(c);
        return s;
    }
};


#endif /*EXAMPLE_LOG_STREAM_H*/
