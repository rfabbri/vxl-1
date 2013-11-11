/*
 *  Util.h
 *  ImageApp
 *
 *  Created by Brendan Moore on 2/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dbseg_levelset2_Mh.h"
#include "dbseg_levelset2_RGBImg.h"
#include "dbseg_levelset2_ImageObj.h"

#define MAXVALUE 255
#define MINVALUE 0
#define PI 3.14159


using namespace std;

/*eli edit
int file_select(struct direct *entry);
void ReadPGM(string filename, ImageObj& image);
void WritePGM(string filename, const ImageObj& image);

void ReadPPM(string filename, RGBImg& image);
void WritePPM(string filename, const RGBImg& image);

void ReadJPEG(string filename, RGBImg& dest);
void ReadJPEG(string filename, ImageObj &dest);
void WriteJPEG(string filename, const RGBImg& src);
void WriteJPEG(string filename, const ImageObj& src);

void ReadImageDir(string path, ImageObj* &dArray);
*/

double rnd(double val);
//eli edit void MakeDirs();

void ShiftImage(ImageObj& image);
void ShiftImage(ImageObj& image, int min, int max);
void ShiftImage(ImageObj& image, double newMin, double newMax, double iMin, double iMax);

void ScaleImage(const ImageObj& src, ImageObj& dest, float pw, float ph);
double BLInterp(const ImageObj& src, double x, double y);

void Threshold(const ImageObj& image, ImageObj& dest, int th);

void DrawHist(const ImageObj& src);


