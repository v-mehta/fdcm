cmake_minimum_required(VERSION 3.5)
project(FDCM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(BEFORE include)

find_package( OpenCV REQUIRED )

set(SOURCE_FILES
        src/Fdcm/edge_image.cpp
        include/Fdcm/edge_image.h
        src/Fdcm/fdcm.cpp
        include/Fdcm/DetWind.h
        src/Fdcm/directional_integral_distance_image.cpp
        include/Fdcm/directional_integral_distance_image.h
        src/Fdcm/display.cpp
        include/Fdcm/display.h
        src/Fdcm/distance_image.cpp
        include/Fdcm/distance_image.h
        src/Fdcm/line_matcher.cpp
        include/Fdcm/line_matcher.h
        src/Fdcm/non_maximum_suppression.cpp
        include/Fdcm/non_maximum_suppression.h
        src/Fdcm/matching_cost_map.cpp
        include/Fdcm/matching_cost_map.h
        include/Fdcm/MMFunctions.h
        src/Fitline/fitline.cpp
        src/Fitline/line_fitter.cpp
        include/Fitline/line_fitter.h
        src/Fitline/line_segment.cpp
        include/Fitline/line_segment.h
        include/Fitline/Point.h
        src/Image/distance_transform.cpp
        include/Image/distance_transform.h
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