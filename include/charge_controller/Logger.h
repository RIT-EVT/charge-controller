#include <EVT/io/UART.hpp>

#ifndef LOGGER_H
#define LOGGER_H
namespace IO = EVT::core::IO;

class Logger {
public:
    enum LogLevel{
        DEBUG=0, INFO=1, WARNING=2, ERROR=3
    };

    void setUART(IO::UART* uart);

    void setLogLevel(LogLevel level);

    void log(LogLevel level, const char* format, ...);

private:
    IO::UART* uart = nullptr;

    LogLevel level = LogLevel::ERROR;
};

extern Logger LOG;

#endif //LOGGER_H
