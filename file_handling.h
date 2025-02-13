#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "project.h"

void writeProjectFile(struct Project *project, const char *fileName);
void readProjectFile(struct Project *project, const char *fileName);

#endif