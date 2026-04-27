# WAV File Generator

A simple C++ program that generates WAV audio files from text-based music notation. An assignment for the C++ course at Reykjavík University, done in spring 2026.

## Features

- Converts text files containing musical notes into playable WAV files
- Supports basic notes (a-g, sharps with uppercase) across multiple octaves
- Generates sine wave tones at specified frequencies
- Includes silence notes for rhythm
- Outputs mono, 16-bit, 44.1kHz WAV files

## Requirements

- C++ compiler (e.g., g++)

## Build Instructions

Compile the program using g++:

```bash
g++ main.cpp -o wav_generator
```

## Usage

1. Place your input `.txt` files in the `music/` directory
2. Run the program with the input filename as an argument (do not include /music/):

```bash
./wav_generator [filename].txt
```

3. The generated `.wav` file will be saved in the `wav/` directory with name specified in the first line of the .txt file

## Example

Generate audio from `baunir_lose.txt`:

```bash
./wav_generator baunir_lose.txt
```

This creates `wav/baunir_lose.wav` - a simple melody you can play in any audio player.

## Input File Format

Input files are plain text with the following structure:

```
[base_name]  # This is now derived from the filename
[bpm]        # Beats per minute (e.g., 100)
[note] [octave] [numerator] [denominator]  # Repeat for each note
...
```

- **Notes**: a-g for natural notes, A-G for sharps
- **Octave**: 0-5
- **Duration**: Fraction of a whole note (e.g., 1 4 for quarter note)
- **Silence**: Use 's' to denote silence

Example:
```
baunir_lose
100
a 2 1 4
e 1 1 12
s 1 12
```


## Learning summary

This project covers:
- WAV file format structure (RIFF, fmt, data chunks)
- Binary file writing in C++
- Basic audio signal generation
- Command-line argument parsing
- File path manipulation
