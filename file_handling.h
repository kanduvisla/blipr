#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "project.h"

void writeProjectFile(struct Project *project, const char *fileName);
struct Project readProjectFile(const char *fileName);

#endif