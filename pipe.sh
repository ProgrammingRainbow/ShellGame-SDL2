#!/usr/bin/env bash

if [ -n "$BASH_VERSION" ]; then
    echo "Running in Bash"
elif [ -n "$ZSH_VERSION" ]; then
    echo "Running in Zsh"
    echo "Setting zero array indexing."
    setopt KSH_ARRAYS
    setopt SH_WORD_SPLIT
else
    echo "Please run in Bash or Zsh."
    exit 1
fi

SERVER="sg-server/sg-server"
PIPE_TO_SERVER="/tmp/sg_pipe_to_server"
PIPE_FROM_SERVER="/tmp/sg_pipe_from_server"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e "$SERVER" ]; then
    echo "Error: $SERVER is missing. Try running 'make -C sg-server release'."
    exit 1
fi

# Create named pipes if needed
[ -p "$PIPE_TO_SERVER" ] || mkfifo "$PIPE_TO_SERVER"
[ -p "$PIPE_FROM_SERVER" ] || mkfifo "$PIPE_FROM_SERVER"

# Start the server in pipe mode with the script PID.
./"$SERVER" --pipe $$ &
SERVER_PID=$!

# Make sure the server started.
if ! kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[CLIENT] Server failed to start. Exiting."
    exit 1
fi

# Open both pipes once and keep them open.
exec 3> "$PIPE_TO_SERVER"
exec 4< "$PIPE_FROM_SERVER"

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    if [ "$SHUTDOWN_SERVER" -ne 0 ]; then
        sg_cmd "quit sg"
    fi

    # Close file descriptors used for the pipes.
    exec 3>&- 2>/dev/null
    exec 4<&- 2>/dev/null

    # Remove named pipes from the filesystem.
    rm "$PIPE_TO_SERVER" 2>/dev/null
    rm "$PIPE_FROM_SERVER" 2>/dev/null

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    # Send argument 1 to named pipe in.
    echo "$1" >&3 || sg_quit 1

    # Read named pipe to global variable reply or array (Blocking).
    case "$1" in
        get*|new*)
            read -r -u 4 reply
            ;;
        arr*|free*)
            read -r -u 4 line
            array=($line)
            ;;
    esac
}
