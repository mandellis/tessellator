#ifndef TESSELLATOR_H
#define TESSELLATOR_H

//! --------
//! C++ STL
//! --------
#include <string>

//! ------------
//! OpenCascade
//! ------------
#include <TopoDS_Shape.hxx>

//! ----------------
//! custom includes
//! ----------------
#include "meshingparameters.h"
#include "QNetworkInterface"

/*namespace format {
enum formatType {
    STL,
    STP,
    NONE
};
}*/

class tessellator
{
public:
    enum formatType{
        STL,
        STP,
        NONE
    };
    //void setType(sT type);
    tessellator(const std::string& shapeFileFullPath = "");

private:

    //format::formatType theType;
    formatType mType;
    TopoDS_Shape m_shape;
    meshingParameters m_mp;
    bool m_shapeLoaded;
    std::string m_absoluteInputFilePath;
    std::string m_outputFileDirectory;


public:

    void setMeshingParameters(const meshingParameters &mp);
    void setLinearDeflection(double ld);
    void setAngularDeflection(double ad);
    void setIsRelative(bool isRelative);
    void setShape(TopoDS_Shape aShape);
    bool perform(const std::string& outputFileName);
    void setFormat(formatType aType);

private:

    //bool import(const std::string& filePath, bool &isSTL, bool &isSTP);
    formatType import(const std::string& filePath);
    bool loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape);

    /*std::string getDeviceIP()
    {
        QNetworkInterface *qni;
        qni = new QNetworkInterface();
        *qni = qni->interfaceFromName(QString("%1").arg("wlan0"));
        return qni->addressEntries().at(0).ip().toString().toStdString();
    }*/
};

#endif // TESSELLATOR_H
