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

private:

    TopoDS_Shape m_shape;
    meshingParameters m_mp;
    bool m_shapeLoaded;

private:

    bool loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape);

public:

    tessellator(const std::string& shapeFileFullPath = "");

    void setMeshingParameters(const meshingParameters &mp);
    void setLinearDeflection(double ld);
    void setAngularDeflection(double ad);
    void setIsRelative(bool isRelative);
};

#endif // TESSELLATOR_H