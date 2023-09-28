#ifndef     ProjectionIncluded
#define     ProjectionIncluded

#include "Model.h"
#include "Transformation.h"

#include <math.h>
#include <set>
#include <iostream>
#include <stdio.h>

#define WIDTH           1280
#define WIDTH_HALF      640
#define HEIGHT          800
#define HEIGHT_HALF     400
#define N               1000
#define FAR             (-100000.0)
#define xCOORD_CROSS    1,0,0,0
#define yCOORD_CROSS    0,1,0,0
#define zCOORD_CROSS    0,0,1,0

class Projection{

private:  
    struct Pxl{
        float z;
        Color c;
    };
    struct Coord {
        int hor;
        int ver;
        bool operator <(const Coord& c) const{
            return (ver > c.ver) || (((c.ver == ver)) && (hor < c.hor));
        }
    };
    std::vector<float> coordCrossX={xCOORD_CROSS};
    std::vector<float> coordCrossY={yCOORD_CROSS};
    std::vector<float> coordCrossZ={zCOORD_CROSS};
    void bresenline (int ax,int ay,int bx,int by,const float pntAzwei,const float SUBy1y2);
    void rasterization (const Color *c);
    Transformation*transformation;
    std::set<Coord>facePixelBuffer;
    std::vector<float>absCoordA;
    std::vector<float>absCoordB;
    std::vector<float>mappedPointA;
    std::vector<float>mappedPointB;
    std::vector<float>transMat;                                              //The result of Transformationmatrix multiplied by Projectionmatrix
    std::vector<float>projMat;
public:
    Projection(Transformation*transformationArg);
    bool isVisible(const unsigned int faceId,Model*current);                         //impliziert BackFaceCulling && FrustumCulling
    void map(Model*current);
    std::vector<std::vector<Pxl>> zBuffer;
    std::vector<float>camPos;
    std::vector<float>*getCoordCrossX();
    std::vector<float>*getCoordCrossY();
    std::vector<float>*getCoordCrossZ();
    void setCoordCrossX(std::vector<float>v);
    void setCoordCrossY(std::vector<float>v);
    void setCoordCrossZ(std::vector<float>v);
};

#endif      //ProjectionIncluded
