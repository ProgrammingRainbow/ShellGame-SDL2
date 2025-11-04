#!/usr/bin/env bash

# Option for choosing communication to server.
if [[ $# -eq 1 ]]; then
    case $1 in
        --client|--shared|--socket|--stdinout|--pipe)
            source "${1#--}.sh" || {
                echo "Error: Failed to source file for '$1'"
                exit 1
            }
            ;;
        *)
            echo "Usage: $0 {--pipe|--stdinout|--socket|--shared|--client}"
            exit 1
            ;;
    esac
else
    echo "Usage: $0 {--pipe|--stdinout|--socket|--shared|--client}"
    exit 1
fi

# Global Variables.
width=1280
height=720
text_size=50
bubble_size=7
fps_display=0
fullscreen=0
mode=1

# Associated Arrays.
declare -A mouse_button=(
    [button1]=0
    [button2]=0
    [button3]=0
)

declare -A mouse_pos=(
    [x]=0
    [y]=0
)

declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
    [3]=0
    [4]=0
)

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 3 4"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[3]=${array[3]}
    pressed[4]=${array[4]}
}

# function to enable/disable fullscreen.
update_fullscreen() {
    if (( fullscreen )); then
        sg_cmd "set sg fullscreen desktop"
    else
        sg_cmd "set sg fullscreen disable"
    fi
}

# Update the FPS text and display it if enabled.
update_fps() {
    sg_cmd "get sg fps"
    sg_cmd "set text string $text_fps FPS: $reply"
    sg_cmd "draw text $text_fps"
}

# Start ShellGame.
sg_cmd "start sg"

# Set the size, title, icon, resizable, and scale quality.
sg_cmd "set sg size $width $height"
sg_cmd "set sg title Test Mouse"
sg_cmd "set sg icon examples/images/bg-bash-logo.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Create text objects and set there center y positions equally spaced.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Button 1: 0"
text_mouse1=$reply
sg_cmd "set text cy $(( height / 6 )) $text_mouse1"

sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Button 2: 0"
text_mouse2=$reply
sg_cmd "set text cy $(( height / 3)) $text_mouse2"

sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Button 3: 0"
text_mouse3=$reply
sg_cmd "set text cy $(( height / 2 )) $text_mouse3"

sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Mouse X: 0"
text_mouse4=$reply
sg_cmd "set text cy $(( height / 6 * 4 )) $text_mouse4"

sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Mouse Y: 0"
text_mouse5=$reply
sg_cmd "set text cy $(( height / 6 * 5 )) $text_mouse5"

# Set all the text objects center x to the center of the screen.
sg_cmd "set text cx $(( width / 2 )) $text_mouse1 $text_mouse2 $text_mouse3 $text_mouse4 $text_mouse5"

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $text_size $bubble_size FPS: 0"
text_fps=$reply

# Setting anchor and position to top left 10 10.
sg_cmd "set text pos 10 10 $text_fps"

# Set the render draw color. This will be the background color.
sg_cmd "set render color 128 46 46"

# Set fullscreen enable/disable.
update_fullscreen

# Set FPS option.
# sg_cmd "set sg fps 60"

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # Get the keys pressed and update associated array.
    get_keys_pressed

    # Escape to quit.
    (( pressed[esc] )) && sg_quit 0

    # Toggle fullscreen.
    if (( pressed[1] )); then
        (( fullscreen = 1 - fullscreen ))
        update_fullscreen
    fi

    # Toggle show fps.
    (( pressed[2] )) && (( fps_display = 1 - fps_display ))

    # Set mode 1 or 2.
    (( pressed[3] )) && mode=1
    (( pressed[4] )) && mode=2

    if (( mode == 1 )); then
        # Get each mouse state individually and update there text object.
        sg_cmd "get mouse held 1"
        sg_cmd "set text string $text_mouse1 Button 1: $reply"

        sg_cmd "get mouse held 2"
        sg_cmd "set text string $text_mouse2 Button 2: $reply"

        sg_cmd "get mouse held 3"
        sg_cmd "set text string $text_mouse3 Button 3: $reply"

        sg_cmd "get mouse x"
        sg_cmd "set text string $text_mouse4 Mouse X: $reply"

        sg_cmd "get mouse y"
        sg_cmd "set text string $text_mouse5 Mouse Y: $reply"
    else
        # Get mouse states as arrays and update there text object.
        sg_cmd "arr mouse held 1 2 3"
        sg_cmd "set text string $text_mouse1 Button 1: ${array[0]}"
        sg_cmd "set text string $text_mouse2 Button 2: ${array[1]}"
        sg_cmd "set text string $text_mouse3 Button 3: ${array[2]}"

        sg_cmd "arr mouse pos"
        sg_cmd "set text string $text_mouse4 Mouse X: ${array[0]}"
        sg_cmd "set text string $text_mouse5 Mouse Y: ${array[1]}"
    fi

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw all text objects.
    sg_cmd "draw text $text_mouse1 $text_mouse2 $text_mouse3 $text_mouse4 $text_mouse5"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
