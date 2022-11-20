#include <charge_controller/Logger.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

Logger LOG;

void Logger::setUART(IO::UART *uart) {
    this->uart = uart;
}

void Logger::setLogLevel(LogLevel level) {
    this->level = level;
}

void Logger::log(LogLevel level, const char *format, ...) {
    if(!uart)
        return;

    if(this->level>level)
        return;

    switch (level) {
        case LogLevel::DEBUG:
            uart->puts("DEBUG::");
            break;
        case LogLevel::INFO:
            uart->puts("INFO::");
            break;
        case LogLevel::WARNING:
            uart->puts("WARNING::");
            break;
        case LogLevel::ERROR:
            uart->puts("ERROR::");
            break;
    }

    va_list args;
    va_start(args, format);

    char string[200];
    if(vsprintf(string, format, args)>0) {
        uint8_t *data = reinterpret_cast<uint8_t *>(&string);
        uart->writeBytes(data, strlen(string));
        uart->puts("\r\n");
    }
//    uart->printf("%s\r\n", string);

    va_end(args);
}