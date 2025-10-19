#!/bin/env bash

SERVER="server/sg-server"
CLIENT="client/sg-client"
SHM_DATA="/dev/shm/sg_shared_data"
SHM_LOCK="/dev/shm/sg_shared_lock"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e $SERVER ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
    exit 1
fi
# Make sure the client is compiled.
if [ ! -e $CLIENT ]; then
    echo "Error: $CLIENT is missing. Try running 'make release'."
    exit 1
fi

# Ensure shared memory file exists
[[ -e "$SHM_DATA" ]] || touch "$SHM_DATA"
[[ -e "$SHM_LOCK" ]] || touch "$SHM_LOCK"

# Make client wait for server to clear the lock.
exec {FD_LOCK}<>"$SHM_LOCK"
printf "\x01" >&$FD_LOCK
exec {FD_LOCK}>&-

# If server is already running kill it.
if pidof "$SERVER" >/dev/null; then
    echo "[CLIENT] Killing existing server..."
    killall "$SERVER"
    sleep 0.1
fi

# Start the server in shared memory mode with the script PID.
./$SERVER --shared $$ &
SERVER_PID=$!

# Make sure the server started.
if ! kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[CLIENT] Server failed to start. Exiting."
    exit 1
fi

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    (( SHUTDOWN_SERVER )) && sg_cmd "set sdl quit"

    # Remove shared memory from the filesystem.
    rm "$SHM_DATA" 2>/dev/null
    rm "$SHM_LOCK" 2>/dev/null

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    if [[ $1 == get* || $1 == new* || $1 == free* ]]; then
        # reply=$("$CLIENT" $1)
        read -r reply < <(./$CLIENT "$1")
    elif [[ $1 == arr* ]]; then
        read -a array < <(./$CLIENT "$1")
    else
        ./$CLIENT "$1"
    fi
}
