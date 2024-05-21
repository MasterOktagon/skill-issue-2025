# this script is tested for UBUNTU LINUX / MINT LINUX
# other OSses might need changes


#update all packages
sudo apt-get upgrade

# check for python install
vpython=0
command -v python3 >/dev/null 2>&1 && vpython=1

if [$vpython>0]; then
    echo -n "python3 has to be installed. Do you want to install it now [Y/n]? "
    read installnow
    if [$installnow != "n" && $installnow != "N"]; then
        sudo apt install python3 python3-pip
    else
        exit
    fi
fi

# install the arduino CLI
sudo curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
# install the ESP32 tool
pip install esptool

#install c++ opencv and make/cmake
sudo apt install "libopencv-core4.5d" make cmake


