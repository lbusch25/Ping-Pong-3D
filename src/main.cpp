//
//  main.cpp
//  
//
//  Created by Bret Jackson on 1/29/17.
//
//

#include <stdio.h>

#include "App.h"

using namespace basicgraphics;

int main(int argc, char** argv)
{
	// Initialize freeimage library
	//FreeImage_Initialise();

	App *app = new App(argc, argv, "Ping Pong 3D", 1280, 720);
	app->run();
	delete app;

	//FreeImage_DeInitialise();

	return 0;

}
