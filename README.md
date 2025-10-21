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

## Creating new ShellGame objects.

| Command                                | Description |
|----------------------------------------|-------------|
| `new image <filename>` → `<img_id>`                  | Creates an image object from an image file. |
| `new rect` → `<rect_id>`                             | Creates a rectangle with all zeros. |
| `new rect image <img_id>` → `<rect_id>`              | Creates a rectangle with width and height populated from an image. |
| `new rect <x> <y> <w> <h>` → `<rect_id>`             | Creates a rectangle with position and size set. |
| `new sprite <filename>` → `<sprt_id>`                | Creates a sprite(image and rect) from an image file. |
| `new text <font> <size> <string>` → `<txt_id>`       | Creates a text object with font, size, and a string. |
| `new music <filename>` → `<mus_id>`                  | Creates a music object from a music file. |
| `new sound <filename>` → `<snd_id>`                  | Creates a sound object from a sound file. |

---

## Updating using Delta Time
These commands work with the object <type> rect, sprite, and image.
| Command                                        | Description |
|------------------------------------------------|-------------|
| `update <type> pos <x_vel> <y_vel> {id array}` | Moves object with x and y velocity using delta time. Applies to each space separated id. |
| `update <type> <attribute> <float> {id array}` | Same as above but with either x or y attribute. |
| `update <type> angle <angle> {id array}`       | Rotates objects by angle using delta time. Applies to each space separated id. |

---

## Set Position, Size, and Rotation Properties
These commands work with the object <type> rect, sprite, and image. All commands are applied to each space separated id.
| Command                                       | Description |
|-----------------------------------------------|-------------|
| `set <type> pos <x> <y> {id array}`           | Sets the x and y position for each id. |
| `set <type> angle <angle> {id array}`         | Sets a rotation angle for each id. |
| `set <type> flip <flip> {id array}`           | Sets horizontal, vertical, or none flip (h,v,n) for each id. |
| `set <type> size <width> <height> {id array}` | Sets object width and height for each id. |
| `set <type> <attribute> <float> {id array}`   | Sets a single attribute (x,l,left,y,t,top,w,width,h,height,r,right,b,bottom,cx,cy) for each id. |

---

## Get Position, Size, or Rotation Property.
These commands work with the object <type> rect, sprite, and image.
| Command                                | Description |
|----------------------------------------|-------------|
| `get <type> <attribute> <id>` → `int`  | Gets an attribute (x,l,left,y,t,top,w,width,h,height,r,right,b,bottom,cx,cy) |
| `get <type> angle <id>` → `int`        | Gets the rotation angle as an int. |
| `getf <type> angle <id>` → `float`     | Gets the rotation angle as a float. |

---

## Array of Position, Size, or Rotation Properties.

| Command Format                          | Description |
|----------------------------------------|-------------|
| `arr <type> pos <id>` → `{int array}`          | Returns an array of x and y. |
| `arr <type> angle {id array}` → `{int array}`  | Returns and array of angles. One for each object id. |
| `arr <type> size <id>` → `{int array}`         | Returns an array of width and height. |
| `arr <type> rect <id>` → `{int array}`         | Returns full rectangle `[l, t, r, b]`. |
| `arr <type> <attribute> {id array}`            | Returns and array an attributes (x,l,left,y,t,top,w,width,h,height,r,right,b,bottom,cx,cy) one for each id given. |

---

## Free/Delete

| Command Format                          | Description |
|----------------------------------------|-------------|
| `free <type> {id array}`               | Frees the object(s) and returns their IDs. |

---
