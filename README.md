# Fast Directional Chamfer Matching

[Paper](https://www.umiacs.umd.edu/users/vashok/MyPapers/HighlySelectiveConf2010/liu_cvpr2010.pdf)

## Objectives

- OpenCV integration
- Easy to run and integrate
- All the optimizations as per the FDCM paper

## Build

```bash
mkdir -p build && cd build
cmake -D CUDA_USE_STATIC_CUDA_RUNTIME=OFF ..  # This is needed to work with OpenCV 2.13+
make
```

- EI - Edge Image Representation of the input image.
- LF - Line Fitting to fit the Edge Image of the template to the query image.
- LM - Functions for Line Matching and its associated cost.
