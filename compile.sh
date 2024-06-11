# MacOS
rm -rf main.app Resources Info.plist

python3 -m nuitka --onefile --include-data-files=site/index.html=index.html --assume-yes-for-downloads --remove-output --macos-create-app-bundle --macos-signed-app-name=com.Phoenixthrush.Keygen --company-name=phoenixthrush main.py
# --linux-icon=site/assets/icon.ico

rm -rf Info.plist Resources _CodeSignature