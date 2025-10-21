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

# Start ShellGame.
sg_cmd "start sg"

# Set the title to ShellGame.
sg_cmd "set sg title ShellGame"

# Load the background image as a sprite.
sg_cmd "new sprite examples/images/background.png"
# Save the returned sprt_id to back_sprt variable.
back_sprt=$reply

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # If esc key is pressed exit.
    sg_cmd "get key pressed esc"
    (( reply )) && sg_quit 0

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Draw the background sprite.
    sg_cmd "draw sprite $back_sprt"

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
