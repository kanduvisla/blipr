#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "project.h"

#ifdef __cplusplus
extern "C" {
#endif

void writeProjectFile(struct Project *project, const char *fileName);
struct Project* readProjectFile(const char *fileName);

#ifdef __cplusplus
}
#endif

#endif