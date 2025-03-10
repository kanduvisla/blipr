# Test Report

---

These are observations done while testing Blipr with real hardware.

--- Bugs:

- Channel / note number is not shown 
- When switching to a different track, the default channel is not set back to 1
- Poly sacrificing / correct track not playing (P01 appears to be P05?)
- Poly sacrificing / cannot select pagebank 5-8 (selected page bank not highlighted)
- It still appears that something goes wrong when setting steps in the sequencer; other pages seem affected.
- Roland SH-4d was not detected by Linux. Not sure if this is a Blipr problem, since `amidi -l` also didn't show it.
- (still not working) Midi configuration with multiple device is not working properly. Was unable to set proper midi device for A. Cannot set to "None"
- (still not working) When configuring midi device, configuration is not directly applied, restart of the program is required. The current device is unassigned, but a new device is not assigned
- Got an error with invalid device Id (probably the Midi Through Port), blipr won't start
- Other pages is not working
- Page bank is pickable, even if it's not allowed or visible
- Steps are set on disabled tiles (takes not in account page length (possible also track length))
- (requires re-testing) It appears that the note off is not properly send when changing the note tone
- Key repeat is repeating also on steps

--- Improvements:

- Missing page numbers / max pages
- Missing transport / option to start / stop
- Missing option to set BPM
- Last set length is not remembered when editing track.
- Same for last set velocity.
- When track has no program, shift 3 should start on program selection
- Show default note, length + velocity in sidebar
- Mute Track
- Solo Track
- Utilities (clear track)
- Save / Remember last note / velocity / etc. per track
- Thumbnail of track on track selection
- Panic button!
- Copy / Paste of ranges of notes
- Shuffle and nudge should be on the same position
- More quick velocity switches

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
