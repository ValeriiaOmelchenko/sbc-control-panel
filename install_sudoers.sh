#!/bin/bash

set -e

USERNAME=pi
SUDOERS_FILE="/etc/sudoers.d/${USERNAME}_reboot"

echo "Creating sudoers rule for '$USERNAME' to run reboot without password..."

echo "${USERNAME} ALL=(ALL) NOPASSWD: /sbin/reboot" | sudo tee "$SUDOERS_FILE" > /dev/null

sudo chmod 0440 "$SUDOERS_FILE"

echo "Sudoers rule created at $SUDOERS_FILE"
