#include "Projection.h"


std::vector<float>*Projection::getCoordCrossX() {
    return &coordCrossX;
}
std::vector<float>*Projection::getCoordCrossY() {
    return &coordCrossY;
}
std::vector<float>*Projection::getCoordCrossZ() {
    return &coordCrossZ;
}
void Projection::setCoordCrossX(std::vector<float>v) {
    coordCrossX=v;
}
void Projection::setCoordCrossY(std::vector<float>v) {
    coordCrossY=v;
}
void Projection::setCoordCrossZ(std::vector<float>v) {
    coordCrossZ=v;
}

/*Prueft zuerst ob sich die Flaeche im Furstum befindet
 * Im Falle dessen wird geprueft ob es sich um ein Backface handelt
 *@param    faceId  
 *@param    *current
 *@post     <absCoordA> is already transformed/mapped for the upcoming line
 *@return   true:   sichtbar
 *          false:  nicht sichtbar */
bool Projection::isVisible(const unsigned int faceId, Model*current){
    bool isInFrustum=false;
    for(unsigned int i=0;i<current->getFacesSize(faceId);i++){
        absCoordA[0]=current->getPointsCoord(current->getFace(faceId)[i],0); 
        absCoordA[1]=current->getPointsCoord(current->getFace(faceId)[i],1); 
        absCoordA[2]=current->getPointsCoord(current->getFace(faceId)[i],2);
        absCoordA[3]=1;
        transformation->matMul(&transMat,4,1,4,&absCoordA,&mappedPointA);
        if(mappedPointA[3]>1){
            mappedPointA[0]/=mappedPointA[3];mappedPointA[1]/=mappedPointA[3];mappedPointA[2]/=mappedPointA[3];mappedPointA[3]/=mappedPointA[3];
        }else continue;
        
        if(mappedPointA[0]>(WIDTH/(-2)) && mappedPointA[0]<(WIDTH/2) && mappedPointA[1]>(HEIGHT/(-2)) && mappedPointA[1]<(HEIGHT/2)){
            isInFrustum = true;
            break;
        }
    }
    if(!isInFrustum) return false;
    
    absCoordB[0]=current->getPointsCoord(current->getFace(faceId)[1],0); 
    absCoordB[1]=current->getPointsCoord(current->getFace(faceId)[1],1);
    absCoordB[2]=current->getPointsCoord(current->getFace(faceId)[1],2); 
    absCoordB[3]=1;
    transformation->matMul(&transMat,4,1,4,&absCoordB,&mappedPointB);
    mappedPointB[0]/=mappedPointB[3];mappedPointB[1]/=mappedPointB[3];mappedPointB[2]/=mappedPointB[3];mappedPointB[3]/=mappedPointB[3];
    
    absCoordA[0]=current->getPointsCoord(current->getFace(faceId)[2],0); 
    absCoordA[1]=current->getPointsCoord(current->getFace(faceId)[2],1);
    absCoordA[2]=current->getPointsCoord(current->getFace(faceId)[2],2); 
    absCoordA[3]=1;
    transformation->matMul(&transMat,4,1,4,&absCoordA,&mappedPointA);
    mappedPointA[0]/=mappedPointA[3];mappedPointA[1]/=mappedPointA[3];mappedPointA[2]/=mappedPointA[3];mappedPointA[3]/=mappedPointA[3];
    
    const float vSubB[3]{mappedPointB[0]-mappedPointA[0],mappedPointB[1]-mappedPointA[1],mappedPointB[2]-mappedPointA[2]};
    
    absCoordA[0]=current->getPointsCoord(current->getFace(faceId)[0],0); 
    absCoordA[1]=current->getPointsCoord(current->getFace(faceId)[0],1); 
    absCoordA[2]=current->getPointsCoord(current->getFace(faceId)[0],2); 
    absCoordA[3]=1;
    transformation->matMul(&transMat,4,1,4,&absCoordA,&mappedPointA);
    mappedPointA[0]/=mappedPointA[3];mappedPointA[1]/=mappedPointA[3];mappedPointA[2]/=mappedPointA[3];mappedPointA[3]/=mappedPointA[3];
    
    const float vSubA[3]{mappedPointA[0]-mappedPointB[0],mappedPointA[1]-mappedPointB[1],mappedPointA[2]-mappedPointB[2]};
    
    //<vSubA> x <vSubB>         => Wo zeigt Normale hin?
    const std::vector<float>n={(vSubA[1]*vSubB[2]-vSubA[2]*vSubB[1]),(vSubA[2]*vSubB[0]-vSubA[0]*vSubB[2]),(vSubA[0]*vSubB[1]-vSubA[1]*vSubB[0])};
    
    //face[1] -> camPos
    const std::vector<float>camPosvSub={(camPos[0]-mappedPointB[0]),(camPos[1]-mappedPointB[1]),(camPos[2]-mappedPointB[2])};
    std::vector<float> skalarProduct(1);
    transformation->matMul(&n,1,1,3,&camPosvSub,&skalarProduct);
    return (skalarProduct[0]<0)?false:true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
void Projection::rasterization (const Color *c) {
    std::set<struct Coord>::iterator nextCoord=facePixelBuffer.begin();
    for(std::set<struct Coord>::iterator currentCoord=facePixelBuffer.begin();currentCoord!=facePixelBuffer.end();currentCoord++){
        nextCoord++;
        if(currentCoord->ver==nextCoord->ver && 
           (HEIGHT/(-2))<currentCoord->ver && currentCoord->ver<(HEIGHT/2) &&
            nextCoord!=facePixelBuffer.end()){
            const float y1=(((WIDTH/(-2))<currentCoord->hor) && (currentCoord->hor<(WIDTH/2))
                             ?(zBuffer[currentCoord->hor+(WIDTH/2)][currentCoord->ver+(HEIGHT/2)].z):(-1));
            const float SUBy1y2 = (((WIDTH/(-2))<nextCoord->hor) && (nextCoord->hor<(WIDTH/2))
                                    ?(zBuffer[nextCoord->hor+(WIDTH/2)][nextCoord->ver+(HEIGHT/2)].z-y1):(-1));
            for(int i=(currentCoord->hor);i<nextCoord->hor;i++){
                Pxl*const currentPxl=&zBuffer[i+(WIDTH/2)][currentCoord->ver+(HEIGHT/2)];
                const float currentDistance=static_cast<float>(y1+((SUBy1y2)/(nextCoord->hor-currentCoord->hor))*(i-currentCoord->hor)+0.5);
                if((WIDTH/(-2))<i && i<(WIDTH/2) && 
                   (HEIGHT/(-2))<currentCoord->ver && currentCoord->ver<(HEIGHT/2) &&
                   (currentPxl->z-currentDistance)<(-1) ){
                    currentPxl->z=currentDistance;
                    currentPxl->c=*c;
                }
            }
        }
    }
    facePixelBuffer.clear();
}
#pragma GCC diagnostic pop

/*Zeichnet die perspektivisch transformierten Flaechen des Models auf welche <current> zeigt*/
void Projection::map (Model*current) {
    for (unsigned int faceId=0;faceId<current->getFaceSize();faceId++) {
        if(isVisible(faceId,current)) {
            transformation->matMul(&transMat,4,1,4,&absCoordA,&mappedPointA);
            
            if(mappedPointA[3]>1){
                mappedPointA[0]/=mappedPointA[3];mappedPointA[1]/=mappedPointA[3];mappedPointA[2]/=mappedPointA[3];mappedPointA[3]/=mappedPointA[3];
            }else continue;
            
            for (unsigned int pointId=1;pointId<current->getFacesSize(faceId);pointId++) {       //NOTE so iteriere ich richtig ueber <points>
                absCoordB[0]=current->getPointsCoord(current->getFace(faceId)[pointId],0);   
                absCoordB[1]=current->getPointsCoord(current->getFace(faceId)[pointId],1);
                absCoordB[2]=current->getPointsCoord(current->getFace(faceId)[pointId],2);   
                absCoordB[3]=1;
                transformation->matMul(&transMat,4,1,4,&absCoordB,&mappedPointB);
                
                if(mappedPointB[3]>1){
                    mappedPointB[0]/=mappedPointB[3];mappedPointB[1]/=mappedPointB[3];mappedPointB[2]/=mappedPointB[3];mappedPointB[3]/=mappedPointB[3];
                }else continue;
                
                if(mappedPointA[0]>mappedPointB[0]) {
                    bresenline(mappedPointA[0],mappedPointA[1],mappedPointB[0],mappedPointB[1],absCoordA[2],(absCoordB[2]-absCoordA[2]));
                }else{
                    bresenline(mappedPointB[0],mappedPointB[1],mappedPointA[0],mappedPointA[1],absCoordA[2],(absCoordB[2]-absCoordA[2]));
                }
                mappedPointA=mappedPointB;
                absCoordA=absCoordB;
            }
            absCoordB[0]=current->getPointsCoord(current->getFace(faceId)[0],0);   
            absCoordB[1]=current->getPointsCoord(current->getFace(faceId)[0],1);
            absCoordB[2]=current->getPointsCoord(current->getFace(faceId)[0],2);
            absCoordB[3]=1;
            transformation->matMul(&transMat,4,1,4,&absCoordB,&mappedPointB);  /*entspr. Punkt wird transformiert, das Ergebnis in <mappedPointB> geschrieben*/
            
            if(mappedPointB[3]>1){
                mappedPointB[0]/=mappedPointB[3];mappedPointB[1]/=mappedPointB[3];mappedPointB[2]/=mappedPointB[3];mappedPointB[3]/=mappedPointB[3];
            }else continue;
            
            if(mappedPointA[0]>mappedPointB[0]) {
                bresenline(mappedPointA[0],mappedPointA[1],mappedPointB[0],mappedPointB[1],absCoordA[2],(absCoordB[2]-absCoordA[2]));
            }else{
                bresenline(mappedPointB[0],mappedPointB[1],mappedPointA[0],mappedPointA[1],absCoordA[2],(absCoordB[2]-absCoordA[2]));
            }
            rasterization(current->color);
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
/*Zieht eine Linie von a -> b
 *Beschreibt zBuffer entspr. der berechneten Pixelkoordinaten/interpolierten Distanzen
 * @param   aX,aY               Die Koordinaten von Punkt A
 * @param   bX,bY               Die Koordinaten von Punkt B 
 * @param   pntAzwei,SUBy1y2    Werte um entspr. Pixel-Distanz zum Auge zu interpolieren*/
void Projection::bresenline (int aX,int aY,int bX,int bY,const float pntAzwei,const float SUBy1y2) {
    Color BLACK={true,0.,0.,0.};
    Coord tmpCoord;
    const int transHor=0-aX;
    const int transVer=0-aY;
    aX+=transHor;   aY+=transVer;   bX+=transHor;   bY+=transVer;                                           // line's Coordinates has been transformed to origin.
    const int mirrorVer=( bX<0 )?-1:1;                                                                       // If X-Coordinate is negative...
    bX*=mirrorVer;                                                                                           // => point get mirrored along vertical axis.
    const int mirrorHor=( bY<0 )?-1:1;                                                                       // If Y-Coordinate is negative...
    bY*=mirrorHor;                                                                                           // => point get mirrored along horizontal axis.
                                                                                        // line's pointB's Coordinates has been mirrored into the first quadrant.
    int error;                                                         // ... float f ... optimized -> Is not longer able to accept floatingpoint values anymore.
    int iFd,iSd;                                                                                                                    // vars defined for bresenAlg
    const int dx=(bX%2!=0 || bY%2!=0)?2*bX:bX;
    const int dy=(bX%2!=0 || bY%2!=0)?2*bY:bY;
    
    if(dy<dx){
        error=dx/2;
        iSd=0;
        for(iFd=0; iFd<=bX; iFd++){
            error-=dy;
            tmpCoord.hor=((iFd*mirrorVer-transHor)-1<(WIDTH/(-2)))?(WIDTH/(-2)+1):(iFd*mirrorVer-transHor);//NOTE gegen Anomalie wenn Flaeche ausserhalb linker Rand
            tmpCoord.ver=iSd*mirrorHor-transVer;
            facePixelBuffer.insert(tmpCoord);
            if((WIDTH/(-2))<tmpCoord.hor && tmpCoord.hor<(WIDTH/2) && 
               (HEIGHT/(-2))<tmpCoord.ver && tmpCoord.ver<(HEIGHT/(2))){
                float* const currentZbufferVal=&zBuffer[tmpCoord.hor+(WIDTH/(2))][tmpCoord.ver+(HEIGHT/(2))].z;
                const float currentDistance=(pntAzwei+((SUBy1y2)/(bX-aX))*(iFd-aX));
                if((*currentZbufferVal-currentDistance)<(0.)){
                    *currentZbufferVal=currentDistance;
                    zBuffer[tmpCoord.hor+(WIDTH/(2))][tmpCoord.ver+(HEIGHT/(2))].c=BLACK;
                }
            }
            if(error<0){
                iSd++;
                error+=dx;
            }
        }
    }else{
        error=dy/2;
        iSd=0;
        for(iFd=0; iFd<=bY; iFd++){
            error-=dx;
            tmpCoord.hor=((iSd*mirrorVer-transHor)-1<(WIDTH/(-2)))?(WIDTH/(-2)+1):(iSd*mirrorVer-transHor);//NOTE gegen Anomalie wenn Flaeche ausserhalb linker Rand
            tmpCoord.ver=iFd*mirrorHor-transVer;
            facePixelBuffer.insert(tmpCoord);
            if((WIDTH/(-2))<tmpCoord.hor && tmpCoord.hor<(WIDTH/2) && 
               (HEIGHT/(-2))<tmpCoord.ver && tmpCoord.ver<(HEIGHT/(2))){
                float* const currentZbufferVal=&zBuffer[tmpCoord.hor+(WIDTH/(2))][tmpCoord.ver+(HEIGHT/(2))].z;
                const float currentDistance=(pntAzwei+((SUBy1y2)/(bY-aY))*(iFd-aY));
                if((*currentZbufferVal-currentDistance)<(0.)){
                    *currentZbufferVal=currentDistance;
                    zBuffer[tmpCoord.hor+(WIDTH/(2))][tmpCoord.ver+(HEIGHT/(2))].c=BLACK;
                }
            }
            if(error<0){
                iSd++;
                error+=dy;
            }
        }
    }
}
#pragma GCC diagnostic pop

Projection::Projection(Transformation*transformationArg) {
    transformation=transformationArg;
    transMat.resize(16);
    transMat={N,0, 0,0,
              0,N, 0,0,
              0,0, 0,0,
              0,0,-1,N};
    camPos.resize(4);
    camPos[2]=1000; //TODO <camPos> an Transformation mitteilen
    absCoordA.resize(4);
    absCoordB.resize(4);
    mappedPointA.resize(4);
    mappedPointB.resize(4);
	zBuffer.resize(WIDTH);
    for(unsigned int x=0;x<WIDTH;x++) {
        zBuffer[x].resize(HEIGHT);
        for(unsigned int y=0;y<HEIGHT;y++) {
            zBuffer[x][y].c=nullColor;
            zBuffer[x][y].z=FAR;            //Zuerst wird jedes Pixel nicht gefaerbt
        }
    }
}
