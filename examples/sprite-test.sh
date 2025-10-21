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

width=1280
height=720
mode=4
size=0
angle=720
fps=0

declare -A pressed=(
    [esc]=0
    [f]=0
    [1]=0
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

declare -a sprites
declare -a sprites_xvel
declare -a sprites_yvel

sprite_random() {
    sg_cmd "arr sprite rect $1"
    w=${array[2]}
    h=${array[3]}
    x=$(( RANDOM % (width - w) ))
    y=$(( RANDOM % (height - h) ))
    sg_cmd "set sprite pos $x $y $1"
}

sg_cmd "start sg"

sg_cmd "set sg title Sprite Test."
sg_cmd "set sg size $width $height"
sg_cmd "set sg icon examples/images/yellow.png"

# sg_cmd "set sg scaling linear"

for i in {0..399}; do
    sg_cmd "new sprite examples/images/yellow.png"
    sprites[$i]=$reply
    sprite_random ${sprites[$i]}
    sprites_xvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
    sprites_yvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
done

sg_cmd "get sprite width ${sprites[0]}"
size=$reply

sg_cmd "new text examples/fonts/freesansbold.ttf 50 FPS: 0"
text_fps=$reply

sg_cmd "set text pos 10 10 $text_fps"

# sg_cmd "set sg fps 10000"

while true; do
    sg_cmd "update sg"

    sg_cmd "arr key pressed esc f 1 w y s"
    pressed[esc]=${array[0]}
    pressed[f]=${array[1]}
    pressed[1]=${array[2]}
    pressed[w]=${array[3]}
    pressed[y]=${array[4]}
    pressed[s]=${array[5]}

    (( pressed[esc] )) && sg_quit 0
    (( pressed[f] )) && fps=$(( 1 - fps ))
    (( pressed[1] )) && sg_cmd "set sg fullscreen toggle"

    if (( pressed[w] )); then
        sg_cmd "set sprite image examples/images/white.png ${sprites[*]}"
        sg_cmd "set sprite size $size $size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/white.png"
    fi
    if (( pressed[y] )); then
        sg_cmd "set sprite image examples/images/yellow.png ${sprites[*]}"
        sg_cmd "set sprite size $size $size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/yellow.png"
    fi
    if (( pressed[s] )); then
        sg_cmd "set sprite image examples/images/player.png ${sprites[*]}"
        sg_cmd "set sprite size $size $size ${sprites[*]}"
        sg_cmd "set sg icon examples/images/player.png"
    fi

    sg_cmd "arr key held left right up down"
    held[left]=${array[0]}
    held[right]=${array[1]}
    held[up]=${array[2]}
    held[down]=${array[3]}

    (( held[left] )) && sg_cmd "update sprite angle -$angle ${sprites[*]}"
    (( held[right] )) && sg_cmd "update sprite angle $angle ${sprites[*]}"

    if (( held[up] )); then
        (( size < 200 )) && size=$(( size + 1 ))
        sg_cmd "set sprite size $size $size ${sprites[*]}"
    fi
    if (( held[down] )); then
        (( size > 2 )) && size=$(( size - 1 ))
        sg_cmd "set sprite size $size $size ${sprites[*]}"
    fi

    for i in "${!sprites[@]}"; do
        sg_cmd "update sprite pos ${sprites_xvel[$i]} ${sprites_yvel[$i]} ${sprites[$i]}"
    done

    sg_cmd "arr sprite top ${sprites[*]}"
    sprites_t=("${array[@]}")
    sg_cmd "arr sprite bottom ${sprites[*]}"
    sprites_b=("${array[@]}")
    sg_cmd "arr sprite left ${sprites[*]}"
    sprites_l=("${array[@]}")
    sg_cmd "arr sprite right ${sprites[*]}"
    sprites_r=("${array[@]}")

    for i in "${!sprites[@]}"; do
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

    sg_cmd "set render clear"

    sg_cmd "draw sprite ${sprites[*]}"

    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "draw text $text_fps"
    fi

    sg_cmd "set render present"
done

sg_quit 0
