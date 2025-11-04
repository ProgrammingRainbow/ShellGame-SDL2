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
text_size=50
bubble_size=7
fps_display=0
fullscreen=0

# Associated Arrays.
declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
)

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
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

# Set the title, icon, resizable, and scale quality.
sg_cmd "set sg title ShellGame Template"
sg_cmd "set sg icon examples/images/bg-bash-logo.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Load and image.
sg_cmd "new image examples/images/ys-background.png"
back_img=$reply

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $text_size $bubble_size FPS: 0"
text_fps=$reply

# Setting anchor and position to top left 10 10.
sg_cmd "set text pos 10 10 $text_fps"

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

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background sprite.
    sg_cmd "draw image NULL NULL $back_img"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
