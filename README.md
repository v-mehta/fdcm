# Fast Directional Chamfer Matching

## Objects

- OpenCV integration
- Easy to run and integrate
- All the optimizations as per the FDCM paper

## Build

```bash
cd build    # You may have to create the directory first
cmake ..
make
```

EI - Edge Image Representation of the input image.
LF - Line Fitting to fit the Edge Image of the template to the query image.
LM - Functions for Line Matching and its associated cost.
