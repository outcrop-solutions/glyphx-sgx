// KMLBoundingBox.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include "stdafx.h"
#endif //WIN32

#include "BoundingBoxCalculator.h"
#include <iostream>
#include <exception>
//#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
	if (argc < 2) {

		std::cout << "File name is missing" << std::endl;
        return 1;
	}
    
    /*boost::filesystem::path inputFile(argv[1]);
    if (!boost::filesystem::exists(inputFile)) {
        std::cout << "Input file does not exist" << std::endl;
        return 2;
    }

    if (!boost::filesystem::is_regular_file(inputFile)) {
        std::cout << "Input file is not a file" << std::endl;
        return 2;
    }

    if (inputFile.extension() != ".kml") {
        std::cout << "Only KML files are supported" << std::endl;
        return 2;
    }*/

	try {
		BoundingBoxCalculator calc;
        std::cout << calc.ReadAndGetBoundingBox(argv[1]) << std::endl;
        //std::cout << calc.ReadAndGetBoundingBox(boost::filesystem::absolute(inputFile).generic_string()) << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
        return 3;
	}

	return 0;
}

