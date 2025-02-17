# blipr

---

blipr is a sequencer.  
blipr is a generator.  
blipr is an inspirator.

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
- Shift 3   : Track Options + Utilities (copy / paste / etc)
- Func      : This buttons is program-agnostic and provides overall operations (configuration / transport / midi / track & pattern selection, etc). It also doubles as a "back"-button when deeper in menu's

## global functionality:

- 1-16      : _(depends on program)_
- ABCD      : _(depends on program)_
- Shift1    : _(depends on program)_
- Shift2    : _(depends on program)_
- Shift3    : Track Options
    - 1-2       : Increase / decrease track length (default=16)
    - 3-4       : Increase / decrease track speed (default=0 / default speed)
    - 5-6       : Increase / decrease track velocity modifier (default=64)
    - 7-8       : Increase / decrease pattern step count before transitioning (default=16, max=256)
    - 9-10      : Increase / decrease track "name" (default="Track")
    - 11-12     : Increase / decrease track "name number" (default="1")
    - 13-14     : Increase / decrease CC1 mapping (default=0)
    - 15-16     : Increase / decrease CC2 mapping (default=0)
    - A         : Copy Track
    - B         : Paste Track
    - C         : Cut / Clear Track
    - D         : (tbd)
- Shift3-A  : Select Progr**A**m
- Shift3-B  : _(tbd)_
- Shift3-C  : Set Midi **C**hannel for Track
- Shift3-D  : Set Midi **D**evice for Track
- Func-1-16 : Select Track 1-16
- Func-A    : Pattern Selector (while still holding Func down, select 1-16)
- Func-B    : Sequence Selector (while still holding Func down, select 1-16)
- Func-C    : Transport (Start / Stop / BPM / Clock Settings)
- Func-D    : Configuration

## configuration

In configuration mode, the following can be configured:

- 1-4 : Select Midi Device for 1, 2, 3 or 4
        When selected, a list of found Midi Devices is shown, selecting a device will save it to the configuration.
- 5-8 : Setup Drumkit configuration 1, 2, 3 or 4
- 15  : Enable / disable autosave
- 16  : Quit (will prompt to save if autosave is disabled)

## track names + numbers

To make it easier to name tracks, you cannot choose a track name, but rather pick from a pre-defined list of names:

```
arp, bass, beat, bell, bleep, bloop, brass, buzz, chime, chord, clap, click, crash, crunch, cymbal, djembe, drone, drop, drum, echo, fade, flute, fm, fx, glitch, glock, groan, growl, guitar, harmonica, harp, hihat, horn, hum, kick, keys, lead, loop, mallet, marimba, melody, moog, mute, noise, organ, pad, perc, piano, ping, pluck, pulse, rattle, reed, reverb, ride, riff, rimshot, rise, roll, sax, scratch, shaker, sine, siren, slap, slide, snap, snare, soundfx, spark, stab, steel, strum, sweep, synth, tamb, tap, thump, tom, trill, twang, vibe, vibraphone, viola, violin, vocal, voice, wah, warble, wave, whine, whisper, whistle, wind, wobble, wurly, xylo, zap, zither
```

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

Can hold 16*16 = 256 patterns

- 16-pad    : Steps
    - Toggle on/off
    - Shift1: Half velocity
    - Shift2: Select note
        - 1-2   : Transpose -12 / +12
        - 3-4   : Transpose -1 / +1
        - 5-6   : Increase / decrease velocity
        - 7-8   : Increase / decrease length
        - 9-10  : Increase / decrease nudge
        - 11-12 : Increase / decrease trig condition
        - 13-14 : Increase / decrease CC1 value
        - 15-16 : Increase / decrease CC2 value
- ABCD  : Select page for playing (next in queue)
    - When holding multiple pages, a chain is created
    - Shift1    : Select page for editing (multiple taps for multipage setup)
    - Shift2    : Select page size (16, 32, 48, 64, 80, 96, 112, 128) (default=16) (note 1-8 is used for page information)

### Basic poly sequencer

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