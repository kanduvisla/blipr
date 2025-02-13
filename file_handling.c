#include <stdio.h>
#include <stdlib.h>
#include "file_handling.h"
#include "project.h"

/**
 * Save project to file
 */
void writeProjectFile(struct Project *project, const char *fileName) {
    FILE *file = fopen(fileName, "w");
    
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    unsigned char arr[PROJECT_BYTE_SIZE];
    projectToByteArray(project, arr);

    printf("Saving project \"%s\"...\n", project->name);
    fwrite(arr, PROJECT_BYTE_SIZE, 1, file);
    fclose(file);
    printf("Saved project file.\n");
}

/**
 * Read Project from file
 */
struct Project* readProjectFile(const char *fileName) {
    FILE *file;

    // Open file
    file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    unsigned char arr[PROJECT_BYTE_SIZE];
    size_t bytesRead = fread(arr, 1, PROJECT_BYTE_SIZE, file);
    fclose(file);

    if (bytesRead != PROJECT_BYTE_SIZE) {
        printf("Error reading file: unexpected file size\n");
        return NULL;
    }

    struct Project* project = malloc(sizeof(struct Project));
    if (project == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    *project = byteArrayToProject(arr);
    return project;
}