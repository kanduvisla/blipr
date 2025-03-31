#include "drumkit_sequencer.hpp"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../drawing_icons.h"
#include "../trigg_helper.hpp"

/**
 * Constructor 
 */
DrumkitSequencer::DrumkitSequencer() {}

/** 
 * Destructor
 */
DrumkitSequencer::~DrumkitSequencer() {}

/**
 * Reset template note to default values
 */
void DrumkitSequencer::resetTemplateNote() {
    templateNote.enabled = false;
    // @TODO: Fetch this from configuration:
    templateNote.note = 36;     // C-2
    templateNote.velocity = 100;
    templateNote.length = 1;
    templateNote.nudge = PP16N; // PP16N = middle, nudge 0
    templateNote.trigg = 0;
    templateNote.cc1Value = 0;
    templateNote.cc2Value = 0;
}

/**
 * Toggle a step
 */
void DrumkitSequencer::toggleStep(struct Step *step, int noteIndex) {
    if (step->notes[noteIndex].enabled) {
        // Decrease velocity:
        if (step->notes[noteIndex].velocity > 100) {
            step->notes[noteIndex].velocity = 100;
        } else if (step->notes[noteIndex].velocity > 50) {
            step->notes[noteIndex].velocity = 50;
        } else if (step->notes[noteIndex].velocity > 25) {
            step->notes[noteIndex].velocity = 25;
        } else {
            step->notes[noteIndex].enabled = false;    
        }
    } else {
        step->notes[noteIndex].enabled = true;
        // Get proper note according to drumkit configuration
        // Use template note
        
        // copyNote(&templateNote, &step->notes[noteIndex]);
        // Reset enabled state, because template note might be a disabled note :-/
        step->notes[noteIndex].enabled = true;
    }
}

/**
 * Handle key input when shift 2 is down
 */
void DrumkitSequencer::handleKeyWithShift2Down(struct Track *track, int index) {
    // Also set selected note
    int polyCount = getPolyCount(track);
    templateNote.velocity = 100;
    switch(index) {
        case 0:
            // Kick
            selectedNote = 0;
            templateNote.note = 36;
            break;
        case 1:
            // Snare
            selectedNote = 1;
            templateNote.note = 40;
            break;
        case 2:
            // Clap
            selectedNote = polyCount == 8 ? 2 : 1;
            templateNote.note = 39;
            break;
        case 3:
            // Rimshot
            selectedNote = 1;
            templateNote.note = 37;
            break;
        case 4:
            // C.Hat 1
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 42;
            break;
        case 5:
            // C.Hat 2
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 44;
            break;
        case 6:
            // O.Hat
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 46;
            break;
        case 7:
            // Ride
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 51;
            break;
        case 8:
            // Crash
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 55;
            break;
        case 9:
            // L.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 45;
            break;
        case 10:
            // M.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 48;
            break;
        case 11:
            // H.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 50;
            break;
        case 12:
            // Cowbell
            selectedNote = polyCount == 8 ? 6 : 1;
            templateNote.note = 56;
            break;
        case 13:
            // Extra 1
            selectedNote = polyCount == 8 ? 6 : 0;
            templateNote.note = 71;
            break;
        case 14:
            // Extra 2
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 73;
            break;
        case 15:
            // Extra 3
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 75;
            break;
        default:
            // Nothing
            break;
    }

    if (polyCount == 1) {
        selectedNote = 0;
    }   
}

/**
 * Draw an overlay on the step button (depends on sequencer type)
 */
void DrumkitSequencer::drawStepButtonOverlay(const int index, const struct Note *note, const struct Step* step, const struct Track *track) {
    if (!note->enabled) {
        return;
    }

    // If this note is not equal to the template note, it means that it is a different drumkit
    // Instrument. So we need to make that visually clear:
    if (note->note != templateNote.note) {
        drawDimmedOverlay(
            index % 4,
            index / 4 + (index % 4),
            BUTTON_WIDTH - 4,
            BUTTON_HEIGHT - 4
        );
    }
}

/**
 * Draw the main sequencer with shift 2 down (overlay)
 * @return bool to determine if the main sequencer should still be drawn or not
 */
bool DrumkitSequencer::drawSequencerMainWithShift2Down() {
    // Draw drumkit instrument selector:
    drawIconOnIndex(0, BLIPR_ICON_KICK);
    // Snare
    // Clap
    // Rimshot

    drawIconOnIndex(4, BLIPR_ICON_CHAT_1);
    drawIconOnIndex(5, BLIPR_ICON_CHAT_2);
    drawIconOnIndex(6, BLIPR_ICON_OHAT);
    drawIconOnIndex(7, BLIPR_ICON_RIDE);

    drawIconOnIndex(8, BLIPR_ICON_CRASH);
    // L. Tom
    // M. Tom
    // H. Tom

    // Cowbell
    // Extra 1
    // Extra 2
    // Extra 3

    return false;
}

/**
 * Draw the sequencer
 */
/*
void DrumkitSequencer::drawSequencerMain(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack
) {
    if (keyStates[BLIPR_KEY_SHIFT_2]) {
        // Draw drumkit instrument selector:
        drawIconOnIndex(0, BLIPR_ICON_KICK);
        // Snare
        // Clap
        // Rimshot

        drawIconOnIndex(4, BLIPR_ICON_CHAT_1);
        drawIconOnIndex(5, BLIPR_ICON_CHAT_2);
        drawIconOnIndex(6, BLIPR_ICON_OHAT);
        drawIconOnIndex(7, BLIPR_ICON_RIDE);

        drawIconOnIndex(8, BLIPR_ICON_CRASH);
        // L. Tom
        // M. Tom
        // H. Tom

        // Cowbell
        // Extra 1
        // Extra 2
        // Extra 3
    } else {
        // Draw currently playing steps:

        // Outline currently active step:
        int width = HEIGHT / 6;
        int height = width;

        // Step indicator:
        int playingPage = 0;

        // Check track speed (we do this by manupulating the pulse):
        uint64_t pulse = *ppqnCounter;
        applySpeedToPulse(selectedTrack, &pulse);
        int trackStepIndex = getTrackStepIndex(&pulse, selectedTrack, false);

        // Get playing page:
        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
            playingPage = trackStepIndex / 16;
        } else {
            playingPage = selectedTrack->selectedPage;
        }    
        
        // Draw outline on currently playing note:
        if (
            playingPage >= selectedTrack->selectedPage && playingPage < selectedTrack->selectedPage + 1
        ) {
                int x = trackStepIndex % 4;
                int y = (trackStepIndex / 4) % 4;
                drawSingleLineRectOutline(
                2 + x + (x * width),
                2 + y + (y * height),
                width,
                height,
                COLOR_WHITE
            );
        }

        // Highlight playing page:
        int polyCount = getPolyCount(selectedTrack);

        // Show cut & copy information:
        if (cutCounter > 0 || copyCounter > 0) {
            char bottomText[64];
            if (cutCounter == 1) {
                snprintf(bottomText, 64, "CUTTED 1 NOTE");
            } else if (cutCounter > 1) {
                snprintf(bottomText, 64, "CUTTED ALL NOTES");
            }

            if (copyCounter == 1) {
                snprintf(bottomText, 64, "PASTED 1 NOTE");
            } else if (copyCounter > 1) {
                snprintf(bottomText, 64, "PASTED ALL NOTES");
            }
            drawCenteredLine(2, HEIGHT - BUTTON_HEIGHT - 12, bottomText, BUTTON_WIDTH * 4, COLOR_YELLOW);
        } else if (!keyStates[BLIPR_KEY_SHIFT_2]) {
            drawPageIndicator(selectedTrack, playingPage);
        }

        int noteIndicatorOffset = 12 - polyCount;

        // Highlight non-empty steps:
        for (int j = 0; j < 4; j++) {
            int height = width;
            for (int i = 0; i < 4; i++) {
                int stepIndex = ((i + (j * 4)) + (selectedTrack->selectedPage * 16)) % 64;

                struct Step step = selectedTrack->steps[stepIndex];
                // Check if this is within the track length, or outside the page length:
                if (
                    ((selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) && (selectedTrack->selectedPage * 16) + i + (j * 4) > selectedTrack->trackLength) ||
                    ((selectedTrack->pagePlayMode == PAGE_PLAY_MODE_REPEAT) && (i + (j * 4)) > selectedTrack->pageLength)
                ) {
                    // No note here
                    drawRect(
                        4 + i + (i * width),
                        4 + j + (j * height),
                        width - 4,
                        height - 4,
                        COLOR_GRAY
                    );
                } else {
                    int noteIndex = (selectedTrack->playingPageBank * polyCount) + selectedNote;

                    if (step.notes[noteIndex].enabled) {
                        struct Note *note = &step.notes[noteIndex];
                        SDL_Color noteColor = note->velocity >= 100 ? COLOR_RED : 
                            (note->velocity >= 50 ? COLOR_DARK_RED : COLOR_LIGHT_GRAY);
                        if (TriggHelper::isTrigged(note->trigg, selectedTrack->repeatCount)) {
                            drawRect(
                                4 + i + (i * width),
                                4 + j + (j * height),
                                width - 4,
                                height - 4,
                                noteColor
                            );
                            // Check if this note has a trigg condition
                            if (note->trigg > 0) {
                                drawSingleLineRectOutline(
                                    4 + i + (i * width),
                                    4 + j + (j * height),
                                    width - 4,
                                    height - 4,
                                    mixColors(COLOR_RED, COLOR_WHITE, 0.5f)
                                );    
                            }                      
                            // If this note is not equal to the template note, it means that it is a different drumkit
                            // Instrument. So we need to make that visually clear:
                            if (note->note != templateNote.note) {
                                drawDimmedOverlay(
                                    4 + i + (i * width),
                                    4 + j + (j * height),
                                    width - 4,
                                    height - 4
                                );
                            }
                        } else {
                            // Here is a note, but it is not trigged by the fill condition:
                            drawSingleLineRectOutline(
                                4 + i + (i * width),
                                4 + j + (j * height),
                                width - 4,
                                height - 4,
                                noteColor
                            );
                        }

                        // Draw nudge box:
                        if (note->nudge < PP16N) {
                            drawRect(
                                2 + i + (i * width),
                                2 + j + (j * height) + (BUTTON_HEIGHT * 0.4),
                                2,
                                5,
                                noteColor
                            );
                        } else if (note->nudge > PP16N) {
                            drawRect(
                                i + (i * width) + BUTTON_WIDTH,
                                2 + j + (j * height) + (BUTTON_HEIGHT * 0.4),
                                2,
                                5,
                                noteColor
                            );
                        }
                    }
                }

                // Draw selection outline:
                if (selectedSteps[i + (j * 4)]) {
                    drawSingleLineRectOutline(
                        4 + i + (i * width),
                        4 + j + (j * height),
                        width - 4,
                        height - 4,
                        COLOR_YELLOW
                    );
                }                
            }
        }   
    }

    // Draw template Note details:
    drawTemplateNote();

    // ABCD Buttons:
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Show utilities:
        char descriptions[4][4] = {"OPT", "CUT", "CPY", "PST"};        
        drawABCDButtons(descriptions);
    } else if (keyStates[BLIPR_KEY_SHIFT_2]) {
        // Note (for polyphony)
        char descriptions[4][4] = {"-", "-", "-", "-"};
        int polyCount = getPolyCount(selectedTrack);
        if (polyCount < 8) {
            snprintf(descriptions[0], 4, "<");
            snprintf(descriptions[1], 4, ">");
        }
        drawABCDButtons(descriptions);
        // Draw page bank title:
        drawCenteredLine(
            2, 
            HEIGHT - BUTTON_HEIGHT - 12, 
            "PAGEBANK",
            BUTTON_WIDTH * 2,
            COLOR_YELLOW
        );
        // Draw page bank number:
        char pageBankText[2];
        snprintf(pageBankText, 2, "%d", selectedPageBank + 1);
        drawText(2 + 28, HEIGHT - BUTTON_HEIGHT + 2, pageBankText, BUTTON_WIDTH, COLOR_WHITE);
        // Draw channel title:
        drawCenteredLine(
            6 + (BUTTON_WIDTH * 2), 
            HEIGHT - BUTTON_HEIGHT - 12, 
            "CHANNEL",
            BUTTON_WIDTH * 2,
            COLOR_YELLOW
        );
        // Draw channel number:
        char channelText[2];
        snprintf(channelText, 2, "%d", selectedNote + 1);
        drawText(92, HEIGHT - BUTTON_HEIGHT + 2, channelText, BUTTON_WIDTH, COLOR_WHITE);
    } else {
        // Page numbers:
        char descriptions[4][4] = {"P00", "P00", "P00", "P00"};
        int startPage = (selectedPageBank * 4) + 1;
        
        // Fill the descriptions array
        for (int i = 0; i < 4; i++) {
            int pageNumber = startPage + i;
            // Format with leading zero if needed
            if (pageNumber < 10) {
                snprintf(descriptions[i], 4, "P0%d", pageNumber);
            } else {
                snprintf(descriptions[i], 4, "P%d", pageNumber);
            }
        }

        drawABCDButtons(descriptions);
    
        // Draw highlighted page:
        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
            if (selectedTrack->selectedPage >= selectedTrack->playingPageBank * 4 && 
                selectedTrack->selectedPage < (selectedTrack->playingPageBank + 1) * 4) {
                // Outline current page:
                drawHighlightedGridTile((selectedTrack->selectedPage % 4) + 16);
            }
        } else {
            // Highlight playing page + queued page:
            if (selectedTrack->queuedPage != selectedTrack->selectedPage) {
                if (selectedTrack->queuedPage >= selectedPageBank * 4 && 
                    selectedTrack->queuedPage < (selectedPageBank + 1) * 4) {
                    // Outline queued page:
                    drawHighlightedGridTileInColor((selectedTrack->queuedPage % 4) + 16, COLOR_RED);
                }
            }

            if (selectedTrack->selectedPage >= selectedPageBank * 4 && 
                selectedTrack->selectedPage < (selectedPageBank + 1) * 4) {
                // Outline queued page:
                drawHighlightedGridTile((selectedTrack->selectedPage % 4) + 16);
            }
        }            
    }
}
*/