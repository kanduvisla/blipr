#include <stdio.h>
#include <stdlib.h>
#include "file_handling.h"
#include "project.h"
#include "print.h"

/**
 * Save project to file
 */
void writeProjectFile(struct Project *project, const char *fileName) {
    FILE *file = fopen(fileName, "w");

    if (file == NULL) {
        printError("Error opening file for writing!");
        return;
    }

    unsigned char *arr = malloc(PROJECT_BYTE_SIZE);
    projectToByteArray(project, arr);

    printLog("Saving project \"%s\"...", project->name);
    fwrite(arr, PROJECT_BYTE_SIZE, 1, file);
    free(arr);
    fclose(file);
    printLog("Saved project file.");
}

/**
 * Read Project from file
 */
struct Project* readProjectFile(const char *fileName) {
    FILE *file;

    // Open file
    file = fopen(fileName, "rb");
    if (file == NULL) {
        printError("Error opening file");
        return NULL;
    }

    unsigned char *arr = malloc(PROJECT_BYTE_SIZE);
    size_t bytesRead = fread(arr, 1, PROJECT_BYTE_SIZE, file);
    fclose(file);

    if (bytesRead != PROJECT_BYTE_SIZE) {
        printError("Error reading file: unexpected file size");
        return NULL;
    }

    struct Project* project = byteArrayToProject(arr);
    free(arr);
    if (project == NULL) {
        printError("Failed to create project from byte array");
        exit(1);
    }

    return project;
}