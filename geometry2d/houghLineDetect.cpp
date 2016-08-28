
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <iostream>

using namespace cv;
using namespace std;
namespace po = boost::program_options;




int main(int argc, char** argv)
{
  po::options_description general_opt("Specific allowed options (for Qt options, see Qt official site) are");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "the input image file." )
    ("output,o", po::value<std::string>(), "the output file containing the resulting lines segments (one segment per lines)." )
    ("rho,r", po::value<double>()->default_value(1.0), "The resolution of the parameter r in pixels. We use 1 pixel." )
    ("theta,t", po::value<double>()->default_value(CV_PI/180.0), "The resolution of the parameter \theta in radians." )
    ("threshold,T", po::value<double>()->default_value(100), "The minimum number of intersections to “detect” a line." )
    ("minLinLength,m", po::value<double>()->default_value(10), " The minimum number of points that can form a line. Lines with less than this number of points are disregarded (use only with --useProbabilist option)." )
    ("maxLineGap,g", po::value<double>()->default_value(3), "The maximum gap between two points to be considered in the same line (use only with --useProbabilist option) " )
    ("useProbabilist,P","use improved probabilist algorithm" );
  
  bool parseOK=true;

  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  }catch(const std::exception& ex){
    parseOK=false;
    cout<< "Error checking program options: "<< ex.what()<< endl;
  }
  po::notify(vm);
  if( !parseOK || vm.count("help")||argc<=1)
    {
      std::cout << "Usage: " << argv[0] << " [input]\n"
                << "Apply OpenCV Hough Transform for line finding."
                << general_opt << "\n";
      return 0;
    }
  const char* filename;
  if(vm.count("input")){
    filename = vm["input"].as<std::string>().c_str();
  }


  Mat src = imread(filename, 0);
  if(src.empty())
    {
      cout << "can not open " << filename << endl;
      return -1;
    }



  // get  hough params:
  double rho = vm["rho"].as<double>();
  double theta = vm["theta"].as<double>();
  double threshold = vm["threshold"].as<double>();
  unsigned int minLength = vm["minLinLength"].as<double>();
  unsigned int maxLineGap = vm["maxLineGap"].as<double>();

  
 
  Mat dst, cdst;
  Canny(src, dst, 50, 200, 3);
  cvtColor(dst, cdst, CV_GRAY2BGR);
  bool outLines = vm.count("output"); 
  ofstream outStream;
  if(outLines)
    {
      std::string outputFileName = vm["output"].as<std::string>();
      outStream.open(outputFileName.c_str(), ofstream::binary);
    }
  
  if(!vm.count("useProbabilist")){
    vector<Vec2f> lines;
    HoughLines(dst, lines, rho, theta, threshold, 0, 0 );
    
    for( size_t i = 0; i < lines.size(); i++ )
      {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
        if(outLines){
          outStream << pt1.x << " " << pt1.y << " " << pt2.x << " " << pt2.y << std::endl;
        }
      }
  }
  else
    {
      vector<Vec4i> lines;
      HoughLinesP(dst, lines, rho, theta, threshold, minLength, maxLineGap );
      for( size_t i = 0; i < lines.size(); i++ )
        {
          Vec4i l = lines[i];
          line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
          if(outLines){
            outStream << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << std::endl;
          }
        
        }
    }
    
  imshow("source", src);
  imshow("detected lines", cdst);

  waitKey();

  return 0;
}

