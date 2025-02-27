#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GRAY    "\x1b[90m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_timestamped_message(const char* level, const char* color, const char* format, va_list args) {
    time_t now;
    struct tm *tm_info;
    char timestamp[20];

    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("%s%s [%s] ", color, timestamp, level);
    vprintf(format, args);
    printf(ANSI_COLOR_RESET "\n");
}

void printLog(const char* format, ...) {
    va_list args;
    va_start(args, format);
    print_timestamped_message("LOG", ANSI_COLOR_GRAY, format, args);
    va_end(args);
}

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    print_timestamped_message("INFO", ANSI_COLOR_RESET, format, args);
    va_end(args);
}

void printWarning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    print_timestamped_message("WARN", ANSI_COLOR_YELLOW, format, args);
    va_end(args);
}

void printError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    print_timestamped_message("ERROR", ANSI_COLOR_RED, format, args);
    va_end(args);
}