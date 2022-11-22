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

class tessellator
{
public:
    enum format{
        STL,
        STEP,
        NONE
    };

private:

    format  formatType;
    TopoDS_Shape m_shape;
    meshingParameters m_mp;
    bool m_shapeLoaded;
    std::string m_absoluteInputFilePath;
    std::string m_outputFileDirectory;

private:

    bool loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape);
    format import(const std::string& fp = "");

public:

    tessellator(const std::string& shapeFileFullPath = "");

    void setMeshingParameters(const meshingParameters &mp);
    void setLinearDeflection(double ld);
    void setAngularDeflection(double ad);
    void setIsRelative(bool isRelative);
    bool perform(const std::string& outputFileName);
};

#endif // TESSELLATOR_H
