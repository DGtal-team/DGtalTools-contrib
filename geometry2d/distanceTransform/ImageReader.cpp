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
 * @file ImageReader.cpp
 * @ingroup Tools
 * @author Nicolas Normand (\c Nicolas.Normand@polytech.univ-nantes.fr)
 * LUNAM Université, Université de Nantes, IRCCyN UMR CNRS 6597
 *
 * @date 2012/09/28
 *
 * LUTBasedNSDistanceTransform computes the 2D translated neighborhood-sequence
 * distance transform of a binary image. It reads the input images from its
 * standard input and writes the result to its standard output.
 *
 * This file is part of the DGtal library.
 */

#include <boost/algorithm/string/predicate.hpp>
#include "PBMImageReader.h"
#ifdef WITH_PNG
#   include "PNGImageReader.h"
#endif

/**
 * Creates an ImageReader.
 *
 * The input image format is determined, in that order, by:
 * - the **format** parameter if not NULL,
 * - a prefix ended by ':' in the file format (*e.g.* 'png:filename'),
 * - the file extension,
 * - the file content.
 * If one of these methods specifies a format that is not available, no
 * ImageReader is created and the function return NULL.
 * Il no format is speficied at all, the default format is used in the last
 * resort.
 */
RowImageProducer<BinaryPixelType>* createImageReader(ImageConsumer<BinaryPixelType> *consumer, std::string filename = std::string("-"), std::string format = std::string("")) {
    FILE *input = NULL;

    // Format wasn't specified in arguments, check if there is a prefix for it.
    if (format == "") {
	size_t n = filename.find(':');
	if (n != std::string::npos) {
	    format = filename.substr(0, n);
	    filename = filename.substr(n+1);
	}
    }

    // Format wasn't specified in arguments nor in the filename prefix, check if
    // there the file has an extension
    if (format == "") {
	size_t n = filename.rfind('.');
	if (n != std::string::npos)
	    format = filename.substr(n+1);
    }

    if (filename == "-") {
	input = stdin;
    }
    else {
	input = fopen(filename.c_str(), "r");
	if (input == NULL) {
	    std::cerr << "Unable to open input stream";
	}
    }

    if (format == "") {
	char c = fgetc(input);
	ungetc(c, input);

	if (c == 'P') {
	    format = "pbm";
	}
    }

    if (format == "pbm") {
	PBMImageReader producer(consumer, input);
	//consumer = NULL;

	while (!feof(input)) {
	    producer.produceAllRows();

	    int c;
	    do {
		c = fgetc(input);
	    }
	    while (c == '\r' || c == '\n');
	    if (!feof(input)) {
		ungetc(c, input);
	    }
	}
    }

#ifdef WITH_PNG
    unsigned char signature[8];
    int readBytes = 0;

    if (format == "") {
	readBytes = fread(signature, 1, 8, input);
    }

    if (format == "png" ||
	(format == "" && readBytes == 8 && png_check_sig(signature, 8))) {

	format = "png";
	PNGImageReader producer(consumer, input);
	//consumer = NULL;

	do {
	    // Assumes following images, if any, are png
	    producer.produceAllRows(8);
	}
	while (fread(signature, 1, 8, input) == 8);
    }
#endif

    if (format == "") {
	std::cerr << "Input image format not recognized" << std::endl;
    }
    if (filename == "-") {
	input = stdin;
    }
    else {
	input = fopen(filename.c_str(), "r");
	// FIXME: where is fclose?
	if (input == NULL)
	    return NULL;
    }

    return NULL;
}
