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


#include "Fdcm/non_maximum_suppression.h"


double LMNonMaximumSuppression::OverlapRatio(DetWind &curWind,DetWind &refWind)
{
	double area1, area2;
	double unionArea, intersectArea;

	int topLeftX,topLeftY,bottomRightX,bottomRightY;

	topLeftX = max(curWind.x_,refWind.x_);
	topLeftY = max(curWind.y_,refWind.y_);
	bottomRightX = min(curWind.x_+curWind.width_,refWind.x_+refWind.width_);
	bottomRightY = min(curWind.y_+curWind.height_,refWind.y_+refWind.height_);

	if(topLeftX>=bottomRightX||topLeftY>=bottomRightY)
		return 0;

	area1 = curWind.width_*curWind.height_;
	area2 = refWind.width_*refWind.height_;
	intersectArea = (bottomRightX-topLeftX)*(bottomRightY-topLeftY);
	unionArea = area1+area2-intersectArea;

	return intersectArea/unionArea;
}


bool LMNonMaximumSuppression::IsOverlapping(DetWind &curWind,vector<DetWind> &wind,double overlapThresh)
{
	for(unsigned int i=0; i< wind.size(); i++)
	{
		if( OverlapRatio( curWind, wind[i] ) >= overlapThresh)
		{
			wind[i].count_++;			
			return true;
		}
	}
	return false;
}

void LMNonMaximumSuppression::ComputeValidWindVaryingQuerySize(MatchingCostMap &matchingCostMap,vector<DetWind> &detWinds,double threshold)
{
	int index ;
	DetWind aWind;
	detWinds.reserve(matchingCostMap.width_[0]*matchingCostMap.height_[0]);
	for(int i=0;i<matchingCostMap.nCostMap_;i++)
	{		
		aWind.width_ = (int)ceil(matchingCostMap.templateWidth_[i]/matchingCostMap.scale_[i]);
		aWind.height_ = (int)ceil(matchingCostMap.templateHeight_[i]/matchingCostMap.scale_[i]);
		for(int y=0;y<matchingCostMap.height_[i];y++)
		{
			for(int x=0;x<matchingCostMap.width_[i];x++)
			{
				index = x + y*matchingCostMap.width_[i];
				if( matchingCostMap.costMap_[i][index] <= threshold)
				{
					// translation
					aWind.x_ = (int)(matchingCostMap.x0_[i]  + x*matchingCostMap.stepSize_[i]*1.0/matchingCostMap.scale_[i]);
					aWind.y_ = (int)(matchingCostMap.y0_[i]  + y*matchingCostMap.stepSize_[i]*1.0/matchingCostMap.scale_[i]);
					aWind.cost_ = matchingCostMap.costMap_[i][index];
					aWind.count_ = 0;
					detWinds.push_back(aWind);
				}
			}
		}
	}
}

void LMNonMaximumSuppression::ComputeValidWindVaryingTemplateSize(MatchingCostMap &matchingCostMap,vector<DetWind> &detWinds,double threshold)
{
	int index ;
	DetWind aWind;
	detWinds.reserve(matchingCostMap.width_[0]*matchingCostMap.height_[0]);
	for(int i=0;i<matchingCostMap.nCostMap_;i++)
	{		
		aWind.width_ = matchingCostMap.templateWidth_[i];
		aWind.height_ = matchingCostMap.templateHeight_[i];
		for(int y=0;y<matchingCostMap.height_[i];y++)
		{
			for(int x=0;x<matchingCostMap.width_[i];x++)
			{
				index = x + y*matchingCostMap.width_[i];
				if( matchingCostMap.costMap_[i][index] <= threshold)
				{
					// translation
					aWind.x_ = matchingCostMap.x0_[i]  + x*matchingCostMap.stepSize_[i];
					aWind.y_ = matchingCostMap.y0_[i]  + y*matchingCostMap.stepSize_[i];
					aWind.cost_ = matchingCostMap.costMap_[i][index];
					aWind.count_ = 0;
					detWinds.push_back(aWind);
				}
			}
		}
	}
}


void LMNonMaximumSuppression::ComputeDetection(MatchingCostMap &matchingCostMap,double threshold,double overlapThresh,vector<DetWind> &wind,int varyingQuerySize)
{
	wind.clear();

	// Scan the matching cost map to find the hypotheses with a matching cost less than the threshold,
	// and construct the detection windows for these hypotheses.
	vector<DetWind> detWinds;
	if(varyingQuerySize)
		ComputeValidWindVaryingQuerySize(matchingCostMap,detWinds,threshold);
	else
		ComputeValidWindVaryingTemplateSize(matchingCostMap,detWinds,threshold);
	
	if(detWinds.size()==0)
	{
		cout<<"\t "<<wind.size()<<"//"<<detWinds.size()<<endl;
		return;
	}


	// Sort the window array in the ascending order of matching cost.
	DetWind *tmpWind = new DetWind [detWinds.size()];
	for(unsigned int i=0;i<detWinds.size();i++)
		tmpWind[i] = detWinds[i];
	MMFunctions::Sort(tmpWind,detWinds.size());


	// Scan the array. 
	// If a query detection window does not overlap the other detection windows which have a smaller cost,
	// create it as a new detection. Otherwise increasing the hit count for the overlapped window.
	for(unsigned int i=0;i<detWinds.size();i++)
	{
		if(!IsOverlapping(tmpWind[i],wind,overlapThresh))
			wind.push_back(tmpWind[i]);
	}

	cout<<"\t "<<wind.size()<<"//"<<detWinds.size()<<endl;
	delete [] tmpWind;
}
