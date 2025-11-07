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
size=16
columns=$(( width / size ))
rows=$(( height / size ))
text_size=50
bubble_size=7
fps=16
fps_display=0
fullscreen=0
paused=0

# Global arrays.
declare -a rects
declare -a draw_rects
declare -a board
declare -a pids

# Global Associative arrays.
declare -A pressed=(
    [esc]=0
    [1]=0
    [2]=0
    [3]=0
    [4]=0
    [up]=0
    [down]=0
    [space]=0
)

# Populating rects array for the board.
create_rects() {
    local i=0
    local x=0
    local y=0
    for (( row = 0; row < $rows; row++ )); do
        for (( column = 0; column < $columns; column++ )); do
            (( y = row * size + 1 ))
            (( x = column * size + 1 ))
            sg_cmd "new rect $x $y $(( size - 2 )) $(( size - 2 ))"
            (( rects[i] = reply ))
            (( i++ ))
        done
    done
}

# Populate the board with randomized zeros or ones.
randomize_board() {
    board=()
    for (( i = 0; i < columns * rows; i++ )); do
        if (( RANDOM % 2 )); then
            (( board[i] = 1 ))
        else
            (( board[i] = 0 ))
        fi
    done
}

# Populate the board with only zeros.
clear_board() {
    board=()
    for (( i = 0; i < columns * rows; i++ )); do
        (( board[i] = 0 ))
    done
}

# Generate a new row based on the old board. Stored in /tmp.
update_row() {
    local row=$1
    local count=0
    local check_c=0
    local check_r=0
    local index=0
    local range_index=0
    local alive=0
    local row_result=()

    (( index = row * columns ))
    for (( c = 0; c < $columns; c++ )); do
        count=0
        for (( range_r = row - 1; range_r < row + 2; range_r++)); do
            (( check_r = (range_r + rows) % rows ))
            (( range_index = check_r * columns ))
            for (( range_c = c - 1; range_c < c + 2; range_c++)); do
                (( check_c = (range_c + columns) % columns ))
                if (( range_r != r || range_c != c )); then
                    (( board[range_index + check_c] )) && (( count++ ))
                fi
            done
        done

        (( alive = board[index] ))
        if (( count == 3 || (alive && count == 2) )); then
            (( row_result[c] = 1 ))
        else
            (( row_result[c] = 0 ))
        fi

        (( index++ ))
    done

    printf "%s " "${row_result[@]}" > "/tmp/row_$row"
}

# Draw the whole board row by row.
draw_board() {
    local index=0
    for (( r = 0; r < rows; r++ )); do
        draw_rects=()
        for (( c = 0; c < columns; c++ )); do
            (( board[$index] )) && draw_rects+=(${rects[$index]})
            (( index++ ))
        done
        (( ${#draw_rects[@]} )) && sg_cmd "draw render fillrect ${draw_rects[*]}"
    done
}

# Capturing pressed keys in an associated array.
get_keys_pressed() {
    sg_cmd "arr key pressed esc 1 2 3 4 up down space"
    pressed[esc]=${array[0]}
    pressed[1]=${array[1]}
    pressed[2]=${array[2]}
    pressed[3]=${array[3]}
    pressed[4]=${array[4]}
    pressed[up]=${array[5]}
    pressed[down]=${array[6]}
    pressed[space]=${array[7]}
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

# Start ShellGame.
sg_cmd "start sg"

# Set the size, title, icon, and resizable.
sg_cmd "set sg size $width $height"
sg_cmd "set sg title Conway's Game of Life"
sg_cmd "set sg icon examples/images/gol-icon.png"
sg_cmd "set sg resizable enable"
sg_cmd "set render scaling best"

# Generate rects and randomize the board.
create_rects
randomize_board

# Create a text object for displaying the FPS.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $text_size $bubble_size FPS: 0"
text_fps=$reply

# Set the FPS Text object anchord to the top left.
sg_cmd "set text pos 10 10 $text_fps"

# Create a text object for displaying Paused.
sg_cmd "new text bubble examples/fonts/freesansbold.ttf $text_size $bubble_size Paused"
text_paused=$reply

# Set the Paused Text object anchord to the top center.
sg_cmd "set text y 10 $text_paused"
sg_cmd "set text cx $(( width / 2 )) $text_paused"

# Set fullscreen enable/disable.
update_fullscreen

# Override the default 60fps
sg_cmd "set sg fps $fps"

# Main game loop.
while true; do
    # 'update sg' must be called at the top of the game loop.
    sg_cmd "update sg"

    # Get the state of the needed keys and update associated array.
    get_keys_pressed

    # Escape to quit.
    (( pressed[esc] )) && sg_quit 0

    # Toggle fullscreen.
    if (( pressed[1] )); then
        (( fullscreen = 1 - fullscreen ))
        update_fullscreen
    fi

    # Toggle show fps.
    (( pressed[2] )) && (( fps_display = 1 - fps_display ))

    # Clear or randomize the board.
    (( pressed[3] )) && clear_board
    (( pressed[4] )) && randomize_board

    # Increase or decrease the games fps.
    if (( pressed[up] )); then
        (( fps < 64 )) && (( fps = fps * 2 ))
        sg_cmd "set sg fps $fps"
    fi
    if (( pressed[down] )); then
        (( fps > 1 )) && (( fps = fps / 2 ))
        sg_cmd "set sg fps $fps"
    fi

    # Toggle pause.
    (( pressed[space] )) && (( paused = 1 - paused ))

    if (( ! paused )); then
        # Run the update for each row in the background grabbing it's pid.
        pids=()
        for (( r = 0; r < rows; r++ )); do
            update_row "$r" &
            pids+=($!)
        done

        # Reassemble board
        board=()
        for (( r = 0; r < rows; r++ )); do
            wait "${pids[$r]}"
            read -a row_data < "/tmp/row_$r"
            board+=("${row_data[@]}")
        done
    fi

    # If the mouse button is clicked toggle the that position on the board.
    sg_cmd "get mouse pressed 1"
    if (( reply )); then
        sg_cmd "arr mouse pos"
        (( index = ( array[1] / size ) * columns + ( array[0] / size ) ))
        (( board[index] = 1 - board[index] ))
    fi

    # Clear the game renderer.
    sg_cmd "set render clear"

    # Set the render draw color for the boxes.
    sg_cmd "set render color 128 128 128"

    # Draw the board by rows.
    draw_board

    # Set the render draw color back to black.
    sg_cmd "set render color 0 0 0"

    # Update the FPS text and display it if enabled.
    (( fps_display )) && update_fps

    # If game is paused display Paused.
    (( paused )) && sg_cmd "draw text $text_paused"

    # Present the renderer.
    sg_cmd "set render present"
done

# If exectution gets this far shut down.
sg_quit 0
