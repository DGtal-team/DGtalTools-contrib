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
 * @file PBMImageReader.cpp
 * @ingroup Tools
 * @author Nicolas Normand (\c Nicolas.Normand@polytech.univ-nantes.fr)
 * Université Bretagne Loire, Université de Nantes,
 * Laboratoire des Sciences du Numérique de Nantes (LS2N) UMR CNRS 6004
 *
 * @date 2012/09/28
 *
 * LUTBasedNSDistanceTransform computes the 2D translated neighborhood-sequence
 * distance transform of a binary image. It reads the input images from its
 * standard input and writes the result to its standard output.
 *
 * This file is part of the DGtal library.
 */

#include "PBMImageReader.h"

void skipCommentLines(FILE *file)
{
    int ch;
    int isComment;

    do
    {
        isComment = 0;
        ch = fgetc(file);
        if (ch == '#')
        {
            isComment = 1;
            int eolFound = 0;
            do
            {
                ch = fgetc(file);
                if (ch == '\n' || ch == '\r')
                    eolFound = 1;
            } while (ch != '\n' && ch != '\r' && !eolFound);
        }
        ungetc(ch, file);
    } while (isComment || ch == EOF);
}

void readpbminit(FILE *pbmFile, int *cols, int *rows, int *format)
{
    skipCommentLines(pbmFile);
    fscanf(pbmFile, "P%d ", format);
    if (*format != 1 && *format != 4)
    {
        *cols = *rows = 0;
        return;
    }
    skipCommentLines(pbmFile);
    fscanf(pbmFile, "%d %d ", cols, rows);
}

PBMImageReader::PBMImageReader(
    ImageConsumer<BinaryPixelType> *consumer, FILE *input)
    : super(consumer)
    , _input(input)
{
}

void PBMImageReader::produceAllRows()
{
    BinaryPixelType *inputRow;
    int cols, rows;
    int col, row;
    int format;

    readpbminit(_input, &cols, &rows, &format);
    assert(cols > 0);
    assert(rows > 0);
    inputRow = (BinaryPixelType *)malloc(sizeof(BinaryPixelType) * cols);
    _consumer->beginOfImage(cols, rows);
    switch (format)
    {
    case 1:
        for (int row = 0; row < rows; row++)
        {
            for (col = 0; col < cols; col++)
            {
                int value;
                fscanf(_input, "%1d", &value);
                inputRow[col] = value != 0;
            }
            _consumer->processRow(inputRow);
        }
        break;
    case 4:
        unsigned int bytesPerRow = (cols + 7) / 8;
        unsigned char *bits = (unsigned char *)malloc(bytesPerRow);
        int byte;
        unsigned char m;

        for (int row = 0; row < rows; row++)
        {
            fread(bits, 1, bytesPerRow, _input);
            for (col = 0, byte = 0, m = 1 << 7; col < cols; col++)
            {
                inputRow[col] = (bits[byte] & m) != 0;
                if (m > 1)
                {
                    m >>= 1;
                }
                else
                {
                    m = 1 << 7;
                    byte++;
                }
            }
            _consumer->processRow(inputRow);
        }
        free(bits);
        break;
    }
    _consumer->endOfImage();
    free(inputRow);
}
