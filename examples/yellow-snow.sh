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
width=800
height=600
player_offset=45
player_top=0
player_left=0
player_right=0
flake_size=0
text_size=40
text_bubble_size=5
ground=535
velocity=300
score=0
playing=1
fps=0
fps_size=40
fps_bubble_size=5
fullscreen=0

# Global arrays
declare -a flakes

# Global Associative arrays.
declare -A pressed=(
    [esc]=0
    [f]=0
    [space]=0
    [w]=0
    [m]=0
)

declare -A held=(
    [left]=0
    [right]=0
)

declare -A flake_rect=(
    [left]=0
    [right]=0
    [cy]=0
)

# Function for resetting flakes 
flake_reset() {
    x=$(( RANDOM % (width - flake_size) ))
    if (( $1 == 1 )); then
        y=$(( -RANDOM % (2 * height + flake_size) ))
    else
        y=$(( -RANDOM % (height + flake_size) ))
    fi

    sg_cmd "set rect pos $x $y $2"
}

# Tell server to start ShellGame.
sg_cmd "start sg"
# sg_cmd "set sg scale 1.5"

sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"
# sg_cmd "set render intscale enable"

# Optionally set Window attributes.
sg_cmd "set sg title Don't eat the Yellow Snow!"
sg_cmd "set sg icon examples/images/yellow.png"
sg_cmd "set sg size $width $height"

# Create new background sprite.
sg_cmd "new sprite examples/images/background.png"
background=$reply

# Create player sprite.
sg_cmd "new sprite examples/images/player.png"
player=$reply

# Set player on the ground and get the top.
sg_cmd "set sprite b $ground $player"
sg_cmd "get sprite t $player"
player_top=$reply

# Set player in the center.
sg_cmd "set sprite cx $(( width / 2 )) $player"

sg_cmd "get sprite l $player"
player_left=$(( reply + 45 ))
sg_cmd "get sprite r $player"
player_right=$(( reply - 45 ))

# Load the white and yellow flake images.
sg_cmd "new image examples/images/yellow.png"
yellow_img=$reply

sg_cmd "new image examples/images/white.png"
white_img=$reply

# Populate the flake size from an image.
sg_cmd "get image w $yellow_img"
flake_size=$reply

# Populate an array of rects for the flakes and reset them.
for i in {0..15}; do
    sg_cmd "new rect image $yellow_img"
    flakes[$i]=$reply
    flake_reset 1 ${flakes[$i]}
done

# Create a new Text object.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $text_size $text_bubble_size Score: $score"
score_text=$reply

# Set the Text to anchor in the top right.
sg_cmd "set text r $(( width - 10 )) $score_text"
sg_cmd "set text t 10 $score_text"

# Load the 2 sounds effects and the music..
sg_cmd "new sound examples/sounds/collect.ogg"
collect_snd=$reply

sg_cmd "new sound examples/sounds/hit.ogg"
hit_snd=$reply

sg_cmd "new music examples/music/winter_loop.ogg"
music=$reply

# Play the loaded music.
sg_cmd "play music $music"

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $fps_size $fps_bubble_size FPS: 0"
text_fps=$reply

# Set the FPS Text object anchord to the top left.
sg_cmd "set text pos 10 10 $text_fps"

# Override the default 60fps
# sg_cmd "set sg fps 10000"

# Main game loop. 
while true; do
    # "update sg" should be run first.
    sg_cmd "update sg"

    # Capturing pressed keys in an associated array.
    sg_cmd "arr key pressed esc space f w m"
    pressed[esc]=${array[0]}
    pressed[space]=${array[1]}
    pressed[f]=${array[2]}
    pressed[w]=${array[3]}
    pressed[m]=${array[4]}

    # Escape to quit.
    (( pressed[esc] )) && sg_quit 0

    # Toggle show fps and fullscreen.
    (( pressed[f] )) && fps=$(( 1 - fps ))

    if (( pressed[w] )); then
        fullscreen=$(( 1 - fullscreen ))
        if (( fullscreen )); then
            sg_cmd "set sg fullscreen desktop"
        else
            sg_cmd "set sg fullscreen disable"
        fi
    fi

    # Toggle playing the music. If game is not in playing state pause music.
    if (( pressed[m] )); then
        sg_cmd "get music playing"
        if (( reply )); then
            sg_cmd "set music halt"
        else
            sg_cmd "play music $music"
            (( playing )) || sg_cmd "set music pause"
        fi
    fi

    if (( playing )); then
        # If the game is in playing state capture left and right in an associated array.
        sg_cmd "arr key held left right"
        held[left]=${array[0]}
        held[right]=${array[1]}

        # Update the player position using delta time. Flip the player image.
        # Check the player with offset is not off the screen.
        if (( held[left] )); then
            sg_cmd "set sprite flip h $player"
            sg_cmd "update sprite pos -$velocity 0 $player"
            sg_cmd "get sprite l $player"
            if (( reply <= -player_offset )); then
                sg_cmd "set sprite l -$player_offset $player"
            fi
        fi

        if (( held[right] )); then
            sg_cmd "set sprite flip n $player"
            sg_cmd "update sprite pos $velocity 0 $player"
            sg_cmd "get sprite r $player"
            if (( reply >= width + player_offset )); then
                sg_cmd "set sprite r $(( width + player_offset )) $player"
            fi
        fi

        # If left or right was pressed update the left and right offsets for collision detection.
        if (( held[left] || held[right] )); then
            sg_cmd "get sprite l $player"
            player_left=$(( reply + $player_offset ))
            sg_cmd "get sprite r $player"
            player_right=$(( reply - $player_offset ))
        fi

        # Update all flakes position with delta time at once.
        sg_cmd "update rect y $velocity ${flakes[*]}"

        # Loop over all flakes in the flakes array.
        for (( i = 0; i < ${#flakes[@]}; i++ )); do

            # Capture left right and cx in an associated array.
            sg_cmd "arr rect rect ${flakes[$i]}"
            flake_rect[left]=${array[0]}
            flake_rect[right]=${array[4]}
            flake_rect[cy]=${array[7]}

            # If cy passes the ground reset the flake.
            if (( flake_rect[cy] > $ground )); then
                flake_reset 0 ${flakes[$i]}
            # Using box collision test if a flake as hit the player.
            elif (( flake_rect[cy] > player_top )); then
                if (( flake_rect[right] > player_left )); then
                    if (( flake_rect[left] < player_right )); then
                        # If it's a yellow flake pause the music and game.
                        # Play hit sound.
                        if (( $i < 5)); then
                            sg_cmd "play sound $hit_snd"
                            sg_cmd "set music pause"
                            playing=0
                        else
                            # Play collect sound and update Text with new string.
                            (( score++ ))
                            sg_cmd "play sound $collect_snd"
                            sg_cmd "set text string $score_text Score: $score"
                            flake_reset 0 ${flakes[$i]}
                        fi
                    fi
                fi
            fi
        done
    else
        # If game is not playing and space is pressed, reset the game, score, and unpause music.
        if (( pressed[space] )); then
            playing=1
            score=0
            sg_cmd "set text string $score_text Score: $score"
            for flake in "${flakes[@]}"; do
                flake_reset 1 $flake
            done
            sg_cmd "get music playing"
            (( $reply )) && sg_cmd "set music resume"
        fi
    fi

    # Clear the renderer.
    sg_cmd "set render clear"

    # Draw the background, player and score.
    sg_cmd "draw sprite $background"
    sg_cmd "draw sprite $player"
    sg_cmd "draw text $score_text"

    # Update the FPS text and display it if enabled.
    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "draw text $text_fps"
    fi

    # Loop over all the flakes and draw them.
    for (( i = 0; i < ${#flakes[@]}; i++ )); do
        if (( $i < 5)); then
            sg_cmd "draw image NULL ${flakes[$i]} $yellow_img"
        else
            sg_cmd "draw image NULL ${flakes[$i]} $white_img"
        fi
    done

    # Present the populated renderer.
    sg_cmd "set render present"
done

sg_quit 0
