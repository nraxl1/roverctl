$REMOTE_USER = "student"
$REMOTE_HOST = "10.43.0.1"
$REMOTE_PATH = "/home/student"
$DRIVE_LETTER = "X:"

function Show-Help {
    Write-Host "Usage: .\sshhelper.ps1 <mount|unmount|ssh|help>"
    Write-Host ""
    Write-Host "mount   - Mounts the remote home directory to drive X:"
    Write-Host "unmount - Unmounts drive X:"
    Write-Host "ssh     - Opens an SSH session to the remote device"
    Write-Host "help    - Shows this help message"
    Write-Host ""
    Write-Host "Note: Requires SSHFS-Win and WinFsp. The script will prompt to install if missing. This script assumes that the remote user is '$REMOTE_USER' and the remote host is '$REMOTE_HOST'."
    Write-Host "Drive letter is set to '$DRIVE_LETTER'."
    Write-Host "You can change these variables at the top of the script."
    Write-Host "This script is wildly untested."
    Write-Host "Undefined behaviour may occur if the volume is already mounted, if the drive letter is in use or if the volume is not unmounted properly."
}

function Check-Dependencies {
    if (-not (Get-Command "net" -ErrorAction SilentlyContinue)) {
        Write-Host "'net' command not found. This script requires Windows networking tools."
        exit 1
    }
    if (-not (Test-Path "HKLM:\SOFTWARE\WinFsp")) {
        Write-Host "WinFsp/SSHFS-Win not detected."
        if (Get-Command "winget" -ErrorAction SilentlyContinue) {
            $response = Read-Host "Do you want to install SSHFS-Win now using winget? [Y/n]"
            if ($response -eq "" -or $response -match "^[Yy]") {
                winget install SSHFS-Win.SSHFS-Win
                Write-Host "Please re-run this script after installation completes."
                exit 0
            } else {
                Write-Host "Please install SSHFS-Win manually from https://github.com/winfsp/sshfs-win"
                exit 1
            }
        } else {
            Write-Host "winget not found. Please install SSHFS-Win manually from https://github.com/winfsp/sshfs-win"
            exit 1
        }
    }
}

param([string]$action)

switch ($action) {
    "mount" {
        Check-Dependencies
        $remote = "\\sshfs\$REMOTE_USER@$REMOTE_HOST!$($REMOTE_PATH -replace '/', '\')"
        net use $DRIVE_LETTER $remote
    }
    "unmount" {
        net use $DRIVE_LETTER /delete
    }
    "ssh" {
        ssh "$REMOTE_USER@$REMOTE_HOST"
    }
    "help" | "--help" | "-H" | "-h" | "/?" {
        Show-Help
    }
    default {
        Show-Help
    }
}