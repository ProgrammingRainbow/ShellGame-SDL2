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
sprite_size=0
rot_vel=720
fps_display=0
fullscreen=0

# Global arrays.
declare -a sprites
declare -a sprites_xvel
declare -a sprites_yvel

# Global Associative arrays.
declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
    [w]=0
    [y]=0
    [s]=0
)

declare -A held=(
    [left]=0
    [right]=0
    [up]=0
    [down]=0
)

declare -A txt_rect=(
    [left]=0
    [top]=0
    [right]=0
    [bottom]=0
)

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 w y s"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[w]=${array[3]}
    pressed[y]=${array[4]}
    pressed[s]=${array[5]}
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

sprite_random() {
    local x=0
    local y=0
    sg_cmd "arr sprite rect $1"
    (( x = RANDOM % ( width - array[2] ) ))
    (( y = RANDOM % ( height - array[3] ) ))
    sg_cmd "set sprite pos $x $y $1"
}

# Start ShellGame.
sg_cmd "start sg"

# Set the size, title, icon, resizable, and scale quality.
sg_cmd "set sg size $width $height"
sg_cmd "set sg title Test Sprites"
sg_cmd "set sg icon examples/images/ys-yellow.png"
sg_cmd "set sg resizable enable"
# sg_cmd "set render scaling best"

# Create 400 sprites and place the in random places on the screen.
for i in {0..399}; do
    sg_cmd "new sprite examples/images/ys-yellow.png"
    sprites[$i]=$reply
    sprite_random ${sprites[$i]}
    sprites_xvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
    sprites_yvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
done

# Get the size of the sprites.
sg_cmd "get sprite width ${sprites[0]}"
sprite_size=$reply

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

    # Get the keys pressed and update the associated array.
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


    # Set the 400 sprite images and window image. 
    if (( pressed[w] )); then
        sg_cmd "set sprite image examples/images/ys-white.png ${sprites[*]}"
        sg_cmd "set sprite size $sprite_size $sprite_size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/ys-white.png"
    fi
    if (( pressed[y] )); then
        sg_cmd "set sprite image examples/images/ys-yellow.png ${sprites[*]}"
        sg_cmd "set sprite size $sprite_size $sprite_size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/ys-yellow.png"
    fi
    if (( pressed[s] )); then
        sg_cmd "set sprite image examples/images/ys-player.png ${sprites[*]}"
        sg_cmd "set sprite size $sprite_size $sprite_size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/ys-player.png"
    fi

    # Get the keys held and update the associated array.
    sg_cmd "arr key held left right up down"
    held[left]=${array[0]}
    held[right]=${array[1]}
    held[up]=${array[2]}
    held[down]=${array[3]}

    (( held[left] )) && sg_cmd "update sprite angle -$rot_vel ${sprites[*]}"
    (( held[right] )) && sg_cmd "update sprite angle $rot_vel ${sprites[*]}"

    if (( held[up] )); then
        (( sprite_size < 200 )) && (( sprite_size++ ))
        sg_cmd "set sprite size $sprite_size $sprite_size ${sprites[*]}"
    fi
    if (( held[down] )); then
        (( sprite_size > 2 )) && (( sprite_size-- ))
        sg_cmd "set sprite size $sprite_size $sprite_size ${sprites[*]}"
    fi

    for (( i = 0; i < ${#sprites[@]}; i++ )); do
        sg_cmd "update sprite pos ${sprites_xvel[$i]} ${sprites_yvel[$i]} ${sprites[$i]}"
    done

    # Get the left, right, top and bottom of all 400 sprites.
    sg_cmd "arr sprite top ${sprites[*]}"
    sprites_t=("${array[@]}")
    sg_cmd "arr sprite bottom ${sprites[*]}"
    sprites_b=("${array[@]}")
    sg_cmd "arr sprite left ${sprites[*]}"
    sprites_l=("${array[@]}")
    sg_cmd "arr sprite right ${sprites[*]}"
    sprites_r=("${array[@]}")

    # Loop over all 400 sprites and check bounds collision.
    for (( i = 0; i < ${#sprites[@]}; i++ )); do
        if (( sprites_yvel[$i] < 0 && sprites_t[$i] < 0 )); then
            sprites_yvel[i]=$(( -sprites_yvel[$i] ))
        elif (( sprites_yvel[$i] > 0 && sprites_b[$i] > $height )); then
            sprites_yvel[$i]=$(( -sprites_yvel[$i] ))
        elif (( sprites_xvel[$i] < 0 && sprites_l[$i] < 0 )); then
            sprites_xvel[$i]=$(( -sprites_xvel[$i] ))
        elif (( sprites_xvel[$i] > 0 && sprites_r[$i] > $width )); then
            sprites_xvel[$i]=$(( -sprites_xvel[$i] ))
        fi
    done

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw all sprite objects.
    sg_cmd "draw sprite ${sprites[*]}"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
