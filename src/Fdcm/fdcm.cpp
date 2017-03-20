//#include <cxcore.h>
#include "Image/Image.h"
#include "Image/ImageIO.h"
#include "Fitline/line_fitter.h"
#include "Fdcm/line_matcher.h"

#include <iostream>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "edge_map.cpp"

void generate_edge_map(string file)
{
    cv::Mat image = cv::imread(file);
    cv::Mat detected_edges;
    cv::blur( image, detected_edges, cv::Size(3,3) );

    int lowThreshold = 50, highThreshold = 80;
    int kernel_size = 3;

    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, highThreshold, kernel_size );

    cv::imshow("Edge map", detected_edges);
    cv::waitKey();

    vector<int> params;
    params.push_back(cv::IMWRITE_PXM_BINARY);
    params.push_back(1);
    cv::imwrite(file.substr(0, file.length()-4) + "_edges.pgm", detected_edges, params);

}

int main(int argc, char *argv[])
{
    //generate_edge_map(argv[1]);
    //return 0;

	if(argc < 7)
	{
		std::cerr <<
                  "[Syntax] fdcm template_edgeMap.pgm input_edgeMap.pgm input_realImage.jpg para_line_fitter.txt template_line_fitter.txt para_line_matcher.txt"
                  << std::endl;
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

    //cv::Mat img = cv::imread(edgeMapName);
    //cv::resize(img, img, cv::Size(640, 480));
    //edgeMapName = "../Query/tmp.pgm";
    //vector<int> params;
    //params.push_back(cv::IMWRITE_PXM_BINARY);
    //params.push_back(1);
    //cv::imwrite(edgeMapName, img, params);

    Image<uchar> *inputImage=NULL;

    //inputImage = cvLoadImage(edgeMapName.c_str(),0);
	cout << "Loading image edge map" << endl;
	inputImage = ImageIO::LoadPGM(edgeMapName.c_str());
	
	if(inputImage==NULL)
	{
		std::cerr<<"[ERROR] Fail in reading image "<< edgeMapName <<std::endl;
		exit(0);
	}

    std::cout << "Loading template edge map" << std::endl;
    Image<uchar> *templateImage = ImageIO::LoadPGM(templateFileName.c_str());
    if(templateImage==NULL)
    {
        std::cerr << "[ERROR] Failed to read template image " << templateFileName << std::endl;
        exit(0);
    }

    LFLineFitter lf_template;
    lf_template.Configure(argv[5]);
    std::cout << "Initializing Template Line Fitter" << endl;
    lf_template.Init();
    std::cout << "Fitting Lines" << std::endl;
    // Template Line Fitting
    lf_template.FitLine(templateImage);

    // Now do the Line Fitting for the image
    LFLineFitter lf;
    lf.Configure(argv[4]);
    lf.Init();
    lf.FitLine(inputImage);

    LMLineMatcher lm;
    lm.Configure(argv[6]);

    std::cout << "Initializing Line Matcher with Template Line Representation" << std::endl;
    lm.Init(lf_template);

    std::cout << "Starting match process" << endl;
    // FDCM Matching

    //vector<DetWind> detWind;
    // lm.Match(lf,detWind);
    //lm.MatchCostMap(lf,outputCostMapName.c_str());

    vector< vector<DetWind> > detWindArrays;
    detWindArrays.clear();
    double maxThreshold = 0.12;

    lm.SingleShapeDetectionWithVaryingQuerySize(lf, maxThreshold, detWindArrays);

    std::cout << "Number of arrays " << detWindArrays.size() << std::endl;
    for(int i=0; i<detWindArrays.size(); i++)
    {
        std::cout << detWindArrays[i].size() << std::endl;
    }
    int last = detWindArrays.size()-1;
    int nDetWindows = detWindArrays[last].size();

    std::cout << "Number of detection windows " << nDetWindows << std::endl;
    vector<DetWind> detWind;

    for(int i=0; i<nDetWindows; i++)
    {
        DetWind wind;
        wind.x_ = detWindArrays[last][i].x_;
        wind.y_ = detWindArrays[last][i].y_;
        wind.width_ = detWindArrays[last][i].width_;
        wind.height_ = detWindArrays[last][i].height_;
        wind.cost_ = detWindArrays[last][i].cost_;
        wind.count_ = detWindArrays[last][i].count_;
        detWind.push_back(wind);
    }

    std::cout << "Number of detection windows " << detWind.size() << std::endl;
    // Display best matcher in edge map
    if(displayImageName.c_str() && detWind.size() > 0)
    {
    cout << "Loading color original image" << endl;
    //Image<RGBMap> *debugImage = ImageIO::LoadPPM(displayImageName.c_str());
    cv::Mat image = cv::imread(displayImageName);

    cout << "Drawing detection window" << endl;
    for(int idx=0; idx<detWind.size(); idx++)
    {
        cv::Point x = cv::Point(detWind[idx].x_, detWind[idx].y_);
        cv::Point y = cv::Point(detWind[idx].x_ + detWind[idx].width_, detWind[idx].y_ + detWind[idx].height_);
        cv::rectangle(image, y, x, cv::Scalar(0, 255, 0));
    }
    cv::imshow("Matched!", image);
    cv::waitKey();
    }

    //cvReleaseImage(&inputImage);
    delete inputImage;

    return 0;
};
