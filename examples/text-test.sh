#!/bin/env bash

if [ $# == 1 ]; then
    case $1 in
        --client) source client.sh ;;
        --shared) source shared.sh ;;
        --socket) source socket.sh ;;
        --pipe) source pipe.sh ;;
        *)
            echo "Usage: $0 {--pipe|--socket|--shared|--client}"
            exit 1
            ;;
    esac

    if [ $? -ne 0 ]; then
        echo "Error: Failed to source file for '$1'"
        exit 1
    fi
else
    echo "Usage: $0 {--pipe|--socket|--shared|--client}"
    exit 1
fi

width=1280
height=720
mode=4
fps=0

declare -a texts
declare -a texts_xvel
declare -a texts_yvel
declare -a texts_avel

declare -A pressed=(
    [esc]=0
    [f]=0
    [w]=0
    [1]=0
    [2]=0
    [3]=0
    [4]=0
    [5]=0
    [6]=0
    [7]=0
    [8]=0
)

declare -A txt_rect=(
    [r]=0
    [l]=0
    [t]=0
    [b]=0
)

sg_cmd "start sg"
sg_cmd "set sg title Text Test."
sg_cmd "set sg size $width $height"

text_reset() {
    x=$(( RANDOM % width ))
    y=$(( RANDOM % height ))
    r=$(( RANDOM % 255 ))
    g=$(( RANDOM % 255 ))
    b=$(( RANDOM % 255 ))
    sg_cmd "set text cx $x $1"
    sg_cmd "set text cy $y $1"
    sg_cmd "set text color $1 $r $g $b"
}

for i in {0..199}; do
    text_size=$(( ( RANDOM % 70 ) + 1 ))
    sg_cmd "new text examples/fonts/freesansbold.ttf $text_size hello"
    texts[$i]=$reply
    text_reset ${texts[$i]}
    texts_xvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
    texts_yvel[$i]=$(( ( RANDOM % 1200 ) - 600 ))
    texts_avel[$i]=$(( ( RANDOM % 720 ) - 360 ))
done

sg_cmd "new text examples/fonts/freesansbold.ttf 50 FPS: 0"
text_fps=$reply

sg_cmd "set text pos 10 10 $text_fps"

# sg_cmd "set sg fps 10000"

while true; do
    sg_cmd "update sg"

    sg_cmd "arr key pressed esc f w 1 2 3 4 5 6 7 8 9"
    pressed[esc]=${array[0]}
    pressed[f]=${array[1]}
    pressed[w]=${array[2]}
    pressed[1]=${array[3]}
    pressed[2]=${array[4]}
    pressed[3]=${array[5]}
    pressed[4]=${array[6]}
    pressed[5]=${array[7]}
    pressed[6]=${array[8]}
    pressed[7]=${array[9]}
    pressed[8]=${array[10]}
    pressed[9]=${array[11]}

    (( pressed[1] )) && mode=1
    (( pressed[2] )) && mode=2
    (( pressed[3] )) && mode=3
    (( pressed[4] )) && mode=4
    (( pressed[5] )) && mode=5
    (( pressed[6] )) && mode=6
    (( pressed[7] )) && mode=7
    (( pressed[8] )) && mode=8
    (( pressed[9] )) && mode=9

    (( pressed[esc] )) && sg_quit 0
    (( pressed[w] )) && sg_cmd "set sg fullscreen toggle"
    (( pressed[f] )) && fps=$(( 1 - fps ))

    for i in "${!texts[@]}"; do
        sg_cmd "update text pos ${texts_xvel[$i]} ${texts_yvel[$i]} ${texts[$i]}"
    done

    if (( mode == 9 )); then
        for i in "${!texts[@]}"; do
            sg_cmd "update text angle ${texts_avel[$i]} ${texts[$i]}"
        done
    fi

    if (( mode == 1 || mode == 2 || mode == 5 || mode == 6)); then
        for i in "${!texts[@]}"; do
            sg_cmd "get text t ${texts[$i]}"
            if (( texts_yvel[i] < 0 && reply < 0 )); then
                texts_yvel[i]=$(( -texts_yvel[i] ))
            fi
            r=$(( ( reply + 100 ) % 255 ))
            sg_cmd "get text b ${texts[$i]}"
            if (( texts_yvel[i] > 0 && reply > $height )); then
                texts_yvel[i]=$(( -texts_yvel[i] ))
            fi
            g=$(( ( reply + 100 ) % 255 ))
            sg_cmd "get text l ${texts[$i]}"
            if (( texts_xvel[$i] < 0 && reply < 0 )); then
                texts_xvel[i]=$(( -texts_xvel[i] ))
            fi
            b=$(( ( reply + 100 ) % 255 ))
            sg_cmd "get text r ${texts[$i]}"
            if (( texts_xvel[$i] > 0 && reply > $width )); then
                texts_xvel[i]=$(( -texts_xvel[i] ))
            fi

            if (( mode == 5 || mode == 6 )); then
                sg_cmd "set text string $i $reply"
                sg_cmd "set text color $i $r $g $b"
            fi
        done
    elif (( mode == 3 || mode == 7 )); then
        for i in "${!texts[@]}"; do
            sg_cmd "arr text rect ${texts[$i]}"
            txt_rect[l]=${array[0]}
            txt_rect[t]=${array[1]}
            txt_rect[r]=${array[4]}
            txt_rect[b]=${array[5]}
            r=$(( ( ${txt_rect[t]} + 500 ) % 255 ))
            g=$(( ( ${txt_rect[b]} + 500 ) % 255 ))
            b=$(( ( ${txt_rect[l]} + 500 ) % 255 ))

            if (( texts_yvel[$i] < 0 && txt_rect[t] < 0 )); then
                texts_yvel[$i]=$(( -texts_yvel[$i] ))
            elif (( texts_yvel[$i] > 0 && txt_rect[b] > $height )); then
                texts_yvel[$i]=$(( -texts_yvel[$i] ))
            elif (( texts_xvel[$i] < 0 && txt_rect[l] < 0 )); then
                texts_xvel[$i]=$(( -texts_xvel[$i] ))
            elif (( texts_xvel[$i] > 0 && txt_rect[r] > $width )); then
                texts_xvel[$i]=$(( -texts_xvel[$i] ))
            fi

            if (( mode == 7 )); then
                sg_cmd "set text string $i ${txt_rect[r]}"
                sg_cmd "set text color $i $r $g $b"
            fi
        done
    elif (( mode == 4 || mode == 8 || mode == 9 )); then
        sg_cmd "arr text t ${texts[*]}"
        texts_t=("${array[@]}")
        sg_cmd "arr text b ${texts[*]}"
        texts_b=("${array[@]}")
        sg_cmd "arr text l ${texts[*]}"
        texts_l=("${array[@]}")
        sg_cmd "arr text r ${texts[*]}"
        texts_r=("${array[@]}")

        for i in "${!texts[@]}"; do
            if (( texts_yvel[$i] < 0 && texts_t[$i] < 0 )); then
                texts_yvel[i]=$(( -texts_yvel[$i] ))
            elif (( texts_yvel[$i] > 0 && texts_b[$i] > $height )); then
                texts_yvel[$i]=$(( -texts_yvel[$i] ))
            elif (( texts_xvel[$i] < 0 && texts_l[$i] < 0 )); then
                texts_xvel[$i]=$(( -texts_xvel[$i] ))
            elif (( texts_xvel[$i] > 0 && texts_r[$i] > $width )); then
                texts_xvel[$i]=$(( -texts_xvel[$i] ))
            fi

            r=$(( ( ${texts_t[$i]} + 500 ) % 255 ))
            g=$(( ( ${texts_b[$i]} + 500 ) % 255 ))
            b=$(( ( ${texts_l[$i]} + 500 ) % 255 ))

            if (( mode == 8 || mode == 9 )); then
                sg_cmd "set text string $i ${texts_r[$i]}"
                sg_cmd "set text color $i $r $g $b"
            fi
        done
    fi

    sg_cmd "set render clear"

    if (( mode == 1 || mode == 5 )); then
        for i in "${!texts[@]}"; do
            sg_cmd "draw text ${texts[$i]}"
        done
    else
        sg_cmd "draw text ${texts[*]}"
    fi

    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "draw text $text_fps"
    fi

    sg_cmd "set render present"
done

sg_quit 0
