# Rhythm-Game
Simple 4-key, Piano tiles-style, rhythm game

A 4-key rhythm game developed in C using Raylib, inspired by other rhythm games such as osu!mania and robeats.

## Overview
The player's goal is to use 4 keyboard lanes to hit falling notes.

The game does not currently include music. Instead, the challenge comes from the increase in speed of falling notes. The player has three lives and loses a life when a note is missed. The goal is to achieve the highest score possible while maintaining a combo.

## Features
- 4-key rhythm game
- Combo
- Combo Multiplier
- 3 Lives
- Time-based hit detection
- Score
- Pause functionality
- Increasing note speed
- Keyboard input handling
- Local highest score

## Technologies

- C
- Raylib
- VSCode
- Git/GitHub

## Controls
Default keybindings:
- Q - Lane 1
- W - Lane 2
- O - Lane 3
- P - Lane 4
- ESC - Pause

## Screenshots

## Running the project

This project is currently designed to be run using Visual Studio Code.

1. Open the project folder in Visual Studio Code
2. Make sure the required Raylib files/dependencies are installed
3. Open the project in VS Code
4. Build and run the project using the configured VS Code build/run setup

   To build run the command:

```bash
/opt/pop/bin/build-wasm.sh src/main.c
```

This will generate a directory *out* with the WASM and index.html files for the 
Raylib program.

The very first time you run a POP WASM application you must run the command:

```bash
/opt/pop/bin/allocate_port.sh
```

You might need to start a new terminal instance for the update to take effect.
To check that everything is fine run the command:

```bash
echo $MY_PORT
```

This should output a 5 digit number.


To run the Raylib program in *out* simply run the command:

```bash
/opt/pop/bin/run-wasm.sh
```

This will run a web server that serves the *out* on the port you allocated above. This is forwarded from the 
remote server to your local machine, which means you can simply open the corresponding web page within a browser 
on your local machine using the address:

```bash
localhost:XXXXX
```

where *XXXXX* is the port number you allocated above.

## What I learned

## Future Improvements

- Adding music
- Different songs
- Difficulty levels
- Visual effects
- Improving the user interface
