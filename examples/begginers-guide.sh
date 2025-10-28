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

# Global Variables
width=1280
height=720
player_vel=300
sprite_vel=180
sprite_y_vel=$sprite_vel
sprite_x_vel=$sprite_vel
fps=0
pos=0

# Associated Arrays
declare -A pressed=(
    [esc]=0
    [space]=0
    [f]=0
    [p]=0
    [m]=0
)

declare -A held=(
    [a]=0
    [s]=0
    [w]=0
    [d]=0
    [left]=0
    [down]=0
    [up]=0
    [right]=0
)

# Start ShellGame.
sg_cmd "start sg"

# Set the title to ShellGame.
sg_cmd "set sg title Begginer's Guide to ShellGame"

# Set the window size.
sg_cmd "set sg size $width $height"

# Set the window icon.
sg_cmd "set sg icon examples/images/bash-logo.png"

# Load the background image as an image.
sg_cmd "new image examples/images/shellgame-background.png"
# Save the returned sprt_id to back_sprt variable.
back_img=$reply

# Create a new rect from the image.
sg_cmd "new rect image $back_img"
back_rect=$reply

# Load an image as a sprite.
sg_cmd "new sprite examples/images/bash-logo.png"
bash_sprt=$reply

sg_cmd "new sprite examples/images/zsh-logo.png"
zsh_sprt=$reply

# Create a text object for displaying the FPS.
sg_cmd "new text examples/fonts/freesansbold.ttf 70 FPS: 0"
text_fps=$reply
# Setting anchor and position of text object.
sg_cmd "set text cx $(( width / 2 )) $text_fps"
sg_cmd "set text top 10 $text_fps"

# Create FPS text with shadow text.
sg_cmd "new text examples/fonts/freesansbold.ttf 70 FPS: 0"
text_fps_shadow=$reply
sg_cmd "set text color $text_fps_shadow 20 20 20"
sg_cmd "set text cx $(( width / 2  + 5 )) $text_fps_shadow"
sg_cmd "set text top 15 $text_fps_shadow"

# Create X position text with shadow text.
sg_cmd "new text examples/fonts/freesansbold.ttf 70 X: 0"
text_x=$reply
sg_cmd "set text pos 10 10 $text_x"
sg_cmd "new text examples/fonts/freesansbold.ttf 70 X: 0"
text_x_shadow=$reply
sg_cmd "set text color $text_x_shadow 20 20 20"
sg_cmd "set text pos 15 15 $text_x_shadow"

# Create Y position text with shadow text.
sg_cmd "new text examples/fonts/freesansbold.ttf 70 Y: 0"
text_y=$reply
sg_cmd "set text top 10 $text_y"
sg_cmd "set text right $(( width - 15 )) $text_y"
sg_cmd "new text examples/fonts/freesansbold.ttf 70 Y: 0"
text_y_shadow=$reply
sg_cmd "set text color $text_y_shadow 20 20 20"
sg_cmd "set text top 15 $text_y_shadow"
sg_cmd "set text right $(( width - 10 )) $text_y_shadow"

# Loading sound effects.
sg_cmd "new sound examples/sounds/zsh2.ogg"
zsh_snd=$reply

sg_cmd "new sound examples/sounds/bash2.ogg"
bash_snd=$reply

# Loading Music.
sg_cmd "new music examples/music/freesoftwaresong-8bit.ogg"
music=$reply

# Set FPS option.
sg_cmd "set sg fps 10000"

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # Getting key pressed states as an array.
    sg_cmd "arr key pressed esc space f p m"
    pressed[esc]=${array[0]}
    pressed[space]=${array[1]}
    pressed[f]=${array[2]}
    pressed[p]=${array[3]}
    pressed[m]=${array[4]}

    # Getting key held states as and array.
    sg_cmd "arr key held a s w d left down up right"
    held[a]=${array[0]}
    held[s]=${array[1]}
    held[w]=${array[2]}
    held[d]=${array[3]}
    held[left]=${array[4]}
    held[down]=${array[5]}
    held[up]=${array[6]}
    held[right]=${array[7]}

    (( pressed[esc] )) && sg_quit 0
    (( pressed[f] )) && fps=$(( 1 - fps ))
    (( pressed[p] )) && pos=$(( 1 - pos ))

    if (( pressed[space] )); then
        sg_cmd "play sound $bash_snd"
        r=$(( RANDOM % 256 ))
        g=$(( RANDOM % 256 ))
        b=$(( RANDOM % 256 ))
        sg_cmd "set render color $r $g $b"
    fi

    # Play or pause music toggle.
    if (( pressed[m] )); then
        sg_cmd "get music playing"
        if (( reply )); then
            sg_cmd "get music paused"
            if (( reply )); then
                sg_cmd "set music resume"
            else
                sg_cmd "set music pause"
            fi
        else
            sg_cmd "play music $music"
        fi
    fi

    # Moving the player sprite using a s w d or left down up right. h
    (( held[a] || held[left] )) && sg_cmd "update sprite x -$player_vel $bash_sprt"
    (( held[s] || held[down] )) && sg_cmd "update sprite y $player_vel $bash_sprt"
    (( held[w] || held[up] )) && sg_cmd "update sprite y -$player_vel $bash_sprt"
    (( held[d] || held[right] )) && sg_cmd "update sprite x $player_vel $bash_sprt"

    sg_cmd "update sprite pos $sprite_x_vel $sprite_y_vel $zsh_sprt"
    sg_cmd "arr sprite rect $zsh_sprt"
    if (( array[0] < 0 )); then 
        sprite_x_vel=$sprite_vel
        sg_cmd "play sound $zsh_snd"
    fi
    if (( array[4] > width )); then
        sprite_x_vel=-$sprite_vel
        sg_cmd "play sound $zsh_snd"
    fi

    if (( array[1] < 0 )); then
        sprite_y_vel=$sprite_vel
        sg_cmd "play sound $zsh_snd"
    fi

    if (( array[5] > height ));then
        sprite_y_vel=-$sprite_vel
        sg_cmd "play sound $zsh_snd"
    fi

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background image.
    sg_cmd "draw image NULL $back_rect $back_img"

    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "set text string $text_fps_shadow FPS: $reply"
        sg_cmd "draw text $text_fps_shadow $text_fps"
    fi

    if (( pos )); then
        sg_cmd "arr sprite pos $zsh_sprt"
        sg_cmd "set text string $text_x X: ${array[0]}"
        sg_cmd "set text string $text_x_shadow X: ${array[0]}"
        sg_cmd "set text string $text_y Y: ${array[1]}"
        sg_cmd "set text string $text_y_shadow Y: ${array[1]}"
        sg_cmd "draw text $text_x_shadow $text_x $text_y_shadow $text_y"
    fi

    # Draw sprites.
    sg_cmd "draw sprite $zsh_sprt $bash_sprt"

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
