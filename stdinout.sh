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
SHUTDOWN_SERVER=1

# Make sure the server is compiled.
if [ ! -e "$SERVER" ]; then
    echo "Error: $SERVER is missing. Try running 'make release'."
    exit 1
fi

# Start the server in a coprocess using --stdin mode.
coproc SERVER_PROC { ./"$SERVER" --stdinout $$; }
if [ "$SG_SHELL" -eq 1 ]; then
    # Zsh assigns the coprocess file descriptor to $SERVER_PROC
    # Write to server's stdin
    exec 3>&"$SERVER_PROC"
    # Read from server's stdout
    exec 4<&SERVER_PROC
else
    # Bash version.
    # Write to server's stdin
    exec 3>&"${SERVER_PROC[1]}"
    # Read from server's stdout
    exec 4<&"${SERVER_PROC[0]}"
fi

sg_quit() {
    # Remove the EXIT trap to prevent recursive cleanup.
    trap - EXIT

    if [ "$SHUTDOWN_SERVER" -ne 0 ]; then
        sg_cmd "quit sg"
    fi

    # Close file descriptors.
    exec 3>&- 2>/dev/null
    exec 4<&- 2>/dev/null

    exit "$1"
}

# If the server initiated shutdown don't shut down recursively.
trap 'SHUTDOWN_SERVER=0; sg_quit 1' SIGUSR2
trap 'SHUTDOWN_SERVER=0; sg_quit 0' SIGUSR1
trap 'sg_quit 0' SIGINT SIGTERM SIGQUIT EXIT

sg_cmd() {
    # Send argument 1 to stdin.
    echo "$1" >&3 || sg_quit 1

    # Read named pipe to global variable reply or array (Blocking).
    case "$1" in
        get*|new*)
            read -r reply <&4
            # read -r -u 4 reply
            ;;
        arr*|free*)
            read -r line <&4
            # read -r -u 4 line
            array=($line)
            ;;
    esac
}
