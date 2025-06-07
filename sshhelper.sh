#!/bin/bash

REMOTE_USER="student"
REMOTE_HOST="10.43.0.1"
REMOTE_PATH="/home/student"
LOCAL_MOUNT="/mnt/pynqhome"

show_help() {
    echo "Usage: $0 {mount|unmount|ssh|help}"
    echo
    echo "mount   - Mounts the remote home directory to $LOCAL_MOUNT"
    echo "unmount - Unmounts $LOCAL_MOUNT"
    echo "ssh     - Opens an SSH session to the remote device"
    echo "help    - Shows this help message"
    echo
    echo "Note: Requires sshfs. Please install sshfs using your distro's package manager if missing."
    echo "This script assumes that the remote user is '$REMOTE_USER' and the remote host is '$REMOTE_HOST'."
    echo "The remote path is set to '$REMOTE_PATH' and the local mount point is set to '$LOCAL_MOUNT' by default."
    echo "You can change the variables at the top of the script."
    echo "Undefined behaviour may occur if the mountpoint is already mounted or not unmounted properly."
}

check_dependencies() {
    if ! command -v sshfs >/dev/null 2>&1; then
        echo "Error: sshfs is not installed."
        echo "Please install sshfs using your distribution's package manager."
        echo "eg. sudo apt install sshfs, sudo dnf install sshfs, or sudo pacman -S sshfs"
        exit 1
    fi
}

check_fusermount() {
    if command -v fusermount >/dev/null 2>&1; then
        FUSERMOUNT_CMD="fusermount"
    elif command -v fusermount3 >/dev/null 2>&1; then
        FUSERMOUNT_CMD="fusermount3"
    else
        echo "Error: fusermount (or fusermount3) is not installed."
        echo "Please install FUSE utilities using your distribution's package manager."
        echo "eg. sudo apt install fuse, sudo dnf install fuse, or sudo pacman -S fuse3"
        exit 1
    fi
}

case "$1" in
    mount)
        check_dependencies
        sudo mkdir -p "$LOCAL_MOUNT"
        sudo chown "$USER":"$USER" "$LOCAL_MOUNT"
        sshfs "$REMOTE_USER@$REMOTE_HOST:$REMOTE_PATH" "$LOCAL_MOUNT"
        ;;
    unmount)
        echo "Attempting to unmount volume"
        check_fusermount
        if ! $FUSERMOUNT_CMD -u "$LOCAL_MOUNT"; then
            ret=$?
            echo "Warning: Failed to unmount volume: $FUSERMOUNT_CMD -u returned with $ret. See the error message above."
            exit 1
        fi
        echo "Unmounted volume successfully."
        echo "Attempting to remove the mountpoint ($LOCAL_MOUNT)..."
        if ! rmdir "$LOCAL_MOUNT"; then
            echo "Warning: Could not remove mountpoint ($LOCAL_MOUNT). It may not be empty or may not exist."
            exit 1
        fi
        echo "Removed mountpoint ($LOCAL_MOUNT)"
        ;;
    ssh)
        ssh "$REMOTE_USER@$REMOTE_HOST"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        show_help
        ;;
esac