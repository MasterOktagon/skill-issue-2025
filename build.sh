
# compile arduino code
#arduino-cli compile ./esp32 -b esp32:esp32:esp32 -u /dev/ttyUSB3 -t

# build rasberry pi code
echo "building Rasberry PI Code..."
mkdir raspi-cpp/build
cd raspi-cpp
cmake .
if command make; then
    echo "running tests..."
    if command ./bin/SkillIssue25-Raspi; then
        echo "tests passed"
        cmake --build . --config release
        make
    else
        echo "Not all tests were succesful"
    fi
fi
cd ..



