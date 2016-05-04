mex -v -c src/Fitline/LFLineFitter.cpp -I.
mex -v -c src/Fitline/LFLineSegment.cpp -I.
mex -v -c src/Image/DistanceTransform.cpp -I.
mex -v -c src/Fdcm/EIEdgeImage.cpp -I.
mex -v -c src/Fdcm/LMDirectionalIntegralDistanceImage.cpp -I.
mex -v -c src/Fdcm/LMDisplay.cpp -I.
mex -v -c src/Fdcm/LMDistanceImage.cpp -I.
mex -v -c src/Fdcm/LMLineMatcher.cpp -I.
mex -v -c src/Fdcm/LMNonMaximumSuppression.cpp -I.
mex -v -c src/Fdcm/MatchingCostMap.cpp -I.

% make fdcm
mex -v mex_fdcm_detect.cpp...
    LFLineFitter.obj LFLineSegment.obj DistanceTransform.obj ...
    EIEdgeImage.obj LMDirectionalIntegralDistanceImage.obj LMDisplay.obj...
    LMDistanceImage.obj LMLineMatcher.obj LMNonMaximumSuppression.obj...
    MatchingCostMap.obj -I.

% make fitline
mex -v mex_fitline.cpp LFLineFitter.obj LFLineSegment.obj -I.

delete *.obj