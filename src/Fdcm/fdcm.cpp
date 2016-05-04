/*
Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/


//#include <cxcore.h>
#include "Image/Image.h"
#include "Image/ImageIO.h"
#include "fitline/LFLineFitter.h"
#include "../../include/Fdcm/LMLineMatcher.h"

#include <iostream>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

int main(int argc, char *argv[])
{
	
	if(argc < 7)
	{
		std::cerr << "[Syntax] fdcm template_edgeMap.pgm input_edgeMap.pgm input_realImage.jpg para_line_fitter.txt template_line_fitter.txt para_line_matcher.txt" << std::endl;
		exit(0);
	}

	string templateFileName(argv[1]);
	string edgeMapName(argv[2]);
	string displayImageName(argv[3]);
	
	//string templateFileName("Exp_Smoothness/template_list.txt");
	//string edgeMapName("Exp_Smoothness/device5-20_edge_cluttered.pgm");
	//string displayImageName("Exp_Smoothness/device5-20_edge_cluttered.pgm");	

	//string templateFileName("data/template_giraffe.txt");
	//string edgeMapName("data/looking_edges.pgm");
	//string displayImageName("data/looking.ppm");
	
	//string templateFileName("data/template_applelogo.txt");
	//string edgeMapName("data/hat_edges.pgm");
	//string displayImageName("data/hat.jpg");

    //Image *inputImage=NULL;
    Image<uchar> *inputImage=NULL;

    //inputImage = cvLoadImage(edgeMapName.c_str(),0);
	cout << "Loading image edge map" << endl;
	inputImage = ImageIO::LoadPGM(edgeMapName.c_str());
	
	if(inputImage==NULL)
	{
		std::cerr<<"[ERROR] Fail in reading image "<< edgeMapName <<std::endl;
		exit(0);
	}

    cout << "Loading template edge map" << endl;
    Image<uchar> *templateImage = ImageIO::LoadPGM(templateFileName.c_str());
    if(templateImage==NULL)
    {
        std::cerr << "[ERROR] Failed to read template image " << templateFileName << endl;
        exit(0);
    }

    LFLineFitter lf_template;
    lf_template.Configure(argv[5]);
    cout << "Initializing Template Line Fitter" << endl;
    lf_template.Init();
    cout << "Fitting Lines" << endl;
    // Template Line Fitting
    lf_template.FitLine(templateImage);

    // Now do the Line Fitting for the image
    LFLineFitter lf;
    lf.Configure(argv[4]);
    lf.Init();
    lf.FitLine(inputImage);

    LMLineMatcher lm;
    lm.Configure(argv[6]);

    cout << "Initializing Line Matcher with Template Line Representation" << endl;
	lm.Init(lf_template);

	cout << "Starting match process" << endl;
	// FDCM Matching

    //vector<LMDetWind> detWind;
	//lm.Match(lf,detWind);
	//lm.MatchCostMap(lf,outputCostMapName.c_str());


    vector< vector<LMDetWind> > detWindArrays;
    detWindArrays.clear();
    double maxThreshold = 0.12;

    lm.SingleShapeDetectionWithVaryingQuerySize(lf, maxThreshold, detWindArrays);

    cout << "Number of arrays " << detWindArrays.size() << endl;
    for(int i=0; i<detWindArrays.size(); i++)
    {
        cout << detWindArrays[i].size() << endl;
    }
    int last = detWindArrays.size()-1;
    int nDetWindows = detWindArrays[last].size();

    cout << "Number of detection windows " << nDetWindows << endl;
    vector<LMDetWind> detWind;

    for(int i=0; i<nDetWindows; i++)
    {
        LMDetWind wind;
        wind.x_ = detWindArrays[last][i].x_;
        wind.y_ = detWindArrays[last][i].y_;
        wind.width_ = detWindArrays[last][i].width_;
        wind.height_ = detWindArrays[last][i].height_;
        wind.cost_ = detWindArrays[last][i].cost_;
        wind.count_ = detWindArrays[last][i].count_;

        detWind.push_back(wind);
    }
    
    cout << "Number of detection windows " << detWind.size() << endl;
    // Display best matcher in edge map
	if(displayImageName.c_str())
	{
        cout << "Loading color original image" << endl;
		//Image<RGBMap> *debugImage = ImageIO::LoadPPM(displayImageName.c_str());
        cv::Mat image = cv::imread(displayImageName);

        cout << "Drawing detection window" << endl;
        cv::Point x = cv::Point(detWind[0].x_, detWind[0].y_);
        cv::Point y = cv::Point(detWind[0].x_ + detWind[0].width_, detWind[0].y_ + detWind[0].height_);
        cv::rectangle(image, y, x, cv::Scalar(0, 255, 0));
        cv::imshow("Matched!", image);
        cv::waitKey();
		//LMDisplay::DrawDetWind(debugImage,detWind[0].x_,detWind[0].y_,detWind[0].width_,detWind[0].height_,RGBMap(0,255,0),4);
		//char outputname[256];
		//sprintf(outputname,"%s.output.ppm",displayImageName.c_str());

        //cout << "Saving image" << endl;
		//ImageIO::SavePPM(debugImage,outputname);
		//delete debugImage;
	}

	//cvReleaseImage(&inputImage);
	delete inputImage;

    return 0;
};
