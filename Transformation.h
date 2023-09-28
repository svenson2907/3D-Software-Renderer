#ifndef     transformationYetIncluded
#define     transformationYetIncluded
#define     ANIMATION_SPEED         0.2
#define     FACTOR_CAMSTEPLENGTH    0.08
#define     xDIRECTION              1,0,0,0
#define     yDIRECTION              0,1,0,0
#define     zDIRECTION              0,0,1,0
#include    "Model.h"
#include    "Anim.h"
#include    <iostream>
#include    <math.h>

class Transformation{
private:
    std::vector<float> moveDirectionX={xDIRECTION};
    std::vector<float> moveDirectionY={yDIRECTION};
    std::vector<float> moveDirectionZ={zDIRECTION};
public:
    void move                   (Model *const m,const char direction);
    void rotatePoint            (const float alpha,const char axis,std::vector<float>*pnt,std::vector<float>*ret);
    void rotateFigure           (const float alpha,const char axis,Model*const m);
    void rotateFigure           (const float alpha,const std::vector<float>n,Model*const m);
    void rotate                 (const float alpha,const char axis,Model*const m,std::vector<float>*um);
    void rotate                 (const float alpha,const char axis,Model*const m,std::vector<unsigned int>relevantPntIds);    
    void matMul                 (const std::vector<float>*mA,const unsigned int rowsA,
                                 const unsigned int colsB,const unsigned int clsArwsB,
                                 const std::vector<float>*mB,std::vector<float>*const mRet);
    void align                  (Model *const m,Model *const target);
    void align                  (Model *const m,std::vector<float>*const target);
    void rotateCam              (const float alpha,const char axis,std::vector<Model>*modelListArg,std::vector<Anim>*animListArg,std::vector<float>*const camPos);
    void moveCamAlongAlignment  (std::vector<Model>*modelListArg,const char direction);
    float focussing             (Anim *const a,std::vector<float>fPre,std::vector<float>fPost,std::vector<float>axis);
    void rotatePoint            (float a,std::vector<float>axis,std::vector<float>*pnt,std::vector<float>*ret);
};

#endif      //transformation
