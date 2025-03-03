# Test Report

---

These are observations done while testing Blipr with real hardware.

--- Bugs:

- Roland SH-4d was not detected by Linux. Not sure if this is a Blipr problem, since `amidi -l` also didn't show it.
- (requires re-test) Midi configuration with multiple device is not working properly. Was unable to set proper midi device for A.
- When configuring midi device, configuration is not directly applied, restart of the program is required.
- Same goes for when changing midi device for track, it is not applied, only after restart.
- Got an error with invalid device Id (probably the Midi Through Port), blipr won't start

--- Improvements:

- Missing transport / option to start / stop
- Missing option to set BPM
- Add indicator on CPU% - separation in calculations + rendering time
- Midi configuration: No option to select "no device"
- Add `--help` option
- Missing escape option from conf menu
- Last set length is not remembered when editing track.
- Same for last set velocity.
- When track has no program, shift 3 should start on program selection
- Show default note, length + velocity in sidebar

--- Fixed

- With new notes, the default "everything" is 255
- Major performance issues when in note editing menu. Probably due to rendering. (fixed with multi threading)
- Major performance issues on Raspberry Pi, especially with key repeat. (fixed with multi threading)
