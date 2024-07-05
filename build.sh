
# compile arduino code
if $1 != "arduino"; then
    ./bin/arduino-cli compile ./esp32 -b esp32:esp32:esp32 #-u /dev/ttyUSB3 -t
fi

# build rasberry pi code
echo "building Rasberry PI Code..."
mkdir raspi-cpp/build
cd raspi-cpp
cmake .
if command make; then
    echo "...complete"
fi
cd ..



