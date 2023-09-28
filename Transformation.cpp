#include "Transformation.h"

/**
 * rotiert a->getModel() entlang y-Achse,
 * so dass <fPre> nach <fPost> zeigt.
 * @a: <Anim> von welchem sich <Model> und <Target> ableiten laesst.
 * @fPre: Richtungsvektor nach <Target> vor der Ausrichtung.
 * @fPost: Richtungsvektor nach <Target> nach der Ausrichtung.
 * @post: Es ist sichergestellt, dass <Model.alignment> in richtige Richtung rotiert wurde. */
float Transformation::focussing (Anim *const a,std::vector<float>fPre,std::vector<float>fPost,std::vector<float>axis) {
    //<alpha> := Winkel zwischen fPre[x,z] und fPost[x,z]
    float alpha = acos((fPre[0]*fPost[0]+fPre[2]*fPost[2])/(sqrt(fPre[0]*fPre[0]+fPre[2]*fPre[2]) * sqrt(fPost[0]*fPost[0]+fPost[2]*fPost[2])));
    if(alpha!=alpha)return 0;   //NOTE floatcheck for not a number
    //<fPre> x rotMatY(alpha)
    std::vector<float>posAlpha(4);
    rotatePoint(alpha,axis,&fPre,&posAlpha);             // Dieser Punkt muss aber um eine andere Y-Achse rotiert werden
    //Distanz zwischen <posAlpha> und <a->focussedTarget>
    float distPosAlpha=sqrt(pow((posAlpha[0]-a->focussedTarget->getTo()->pos[0]),2)+
                            pow((posAlpha[2]-a->focussedTarget->getTo()->pos[2]),2));
    //<fPre> x rotMatY(-alpha) 
    std::vector<float>negAlpha(4);
    alpha*=(-1);
    rotatePoint(alpha,axis,&fPre,&negAlpha);
    //Distanz zwischen <posAlpha> und <a->focussedTarget>
    float distNegAlpha=sqrt(pow((negAlpha[0]-a->focussedTarget->getTo()->pos[0]),2)+
                            pow((negAlpha[2]-a->focussedTarget->getTo()->pos[2]),2));
    //Vergleich -> kleinere Distanz(distNegAlpha,distPosAlpha) -> zu verwendendes <alpha>
    std::vector<float>tmpVec(4);
    if((distNegAlpha-distPosAlpha)<EPSILON) {
        return alpha;
    }else {
        return alpha*(-1);
    }
}

void Transformation::rotatePoint(float a,std::vector<float>axis,std::vector<float>*pnt,std::vector<float>*ret) {
    std::vector<float>rotMat{
        static_cast<float>(pow(axis[0],2))*(1-cos(a))+cos(a), axis[0]*axis[1]*(1-cos(a))-axis[2]*sin(a), axis[0]*axis[2]*(1-cos(a))+axis[1]*sin(a) , 0,
        axis[1]*axis[0]*(1-cos(a))+axis[2]*sin(a), static_cast<float>(pow(axis[1],2))*(1-cos(a))+cos(a), axis[1]*axis[2]*(1-cos(a))-axis[0]*sin(a) , 0,
        axis[2]*axis[0]*(1-cos(a))-axis[1]*sin(a), axis[2]*axis[1]*(1-cos(a))+axis[0]*sin(a), static_cast<float>(pow(axis[2],2))*(1-cos(a))+cos(a) , 0,
        0                                        , 0                                        , 0                                                    , 1
    };
    matMul(&rotMat,4,1,4,pnt,ret);
}

void Transformation::align (Model *const m,std::vector<float>*const target) {
    std::vector<float>f={m->pos[0]-target->at(0),m->pos[1]-target->at(1),m->pos[2]-target->at(2)};
    float k=1/sqrt(pow(f[0],2)+pow(f[1],2)+pow(f[2],2));
    f={f[0]*k,f[1]*k,f[2]*k};
    m->setAlignment(2,f);
    
    std::vector<float>r={0,f[2],(-1)*f[1]};
    k=1/sqrt(pow(r[0],2)+pow(r[1],2)+pow(r[2],2));
    r={k*r[0],k*r[1],k*r[2]};
    m->setAlignment(0,r);
    
    std::vector<float>u={f[1]*r[2]-f[2]*r[1],f[2]*r[0]-f[0]*r[2],f[0]*r[1]-f[1]*r[0]};
    k=1/sqrt(pow(u[0],2)+pow(u[1],2)+pow(u[2],2));
    u={k*u[0],k*u[1],k*u[2]};
    m->setAlignment(1,u);
}

/**
 * @post fuer Richtung 'f' wird <m> in Richtung <target> bewegt.
 * @post fuer Richtung 'r' wird <m> orthogonal zur Richtung 'f' bewegt.
 * @post fuer Richtung 'u' wird <m> orthogonal zur Richtung 'f' und orthogonal zur Richtung 'r' bewegt. */
void Transformation::align(Model *const m,Model *const target) {
    std::vector<float>f={m->pos[0]-target->pos.at(0),m->pos[1]-target->pos.at(1),m->pos[2]-target->pos.at(2)};
    float k=1/sqrt(pow(f[0],2)+pow(f[1],2)+pow(f[2],2));
    f={f[0]*k,f[1]*k,f[2]*k};
    m->setAlignment(2,f);
    
    std::vector<float>r={0,f[2],(-1)*f[1]};
    k=1/sqrt(pow(r[0],2)+pow(r[1],2)+pow(r[2],2));
    r={k*r[0],k*r[1],k*r[2]};
    m->setAlignment(0,r);
    
    std::vector<float>u={f[1]*r[2]-f[2]*r[1],f[2]*r[0]-f[0]*r[2],f[0]*r[1]-f[1]*r[0]};
    k=1/sqrt(pow(u[0],2)+pow(u[1],2)+pow(u[2],2));
    u={k*u[0],k*u[1],k*u[2]};
    m->setAlignment(1,u);
}

void Transformation::move(Model *const m,const char direction) {
    switch(direction) {
        case 'r':{                                                                                                                                      /*Right*/
            m->pos[0]+=m->getAlignment(0,0)*ANIMATION_SPEED;
            m->pos[1]+=m->getAlignment(0,1)*ANIMATION_SPEED;
            m->pos[2]+=m->getAlignment(0,2)*ANIMATION_SPEED;
            break;
        }
        case 'l':{                                                                                                                                       /*Left*/
            m->pos[0]+=m->getAlignment(0,0)*(-ANIMATION_SPEED);
            m->pos[1]+=m->getAlignment(0,1)*(-ANIMATION_SPEED);
            m->pos[2]+=m->getAlignment(0,2)*(-ANIMATION_SPEED);
            break;
        }
        case 'b':{                                                                                                                                  /*Backwards*/
            m->pos[0]+=m->getAlignment(2,0)*ANIMATION_SPEED;
            m->pos[1]+=m->getAlignment(2,1)*ANIMATION_SPEED;
            m->pos[2]+=m->getAlignment(2,2)*ANIMATION_SPEED;
            break;
        }
        case 'f':{                                                                                                                                   /*Forwards*/
            m->pos[0]+=m->getAlignment(2,0)*(-ANIMATION_SPEED);
            m->pos[1]+=m->getAlignment(2,1)*(-ANIMATION_SPEED);
            m->pos[2]+=m->getAlignment(2,2)*(-ANIMATION_SPEED);
            break;
        }
        case 'u':{                                                                                                                                         /*Up*/
            m->pos[0]+=m->getAlignment(1,0)*ANIMATION_SPEED;
            m->pos[1]+=m->getAlignment(1,1)*ANIMATION_SPEED;
            m->pos[2]+=m->getAlignment(1,2)*ANIMATION_SPEED;
            break;
        }
        case 'd':{                                                                                                                                       /*Down*/
            m->pos[0]+=m->getAlignment(1,0)*(-ANIMATION_SPEED);
            m->pos[1]+=m->getAlignment(1,1)*(-ANIMATION_SPEED);
            m->pos[2]+=m->getAlignment(1,2)*(-ANIMATION_SPEED);
            break;
        }
        default:    break;
    }
}

/*
 * Multiplies mA and mB and writes the result to mRet
 * @param *mA:         The Pointer to mA's sequence.
 * @param rowsA:       The number of mA's rows.
 * @param colsB:       The number of mB's columns.
 * @param colsArowsB:  The equal number of mA's columns and mB's rows
 * @param *mB:         The Pointer to mB's sequence.
 * @param *mRet:       The Pointer to the result's/mRet's sequence */
void Transformation::matMul(const std::vector<float>*mA,const unsigned  int rowsA,const unsigned int colsB,const unsigned int clsArwsB,
                            const std::vector<float>*mB,std::vector<float>*const mRet){
    for (unsigned int rA=0;rA<rowsA;++rA) {
        for (unsigned int cB=0;cB<colsB;++cB) {
            const unsigned int index=rA*colsB;
            const unsigned int rAMulColsArowsB=rA*clsArwsB;
            mRet->operator[](index+cB)=0;
            for (unsigned int s=0;s<clsArwsB;++s) {
                mRet->operator[](index+cB)+=(mA->operator[]((rAMulColsArowsB)+s))*(mB->operator[](colsB*s+cB));
            }
        }
    }
}

void Transformation::rotateFigure (const float a,const std::vector<float>n,Model*const m) {
    std::vector<float>rotMat{
        static_cast<float>(pow(n[0],2))*(1-cos(a))+cos(a), n[0]*n[1]*(1-cos(a))-n[2]*sin(a), n[0]*n[2]*(1-cos(a))+n[1]*sin(a) , 0,
        n[1]*n[0]*(1-cos(a))+n[2]*sin(a), static_cast<float>(pow(n[1],2))*(1-cos(a))+cos(a), n[1]*n[2]*(1-cos(a))-n[0]*sin(a) , 0,
        n[2]*n[0]*(1-cos(a))-n[1]*sin(a), n[2]*n[1]*(1-cos(a))+n[0]*sin(a), static_cast<float>(pow(n[2],2))*(1-cos(a))+cos(a) , 0,
        0                                                    , 0                                                    , 0                                   , 1
    };
    std::vector<float>rotatedPoint={0,0,0,0};       //NOTE ansonsten SegFault, weil <rotatedPoint> unbeschrieben referenziert wuerde
    for (unsigned int i=0;i<m->getPointsSize();i++) {
        for(unsigned int r=0;r<4;++r) {
            rotatedPoint[r]=0;
            for(unsigned int p=0;p<4;++p) {
                rotatedPoint[r]+=rotMat[r*4+p] * m->getPoint(i)->at(p);
            }
        }
        m->setPoint(i,rotatedPoint);
    }
}

/* Laesst einen Punkt entspr. um den Ursprung rotieren
 * @param angle Winkelnangebe [Bogenmass]
 * @param axis  Angabe der Achse um welche rotiert wird
 * @param m     Zu rotierendes Model
 * @var rotMat  Sequenz in welche die Werte entspr. Rotatiosnmatrix geschrieben wird
 * @var tmpVec  Der PunktSpaltenvektor muss ueber die gesamte Dauer der skalaren Multiplikation unveraendert bleiben,
 *              deswegen muess sich jedes einzelne Ergebnis(pro Dimension) in einer anderen Sequenz gemerkt werden,
 *              welche nachtraeglich an den PunktSpaltenvektor uebergeben wird.
 * @pre         Die Winkelangabe<angle> muss in Bogenmass (alpha*=PI/180;) angegebene sein.*/
void Transformation::rotatePoint (const float alpha,const char axis,std::vector<float>*pnt,std::vector<float>*ret) {
    std::vector<float>tmpVec(4);
    std::vector<float>rotMat(16);
    switch (axis) {
        case 'x': {
            rotMat={ 1,         0,              0,0,
                     0,cos(alpha),(-1*sin(alpha)),0,
                     0,sin(alpha),     cos(alpha),0,
                     0,         0,              0,1};
            break;
        }
        case 'y': {
            rotMat={   cos(alpha),0,sin(alpha),0,
                                0,1,         0,0,
                    -1*sin(alpha),0,cos(alpha),0,
                                0,0,         0,1};
            break;
        }
        case 'z': {
            rotMat={ cos(alpha),-1*sin(alpha),0,0,
                     sin(alpha),   cos(alpha),0,0,
                              0,            0,1,0,
                              0,            0,0,1};
            break;
        }
        default:    break;
    }
    
    matMul(&rotMat,4,1,4,pnt,ret);
}

/*Rotation des Models, ohne seine Ausrichtung zu aendern.
 * @param angle Winkelnangebe [Bogenmass]
 * @param axis  Angabe der Achse um welche rotiert wird
 * @param m     Zu rotierendes Model
 * @var rotMat  Sequenz in welche die Werte entspr. Rotatiosnmatrix geschrieben wird
 * @var tmpVec  Der PunktSpaltenvektor muss ueber die gesamte Dauer der skalaren Multiplikation unveraendert bleiben,
 *              deswegen muess sich jedes einzelne Ergebnis(pro Dimension) in einer anderen Sequenz gemerkt werden,
 *              welche nachtraeglich an den PunktSpaltenvektor uebergeben wird.
 * @pre         Die Winkelangabe<angle> muss in Bogenmass (alpha*=M_PI/180;) angegebene sein.*/
void Transformation::rotateFigure (const float alpha,const char axis,Model*const m) {
    std::vector<float>tmpVec(4);
    std::vector<float>rotMat(16);
    switch (axis) {
        case 'x': {
            rotMat={ 1,         0,              0,0,
                     0,cos(alpha),(-1*sin(alpha)),0,
                     0,sin(alpha),     cos(alpha),0,
                     0,         0,              0,1};
            break;
        }
        case 'y': {
            rotMat={   cos(alpha),0,sin(alpha),0,
                                0,1,         0,0,
                    -1*sin(alpha),0,cos(alpha),0,
                                0,0,         0,1};
            break;
        }
        case 'z': {
            rotMat={ cos(alpha),-1*sin(alpha),0,0,
                     sin(alpha),   cos(alpha),0,0,
                              0,            0,1,0,
                              0,            0,0,1};
            break;
        }
        default:    break;
    }
    for(unsigned int i=0;i<m->getPointsSize();i++) {
        matMul(&rotMat,4,1,4,m->getPoint(i),&tmpVec);
        m->setPoint(i,tmpVec);
    }
    for (unsigned int i=0;i<m->getJoints().size();++i) {
        rotatePoint(alpha,axis,&m->getJoints()[i],&tmpVec);
        m->setJointsAxis(i,tmpVec);
    }
}

void Transformation::rotate(const float alpha,const char axis,Model*const m,std::vector<unsigned int>relevantPntIds) {
    std::vector<float>rotatedPoint(4);
    for (unsigned int i=0;i<relevantPntIds.size();i++) {
        rotatePoint(alpha,axis,m->getPoint(relevantPntIds.at(i)),&rotatedPoint);
        m->setPoint(relevantPntIds.at(i),rotatedPoint);
    }
    
    rotatePoint(alpha,axis,&m->pos,&rotatedPoint);                        // Rotation des Position-Vektors
    m->pos=rotatedPoint;
}

/*  In entspr. Schritten muss vorgegangen werden, um ein <Model> um ein/einen anderes Model/Vektor zu rotieren:
 *      I)      [Model]<m> und [Model]<um> linear transformieren, so dass <um> am Ursprung liegt.
 *              Eigentlich muss hierbei nur [Model]<m> linear um den Abstand von <um> zum Urspung transformiert werden, 
 *              sodass Ursprung fuer <um> angenommen werden kann.
 *      II)     <m.points> entspr. der Vorgaben rotieren, jede PunktKoordinate <m.points> durch Subtraktion von <m.pos> dem Inertialsystem anpassen.
 *      III)    <m.pos> entspr. der Vorgaben rotieren und Koordinate der rotierten pos merken.
 *      IV)     [Model]<m> linear um den Abstand von <um> zurueck transformieren.   */
void Transformation::rotate (const float alpha,const char axis,Model*m,std::vector<float>*um) {
    m->pos[0]-=um->at(0); m->pos[1]-=um->at(1); m->pos[2]-=um->at(2);
    std::vector<float>rotatedPoint(4);
    for (unsigned int i=0;i<m->getPoints()->size();i++) {
        rotatePoint(alpha,axis,m->getPoint(i),&rotatedPoint);
        m->setPoint(i,rotatedPoint);
    }
    
    rotatePoint(alpha,axis,&m->pos,&rotatedPoint);                        // Rotation des Position-Vektors
    m->pos=rotatedPoint;
    m->pos[0]+=um->at(0); m->pos[1]+=um->at(1); m->pos[2]+=um->at(2);
    
    for (unsigned int i=0;i<m->getJoints().size();++i) {
        rotatePoint(alpha,axis,&m->getJoints()[i],&rotatedPoint);
        m->setJointsAxis(i,rotatedPoint);
    }
    
    rotatedPoint={0,0,0,0};
    for (unsigned int i=0;i<m->getJointsPos().size();++i) {
        rotatePoint(alpha,axis,&m->getJointsPos()[i],&rotatedPoint);
        m->setJointsPos(i,rotatedPoint);
    }
}

void Transformation::rotateCam (const float alpha,const char axis,std::vector<Model>*modelListArg,std::vector<Anim>*animListArg,std::vector<float>*const camPos){
    std::vector<float>tmpVec(4);
    std::vector<std::vector<float>>tmpRot(2,std::vector<float>(4));
    for (auto&m:*modelListArg) {                                                    //<Model>s aus <modelListArg> iterieren
        rotate(alpha,axis,&m,camPos);                                               //<Model>s entspr um <camPos> drehen
    }
    
    for (auto&a:*animListArg) {                                             //<Anim>s aus <animListArg> iterieren
        align(a.getModel(),a.focussedTarget->getTo());                      //<Anim>s <Model>s auf ihr durch Kameradrehung veraendertes <Target.pos> ausrichten
    }

    switch (axis) {
        case 'x':{
            rotatePoint(alpha,axis,&moveDirectionX,&tmpVec);
            moveDirectionX=tmpVec;
            break;
        }
        case 'y':{
            rotatePoint(alpha,axis,&moveDirectionY,&tmpVec);
            moveDirectionY=tmpVec;
            break;
        }
        case 'z':{
            rotatePoint(alpha,axis,&moveDirectionZ,&tmpVec);
            moveDirectionZ=tmpVec;
            break;
        }
        default:    break;
    }
}

//bewegt stellvertretend zur Kamera alle <Model>s entgegengesetzt der linearen Bewegung der Kamera, so dass es so wirkt als ob die Kamera bewegt wird.
void Transformation::moveCamAlongAlignment (std::vector<Model>*modelListArg,const char direction) {
    switch (direction) {
        case 'r':{                                                                                                                                      /*Right*/
            for (auto&m:*modelListArg) {
                m.pos[0]-=moveDirectionX.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]-=moveDirectionX.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]-=moveDirectionX.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        case 'l':{                                                                                                                                       /*Left*/
            for (auto&m:*modelListArg) {
                m.pos[0]+=moveDirectionX.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]+=moveDirectionX.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]+=moveDirectionX.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        case 'b':{                                                                                                                                  /*Backwards*/
            for (auto&m:*modelListArg) {
                m.pos[0]-=moveDirectionZ.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]-=moveDirectionZ.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]-=moveDirectionZ.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        case 'f':{                                                                                                                                   /*Forwards*/
            for (auto&m:*modelListArg) {
                m.pos[0]+=moveDirectionZ.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]+=moveDirectionZ.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]+=moveDirectionZ.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        case 'u':{                                                                                                                                         /*Up*/
            for (auto&m:*modelListArg) {
                m.pos[0]-=moveDirectionY.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]-=moveDirectionY.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]-=moveDirectionY.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        case 'd':{                                                                                                                                       /*Down*/
            for (auto&m:*modelListArg) {
                m.pos[0]+=moveDirectionY.at(0)*FACTOR_CAMSTEPLENGTH;
                m.pos[1]+=moveDirectionY.at(1)*FACTOR_CAMSTEPLENGTH;
                m.pos[2]+=moveDirectionY.at(2)*FACTOR_CAMSTEPLENGTH;
            }
            break;
        }
        default:    break;
    }
}
