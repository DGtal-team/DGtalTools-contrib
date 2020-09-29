#include <iostream>
#include <fstream>

#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/PointListReader.h>

#include "CLI11.hpp"

using namespace DGtal;

/** 
@code:
Basic edit mesh.

Usage: ./xyzScale [OPTIONS] 1

Positionals:
  1 TEXT:FILE REQUIRED                  input file name of the xyz point set.

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         input file name of the xyz point set.
  -o,--outputFile TEXT=output.xyz       output file name of the resulting xyz point set.
  --scale FLOAT=1                       change the scale factor
*/
  
int
main(int argc,char **argv)

{
  // parse command line CLI-------------------------------------------------------
  CLI::App app;
  app.description("Basic edit mesh.\n");
  std::string inputName;
  std::string outputName {"output.xyz"};
  double scale {1.0};
  
  app.add_option("--input,-i,1", inputName, "input file name of the xyz point set.")->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o", outputName, "output file name of the resulting xyz point set.", true);
  app.add_option("--scale", scale, "change the scale factor", true);

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  std::vector<Z3i::RealPoint> vPt = PointListReader<Z3i::RealPoint>::getPointsFromFile(inputName);
  std::ofstream res;
  res.open(outputName);
  for(auto &p: vPt)
    {
      res << p[0]*scale << " "<< p[1] *scale << " "<< p[2] *scale << std::endl;
    }
  res.close();

  return 0;
}
