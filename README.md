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
        - A     : Track Options
            - 1     : Change track play mode (continuous or by page)
            - 2     : Change poly
            - 3-4   : ✅ Increase / decrease track / page length (depending on play mode)
            - 5-6   : Increase / decrease track speed (default=0 / default speed)
            - 7-8   : ✅ Set page size (1-16)
            - 9     : ✅ Set Midi Device
            - 10    : ✅ Set Midi Channel
            - 11    : ✅ Change track play mode (continuous or by page)
            - 12    : Change track polyphony (8, 4, 2, 1)
            - 11    : Change page repeat (how many times repeat a page before the transition happens?)
            - 12    : Change track name
            - 13-14 : Increase / decrease CC1 mapping (default=0)
            - 15-16 : Increase / decrease CC2 mapping (default=0)
        - B     : ✅ Program Selector for this Track
        - C     : Pattern Options & Sequence Options
            - 1     : Sync pages (when switching pages with repeat page mode, do all tracks play the same page (true), or are the pages individually configured?)
            - 12    : Change pattern name
            - 13-14 : Increase / decrease Midi Program Bank (default=0)
            - 15-16 : Increase / decrease Midi Program Pattern (default=0)
        - D     : Utilities
            - 1     : Copy
            - 2     : Paste
            - 3     : Cut
            - 4     : Clear
            - 5-6   : Nudge
            - 7-8   : Transpose
- Func-1-16 : ✅ Select Track 1-16
- Func-A    : ✅ Pattern Selector (while still holding Func down, select 1-16)
- Func-B    : ✅ Sequence Selector (while still holding Func down, select 1-16)
- Func-C    : Transport (Start / Stop / BPM / Clock Settings)
- Func-D    : Configuration
    - 1         : ✅ Midi Configuration
        - ABCD  : ✅ Select Midi Device Slot A, B, C or D
        - 1-16  : ✅ Select Midi Device

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

0 : Default speed
1 : 1/2 speed
2 : 1/4 speed
3 : 1/8 speed
4 : 1/16 speed
5 : 2/1 speed (double)
6 : 4/1 speed
7 : 8/1 speed
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
    - ✅ Shift1: Half velocity
    - Shift2: Select note
        - 1-4   : ✅ Transpose -12 / -1 / +1 / +12
        - 5-6   : ✅ Increase / decrease velocity
        - 7-8   : ✅ Increase / decrease length
        - 9-10  : Increase / decrease nudge
        - 11-12 : Increase / decrease trig condition
        - 13-14 : Increase / decrease CC1 value
        - 15-16 : Increase / decrease CC2 value
        - A-B   : (For polyphony) Channel -/+
        - C     : Copy Note (2x to copy entire step/all notes on step)
        - D     : Paste Note/Step
- ABCD  : ✅ Select page for playing (or next in queue, depending on page play mode)
    - ✅ When pages are playing after each other (play mode), this button selects which page to edit
    - ✅ When page play mode is repeat page, this button queues the next page
    - When holding multiple pages, a chain is created
    - Shift1    : Select page bank
    - Shift2    : Select page size (1-512) (default=16) (note 1-8 is used for page information)

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

### Basic poly sequencer

Each pattern has 64 steps, divided in 4 pages. Each step can have up to 8 notes.

Track length determins how many pages / polyphony is possible:

- with 8-voice polyphony, you can have max. 64 steps.
- with 4-voice polyphony, you can have max. 128 steps.
- with 2-voice polyphony, you can have max. 256 steps.
- with monophony, you can have 512 steps.

Same as "Basic 16 step sequencer, but pages are sacrified for polyphony. The following is different between the 2 sequencers:

- Shift2-1-16   : When holding Shift-2, ABCD can be used to select voice:
    - A : Voice 1/2
    - B : Voice 3/4 (if applicable)
    - C : Voice 5/6 (if applicable)
    - D : Voice 7/8 (if applicable)
- Shift2-ABCD   : Select page size / polyphony count
    - 16 steps : 8 voices
    - 32 steps : 4 voices
    - 64 steps : 2 voices

### SH4D-sequencer / Drumkit sequencer

This sequencer can be used if there is a drumkit assigned to notes. It's basically the same as the basic poly sequencer, but instead of notes, the drum names are used. Drum names / note configurations can be configured in the configuration menu. Kits contain:

- Kick
- Snare
- Clap
- Rimshot
- Closed Hat
- Open Hat
- Ride
- Cymbal
- L. Tom
- M. Tom
- H. Tom
- Cowbell
- ...
- ...
- ...
- ...

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