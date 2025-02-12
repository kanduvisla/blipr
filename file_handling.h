#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "project.h"

void writeProjectFile(struct Project project);
struct Project readProjectFile();

#endif