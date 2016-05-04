cmake_minimum_required(VERSION 3.5)
project(FDCM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(BEFORE include)

find_package( OpenCV REQUIRED )

set(SOURCE_FILES
        src/Fdcm/EIEdgeImage.cpp
        include/Fdcm/EIEdgeImage.h
        src/Fdcm/fdcm.cpp
        include/Fdcm/LMDetWind.h
        src/Fdcm/LMDirectionalIntegralDistanceImage.cpp
        include/Fdcm/LMDirectionalIntegralDistanceImage.h
        src/Fdcm/LMDisplay.cpp
        include/Fdcm/LMDisplay.h
        src/Fdcm/LMDistanceImage.cpp
        include/Fdcm/LMDistanceImage.h
        src/Fdcm/LMLineMatcher.cpp
        include/Fdcm/LMLineMatcher.h
        src/Fdcm/LMNonMaximumSuppression.cpp
        include/Fdcm/LMNonMaximumSuppression.h
        src/Fdcm/MatchingCostMap.cpp
        include/Fdcm/MatchingCostMap.h
        include/Fdcm/MMFunctions.h
        src/Fitline/fitline.cpp
        src/Fitline/LFLineFitter.cpp
        include/Fitline/LFLineFitter.h
        src/Fitline/LFLineSegment.cpp
        include/Fitline/LFLineSegment.h
        include/Fitline/Point.h
        src/Image/DistanceTransform.cpp
        include/Image/DistanceTransform.h
        include/Image/Image.h
        include/Image/ImageDraw.h
        include/Image/ImageIO.h
        Query/ceazanne.jpg
        Query/ceazanne_edges.pgm
        Query/four.jpg
        Query/four_edges.pgm
        Query/pale.jpg
        Query/pale_edges.pgm
        Query/ray.jpg
        Query/ray_edges.pgm
        Template/handdrawn_bottle.pgm
        Template/handdrawn_bottle.txt
        #demo.m
        #make.m
        #mex_fdcm_detect.cpp
        #mex_fdcm_detect.mexw32
        #mex_fdcm_detect.mexw64
        #mex_fitline.cpp
        #mex_fitline.mexw32
        #mex_fitline.mexw64
        #README.txt
        )

add_executable(fdcm ${SOURCE_FILES})

target_link_libraries(fdcm ${OpenCV_LIBS})