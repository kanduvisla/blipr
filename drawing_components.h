#ifndef DRAWING_COMPONENTS_H
#define DRAWING_COMPONENTS_H

#include <stdbool.h>

void drawButton(int x, int y, int width, int height);
void drawSequencerButton(int x, int y, int width, int height, bool isActive);
void drawBPMBlinker(int noteCounter, int ppqnCounter);
void drawNoteCounter(int noteCounter);
void drawPageCounter(int pageCounter);

#endif