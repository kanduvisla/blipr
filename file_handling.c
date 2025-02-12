#include <stdio.h>
#include <stdlib.h>
#include "file_handling.h"
#include "project.h"

#define MAX_LINE_LENGTH 1000

void writeProjectFile(struct Project *project, const char *fileName) {
    FILE *file = fopen(fileName, "w");
    
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    fwrite(project, sizeof(struct Project), 1, file);
    fclose(file);
    printf("Saved project file.\n");
}

struct Project readProjectFile(const char *fileName) {
    struct Project project = initializeProject();
    
    return project;
}