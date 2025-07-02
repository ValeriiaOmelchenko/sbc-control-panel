#!/bin/bash

set -e

SERVICE_NAME="sbc_cp.service"
TARGET_PATH="/etc/systemd/system/$SERVICE_NAME"

echo "Copying the systemd service file..."
sudo cp "$SERVICE_NAME" "$TARGET_PATH"

echo "Reloading systemd daemon..."
sudo systemctl daemon-reload

echo "Enabling the service to start on boot..."
sudo systemctl enable "$SERVICE_NAME"

echo "Starting the service..."
sudo systemctl start "$SERVICE_NAME"

echo "Service $SERVICE_NAME has been configured and started successfully."
