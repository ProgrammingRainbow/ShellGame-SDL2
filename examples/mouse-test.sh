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

mode=1
fps=0

declare -A mouse_button=(
    [button1]=0
    [button2]=0
    [button3]=0
)

declare -A mouse_pos=(
    [x]=0
    [y]=0
)

declare -A pressed=(
    [esc]=0
    [f]=0
    [1]=0
    [2]=0
)

sg_cmd "start sg"
sg_cmd "set sg title Test Mouse"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 Button 1: 0"
text_mouse1=$reply
sg_cmd "set text cy 100 $text_mouse1"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 Button 2: 0"
text_mouse2=$reply
sg_cmd "set text cy 200 $text_mouse2"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 Button 3: 0"
text_mouse3=$reply
sg_cmd "set text cy 300 $text_mouse3"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 Mouse X: 0"
text_mouse4=$reply
sg_cmd "set text cy 400 $text_mouse4"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 Mouse Y: 0"
text_mouse5=$reply
sg_cmd "set text cy 500 $text_mouse5"

sg_cmd "set text cx 400 $text_mouse1 $text_mouse2 $text_mouse3 $text_mouse4 $text_mouse5"

sg_cmd "new text examples/fonts/freesansbold.ttf 50 FPS: 0"
text_fps=$reply

sg_cmd "set text pos 10 10 $text_fps"

sg_cmd "set sg fps 10000"

while true; do
    sg_cmd "update sg"

    sg_cmd "arr key pressed esc f 1 2"
    pressed[esc]=${array[0]}
    pressed[f]=${array[1]}
    pressed[1]=${array[2]}
    pressed[2]=${array[3]}

    (( pressed[esc] )) && sg_quit 0
    (( pressed[f] )) && fps=$(( 1 - fps ))
    (( pressed[1] )) && mode=1
    (( pressed[2] )) && mode=2

    if (( mode == 1 )); then
        sg_cmd "get mouse held 1"
        sg_cmd "set text string $text_mouse1 Button 1: $reply"

        sg_cmd "get mouse held 2"
        sg_cmd "set text string $text_mouse2 Button 2: $reply"

        sg_cmd "get mouse held 3"
        sg_cmd "set text string $text_mouse3 Button 3: $reply"

        sg_cmd "get mouse x"
        sg_cmd "set text string $text_mouse4 Mouse X: $reply"

        sg_cmd "get mouse y"
        sg_cmd "set text string $text_mouse5 Mouse Y: $reply"
    else
        sg_cmd "arr mouse held 1 2 3"
        sg_cmd "set text string $text_mouse1 Button 1: ${array[0]}"
        sg_cmd "set text string $text_mouse2 Button 2: ${array[1]}"
        sg_cmd "set text string $text_mouse3 Button 3: ${array[2]}"

        sg_cmd "arr mouse pos"
        sg_cmd "set text string $text_mouse4 Mouse X: ${array[0]}"
        sg_cmd "set text string $text_mouse5 Mouse Y: ${array[1]}"
    fi

    sg_cmd "set render clear"

    sg_cmd "draw text $text_mouse1 $text_mouse2 $text_mouse3 $text_mouse4 $text_mouse5"

    if (( fps )); then
        sg_cmd "get sg fps"
        sg_cmd "set text string $text_fps FPS: $reply"
        sg_cmd "draw text $text_fps"
    fi

    sg_cmd "set render present"
done

sg_quit 0
