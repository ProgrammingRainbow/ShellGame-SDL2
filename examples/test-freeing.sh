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
velocity=300
fps_display=0
fullscreen=0

# Associated Arrays.
declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
    [h]=0
    [v]=0
    [n]=0
)

declare -A held=(
    [a]=0
    [s]=0
    [w]=0
    [d]=0
    [left]=0
    [right]=0
)

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 h v n"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[h]=${array[3]}
    pressed[v]=${array[4]}
    pressed[n]=${array[5]}
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
sg_cmd "set sg title Test Freeing"
sg_cmd "set sg icon examples/images/bg-bash-logo.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Load and image.
sg_cmd "new image examples/images/ys-background.png"
back_img=$reply
echo "new back_img is $back_img"

# Load another image.
sg_cmd "new image examples/images/ys-background.png"
back2_img=$reply
echo "new back2_img is $back2_img"

# Free the first image.
sg_cmd "free image $back_img"
back_img=$array
echo "freed back_img is $back_img"

# Load another image it should go into the first position.
sg_cmd "new image examples/images/ys-background.png"
back_img=$reply
echo "new back_img is $back_img"

# Load yet another image. This should go at the end.
sg_cmd "new image examples/images/ys-background.png"
back3_img=$reply
echo "new back3_img is $back3_img"

# Create a new rect from the image.
sg_cmd "new rect image $back_img"
back_rect=$reply

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

    # Flip the image horizontally or vertically.
    (( pressed[h] )) && sg_cmd "set rect flip h $back_rect"
    (( pressed[v] )) && sg_cmd "set rect flip v $back_rect"
    (( pressed[n] )) && sg_cmd "set rect flip n $back_rect"

    # Get the keys held down and update associated array.
    sg_cmd "arr key held a s w d left right"
    held[a]=${array[0]}
    held[s]=${array[1]}
    held[w]=${array[2]}
    held[d]=${array[3]}
    held[left]=${array[4]}
    held[right]=${array[5]}

    # Move the image with delta time, using a s w d.
    (( held[a] )) && sg_cmd "update rect x -$velocity $back_rect"
    (( held[s] )) && sg_cmd "update rect y $velocity $back_rect"
    (( held[w] )) && sg_cmd "update rect y -$velocity $back_rect"
    (( held[d] )) && sg_cmd "update rect x $velocity $back_rect"

    # Rotate the image with delta time, using left and right.
    (( held[left] )) && sg_cmd "update rect angle -$velocity $back_rect"
    (( held[right] )) && sg_cmd "update rect angle $velocity $back_rect"

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background image.
    sg_cmd "draw image NULL $back_rect $back_img"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
