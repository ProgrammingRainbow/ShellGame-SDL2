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
player_vel=300
player_normalized_vel=232
player_x_vel=0
player_y_vel=0
sprite_vel=180
sprite_y_vel=$sprite_vel
sprite_x_vel=$sprite_vel
fps_display=0
pos_display=0
fullscreen=0
play_music=0
play_sound=0

# Associated Arrays.
declare -A pressed=(
    [esc]=0
    [space]=0
    [f]=0
    [p]=0
    [m]=0
    [n]=0
    [1]=0
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

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 3 4 5 space"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[3]=${array[3]}
    pressed[4]=${array[4]}
    pressed[5]=${array[5]}
    pressed[space]=${array[6]}
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

# function to enable/disable music.
update_music() {
    if (( play_music )); then
        sg_cmd "get music playing"
        if (( reply )); then
            sg_cmd "set music resume"
        else
            sg_cmd "play music $music"
        fi
    else
        sg_cmd "set music pause"
    fi
}

# Start ShellGame.
sg_cmd "start sg"

# Set the size, title, icon, resizable, and scale quality.
sg_cmd "set sg size $width $height"
sg_cmd "set sg title Begginer's Guide to ShellGame"
sg_cmd "set sg icon examples/images/bg-bash-logo.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Load the background image as an image.
sg_cmd "new image examples/images/bg-background.png"
# Save the returned sprt_id to back_sprt variable.
back_img=$reply

# Create a new rect from the image.
sg_cmd "new rect image $back_img"
back_rect=$reply

# Load an image as a sprite.
sg_cmd "new sprite examples/images/bg-bash-logo.png"
bash_sprt=$reply

sg_cmd "new sprite examples/images/bg-zsh-logo.png"
zsh_sprt=$reply

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 FPS: 0"
text_fps=$reply

# Setting anchor and position of text object.
sg_cmd "set text cx $(( width / 2 )) $text_fps"
sg_cmd "set text top 10 $text_fps"

# Create X position text with shadow text.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 X: 0"
text_x=$reply
sg_cmd "set text pos 10 10 $text_x"

# Create Y position text with shadow text.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf 70 10 Y: 0"
text_y=$reply
sg_cmd "set text top 10 $text_y"
sg_cmd "set text right $(( width - 15 )) $text_y"

# Loading sound effects.
sg_cmd "new sound examples/sounds/zsh2.ogg"
zsh_snd=$reply

sg_cmd "new sound examples/sounds/bash2.ogg"
bash_snd=$reply

# Loading Music.
sg_cmd "new music examples/music/freesoftwaresong-8bit.ogg"
music=$reply

# Set play music enable/disable.
update_music

# Set fullscreen enable/disable.
update_fullscreen

# Set FPS option.
# sg_cmd "set sg fps 60"

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # Get the state of the needed keys and update associated array.
    get_keys_pressed

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

    # Escape to quit.
    (( pressed[esc] )) && sg_quit 0

    # Toggle fullscreen.
    if (( pressed[1] )); then
        (( fullscreen = 1 - fullscreen ))
        update_fullscreen
    fi

    # Toggle show fps.
    (( pressed[2] )) && (( fps_display = 1 - fps_display ))

    # Toggle playing music. If game is not in playing start pause music.
    if (( pressed[3] )); then
        play_music=$(( 1 - play_music ))
        update_music
    fi

    # Toggle sound.
    (( pressed[4] )) && (( play_sound = 1 - play_sound ))

    # Toggle zsh possition text.
    (( pressed[5] )) && (( pos_display = 1 - pos_display ))

    # If space is pressed set the render draw color to a random color. Play sound.
    if (( pressed[space] )); then
        (( play_sound )) && sg_cmd "play sound $bash_snd"
        r=$(( RANDOM % 256 ))
        g=$(( RANDOM % 256 ))
        b=$(( RANDOM % 256 ))
        sg_cmd "set render color $r $g $b"
    fi

    # Moving the player sprite using a s w d or left down up right.
    # Get x and y as -1, 0, or 1
    player_x_vel=0
    player_y_vel=0
    (( held[a] || held[left] )) && player_x_vel=-1 
    (( held[s] || held[down] )) && player_y_vel=1
    (( held[w] || held[up] )) && player_y_vel=-1
    (( held[d] || held[right] )) && player_x_vel=1

    # If moving in a diagnal use a normalized velocity 0.707.
    if (( player_x_vel && player_y_vel )); then
        sg_cmd "update sprite pos $(( player_x_vel * player_normalized_vel )) \
            $(( player_y_vel * player_normalized_vel )) $bash_sprt"
    else
        sg_cmd "update sprite pos $(( player_x_vel * player_vel )) \
            $(( player_y_vel * player_vel )) $bash_sprt"
    fi

    # Update the zsh sprite using delta time.
    sg_cmd "update sprite pos $sprite_x_vel $sprite_y_vel $zsh_sprt"

    # Check for collision with the borders and flip direction.
    sg_cmd "arr sprite rect $zsh_sprt"
    if (( array[0] < 0 )); then 
        sprite_x_vel=$sprite_vel
        (( play_sound )) && sg_cmd "play sound $zsh_snd"
    fi
    if (( array[4] > width )); then
        sprite_x_vel=-$sprite_vel
        (( play_sound )) && sg_cmd "play sound $zsh_snd"
    fi
    if (( array[1] < 0 )); then
        sprite_y_vel=$sprite_vel
        (( play_sound )) && sg_cmd "play sound $zsh_snd"
    fi
    if (( array[5] > height ));then
        sprite_y_vel=-$sprite_vel
        (( play_sound )) && sg_cmd "play sound $zsh_snd"
    fi

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background image.
    sg_cmd "draw image NULL $back_rect $back_img"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    if (( pos_display )); then
        sg_cmd "arr sprite pos $zsh_sprt"
        sg_cmd "set text string $text_x X: ${array[0]}"
        sg_cmd "set text string $text_y Y: ${array[1]}"
        sg_cmd "draw text $text_x $text_y"
    fi

    # Draw sprites.
    sg_cmd "draw sprite $zsh_sprt $bash_sprt"

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
