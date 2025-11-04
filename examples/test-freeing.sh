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

velocity=300
fps=0

declare -A pressed=(
    [esc]=0
    [f]=0
    [1]=0
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

sg_cmd "start sg"
sg_cmd "set sg title Test Freeing"

sg_cmd "new image examples/images/background.png"
back_img=$reply
echo "new back_img is $back_img"

sg_cmd "new image examples/images/background.png"
back2_img=$reply
echo "new back2_img is $back2_img"

sg_cmd "free image $back_img"
back_img=$array
echo "freed back_img is $back_img"

sg_cmd "new image examples/images/background.png"
back_img=$reply
echo "new back_img is $back_img"

sg_cmd "new image examples/images/background.png"
back3_img=$reply
echo "new back3_img is $back3_img"

# sg_cmd "free image $back_img"
# back_img=$array

sg_cmd "new rect image $back_img"
back_rect=$reply

sg_cmd "new text examples/fonts/freesansbold.ttf 50 FPS: 0"
text_fps=$reply

sg_cmd "set text pos 10 10 $text_fps"

# sg_cmd "set sg fps 10000"

while true; do
    sg_cmd "update sg"

    sg_cmd "arr key pressed esc f 1 h v n"
    pressed[esc]=${array[0]}
    pressed[f]=${array[1]}
    pressed[1]=${array[2]}
    pressed[h]=${array[3]}
    pressed[v]=${array[4]}
    pressed[n]=${array[5]}

    (( pressed[esc] )) && sg_quit 0
    (( pressed[f] )) && fps=$(( 1 - fps ))
    (( pressed[1] )) && sg_cmd "set sg fullscreen toggle"
    (( pressed[h] )) && sg_cmd "set rect flip h $back_rect"
    (( pressed[v] )) && sg_cmd "set rect flip v $back_rect"
    (( pressed[n] )) && sg_cmd "set rect flip n $back_rect"

    sg_cmd "arr key held a s w d left right"
    held[a]=${array[0]}
    held[s]=${array[1]}
    held[w]=${array[2]}
    held[d]=${array[3]}
    held[left]=${array[4]}
    held[right]=${array[5]}

    (( held[a] )) && sg_cmd "update rect x -$velocity $back_rect"
    (( held[s] )) && sg_cmd "update rect y $velocity $back_rect"
    (( held[w] )) && sg_cmd "update rect y -$velocity $back_rect"
    (( held[d] )) && sg_cmd "update rect x $velocity $back_rect"
    (( held[left] )) && sg_cmd "update rect angle -$velocity $back_rect"
    (( held[right] )) && sg_cmd "update rect angle $velocity $back_rect"

    sg_cmd "set render clear"

    sg_cmd "draw image NULL $back_rect $back_img"

    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "draw text $text_fps"
    fi

    sg_cmd "set render present"
done

sg_quit 0
