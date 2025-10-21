#!/usr/bin/env bash

SG_SHELL=0
if [ -n "$BASH_VERSION" ]; then
    echo "Running in Bash"
elif [ -n "$ZSH_VERSION" ]; then
    echo "Running in Zsh"
    echo "Setting zero array indexing."
    setopt KSH_ARRAYS
    setopt SH_WORD_SPLIT
    SG_SHELL=1
else
    echo "Please run in Bash or Zsh."
    exit 1
fi

SERVER="sg-server/sg-server"
SOCKET_PATH="/tmp/sg_socket"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e "$SERVER" ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
    exit 1
fi

# Start the server in socket mode with the script PID.
./"$SERVER" --socket $$ &
DAEMON_PID=$!

# Make sure the server started and wait for the socket.
while [ ! -e "$SOCKET_PATH" ]; do
    if ! kill -0 "$DAEMON_PID" 2>/dev/null; then
        echo "[CLIENT] Server failed to start. Exiting."
        exit 1
    fi
    sleep 0.1
done

# Open bidirectional socket connection to server using socat coprocess.
if [ "$SG_SHELL" -eq 1 ]; then
    # Zsh assigns the coprocess file descriptor to $COPROC
    coproc { socat - UNIX-CONNECT:"$SOCKET_PATH"; }
    # Write to socket.
    exec 3>&p
    # Read from socket.
    exec 4<&p
else
    # Bash version.
    eval 'coproc SOCKET { socat - UNIX-CONNECT:"$SOCKET_PATH"; }'
    # Write to socket.
    exec 3>&"${SOCKET[1]}"
    # Read from socket.
    exec 4<&"${SOCKET[0]}" 
fi

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    if [ "$SHUTDOWN_SERVER" -ne 0 ]; then
        sg_cmd "quit sg"
    fi

    # Close the socket file descriptors.
    exec 3>&- 2>/dev/null
    exec 4<&- 2>/dev/null

    # Remove named socket from the filesystem.
    rm "$SOCKET_PATH" 2>/dev/null

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    # Send argument 1 to socket.
    echo "$1" >&3 || sg_quit 1

    # Read socket to global variable reply or array (Blocking).
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
