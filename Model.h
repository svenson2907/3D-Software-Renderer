#ifndef modelYetIncluded
#define modelYetIncluded
#include <string>
#include <cstring>
#include <vector>
#include <math.h>

#define DIM         4
#define EPSILON     0.01

struct Color{
    bool isColored;
    float red;
    float green;
    float blue;
};

const Color nullColor={false,0,0,0};

class Model {
private:
    std::vector<std::vector<unsigned short>>faces;
    std::vector<std::vector<float>>points;
    std::vector<std::vector<float>>alignment;
    std::vector<std::vector<unsigned int>>pntIdsToRotate;   //PunktIDs(<points>)        [<id>]
    std::vector<std::vector<float>>n;                       //Einheitsvektor <joint>    [<id>]
    std::vector<std::vector<float>>jointsPos;               //Position des Drehpunkts   [<id>]
    
    void extractPntIDsOf(Model*mPart);
    bool getsProjected;
public:
    Model (std::string nameArg,std::vector<float>posArg,std::vector<std::vector<float>>pointsArg,std::vector<std::vector<unsigned short>>facesArg);
    ~Model ();
    Color*color;
    std::string name;
    char direction;
    std::vector<float>pos;
    std::vector<float>*getPos();
    std::vector<unsigned short>getFace(unsigned int faceId);
    unsigned int getFacesSize(unsigned int faceId);
    unsigned int getFaceSize();
    std::vector<std::vector<float>>*getPoints();
    std::vector<float>*getPoint(unsigned int id);
    float getPointsCoord(unsigned short i,unsigned short c);
    unsigned int getPointsSize();
    void setPoint(unsigned int i,std::vector<float>pnt);
    std::vector<float>*getAlignmentsLineVec(unsigned int x);
    float getAlignment(unsigned int x,unsigned int y);
    void setAlignment(unsigned int l,std::vector<float>align);
    bool getGetsProjected();
    void setGetsProjected(const bool b);
    std::vector<std::vector<float>> getJoints();
    std::vector<std::vector<float>> getJointsPos();
    void setJointsPos(const unsigned int id,std::vector<float> jointsPosArg);
    void addJoint(Model*mPart,const std::vector<float>nArg,const std::vector<float>jointsPosArg);   //Schnittstelle um von aussen <Joint> zu initialisieren
    void setJointsAxis(unsigned int id,std::vector<float>nArg);         //Welt dreht um Kamera => <Model>s werden neu positioniert -> analog auch entspr. <Joint>s
    void rotateJoint(const float alpha,const unsigned int id);
    void rotateModel(const float alpha,const std::vector<float>rotAxis);
};
#endif
