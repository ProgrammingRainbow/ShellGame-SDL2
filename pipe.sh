#!/bin/env bash

SERVER="server/sg-server"
PIPE_TO_SERVER="/tmp/sg_pipe_to_server"
PIPE_FROM_SERVER="/tmp/sg_pipe_from_server"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e $SERVER ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
    exit 1
fi

# Create named pipes if needed
[[ -p "$PIPE_TO_SERVER" ]] || mkfifo "$PIPE_TO_SERVER"
[[ -p "$PIPE_FROM_SERVER" ]] || mkfifo "$PIPE_FROM_SERVER"

# Start the server in pipe mode with the script PID.
./$SERVER --pipe $$ &
SERVER_PID=$!

# Make sure the server started.
if ! kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[CLIENT] Server failed to start. Exiting."
    exit 1
fi

# Open both pipes once and keep them open.
exec {TO_SERVER}> "$PIPE_TO_SERVER"
exec {FROM_SERVER}< "$PIPE_FROM_SERVER"

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    (( SHUTDOWN_SERVER )) && sg_cmd "quit sg"

    # Close file descriptors used for the pipes.
    exec {TO_SERVER}>&- 2>/dev/null
    exec {FROM_SERVER}<&- 2>/dev/null

    # Remove named pipes from the filesystem.
    rm "$PIPE_TO_SERVER"
    rm "$PIPE_FROM_SERVER"

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    # Send argument 1 to named pipe in.
    echo "$1" >&$TO_SERVER || sg_quit 1

    # Read named pipe to global variable reply or array (Blocking).
    if [[ $1 == get* || $1 == new* ]]; then
        read -r reply <&$FROM_SERVER
    elif [[ $1 == arr* || $1 == free* ]]; then
        read -a array <&$FROM_SERVER
    fi
}
