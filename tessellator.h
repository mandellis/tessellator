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

class tessellator
{
public:
    enum class sT{
        STL,
        STP
    };
    void setType(sT type);

private:

    TopoDS_Shape m_shape;
    meshingParameters m_mp;
    bool m_shapeLoaded;
    //sT aaa_1;
    std::string m_absoluteInputFilePath;
    std::string m_outputFileDirectory;

private:

    //bool import(const std::string& filePath, bool &isSTL, bool &isSTP);
    bool import(const std::string& filePath,sT type);
    bool loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape);

    std::string getDeviceIP()
    {
        QNetworkInterface *qni;
        qni = new QNetworkInterface();
        *qni = qni->interfaceFromName(QString("%1").arg("wlan0"));
        return qni->addressEntries().at(0).ip().toString().toStdString();
    }

public:

    tessellator(const std::string& shapeFileFullPath = "");

    void setMeshingParameters(const meshingParameters &mp);
    void setLinearDeflection(double ld);
    void setAngularDeflection(double ad);
    void setIsRelative(bool isRelative);
    void setShape(TopoDS_Shape aShape);
    bool perform(const std::string& outputFileName);



};

#endif // TESSELLATOR_H
