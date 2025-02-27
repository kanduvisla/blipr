# Test Report

---

These are observations done while testing Blipr with real hardware.

- Roland SH-4d was not detected by Linux. Not sure if this is a Blipr problem, since `amidi -l` also didn't show it.
- Midi configuration with multiple device is not working properly. Was unable to set proper midi device for A.
- When configuring midi device, configuration is not directly applied, restart of the program is required.
- No option to select "no device"
- Major performance issues on Raspberry Pi, especially with key repeat
- Major performance issues when in note editing menu. Probably due to rendering.
- Missing escape option from conf menu
- Add `--help` option
- Got an error with invalid device Id (probably the Midi Through Port)
- With new notes, the default "everything" is 255