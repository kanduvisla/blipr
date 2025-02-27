# Test Report

---

These are observations done while testing Blipr with real hardware.

- Roland SH-4d was not detected by Linux. Not sure if this is a Blipr problem, since `amidi -l` also didn't show it.
- Midi configuration with multiple device is not working properly. Was unable to set proper midi device for A.
- No option to select "no device"
- Major performance issues on Raspberry Pi, especially with key repeat
- Missing escape option from conf menu
- Add `--help` option
- Got an error with invalid device Id (probably the Midi Through Port)
