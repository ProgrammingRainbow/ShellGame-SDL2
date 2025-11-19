#!/usr/bin/env bash

declare -a network
if [[ $# -eq 1 ]]; then
    if [ -e "$1" ]; then
        echo
        echo "Loading network $1"
        read -a network < "$1"
    else
        echo "Error: file $1 not found!"
        exit 1
    fi
else
    echo "Usage: $0 {network file}"
    exit 1
fi

source pipe.sh

# Global constants.
readonly WIDTH=800
readonly HALF_WIDTH=$(( WIDTH / 2 ))
readonly HEIGHT=600
readonly PLAYER_OFFSET=48
readonly PLAYER_TOP=377
readonly PLAYER_Y=456
readonly TEXT_SIZE=50
readonly BUBBLE_SIZE=7
readonly GROUND=535
readonly VELOCITY=5

# Global Variables.
player_x=$HALF_WIDTH
score=0
playing=1
fps_display=0
fps_unlock=0

# Global AI constants
readonly INPUTS=15
readonly OUTPUTS=2
readonly TRIALS=$(( network[1] ))
readonly GENS=$(( network[2] ))
readonly MUTS=$(( network[3] ))
readonly LAYER_COUNT=$(( network[4] ))
LAYERS=("${network[@]:5:5}")
readonly LAYERS
readonly X_RANGE=$(( network[10] ))
readonly Y_RANGE=$(( network[11] ))
readonly INDEX_NETWORK=12
readonly SCALE=1000

# Global arrays.
declare -a flakes
declare -a flakes_x
declare -a flakes_y
declare -a inputs
declare -a outputs

# Global Associative arrays.
declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
    [space]=0
)

declare -A held=(
    [left]=0
    [right]=0
)

# Function for resetting flakes 
flake_reset() {
    local x=0
    local y=0
    (( x = RANDOM % WIDTH ))
    (( y = -RANDOM % ( HEIGHT + ( HEIGHT * $1 ) ) ))

    flakes_x[$2]=$x
    flakes_y[$2]=$y
    sg_cmd "set rect cx $x ${flakes[$2]}"
    sg_cmd "set rect cy $y ${flakes[$2]}"
}

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 space"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[space]=${array[3]}
}

# Update the FPS text and display it if enabled.
update_fps() {
    sg_cmd "get sg fps"
    sg_cmd "set text string $text_fps FPS: $reply"
    sg_cmd "draw text $text_fps"
}

echo
echo "filename:  $1"
echo "score: $(( network[0] / network[1] ))"
echo "layer_count: $LAYER_COUNT"
printf "%s" "layers:"
for (( i = 5; i < 10; i++ )); do
    (( network[i] )) && printf " %s" "${network[$i]}"
done
echo
echo "trials: $TRIALS"
echo "generations: $GENS"
echo "mutations: $MUTS"
echo "x_range: $X_RANGE"
echo "y_range: $Y_RANGE"
echo

sleep 5

# Tell server to start ShellGame.
sg_cmd "start sg"

# Set the size, title, icon, resizable, and scale quality.
sg_cmd "set sg size $WIDTH $HEIGHT"
sg_cmd "set sg title Don't eat the Yellow Snow!"
sg_cmd "set sg icon examples/images/ys-yellow.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Scale the window but keeps the logical renderer original.
sg_cmd "set sg scale 1.5"

# Create new background sprite.
sg_cmd "new sprite examples/images/ys-background.png"
background=$reply

# Create player sprite.
sg_cmd "new sprite examples/images/ys-player.png"
player=$reply

# Set player on the ground and get the top.
sg_cmd "set sprite b $GROUND $player"

# Set player in the center.
sg_cmd "set sprite cx $player_x $player"
sg_cmd "set sprite cy $PLAYER_Y $player"

# Load the white and yellow flake images.
sg_cmd "new image examples/images/ys-yellow.png"
yellow_img=$reply

sg_cmd "new image examples/images/ys-white.png"
white_img=$reply

# Populate an array of rects for the flakes and reset them.
for i in {0..15}; do
    sg_cmd "new rect image $yellow_img"
    flakes[$i]=$reply
    flake_reset 1 $i
done

# Create a new Text object.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $TEXT_SIZE $BUBBLE_SIZE Score: $score"
score_text=$reply

# Set the Text to anchor in the top right.
sg_cmd "set text r $(( WIDTH - 10 )) $score_text"
sg_cmd "set text t 10 $score_text"

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $TEXT_SIZE $BUBBLE_SIZE FPS: 0"
text_fps=$reply

# Set the FPS Text object anchord to the top left.
sg_cmd "set text pos 10 10 $text_fps"

# Override the default 60fps
# sg_cmd "set sg fps 10000"

# Main game loop. 
while true; do
    # "update sg" should be run first.
    sg_cmd "update sg"

    # Get the state of the needed keys and update associated array.
    get_keys_pressed

    # Escape to quit.
    (( pressed[esc] )) && sg_quit 0

    # Toggle show fps.
    (( pressed[1] )) && (( fps_display = 1 - fps_display ))

    if (( pressed[2] )); then
        if (( fps_unlock )); then
            fps_unlock=0
            sg_cmd "set sg fps 60"
        else
            fps_unlock=1
            sg_cmd "set sg fps 10000"
        fi
    fi

    if (( playing )); then
        # If the game is in playing state capture left and right in an associated array.
        sg_cmd "arr key held left right"
        held[left]=${array[0]}
        held[right]=${array[1]}

        for i in {0..15}; do
            (( rel_x = ( flakes_x[i] - player_x ) ))
            if (( rel_x > HALF_WIDTH )); then
                (( rel_x -= WIDTH ))
            elif (( rel_x < -HALF_WIDTH )); then
                (( rel_x += WIDTH ))
            fi
            if (( rel_x > 0 )); then
                if (( rel_x < X_RANGE )); then
                    (( rel_x = X_RANGE - rel_x ))
                else
                    (( rel_x = 0 ))
                fi
            else
                if (( rel_x > -X_RANGE )); then
                    (( rel_x = -X_RANGE + rel_x ))
                else
                    (( rel_x = 0 ))
                fi
            fi
            (( rel_y = ( PLAYER_Y - flakes_y[i] ) ))
            (( rel_y < 0)) && (( rel_y = -rel_y ))
            if (( rel_y < Y_RANGE )); then
                (( rel_y = Y_RANGE - rel_y ))
            else
                (( rel_y = 0 ))
            fi
            (( inputs[$i] = rel_y * rel_x * 200 ))
        done

        # Hidden layers
        input_count=$INPUTS
        index=$INDEX_NETWORK
        for (( l = 0; l < LAYER_COUNT; l++ )); do
            for (( n = 0; n < LAYERS[l]; n++ )); do
                (( outputs[n] = 0 ))
                for (( i = 0; i < input_count; i++ )); do
                    (( outputs[n] += inputs[i] * network[index] ))
                    (( index++ ))
                done
                (( outputs[n] /= SCALE ))
                (( outputs[n] += network[index] ))
                (( index++ ))
                (( outputs[n] = (( outputs[n] > 0 ) ? outputs[n] : 0 ) ))
            done
            (( input_count = LAYERS[l] ))
            inputs=("${outputs[@]}")
        done

        # Output layer
        for (( n = 0; n < OUTPUTS; n++ )); do
            (( outputs[n] = 0 ))
            for (( i = 0; i < input_count; i++ )); do
                (( outputs[n] += inputs[i] * network[index] ))
                (( index++ ))
            done
            (( outputs[n] /= SCALE ))
            (( outputs[n] += network[index] ))
            (( index++ ))
            (( outputs[n] = (( outputs[n] > 0 ) ? outputs[n] : 0 ) ))
        done

        if (( held[left] || outputs[0] > outputs[1] )); then
            sg_cmd "set sprite flip h $player"
            (( player_x -= VELOCITY ))
            (( player_x < 0 )) && (( player_x += WIDTH ))
        fi
        if (( held[right] || outputs[1] > outputs[0] )); then
            sg_cmd "set sprite flip n $player"
            (( player_x += VELOCITY ))
            (( player_x > WIDTH )) && (( player_x -= WIDTH ))
        fi

        sg_cmd "set sprite cx $player_x $player"

        for i in {0..15}; do
            (( flakes_y[i] += VELOCITY ))
            sg_cmd "set rect cy ${flakes_y[$i]} ${flakes[$i]}"
            if (( flakes_y[i] > PLAYER_TOP )); then
                if (( flakes_y[i] > GROUND )); then
                    flake_reset 0 $i
                    continue
                elif (( flakes_x[i] > player_x - PLAYER_OFFSET )); then
                    if (( flakes_x[i] < player_x + PLAYER_OFFSET )); then
                        if (( $i < 5)); then
                            (( play_sound )) && sg_cmd "play sound $hit_snd"
                            sg_cmd "set music pause"
                            playing=0
                        else
                            (( score++ ))
                            (( play_sound )) && sg_cmd "play sound $collect_snd"
                            sg_cmd "set text string $score_text Score: $score"
                            flake_reset 0 $i
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
            for i in {0..15}; do
                flake_reset 1 $i
            done
        fi
    fi

    # Clear the renderer.
    sg_cmd "set render clear"

    # Draw the background, player and score.
    sg_cmd "draw sprite $background"
    sg_cmd "draw sprite $player"
    sg_cmd "draw text $score_text"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # Loop over all the flakes and draw them.
    for (( i = 0; i < ${#flakes[@]}; i++ )); do
        if (( $i < 5)); then
            sg_cmd "draw image NULL ${flakes[$i]} $yellow_img"
        else
            sg_cmd "draw image NULL ${flakes[$i]} $white_img"
        fi
    done

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
