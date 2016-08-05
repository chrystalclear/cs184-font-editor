#include "CGL/CGL.h"
#include "CGL/viewer.h"

#include "fonttester.h"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace CGL;


int main( int argc, char** argv ) {

  AppConfig config; int opt;
  while ( (opt = getopt(argc, argv, "t:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 't':
        config.text_input = string(optarg);
        break;
    default:
        return 1;
    }
  }

  // print usage if no argument given
  if (optind >= argc) {
    std::cout << "Where's my .ttf file???";
    return 1;
  }

  char* fontFilePath = argv[optind];
  cout << "Input font file: " << fontFilePath << "\n";
  config.directory_filename = fontFilePath;


  // create application
  FontTester *app  = new FontTester(config);

  // create viewer
  Viewer viewer = Viewer();

  // set renderer
  viewer.set_renderer(app);

  // init viewer
  viewer.init();

  // start viewer
  viewer.start();

  return 0;

}
