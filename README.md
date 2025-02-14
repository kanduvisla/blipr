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
(empty) = Space
F       = (Func) Right Shift
.       = tbd
```

Basically the functionality for all programs is identical, with the following mantra:

- 16-pad    : These are functional buttons, depending on the program
- ABCD      : These are functional buttons, depending on the program
- Shift 1   : This button can be used on the 16-pad, or ABCD pad to do 1 alternative action
- Shift 2   : This button can be used to "zoom in" on 1 of the 16-pad or ABCD buttons to provide more options for that specific button (for example: extra options for a single step in the sequencer)
- Space     : (tbd)
- Func      : This buttons is program-agnostic and provides overall operations (configuration / transport / midi / track & pattern selection, etc).

## programs

Holding R. Shift enables the global menu. The keys will then do the following:
- 1234      : Select midi device
- 5-16      : Select midi channel of selected device
- A         : 
- B         : 
- C         : File Operations
    - Load / Save Project
- D         : Select Program
    - 1-16 program
- Arrows    : increase/decrease BPM
- L.Shift   : Quit

Basically your workflow is the following:

1. Select the Midi device / midi channel that you want to work on
2. Select the program you want to use

### 1: Basic 16-step sequencer

Can hold 16*16 = 256 patterns

- 16-step sequencer
    - 16-pad = steps
        - Toggle on/off
        - Shift 1: Half velocity
        - Shift 2: Select note
            - While Shift 2:
                - A/B: nudge left/right
                - C/D: increase/decrease note
                - C: Select pattern
                    - After this, select 1-16
                - D: Select Sequence
                    - After this, select 1-16
    - ABCD   = page A,B,C or D
        - L.Shift ABCD = page length (16, 32, 48, 64)

### 2: Tracker

### 3: Euclidean Rhytm Generator

### 4: Fibonacci

### 5: Circle of Fifths

### 6: L-System

### 7: Arpeggiator

### 8: Random

### 9: Phase Pattern Generator

