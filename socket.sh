#!/bin/env bash

SERVER="server/sg-server"
SOCKET_PATH="/tmp/sg_socket"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e $SERVER ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
    exit 1
fi

# Start the server in socket mode with the script PID.
./$SERVER --socket $$ &
DAEMON_PID=$!

# Make sure the server started and wait for the socket.
while [[ ! -e "$SOCKET_PATH" ]]; do
    if ! kill -0 "$DAEMON_PID" 2>/dev/null; then
        echo "[CLIENT] Server failed to start. Exiting."
        exit 1
    fi
    sleep 0.1
done

# Open bidirectional socket connection to server using socat coprocess.
coproc SOCKET { socat - UNIX-CONNECT:"$SOCKET_PATH"; }
SOCKET_IN=${SOCKET[0]}
SOCKET_OUT=${SOCKET[1]}

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    (( SHUTDOWN_SERVER )) && sg_cmd "quit sg"

    # Close the socket file descriptors.
    exec {SOCKET_OUT}>&- 2>/dev/null
    exec {SOCKET_IN}<&- 2>/dev/null

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    # Send argument 1 to socket.
    echo "$1" >&"$SOCKET_OUT" || sg_quit 1

    # Read socket to global variable reply or array (Blocking).
    if [[ $1 == get* || $1 == new* || $1 == free* ]]; then
        read -r reply <&"$SOCKET_IN"
    elif [[ $1 == arr* ]]; then
        read -a array <&"$SOCKET_IN"
    fi
}
