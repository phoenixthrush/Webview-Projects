# MacOS
rm -rf main.app Resources Info.plist

python3 -m nuitka --include-data-files=site/index.html=index.html --assume-yes-for-downloads --remove-output --macos-create-app-bundle --macos-app-icon=site/assets/icon.png --macos-signed-app-name=com.Phoenixthrush.Keygen --macos-app-name=Keygen --company-name=phoenixthrush main.py
# --linux-icon=site/assets/icon.ico

rm -rf Info.plist Resources _CodeSignature