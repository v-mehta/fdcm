/*
 *
 * Adapted Juan Quiroz's C++ code to convert a .obj file into a .pgm file to be used in Chamfer matching.
 *
 * Creates a PGM file from an .obj file.
 *
 */

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>

// Default map size
const int MAPSIZE = 369;
const int GRAYSCALE = 255;

using namespace std;

struct Point3d{
    float x,y,z;
};

void usage()
{
    // Print how to use command
    printf("\ncreatebitmap.cpp -f file.obj\n\n");
    exit(0);
}


int convert_to_pgm(string fileName="testing.obj", char c='f')
{
    string outFileName;
    bool noNegative=false, belowWater=true, normalize=false, convert=true;
    int bitmap[MAPSIZE][MAPSIZE]={0};

    // Parse command line

    switch(c)
    {
        // Retrieve .obj file name
        case 'f':
            outFileName = fileName;
            break;
        case 'h':
            usage();
        break;
        case 'n':
            noNegative=true;
            break;
        case 'p':
            belowWater=false;
            break;
        case 'i':
            for ( int i=0; i < MAPSIZE; ++i )
                for ( int j=0; j < MAPSIZE; ++j )
                    bitmap[i][j]=255;
            break;
        case 'c':
            convert=false;
            break;
        case 's':
            normalize=true;
    }

    FILE *fin=NULL;
    char check, dummy[120];
    Point3d point;
    int i=1, index=0;
    long size=0;
    float minX, minY, maxX, maxY, maxZ, minZ;
    float xOffset=0, zOffset=0,yOffset=0;
    int tempx=0, tempy=0, tempz=0;


    bool minSet=false;

    Point3d *coords=NULL;
    Point3d *copyCoords=NULL;

    point.x=point.y=point.z=0;
    minX=minY=maxX=maxY=maxZ=minZ=0;

    // count the number of vertices on obj file
    string systemCall = "cat " + fileName + " | egrep '^v ' | wc -l > countxyz";
    system(systemCall.c_str());

    // Read in the number of vertices
    fin = fopen("countxyz", "r");
    fscanf(fin, "%ld", &size);
    fclose(fin);
    // Exit if invalid file name provided
    if ( size == 0 )
    {
        printf("Invalid file.\n");
        usage();
    }

    coords = new Point3d[size];
    copyCoords = new Point3d[size];

    fin = fopen(fileName.c_str(), "r");
    if ( fin == NULL )
    {
        printf("Could not open file for reading.\n");
        return 1;
    }

    // Read in geometric data from obj file
    while ( !feof(fin) )
    {
        fgets(dummy,120,fin);
        // if line contains vertex data
        if ( dummy[0] == 'v' && dummy[1]==' ')
        {
            // Retrieve x, y, z values
            sscanf(dummy, "%c %f %f %f", &check,&point.x,&point.y,&point.z); 

            // set minimum to first vertex 
            if ( !minSet )
            {
                minX = point.x;
                minY = point.y;
                maxX = point.x;
                maxY = point.y;
                maxZ = point.z;
                minZ = point.z;
                minSet=true;
            }
            else
            {
                // Find the min and max for all x,y,z
                if ( point.x < minX )
                    minX = point.x;
                else if ( point.x > maxX )
                    maxX = point.x;
                if ( point.y > maxY )
                    maxY = point.y;
                else if ( point.y < minY )
                    minY = point.y;
                if ( point.z > maxZ )
                    maxZ = point.z;
                else if ( point.z < minZ )
                    minZ = point.z;
            }
            coords[i]=point;
            ++i;
        }
    } 

    fclose(fin);

    printf("Stats:\n");
    printf("x Min: %f,\tx Max: %f\n", minX, maxX); 
    printf("y Min: %f,\ty Max: %f\n", minY, maxY); 
    printf("z Min: %f,\tz Max: %f\n", minZ, maxZ); 

    // Compute the offset which makes all x and z coordinates non-negative
    if ( minX < 0 )
        xOffset = -minX;
    if ( minZ < 0 )
        zOffset = -minZ;
    if ( minY < 0 )
        yOffset = -minY;

    maxX += xOffset;
    maxZ += zOffset;
    maxY += yOffset;

    if (belowWater)
    {
        for (i=0; i < size; ++i)
        {
            tempz = (int)rint(((coords[i].x+xOffset)/maxX)*(MAPSIZE-1));
            tempx = (int)rint(((coords[i].z+zOffset)/maxZ)*(MAPSIZE-1));
            
            if ( coords[i].y >= 0)
                bitmap[tempx][tempz]=GRAYSCALE;
            else
                bitmap[tempx][tempz]=GRAYSCALE-(int) rint((coords[i].y/minY)*GRAYSCALE);
        }

    }
    else if ( noNegative )
    {
        for (i=0; i < size; ++i)
        {
            tempx = (int)rint(((coords[i].x+xOffset)/maxX)*(MAPSIZE-1));
            tempz = (int)rint(((coords[i].z+zOffset)/maxZ)*(MAPSIZE-1));

            bitmap[tempx][tempz]=(int) rint(((coords[i].y+yOffset)/maxY)*GRAYSCALE);
        }
    }
    else
    {
        for (i=0; i < size; ++i)
        {
            tempz = (int)rint(((coords[i].x+xOffset)/maxX)*(MAPSIZE-1));
            tempx = (int)rint(((coords[i].z+zOffset)/maxZ)*(MAPSIZE-1));

            if ( coords[i].y <= 0 )
                bitmap[tempx][tempz]=0;
            else
                bitmap[tempx][tempz]= (int)rint((coords[i].y/maxY)*GRAYSCALE);
        }
    }

    // If normalize flag is set, then average each point with all its neighbors
    if ( normalize )
    {
        int sum=0;
        for (int i=1; i<MAPSIZE-1; ++i)
            for (int j=1; j<MAPSIZE-1; ++j)
            {
                sum=0;
                for ( int k=i-1; k < i+2; ++k )
                    for ( int l=j-1; l < j+2; ++l )
                        sum+=bitmap[k][l];

                bitmap[i][j] = (int)rint((float)sum/9.0);
            }
    }


    // Write bitmap to PGM format
    ofstream ofp;

    outFileName += ".pgm";
    ofp.open(outFileName.c_str(), ios::out);

    //Output header data
    ofp << "P5\n";
    ofp << MAPSIZE << " " << MAPSIZE<<endl;
    ofp << GRAYSCALE << endl;

    unsigned char *image;
    image = (unsigned char *) new unsigned char[MAPSIZE*MAPSIZE];

    // Convert bitmap data to unsigned char array
    for (int i=0; i<MAPSIZE; ++i)
        for (int j=0; j<MAPSIZE; ++j)
            image[i*MAPSIZE+j]=(unsigned char)bitmap[i][j];

    // Write image to file
    ofp.write( reinterpret_cast<char *>(image), (MAPSIZE*MAPSIZE)*sizeof(unsigned char));
    ofp.close();

    delete [] image;
    system("rm countxyz");

    return 0;
}

/*int main(int argc, char** argv)
{
    convert_to_pgm(argv[1]);
}*/