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
        - L.Shift = select note
            - While holding L.Shift
                - A: Half velocity
                - Arrows: nudge left/right
                - B: Change note
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

