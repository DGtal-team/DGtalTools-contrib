#include <iostream>
#include <fstream>

#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/PointListReader.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


using namespace DGtal;
namespace po = boost::program_options;



int
main(int argc,char **argv)

{
  typedef typename Z3i::RealPoint TPoint;
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "input file name of the xyz point set.")
    ("output,o", po::value<std::string>()->default_value("output.xyz"), "output file name of the resulting xyz point set.")
    ("scale,s", po::value< double >()->default_value(1.0), "change the scale factor" );


  bool parseOK=true;
  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, general_opt), vm);
    }
  catch(const std::exception& ex)
    {
      trace.info()<< "Error checking program options: "<< ex.what()<< std::endl;
      parseOK=false;
    }
  po::notify(vm);
  if(vm.count("help")||argc<=1|| !parseOK )
    {
      trace.info()<< "Basic edit mesh " <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
      return 0;
    }
  
  
  std::string inputName = vm["input"].as<std::string>();
  std::string outputName = vm["output"].as<std::string>();
  
  double scale = vm["scale"].as<double>();
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
