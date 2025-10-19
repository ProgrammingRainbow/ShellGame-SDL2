#!/bin/env bash

SERVER="server/sg-server"
SHM_DATA="/dev/shm/sg_shared_data"
SHM_LOCK="/dev/shm/sg_shared_lock"
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e $SERVER ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
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
if pidof "$SERVER_NAME" >/dev/null; then
    echo "[CLIENT] Killing existing server..."
    killall "$SERVER_NAME"
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

    (( SHUTDOWN_SERVER )) && sg_cmd "quit sg"

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
    while true; do
        # Check lock status.
        exec {FD_LOCK}<>"$SHM_LOCK"
        read -r -n1 -u $FD_LOCK status
        exec {FD_LOCK}<&-

        # If status is not free loop.
        [[ "$status" != $'\x00' ]] && continue

        # Send argument to the shared data.
        exec {FD_DATA}<>"$SHM_DATA"
        printf "%s\x00" "$1" >&$FD_DATA
        exec {FD_DATA}>&-

        # Set shared lock.
        exec {FD_LOCK}<>"$SHM_LOCK"
        printf "\x01" >&$FD_LOCK
        exec {FD_LOCK}>&-
            
        if [[ $1 == get* || $1 == new* || $1 == free* ]]; then
            while true; do
                # Check lock status again.
                exec {FD_LOCK}<>"$SHM_LOCK"
                read -r -n1 -u $FD_LOCK status
                exec {FD_LOCK}<&-

                if [[ "$status" == $'\x02' ]]; then
                    # Read from shared data to global reply.
                    exec {FD_DATA}<>"$SHM_DATA"
                    read -r -u $FD_DATA reply
                    exec {FD_DATA}<&-

                    # Unset shared lock.
                    exec {FD_LOCK}<>"$SHM_LOCK"
                    printf "\x00" >&$FD_LOCK
                    exec {FD_LOCK}>&-

                    break
                fi
            done
        elif [[ $1 == arr* ]]; then
            while true; do
                # Check lock status again.
                exec {FD_LOCK}<>"$SHM_LOCK"
                read -r -n1 -u $FD_LOCK status
                exec {FD_LOCK}<&-

                if [[ "$status" == $'\x02' ]]; then
                    # Read from shared data to global reply.
                    exec {FD_DATA}<>"$SHM_DATA"
                    read -u $FD_DATA -a array
                    exec {FD_DATA}<&-

                    # Unset shared lock.
                    exec {FD_LOCK}<>"$SHM_LOCK"
                    printf "\x00" >&$FD_LOCK
                    exec {FD_LOCK}>&-

                    break
                fi
            done
        fi
        break
    done
}
