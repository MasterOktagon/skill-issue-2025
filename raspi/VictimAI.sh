#! /usr/bin/bash
echo "script startet"
sudo pigpiod
sudo . ~/skill-issue-2025/venv/bin/activate
sudo python3 main.py
