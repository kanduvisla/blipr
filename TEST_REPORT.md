# Test Report

---

These are observations done while testing Blipr with real hardware.

--- Bugs:

- Double speed does not seem to work good
- Key repeat is repeating also on steps
- Steps are set on disabled tiles (takes not in account page length (possible also track length))
- (confirm if this still happens) It still appears that something goes wrong when setting steps in the sequencer; other pages seem affected.

--- Improvements:

- When no notes are selected cut/copy/paste = on notes/track/step
- Missing transport / option to start / stop
- When track has no program, shift 3 should start on program selection
- Mute Track
- Solo Track
- Utilities (clear track)
- Program icon on track selection
- Thumbnail of track on track selection
- Panic button!
- Shuffle and nudge should be on the same position in the menu
- More quick velocity switches (with drumkit sequencer)
- Undo / Redo
- Autosave / Backup
- Fix compiler warnings :-)
- `use` consts on locations where properties are read-only
- Replace `sprintf` with `snprintf`

--- Fixed

- With new notes, the default "everything" is 255
- Major performance issues when in note editing menu. Probably due to rendering. (fixed with multi threading)
- Major performance issues on Raspberry Pi, especially with key repeat. (fixed with multi threading)
- Switching to next note crashes
- Opening midi configuration crashes
- When changing midi device for track, it is not applied, only after restart.
- Trigg is not working as expected. It does something, but it takes the wrong one
- Nudge cannnot go negative
- Add `--help` option
- Missing escape option from conf menu
- Add indicator on CPU% - separation in calculations + rendering time
- Midi configuration: No option to select "no device"
- Set default mode to page repeat
- With a normal sequence, if I am on page 2, and try to shift2 to edit a note, i cannot select a note, it automatically shows the edit note screen. The same goes for page repeat mode
- Channel / note number is not shown
- When switching to a different track, the default channel is not set back to 1
- Poly sacrificing / correct track not playing (P01 appears to be P05?)
- Poly sacrificing / cannot select pagebank 5-8 (selected page bank not highlighted)
- Other pages is not working
- Page bank is pickable, even if it's not allowed or visible
- Missing option to set BPM
- Shift2 hides all steps, this is undesired because it's not helping when choosing the proper channel for editing.
- Shift1+Shift2+Note to directly edit a single note
- Paste does not highlight selected step / empty step is not highlighted
- Paste only pastes one step (the last?)
- Pattern change does not trigger a PC
- When configuring midi device, configuration is not directly applied, restart of the program is required. The current device is unassigned, but a new device is not assigned
- Copy / pasting: single paste pastes all notes, while this should only happen after copy twice
- Copy / pasting: when selecting, create range when there are empty values in between
- Last set length is not remembered when editing track.
- Same for last set velocity.
- Copy / Paste of ranges of notes
- Save / Remember last note / velocity / etc. per track
- Selected note is not "picked" (pipet tool)
- "##" does not show with multiple steps with different values
- Message is not clear when pasting notes "COPIED ALL NOTES"?
- Page bank shows -1 (probably wrong location of page bank nr)
- Page indicator when page > 4
- Set number of repeats for transitioning to next page
- Show these transition repeats
- Page bank selection
- Queue patterns same as pages
- Missing page numbers / max pages
- Show default note, length + velocity in sidebar
- Configuration: Midi PC representation needs to be represented +1 (1-16 instead of 0-15)
- Note length is now too long
- Midi CC channel can the same as device (1 = 1, don't +1 them)
- Show queued pattern when selecting
- When more sequencer is used, it seems that note offs are triggered too soon (is note length taken globally instead of per note?)
- Rapidly pressing "Fn" seems to reset the PC / Pattern
- It appears that the note off is not properly send when changing the note tone
- When editing with poly < 8 to get more pages, steps are set in different tracks. There goes something wrong with settings notes
- Cannot set midi device for slot B, C and D (need to review configuration layout)
- Page bank seems to show steps from different track?
- Page bank > 5 shows in step editor but is not playing
- When switching page bank, page is also changed. Defferentiate in playingPageBank and selectedPageBank
- When selecting a pagebank but not yet a page, the page already starts playing

-- Won't fix / not related:

- Roland SH-4d was not detected by Linux. Not sure if this is a Blipr problem, since `amidi -l` also didn't show it.
- (still not working) Midi configuration with multiple device is not working properly. Was unable to set proper midi device for A. Cannot set to "None"
- Got an error with invalid device Id (probably the Midi Through Port), blipr won't start
- Weird yellow X (probably "<" and ">" characters of pagebank)
