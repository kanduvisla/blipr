#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../project.h"

/// --- Note tests

void testNoteToByteArray() {
    struct Note note;
    note.enabled = true;
    note.note = 78;
    note.velocity = 110;
    note.nudge = 0;

    unsigned char arr[NOTE_BYTE_SIZE];
    noteToByteArray(&note, arr);
    
    assertByte(arr[0], 0x01);
    assertByte(arr[1], 0x4E);
    assertByte(arr[2], 0x6E);
    assertByte(arr[3], 0x3F);
}

void testByteArrayToNote() {
    unsigned char arr[NOTE_BYTE_SIZE] = {0x01, 0x4E, 0x6E, 0x3F};
    struct Note note = byteArrayToNote(arr);

    assert(note.enabled == true);
    assert(note.note == 78);
    assert(note.velocity == 110);
    assert(note.nudge == 0);
}

/// --- Entry point

void testProjectFile() {
    testNoteToByteArray();
    testByteArrayToNote();
}
