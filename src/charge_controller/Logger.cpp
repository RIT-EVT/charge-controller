#include <charge_controller/Logger.h>
#include <cstdarg>
#include <cstdio>

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
            uart->printf("DEBUG::");
            break;
        case LogLevel::INFO:
            uart->printf("INFO::");
            break;
        case LogLevel::WARNING:
            uart->printf("WARNING::");
            break;
        case LogLevel::ERROR:
            uart->printf("ERROR::");
            break;
    }

    va_list args;
    va_start(args, format);

    char string[200];
    vsprintf(string, format, args);
    uart->printf("%s\r\n", string);

    va_end(args);
}