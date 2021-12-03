// GlyphEngineTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Engine.h"

int main()
{
    //std::cout << "Hello World!\n";
	GlyphEngine::Engine* glyphEngine = new GlyphEngine::Engine();
	glyphEngine->initiate();


}
