#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include <QObject>

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

class tessellator: public QObject
{

private:

    TopoDS_Shape m_shape;
    meshingParameters m_mp;
    bool m_shapeLoaded;
    std::string m_outputFileDirectory;

private:

    bool loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape);

public:

    tessellator(const std::string& shapeFileFullPath = "", QObject* parent=0);

    void setMeshingParameters(const meshingParameters &mp);
    void setLinearDeflection(double ld);
    void setAngularDeflection(double ad);
    void setIsRelative(bool isRelative);
    bool perform(const std::string& outputFileName);
};

#endif // TESSELLATOR_H
