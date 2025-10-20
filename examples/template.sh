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

sg_cmd "start sg"
sg_cmd "set sg title Template"

sg_cmd "new sprite examples/images/background.png"
back_sprt=$reply

while true; do
    sg_cmd "update sg"

    sg_cmd "get key pressed esc"
    (( reply )) && sg_quit 0

    sg_cmd "set render clear"

    sg_cmd "draw sprite $back_sprt"

    sg_cmd "set render present"
done

sg_quit 0
