# Netvisix
Netvisix listens on your local network interface and visualizes the network packet flow between hosts.
Also packet statistics per host are available.  

This project started in 2014 and was initially hosted on bitbucket.  
Feature requests and bug reports are welcome.

Netvisix is using the following libraries:  
- libtins   
- libpcap/WinPcap (used by libtins)   
- Qt5

## Installation
Build it by yourself or download the ![latest](https://github.com/bewue/Netvisix/releases/latest) binary package.

## Linux Build (Ubuntu)
    sudo apt install qt5-qmake qtbase5-dev build-essential libpcap-dev

    git clone https://github.com/bewue/Netvisix.git

    cd Netvisix
    mkdir Build
    cd Build

    qmake ../Netvisix/Netvisix.pro
    make

## Screenshot
![](https://github.com/bewue/Misc/blob/main/Pictures/Netvisix.png)
