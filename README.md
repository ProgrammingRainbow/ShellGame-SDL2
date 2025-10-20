# ShellGame (SDL2)
ShellGame is an SDL2 to Shell bridge for making games in the Shell. It uses an SDL2 game engine server written in C. There are multiple options for starting that server and establishing communication. Simply source one of those scripts and start writing your game.

## ShellGame Usage
- One of the server scripts must be sourced to start the server, establish communication, and define the sg_cmd function.
- All commands listed below are passed as a string to the sg_cmd function.
- The symbols <> represent single replacements such as variables. {} represent arrays or space separated variables.
- All `set` commands are actions and return nothing.
- `get` commands return a single value via the global `$reply` variable. That value will need to be saved in another variable.
- `arr` commands return multiple values via the global `$array` variable. This array will need to be saved in another array or variables.

## Simple Example
This opens a window using the pipe backend. It loads a background image as a sprite. If the window is closed or escape is pressed it exits.
```bash
#!/usr/bin/env bash

# Source the Pipe backend.
source pipe.sh || exit 1

# Start ShellGame.
sg_cmd "start sg"

# Set the title to ShellGame.
sg_cmd "set sg title ShellGame"

# Load the background image as a sprite.
sg_cmd "new sprite examples/images/background.png"
# Save the returned sprt_id to back_sprt variable.
back_sprt=$reply

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # If esc key is pressed exit.
    sg_cmd "get key pressed esc"
    (( reply )) && sg_quit 0

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background sprite.
    sg_cmd "draw sprite $back_sprt"

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
```
---

## ShellGame Core Commands

| Command                          | Description |
|----------------------------------|-------------|
| `start sg`                       | Initializes the ShellGame engine. Must be called before any other commands. |
| `update sg`                      | Updates the engine state. Must be the first call inside the game loop. |
| `quit sg`                        | Gracefully shuts down the engine and exits the game. |

---

## ShellGame Window Commands

| Command                                | Description |
|----------------------------------------|-------------|
| `set sg title <string>`                | Sets the window title to the specified string.  |
| `set sg icon <filename>`               | Sets the window icon using the given image file. |
| `set sg size <width> <height>`         | Sets the window dimensions. |
| `set sg fullscreen enable`             | Enables fullscreen mode. |
| `set sg fullscreen disable`            | Disables fullscreen mode. |
| `set sg fullscreen toggle`             | Toggles between fullscreen and windowed mode. |
| `get sg width` → `<int>`               | Retrieves the current window width. |
| `get sg height` → `<int>`              | Retrieves the current window height. |
| `arr sg size` → `{int array}`          | Returns an array with `[width, height]` values. |

---

## ShellGame Image Scaling Style
These settings are applied to new or updated images/sprites/text. Not existing ones.
| Command                                | Description |
|----------------------------------------|-------------|
| `set sg scaling nearest`               | Sets pixel scaling to nearest for pixel art style. |
| `set sg scaling linear`                | Sets pixel scaling to linear for smooth style. |

---

## ShellGame FPS Commands

| Command                    | Description |
|---------------------------|-------------|
| `set sg fps <int>`        | Sets the target frames per second. |
| `get sg fps` → `<int>`    | Retrieves the current frame rate. Updated every second. |

---




