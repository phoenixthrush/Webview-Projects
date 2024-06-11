#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
  # MacOS
  python3 -m nuitka --include-data-files=site/index.html=index.html --assume-yes-for-downloads --remove-output --macos-create-app-bundle --macos-app-icon=site/assets/icon.png --macos-signed-app-name=com.Phoenixthrush.Keygen main.py
else
  # Linux
  python3 -m nuitka --include-data-files=site/index.html=index.html --assume-yes-for-downloads --remove-output --onefile --linux-icon=site/assets/icon.png main.py
fi