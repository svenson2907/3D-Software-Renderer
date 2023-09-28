#include "Model.h"
#include <string>
#include <iostream>
#include <exception>
#include <stdlib.h>
#include <cstring>

Model::Model (std::string nameArg,std::vector<float>posArg,std::vector<std::vector<float>>pointsArg,std::vector<std::vector<unsigned short>>facesArg) {
    name=nameArg;
    faces=facesArg;
    points=pointsArg;
    alignment.resize(4);
    alignment[0].resize(4);alignment[0]={1,0,0,0};
    alignment[1].resize(4);alignment[1]={0,1,0,0};
    alignment[2].resize(4);alignment[2]={0,0,1,0};
    alignment[3].resize(4);alignment[3]={0,0,0,1};
    pos=posArg;
    direction='f';
    getsProjected=true;
}

bool Model::getGetsProjected() {
    return getsProjected;
}

void Model::setJointsAxis(unsigned int id,std::vector<float>nArg) {
    n[id]=nArg;
}

std::vector<std::vector<float>> Model::getJoints() {
    return n;
}

void Model::rotateModel(const float a,const std::vector<float>axis) {
    std::vector<float>rotMat{
        static_cast<float>(pow(axis[0],2))*(1-cos(a))+cos(a), axis[0]*axis[1]*(1-cos(a))-axis[2]*sin(a), axis[0]*axis[2]*(1-cos(a))+axis[1]*sin(a) , 0,
        axis[1]*axis[0]*(1-cos(a))+axis[2]*sin(a), static_cast<float>(pow(axis[1],2))*(1-cos(a))+cos(a), axis[1]*axis[2]*(1-cos(a))-axis[0]*sin(a) , 0,
        axis[2]*axis[0]*(1-cos(a))-axis[1]*sin(a), axis[2]*axis[1]*(1-cos(a))+axis[0]*sin(a), static_cast<float>(pow(axis[2],2))*(1-cos(a))+cos(a) , 0,
        0                                        , 0                                        , 0                                                    , 1
    };
    std::vector<float>rotatedPoint={0,0,0,0};       //NOTE ansonsten SegFault, weil <rotatedPoint> unbeschrieben referenziert wuerde
    for (unsigned int i=0;i<points.size();i++) {
        for(unsigned int r=0;r<4;++r) {
            rotatedPoint[r]=0;
            for(unsigned int p=0;p<4;++p) {
                rotatedPoint[r]+=rotMat[r*4+p] * points[i][p];
            }
        }
        setPoint(i,rotatedPoint);
    }
    
    for (unsigned int i=0;i<n.size();++i) {
        for(unsigned int r=0;r<4;++r) {
            rotatedPoint[r]=0;
            for(unsigned int p=0;p<4;++p) {
                rotatedPoint[r]+=rotMat[r*4+p] * n[i][p];
            }
        }
        n[i]=rotatedPoint;
    }
}

std::vector<std::vector<float>> Model::getJointsPos() {
    return jointsPos;
}

void Model::setJointsPos(const unsigned int id,std::vector<float> jointsPosArg) {
    jointsPos[id]=jointsPosArg;
}

void Model::rotateJoint(const float a,const unsigned int id) {
    std::vector<float>rotMat{
        static_cast<float>(pow(n[id][0],2))*(1-cos(a))+cos(a), n[id][0]*n[id][1]*(1-cos(a))-n[id][2]*sin(a), n[id][0]*n[id][2]*(1-cos(a))+n[id][1]*sin(a) , 0,
        n[id][1]*n[id][0]*(1-cos(a))+n[id][2]*sin(a), static_cast<float>(pow(n[id][1],2))*(1-cos(a))+cos(a), n[id][1]*n[id][2]*(1-cos(a))-n[id][0]*sin(a) , 0,
        n[id][2]*n[id][0]*(1-cos(a))-n[id][1]*sin(a), n[id][2]*n[id][1]*(1-cos(a))+n[id][0]*sin(a), static_cast<float>(pow(n[id][2],2))*(1-cos(a))+cos(a) , 0,
        0                                                    , 0                                                    , 0                                   , 1
    };
    std::vector<float>rotatedPoint={0,0,0,0};       //NOTE ansonsten SegFault, weil <rotatedPoint> unbeschrieben referenziert wuerde
    
    for (unsigned int i=0;i<pntIdsToRotate[id].size();++i) {
        
        points[pntIdsToRotate[id][i]][0]-=jointsPos[id][0];
        points[pntIdsToRotate[id][i]][1]-=jointsPos[id][1];
        points[pntIdsToRotate[id][i]][2]-=jointsPos[id][2];
        points[pntIdsToRotate[id][i]][3]-=jointsPos[id][3];
        
        for(unsigned int r=0;r<4;++r) {
            rotatedPoint[r]=0;
            for(unsigned int p=0;p<4;++p) {
                rotatedPoint[r]+=rotMat[r*4+p] * points[pntIdsToRotate[id][i]][p];
            }
        }
        points[pntIdsToRotate[id][i]]=rotatedPoint;
        
        points[pntIdsToRotate[id][i]][0]+=jointsPos[id][0];
        points[pntIdsToRotate[id][i]][1]+=jointsPos[id][1];
        points[pntIdsToRotate[id][i]][2]+=jointsPos[id][2];
        points[pntIdsToRotate[id][i]][3]+=jointsPos[id][3];
    }
}

void Model::setAlignment(unsigned int l,std::vector<float>alignmentArg) {
    alignment[l][0]=alignmentArg[0];
    alignment[l][1]=alignmentArg[1];
    alignment[l][2]=alignmentArg[2];
    alignment[l][3]=alignmentArg[3];
}

std::vector<unsigned short>Model::getFace(unsigned int faceId) {
    return faces[faceId];
}

std::vector<float>*Model::getAlignmentsLineVec(unsigned int x) {
    return &alignment[x];
}
    
float Model::getAlignment(unsigned int x,unsigned int y) {
    return alignment[x][y];
}

unsigned int Model::getFaceSize() {
    return faces.size();
}

unsigned int Model::getFacesSize(unsigned int faceId) {
    
    return faces[faceId].size();
}

float Model::getPointsCoord(unsigned short i,unsigned short c) {
    return points[i][c]+pos[c];
}

void Model::setPoint(unsigned int i,std::vector<float>pnt) {
    points[i][0]=pnt[0];
    points[i][1]=pnt[1];
    points[i][2]=pnt[2];
    points[i][3]=pnt[3];
}

void Model::extractPntIDsOf(Model*mPart) {
    pntIdsToRotate.resize(static_cast<unsigned int>(pntIdsToRotate.size()+1),std::vector<unsigned int>(0));
    for(unsigned int j=0;j<points.size();j++) {
        for(unsigned int i=0;i<mPart->getPointsSize();i++) {
            if((fabs(points[j].at(0)-mPart->getPoint(i)->at(0)) < EPSILON) &&
               (fabs(points[j].at(1)-mPart->getPoint(i)->at(1)) < EPSILON) &&
               (fabs(points[j].at(2)-mPart->getPoint(i)->at(2)) < EPSILON)) {
                pntIdsToRotate.at(static_cast<unsigned int>(pntIdsToRotate.size()-1)).push_back(j);
            }
        }
    }
}

void Model::setGetsProjected(bool b) {
    getsProjected=b;
}

/**
 *Uniformly initializes <Joint> structures according to <ID>
 * @param mPart a absolute subset of this.points in order to reference the significant <ModelPart>-structure
 * @param nArg unit vector to define the rotations-direction
 * @post mPart-Models wont get projected anymore*/
void Model::addJoint(Model*mPart,const std::vector<float>nArg,const std::vector<float>jointsPosArg) {
    mPart->setGetsProjected(false);
    extractPntIDsOf(mPart);
    n.push_back(nArg);
    jointsPos.push_back(jointsPosArg);
}

std::vector<float>*Model::getPoint(unsigned int id) {
    return &points[id];
}

unsigned int Model::getPointsSize() {
    return points.size();
}

std::vector<std::vector<float>>*Model::getPoints() {
    return &points;
}

Model::~Model(void) {
    for (unsigned int i=0;i<points.size();i++) {
        points.clear();
    }
    for (unsigned int i=0;i<faces.size();i++) {
        faces[i].clear();
    }
    faces.clear();
    name.clear();
    pos.clear();
    alignment.clear();
}
