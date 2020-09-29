/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file houghLineDetect.cpp
 * @ingroup geometry2d
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * @date 2016/08/29
 *
 * Source file of the tool houghLineDetect
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"

#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "CLI11.hpp"


#include <iostream>

using namespace cv;
using namespace std;
using namespace DGtal;



/**
 @page houghLineDetect houghLineDetect
 
 @brief  Apply the hough transform from the OpenCV implementation (see http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html).

 @b Usage:   houghLineDetect [input]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  the input image file.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         the input image file.
   -o,--output TEXT                      the output file containing the resulting lines segments (one segment per lines).
   -r,--rho FLOAT=1                      The resolution of the parameter r in pixels. We use 1 pixel.
   -t,--theta FLOAT=0.0174533            The resolution of the parameter   heta in radians.
   -T,--threshold FLOAT=100              The minimum number of intersections to “detect” a line.
   -m,--minLinLength UINT=10              The minimum number of points that can form a line. Lines with less than this number of points are disregarded (use only with --useProbabilist option).
   -g,--maxLineGap FLOAT=3               The maximum gap between two points to be considered in the same line (use only with --useProbabilist option)
   -P,--useProbabilist                   use improved probabilist algorithm
   
 @endcode

 @b Example: 

 @code
   houghLineDetect -i  $DGtal/examples/samples/church.pgm -T 100 -P
 @endcode


 @see
 @ref houghLineDetect.cpp

 */


int main( int argc, char** argv )
{
  // parse command line -------------------------------------------------------
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputFileName;
  std::string outputFileName;
  double rho {1.0};
  double theta {CV_PI/180.0};
  double threshold {100.0};
  unsigned int minLength {10};
  double maxLineGap {3.0};
  bool useProbabilist {false};
  
  
  app.description("Apply the Hough transform from the OpenCV implementation (see http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html).\nTypical use example:\n \t houghLineDetect -i  $DGtal/examples/samples/church.pgm -T 100 -P \n");
  
  app.add_option("-i,--input,1", inputFileName, "the input image file." )
  ->required()
  ->check(CLI::ExistingFile);
  app.add_option("--output,-o", outputFileName,  "the output file containing the resulting lines segments (one segment per lines).");
  app.add_option("--rho,-r", rho,  "The resolution of the parameter r in pixels. We use 1 pixel.", true );
  app.add_option("--theta,-t", theta,"The resolution of the parameter \theta in radians.", true );
  app.add_option("--threshold,-T", threshold, "The minimum number of intersections to “detect” a line.", true);
  app.add_option("--minLinLength,-m", minLength," The minimum number of points that can form a line. Lines with less than this number of points are disregarded (use only with --useProbabilist option).", true );
  app.add_option("--maxLineGap,-g",maxLineGap, "The maximum gap between two points to be considered in the same line (use only with --useProbabilist option)",true );
  app.add_flag("--useProbabilist,-P", "use improved probabilist algorithm");
  
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  
  Mat src = imread(inputFileName, 0);
  if(src.empty())
    {
      cout << "can not open " << inputFileName << endl;
      return -1;
    }


  //  recover the  args ----------------------------------------------------
  
 
  Mat dst, cdst;
  Canny(src, dst, 50, 200, 3);
  cvtColor(dst, cdst, cv::COLOR_GRAY2BGR);
  bool outLines = outputFileName.size()!=0;
  ofstream outStream;
  if(outLines)
    {
      outStream.open(outputFileName.c_str(), ofstream::binary);
    }
  
  if(useProbabilist){
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
        line( cdst, pt1, pt2, Scalar(0,0,255), 1, LINE_AA);
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
          line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, LINE_AA);
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

