#include <SDL.h>
#include <stdbool.h>
#include <string.h>
#include "../drawing_icons.h"
#include "../colors.h"
#include "../drawing_text.h"
#include "../drawing_components.h"
#include "../constants.h"
#include "../project.h"
#include "../utils.h"
#include <portmidi.h>
#include <porttime.h>

bool isMidiConfigActive = false;
int selectedMidiDevice = BLIPR_MIDI_DEVICE_A;

bool isMainScreen() {
    return !isMidiConfigActive;
}

void resetConfigurationScreen() {
    isMidiConfigActive = false;
    selectedMidiDevice = BLIPR_MIDI_DEVICE_A;
}

void drawConfigSelection(struct Project *project) {
    if (isMainScreen()) {
        drawIconOnIndex(0, BLIPR_ICON_MIDI);
        drawCenteredLine(2, 133, "CONFIGURATION", TITLE_WIDTH, COLOR_WHITE);       
    } else {
        if (isMidiConfigActive) {
            drawCenteredLine(2, 130, "SELECT A DEVICE FOR", TITLE_WIDTH, COLOR_WHITE);
            drawCenteredLine(2, 137, "MIDI SLOT A,B,C OR D", TITLE_WIDTH, COLOR_WHITE);
            // Highlight current selected Midi device:
            drawHighlightedGridTile(16 + selectedMidiDevice);

            // Midi device #0 is "NONE"
            bool isSelected = false;
            if (
                (selectedMidiDevice == BLIPR_MIDI_DEVICE_A && strcmp(project->midiDeviceAName, "") == 0) ||
                (selectedMidiDevice == BLIPR_MIDI_DEVICE_B && strcmp(project->midiDeviceBName, "") == 0) ||
                (selectedMidiDevice == BLIPR_MIDI_DEVICE_C && strcmp(project->midiDeviceCName, "") == 0) ||
                (selectedMidiDevice == BLIPR_MIDI_DEVICE_D && strcmp(project->midiDeviceDName, "") == 0)
            ) { 
                drawText(4, 4, "0:*NONE", WIDTH, COLOR_WHITE);
            } else {
                drawText(4, 4, "0: NONE", WIDTH, COLOR_WHITE);
            }

            // List Midi Devices:
            int num_devices = Pm_CountDevices();
            int y = 1;
            for (int i = 0; i < num_devices; i++) {
                const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
                if (info->input) {
                    char line[32];
                    char name[32];
                    memcpy(name, info->name, 32);
                    upperCase(name);
                    isSelected = false;          
                    if (selectedMidiDevice == BLIPR_MIDI_DEVICE_A) { 
                        isSelected =  strcmp(project->midiDeviceAName, info->name) == 0;
                    } else if (selectedMidiDevice == BLIPR_MIDI_DEVICE_B) { 
                        isSelected =  strcmp(project->midiDeviceBName, info->name) == 0;
                    } else if (selectedMidiDevice == BLIPR_MIDI_DEVICE_C) { 
                        isSelected =  strcmp(project->midiDeviceCName, info->name) == 0;
                    } else if (selectedMidiDevice == BLIPR_MIDI_DEVICE_D) { 
                        isSelected =  strcmp(project->midiDeviceDName, info->name) == 0;
                    }
                    int result = snprintf(line, sizeof(line), "%d:%s%s", y + 1, isSelected? "*" : " ", name);
                    if (result < 0 || result >= sizeof(line)) {
                        // Handle error: string was truncated or an error occurred
                        snprintf(line, sizeof(line), "%d:(ERROR)", y + 1);
                    }
                    drawText(4, 4 + (y * 6), line, WIDTH, COLOR_WHITE);
                    y ++;
                }
            }
            // ABCD Buttons:
            char descriptions[4][4] = {" A ", " B ", " C ", " D "};
            drawABCDButtons(descriptions);            
        }      
    }
}

/**
 * Set the midi device name on the project
 */
void setMidiDeviceName(struct Project *project, int deviceOnProject, int indexInList) {
    if (indexInList == 99) {
        // Device is set to "NONE"
        if (deviceOnProject == BLIPR_MIDI_DEVICE_A) { memset(project->midiDeviceAName, 0, 32); } else
        if (deviceOnProject == BLIPR_MIDI_DEVICE_B) { memset(project->midiDeviceAName, 0, 32); } else
        if (deviceOnProject == BLIPR_MIDI_DEVICE_C) { memset(project->midiDeviceAName, 0, 32); } else
        if (deviceOnProject == BLIPR_MIDI_DEVICE_D) { memset(project->midiDeviceAName, 0, 32); }
        return;
    }

    int num_devices = Pm_CountDevices();
    int j = 0;
    for (int i = 0; i < num_devices; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (info->input) {
            if (j == indexInList) {
                printf("Setting Midi Device %d to %s\n", deviceOnProject, info->name);
                if (deviceOnProject == BLIPR_MIDI_DEVICE_A) { 
                    memcpy(project->midiDeviceAName, info->name, 32);
                }
                else if (deviceOnProject == BLIPR_MIDI_DEVICE_B) { 
                    memcpy(project->midiDeviceBName, info->name, 32);
                }
                else if (deviceOnProject == BLIPR_MIDI_DEVICE_C) { 
                    memcpy(project->midiDeviceCName, info->name, 32);
                }
                else if (deviceOnProject == BLIPR_MIDI_DEVICE_D) { 
                    memcpy(project->midiDeviceDName, info->name, 32);
                }
                return;
            }
            j++;
        }
    }
}

bool updateConfiguration(struct Project *project, SDL_Scancode key) {
    if (isMainScreen()) {
        // No config selected, so we're on the main screen
        if (key == BLIPR_KEY_1) {
            isMidiConfigActive = true;
        }
    } else {
        if (isMidiConfigActive) {
            if (key == BLIPR_KEY_A) { selectedMidiDevice = BLIPR_MIDI_DEVICE_A; }
            else if (key == BLIPR_KEY_B) { selectedMidiDevice = BLIPR_MIDI_DEVICE_B; }
            else if (key == BLIPR_KEY_C) { selectedMidiDevice = BLIPR_MIDI_DEVICE_C; }
            else if (key == BLIPR_KEY_D) { selectedMidiDevice = BLIPR_MIDI_DEVICE_D; }
            else if (key == BLIPR_KEY_1) { setMidiDeviceName (project, selectedMidiDevice, 99); return true; }
            else if (key == BLIPR_KEY_2) { setMidiDeviceName (project, selectedMidiDevice, 0); return true; }
            else if (key == BLIPR_KEY_3) { setMidiDeviceName (project, selectedMidiDevice, 1); return true; }
            else if (key == BLIPR_KEY_4) { setMidiDeviceName (project, selectedMidiDevice, 2); return true; }
            else if (key == BLIPR_KEY_5) { setMidiDeviceName (project, selectedMidiDevice, 3); return true; }
            else if (key == BLIPR_KEY_6) { setMidiDeviceName (project, selectedMidiDevice, 4); return true; }
            else if (key == BLIPR_KEY_7) { setMidiDeviceName (project, selectedMidiDevice, 5); return true; }
            else if (key == BLIPR_KEY_8) { setMidiDeviceName (project, selectedMidiDevice, 6); return true; }
            else if (key == BLIPR_KEY_9) { setMidiDeviceName (project, selectedMidiDevice, 7); return true; }
            else if (key == BLIPR_KEY_10) { setMidiDeviceName (project, selectedMidiDevice, 8); return true; }
            else if (key == BLIPR_KEY_11) { setMidiDeviceName (project, selectedMidiDevice, 9); return true; }
            else if (key == BLIPR_KEY_12) { setMidiDeviceName (project, selectedMidiDevice, 10); return true; }
            else if (key == BLIPR_KEY_13) { setMidiDeviceName (project, selectedMidiDevice, 11); return true; }
            else if (key == BLIPR_KEY_14) { setMidiDeviceName (project, selectedMidiDevice, 12); return true; }
            else if (key == BLIPR_KEY_15) { setMidiDeviceName (project, selectedMidiDevice, 13); return true; }
            else if (key == BLIPR_KEY_16) { setMidiDeviceName (project, selectedMidiDevice, 14); return true; }
        }
    }

    return false;
}