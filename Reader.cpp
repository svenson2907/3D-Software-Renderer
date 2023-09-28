#include "Reader.h"

void Reader::allocPntrs(){                                                     /*Reads the significant values for memorymanagement and allocates needed memory*/
    while (std::getline(fs, s)) {
        if (s.find("<Anzahl Punkte>")!=std::string::npos) {
            s.erase(0,15);
            amtPnts=static_cast<unsigned int>(atoi(s.c_str()));
            points.resize(amtPnts);
            for (unsigned int i=0;i<amtPnts;i++) {
                points[i].resize(DIM);
            }
        }
        if (s.find("<Anzahl Flächen>")!=std::string::npos) {
            s.erase(0,17);
            amtFaces=static_cast<unsigned int>(atoi(s.c_str()));
            faces.resize(amtFaces);
            pointsPerFace.resize(amtFaces);
        }
        if (s.find("<Anzahl Punkte pro Fläche>")!=std::string::npos) {
            s.erase(0,27);
            for (unsigned int i=0;i<amtFaces;i++) {
                pointsPerFace[i]=static_cast<unsigned int>(atoi(s.c_str()));
                faces[i].resize(pointsPerFace[i]);
                s.erase(0,s.find(',')+1);
            }
        }
    }
    pos.resize(4);
    alignment.resize(4);
    for(unsigned int i=0;i<4;i++){
        alignment[i].resize(4);
    }
    fs.close();
}

/* The source file gets read and the informations get stored in the correct data structures
 * @param   filesPath
 * @return  Pointer to the Model-instance */
Model* Reader::readData (const char *filesPath) {
    fs.open(filesPath, std::ios::in);                                                                 /*opens the file with input operations - for allocPntrs()*/
    allocPntrs();
    fs.open(filesPath, std::ios::in);                                                   /*opens the file with input operations - for the following reader-Loop.*/
    while (std::getline(fs, s)) {                                                                     /*reads the complete file and sets eofbit end of bit flag*/
        if(s.find("<Name>")!=std::string::npos) {
            s.erase(0,6);                                                               /*Redundante Information("<Name>") aus eingelesenem Datensatz entfernen*/
            name=s.c_str();                                                                                   /*uebrige Zeichenkette[Name] an <name> uebergeben*/
        }
        if(s.find("<Position>")!=std::string::npos) {
            s.erase(0,10);
            pos[0]=atof(s.c_str());
            s.erase(0,s.find(',')+1);
            pos[1]=atof(s.c_str());
            s.erase(0,s.find(',')+1);
            pos[2]=atof(s.c_str());
            pos[3]=1;
        }
        if(s.find("<Punkte>")!=std::string::npos) {
            s.erase(0,8);                                                             /*Redundante Information("<Punkte>") aus eingelesenem Datensatz entfernen*/
            for (unsigned int i=0;i<amtPnts;i++) {
                points[i][0]=atof(s.c_str());
                s.erase(0,s.find(',')+1);
                points[i][1]=atof(s.c_str());
                s.erase(0,s.find(',')+1);
                points[i][2]=atof(s.c_str());
                s.erase(0,s.find(';')+1);
                points[i][3]=1;
            }
        }
        if(s.find("<Flächen>")!=std::string::npos) {
            s.erase(0,10);                                                          /*Redundante Information("<Flächen>") aus eingelesenem Datensatz entfernen*/
            for (unsigned int i=0;i<amtFaces;i++) {
                for (unsigned int j=0;j<pointsPerFace[i]-1;j++) {
                    faces[i][j]=static_cast<unsigned short>(atoi(s.c_str()));
                    s.erase(0,s.find(',')+1);
                }
                faces[i][pointsPerFace[i]-1]=static_cast<unsigned short>(atoi(s.c_str()));
                s.erase(0,s.find(';')+1);
            }
        }
    }
    fs.close();
    return new Model(name,pos,points,faces);
}

void Reader::readData (std::vector<Model>*const modelList,std::vector<struct Color>*const colorList) {
    Reader*r=new Reader();
    std::vector<float>modelColor(3);
    std::vector<float>modelPos(3);
    std::string dataName;
    std::string modelName;
    char key;
    unsigned int cId;
    float rAlpha;
    char rAxis;
    std::vector<float> rUm;
    fs.open(SCENEINIT_DATA, std::ios::in);
    while (std::getline(fs, s)) {
        if (s.find("<Color>")!=std::string::npos) {
            s.erase(0,8);
            modelColor[0]=atof(s.c_str());
            s.erase(0,s.find(',')+1);
            modelColor[1]=atof(s.c_str());
            s.erase(0,s.find(',')+1);
            modelColor[2]=atof(s.c_str());
            colorList->push_back(Color{true,modelColor[0],modelColor[1],modelColor[2]});
        }
        if (s.find("<Model>")!=std::string::npos) {             //ein <Model>-Eintrag existiert in sceneInit.txt fuer jedes zu ladene <Model>
            s.erase(0,8);
            dataName = s.substr(0,s.find(".")+4);
            s.erase(0,s.find('"')+2);
            
            Model*m=r->readData(dataName.c_str());
            m->color=&colorList->at(1);
            modelList->push_back(*m);
            
            while(s.find(';') != std::string::npos) {
                key = s[0];
                s.erase(0,s.find(':')+1);
                switch(key) {
                    case 'c':
                        cId = static_cast<unsigned int>(atoi(s.c_str()));
                        modelList->at(modelList->size()-1).color=&colorList->at(cId);
                        break;
                    case 'n':
                        s.erase(0,s.find('"')+1);
                        modelName=s.substr(0,s.find(";")-1);
                        modelList->at(modelList->size()-1).name=modelName;
                        break;
                    case 'p':
                        s.erase(0,s.find('(')+1);
                        modelPos[0]=atof(s.c_str());
                        s.erase(0,s.find(',')+1);
                        modelPos[1]=atof(s.c_str());
                        s.erase(0,s.find(',')+1);
                        modelPos[2]=atof(s.c_str());
                        modelList->at(modelList->size()-1).pos={modelPos[0],modelPos[1],modelPos[2]};
                        break;
                    default:  break;
                }
                s.erase(0,s.find(';')+1);
            }
        }
    }
    printf(SCENEINIT_READ);
    fs.close();
}

void Reader::readAnim (std::vector<Anim>*const animList,std::vector<Model>*const modelList,Transformation*transformation) {
    std::string modelName,modelPartsName;
    float rotAxisNorm;
    char key,animCategory;
    unsigned int modelPartsId;
    std::vector<float>rotAxis(3);
    std::vector<float>jointsPos(3);
    fs.open(ANIMINIT_DATA, std::ios::in);
    while (std::getline(fs, s)) {
        if (s.find("<Anim>")!=std::string::npos) {
            s.erase(0,7);
            modelName=s.substr(0,s.find(";")-1);
            s.erase(0,s.find('"')+2);
            animCategory=s.substr(0,1)[0];
            s.erase(0,s.find(';')+1);
            for(unsigned int i=0;i<modelList->size();i++){
                if(modelList->at(i).name.compare(modelName) == 0){
                    Anim*a=new Anim(&modelList->at(i),animCategory);
                    animList->push_back(*a);
                }
            }
            while(s.find(';') != std::string::npos) {
                key = s[0];
                s.erase(0,s.find(':'));
                switch(key) {
                    case 'j':
                        while(key=='j') {
                            modelPartsName=s.substr(2,s.find(",")-3);
                            s.erase(0,s.find('(')+1);
                            jointsPos[0]=atof(s.c_str());
                            s.erase(0,s.find(',')+1);
                            jointsPos[1]=atof(s.c_str());
                            s.erase(0,s.find(',')+1);
                            jointsPos[2]=atof(s.c_str());
                            s.erase(0,s.find(','));
                            
                            s.erase(0,s.find('(')+1);
                            rotAxis[0]=atof(s.c_str());
                            s.erase(0,s.find(',')+1);
                            rotAxis[1]=atof(s.c_str());
                            s.erase(0,s.find(',')+1);
                            rotAxis[2]=atof(s.c_str());
                            s.erase(0,s.find(';'));
                            
                            //NOTE <rotAxis> normieren
                            rotAxisNorm=static_cast<float>(sqrt(rotAxis[0]*rotAxis[0] + rotAxis[1]*rotAxis[1] + rotAxis[2]*rotAxis[2]));
                            rotAxis[0]/=rotAxisNorm;
                            rotAxis[1]/=rotAxisNorm;
                            rotAxis[2]/=rotAxisNorm;
                            
                            for(unsigned int i=0;i<modelList->size();++i) {
                                if(modelList->at(i).name.compare(modelName.c_str())==0) {
                                    for(unsigned int k=0;k<modelList->size();++k) {
                                        if(modelList->at(k).name.compare(modelPartsName.c_str())==0) {
                                            modelPartsId=k;
                                            break;
                                        }
                                    }
                                    modelList->at(i).addJoint(&modelList->at(modelPartsId),{rotAxis[0],rotAxis[1],rotAxis[2]},jointsPos);
                                }
                            }
                            key = s[0];
                        }
                        break;
                    case 't':
                        s.erase(0,s.find('"')+1);
                        modelName=s.substr(0,s.find(";")-1);
                        if (animList->at(animList->size()-1).getNrTargets()==0) {
                            for(unsigned int i=0;i<modelList->size();i++){
                                if(modelList->at(i).name.compare(modelName) == 0){
                                    animList->at(animList->size()-1).addTarget(new Target(animList->at(animList->size()-1).getModel(),&modelList->at(i)));
                                }
                            }
                        }else{
                            for(unsigned int i=0;i<modelList->size();i++){
                                if(modelList->at(i).name.compare(modelName) == 0){
                                    animList->at(animList->size()-1).addTarget(new Target( animList->at(animList->size()-1).getTarget(
                                                                        static_cast<unsigned int>(animList->at(animList->size()-1).getNrTargets()-1))->getTo(),
                                                                                           &modelList->at(i)
                                                                                         ));
                                }
                            }
                        }
                        break;
                    default:  break;
                }
                s.erase(0,s.find(';')+1);
            }
            
            for(auto &anim:*animList) {
                anim.addTarget(new Target(anim.getTarget(static_cast<unsigned int>(anim.getNrTargets()-1))->getTo(),anim.getTarget(1)->getFrom()));
            }
            animList->at(animList->size()-1).focussedTarget=animList->at(animList->size()-1).getTarget(0);
            for(unsigned int i=0;i<modelList->size();i++){
                if(modelList->at(i).name.compare(animList->at(animList->size()-1).focussedTarget->getTo()->name) == 0){
                    transformation->align(animList->at(animList->size()-1).getModel(),&modelList->at(i));
                }
            }
        }
    }    
    printf(ANIMINIT_READ);
    fs.close();
}

void Reader::readTrafo(std::vector<Anim>*const animList,Transformation*transformation) {
    std::string modelName;
    char key,axis;
    float alpha;
    Model*m;
    fs.open(TRAFOINIT_DATA, std::ios::in);
    while (std::getline(fs, s)) {
        if (s.find("<Trafo>")!=std::string::npos) {
            s.erase(0,8);
            modelName=s.substr(0,s.find(";")-1);
            s.erase(0,s.find('"')+2);
            for(unsigned int i=0;i<animList->size();i++){
                if(animList->at(i).getModel()->name.compare(modelName) == 0){
                    m=animList->at(i).getModel();
                }
            }
            //TODO moegliche 'r's einlesen/anwenden
            while(s.find(';') != std::string::npos) {
                key = s[0];
                s.erase(0,s.find(':')+2);
                switch(key) {
                    case 'r':
                        alpha=atof(s.c_str())*(M_PI/180);
                        s.erase(0,s.find(',')+1);
                        axis=s[1];
                        transformation->rotateFigure(alpha,axis,m);
                        s.erase(1,s.find(';')+1);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    printf(TRAFOINIT_READ);
    fs.close();
}
