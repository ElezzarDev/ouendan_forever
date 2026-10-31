#!/bin/bash

# 1. SET THE PATH TO YOUR AZAHAR EMULATOR EXECUTABLE (Windows Path Style)
AZAHAR_PATH="C:\Program Files\Azahar\azahar.exe"

# 2. FORCE BASH TO MOVE INTO THE SCRIPT'S OWN DIRECTORY
# This ensures it always finds your Makefile regardless of how MSYS2 is opened!
cd "$(dirname "$0")"

echo "[1/3] Cleaning old build assets..."
make clean

echo "[2/3] Compiling via devkitARM..."
make

# Look for the generated 3DS homebrew file
TARGET_3DSX=$(ls *.3dsx 2>/dev/null | head -n 1)

if [ -f "$TARGET_3DSX" ]; then
    echo ""
    echo "=========================================="
    echo " [✓] BUILD SUCCESSFUL!"
    echo "=========================================="
    
    # Force the script to wait for user input
    read -p "Press [Enter] to launch Azahar and exit this window..."
    
    # MSYS2 trick: convert the current folder path to a Windows-style path
    WIN_PWD=$(cygpath -w "$(pwd)")
    WIN_TARGET="${WIN_PWD}\\${TARGET_3DSX}"
    
    # Clean up the emulator executable path formatting for bash execution
    CLEAN_AZAHAR=$(echo "$AZAHAR_PATH" | tr '\\' '/')
    
    # Explicitly wrap both paths inside clean quotes to handle "Program Files" spaces properly
    cygstart "$CLEAN_AZAHAR" \""$WIN_TARGET"\" 2>/dev/null || "$CLEAN_AZAHAR" "$WIN_TARGET" &
    
    exit 0
else
    echo ""
    echo "=========================================="
    echo " [X] ERROR: Build failed. Check compiler logs."
    echo "=========================================="
    read -p "Press [Enter] to exit..."
    exit 1
fi
