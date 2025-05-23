# blipr

---

blipr is a sequencer.  
blipr is a generator.  
blipr is an inspirator.

---

## Disclaimer

This project is still very beta and WIP. Things are subject to change.

## layout

This program is made to be operated with a 24-key keypad:

```
16-pad:
[1] [2] [3] [4]
[q] [w] [e] [r]
[a] [s] [d] [f]
[z] [x] [c] [v] 
ABCD-pad:
[7]  [8]  [9] [0]
Func-pad:
[^1] [^2] [ ] [F]

^1      = (Shift 1) Left Ctrl
^2      = (Shift 2) Left Shift
^3      = (Shift 3) Right Ctrl
F       = (Func) Right Shift
.       = tbd
```

Basically the functionality for all programs is identical, with the following mantra:

- 16-pad    : These are functional buttons, depending on the program
- ABCD      : These are functional buttons, depending on the program
- Shift 1   : This button can be used on the 16-pad, or ABCD pad to do 1 alternative action
- Shift 2   : This button can be used to "zoom in" on 1 of the 16-pad or ABCD buttons to provide more options for that specific button (for example: extra options for a single step in the sequencer)
- Shift 1+2 : Utilities?
- Shift 3   : Track Options
- Func      : This buttons is program-agnostic and provides overall operations (configuration / transport / midi / track & pattern selection, etc). It also doubles as a "back"-button when deeper in menu's

## global functionality:

- 1-16      : _(depends on program)_
- ABCD      : _(depends on program)_
- Shift1    : _(depends on program)_
- Shift2    : _(depends on program)_
- Shift3    : Track Options / Pattern Options / Sequence Options / Utilities
    - When holding Shift3, ABCD is for different options:
        - A     : Select Track
        - B     : Track Options
            - 1     : ✅ Change track play mode (continuous or by page)
            - 2     : ✅ Change track polyphony (8, 4, 2, 1)
            - 3-4   : ✅ Increase / decrease track / page length (depending on play mode)
            - 5-6   : ✅ Increase / decrease track speed (default=0 / default speed)
            - 7-8   : ✅ Set shuffle
            - 9     : ✅ Set Midi Device
            - 10    : ✅ Set Midi Channel
            - 11-12 : ✅ Change page repeat (how many times repeat a page before the transition happens?)
            - 13-14 : ✅ Increase / decrease CC1 mapping (default=0)
            - 15-16 : ✅ Increase / decrease CC2 mapping (default=0)
        - C     : ✅ Program Selector for this Track
        - D     : Pattern Options
            - 1-2   : ✅ Change BPM
            - 3-4   : ✅ Change Pattern Length (used for determine pattern transition)
            - ...   : Sync pages (when switching pages with repeat page mode, do all tracks play the same page (true), or are the pages individually configured?)
            - ...   : Change pattern name
            - 8-9   : ✅ Set Midi device A PC
            - 10-11 : ✅ Set Midi device B PC
            - 12-13 : ✅ Set Midi device C PC
            - 14-15 : ✅ Set Midi device D PC
- Func-A    : ✅ Pattern Selector (while still holding Func down, select 1-16)
- Func-B    : ✅ Sequence Selector (while still holding Func down, select 1-16)
- Func-C    : Configuration
            - 1     : ✅ Set Midi Device A
            - 2     : ✅ Set Midi Device B
            - 3     : ✅ Set Midi Device C
            - 4     : ✅ Set Midi Device D
            - 5     : ✅ Set Midi Device A PC Channel
            - 6     : ✅ Set Midi Device B PC Channel
            - 7     : ✅ Set Midi Device C PC Channel
            - 8     : ✅ Set Midi Device D PC Channel
- Func-D    : Transport (Start / Stop / BPM / Clock Settings)

## configuration

In configuration mode, the following can be configured:

- 1-4 : Select Midi Device for 1, 2, 3 or 4
        When selected, a list of found Midi Devices is shown, selecting a device will save it to the configuration.
- 5-8 : Setup Drumkit configuration 1, 2, 3 or 4
- 15  : Enable / disable autosave
- 16  : Quit (will prompt to save if autosave is disabled)

## track names + numbers

When entering a name, the 16-pad can be used to enter characters. 

- 13-16  : [left], [right], [space], [backspace]
- ABCD   : Select page
- A      : ABCD EFGH IJKL
- B      : MNOP QRST UVWX
- C      : YZ01 2345 6789
- D      : .!?- ~/:_ ()[]
- Func   : Ok (close and set)
- Shift3 : Cancel 

## track speed

0 : ✅ Default speed
1 : ✅ 1/2 speed
2 : ✅ 1/4 speed
3 : ✅ 1/8 speed
4 : 1/16 speed
5 : ✅ 2/1 speed (double)
6 : ✅ 4/1 speed
7 : ✅ 8/1 speed
8 : 16/1 speed
5 : 1/3 speed
6 : 1/6 speed
7 : 1/12 speed
8 : 1/24 speed
9 : 3/1
10: 6/1
11: 12/1
12: 24/1

## programs

Basically your workflow is the following:

1. Select the Sequence that you want to work on (default = 0, Func-B = Sequence Selector)
2. Select the Pattern that you want to work on (default = 0, Func-A = Pattern Selector)
3. Select the Track that you want to work on (default = 0, Func = Track Selector)
4. Configure the Midi Device + Midi Channel for this track (Shift3-D = Set Midi Device, Shift3-C = Set Midi Channel)
5. Select the program you want to use (Shift3-A)
6. Start bleepin-n-bloopin'

### 1: Basic 16-step sequencer

Each pattern has 64 steps, divided in 4 pages. Each step can have up to 8 notes. Since this sequencer is monophonic, note 1-8 are used for extra page size. This makes it possible for a total track to have 512 steps. Track length is used to determine how many pages are possible: (<= 128 steps = 4 pages, 129-256 = 2 pages, 257-512 = 1 page)

- 16-pad    : Steps
    - ✅ Toggle on/off
    - ✅ Shift1: Select step(s)
        - A : ✅ Open Note editor for current note
            - 1-4   : ✅ Transpose -12 / -1 / +1 / +12
            - 5-6   : ✅ Increase / decrease velocity
            - 7-8   : ✅ Increase / decrease length
            - 9-10  : ✅ Increase / decrease nudge
            - 11-12 : ✅ Increase / decrease trig condition
            - 13-14 : ✅ Increase / decrease CC1 value
            - 15-16 : ✅ Increase / decrease CC2 value
        - B : ✅ Cut (once for note(s), twice for step(s))
        - C : ✅ Copy
        - D : ✅ Paste (once for note(s), twice for step(s))
    - Shift2: 
        - A-B : ✅ Increase / decrease page bank
        - C-D : ✅ Increase / decrease selected note / channel
- ABCD  : ✅ Select page for playing (or next in queue, depending on page play mode)
    - ✅ When pages are playing after each other (play mode), this button selects which page to edit
    - ✅ When page play mode is repeat page, this button queues the next page

#### Steps / Pages / Notes / Polyphony

Here is a bit of explanation how to think of steps, pages and notes.

- The sequencer is polyphonic, but you can sacrifice polyphony to have more steps in your track.
    - 8 note polyphony: max. 64 steps
    - 4 note polyphony: max. 128 steps
    - 2 note polyphony: max. 256 steps
    - 1 note polyphony: max. 512 steps
- Polyphony (and thus max steps) is determined per track
- Pages are sets of max. 16 steps (a page can be configured to have fewer steps), so then the ABCD buttons are:
    - 64 Steps:     A:1,        B:2,        C:3,        D:4
    - 128 Steps:    A:1,5       B:2,6       C:3,7       D:4,8
    - 256 Steps:    A:1,5,9     B:2,6,10    C:3,7,11    D:4,8,12
    - 512 Steps:    A:1,5,9,13  B:2,6,10,14 C:3,7,11,15 D:4,8,12,16

### Drumkit sequencer

This sequencer can be used if there is a drumkit assigned to notes. It's basically the same as the basic sequencer, but instead of notes, the drum names are used. Drum names / note configurations can be configured in the configuration menu. 

The main differences with the basic sequencer are:

- Tapping on a step does not toggle it, but changes it's velocity to 100, 50, 25 or OFF.
- When holding down Shift-2, buttons 1-16 are used to select the drum
- Drums are shared between different channels, depending on polyphony:

| Sample    | P8 Ch | P4 Ch | P2 Ch |
| --------- | ----- | ----- | ----- |
| Kick      |   1   |   1   |   1   |
| Snare     |   2   |   2   |   2   |
| Clap      |   3   |   2   |   2   |
| Rimshot   |   2   |   2   |   2   |
| C.Hat 1   |   4   |   3   |   2   |
| C.Hat 2   |   4   |   3   |   2   |
| O.Hat     |   5   |   4   |   2   |
| Ride      |   5   |   4   |   2   |
| Cymbal    |   5   |   4   |   2   |
| L. Tom    |   6   |   2   |   2   |
| M. Tom    |   6   |   2   |   2   |
| H. Tom    |   6   |   2   |   2   |
| Cowbell   |   7   |   2   |   2   |
| Extra 1   |   7   |   1   |   1   |
| Extra 2   |   8   |   3   |   2   |
| Extra 3   |   8   |   4   |   2   |

With a polyphony of 1, everything is on the same channel, obviously.

### Cycles kick machine

### 2: Tracker

### 3: Euclidean Rhytm Generator

### 4: Fibonacci

### 5: Circle of Fifths

### 6: L-System

### 7: Arpeggiator

### 8: Random

### 9: Phase Pattern Generator

### Live Performance mode

Focusses on playing, not editing. 
Makes it easy to switch between patterns and sequences with Shift-keys.
Includes effects on tracks and mute groups toggling, for example, reverse playback or randomization of tracks.

- A : Go to pattern play mode
- B : Select pattern (if no pattern is selected in 4 beats, default back to A)
- C : Select sequence (if no sequence is selected in 4 beats, default back to A)
- D : FX mode

## Dependencies

When you want to build and compile yourself, make sure you have the following dependencies installed on your system:

- SDL2
- Portmidi