#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

void printLog(const char* format, ...);
void print(const char* format, ...);
void printWarning(const char* format, ...);
void printError(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif