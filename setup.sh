#!/bin/sh

Version=1.0.0

if [ -d "ThirdParty/CNTK/lib" ]; then
    echo "ThirParty libraries is ready to downloaded"
else
    if [ -f "ThirdParty/libs.zip" ]; then
        echo "Okay you have zip"
    else
        echo "Download prebuilt ThirdParty libraries for DeepLibrary"
        (cd ThirdParty; curl -O -L https://github.com/keicoon/DeepLibraryUE4Components/releases/download/$Version/libs.zip)
    fi

    (cd ThirdParty; unzip libs.zip;)
fi
