#!/bin/bash

echo Building converthash library...
c++ -dynamiclib -o libconverthash.dylib -I../../jdk1.7.0_79/include/ -I../../jdk1.7.0_79/include/darwin/ -I../../../3rdParty/include/boost_1_60_0/ HashInput/main.cpp
