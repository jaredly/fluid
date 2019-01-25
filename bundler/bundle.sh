
rm -rf qmoji.app
./macapp.sh qmoji icon.png
DEST=qmoji.app/Contents/MacOS
cp ../lib/bs/native/qmojiprod.native $DEST/qmoji
cp -r ../assets   qmoji.app/Contents/MacOS/
zip -r qmoji.zip qmoji.app