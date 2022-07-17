//! ----------------
//! custom includes
//! ----------------
#include "tessellator.h"
#include "meshingparameters.h"

//! ----
//! C++
//! ----
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

//! ------------
//! OpenCascade
//! ------------
#include <STEPControl_Reader.hxx>

//! ----------------------
//! function: constructor
//! details:
//! ----------------------
tessellator::tessellator(const std::string& shapeFileFullPath)
{
    //! init meshing parameters
    m_mp = meshingParameters();
}

//! -------------------------------
//! function: setMeshingParameters
//! details:
//! -------------------------------
void tessellator::setMeshingParameters(const meshingParameters& mp)
{
    m_mp = mp;
}

//! ------------------------------
//! function: setLinearDeflection
//! details:
//! ------------------------------
void tessellator::setLinearDeflection(double ld)
{
    m_mp.setLinearDeflection(ld);
}

//! -------------------------------
//! function: setAngularDeflection
//! details:
//! -------------------------------
void tessellator::setAngularDeflection(double ad)
{
    m_mp.setAngularDeflection(ad);
}

//! ------------------------
//! function: setIsRelative
//! details:
//! ------------------------
void tessellator::setIsRelative(bool isRelative)
{
    m_mp.setIsRelative(isRelative);
}

//! -----------------------
//! function: readStepFile
//! details:
//! -----------------------
bool tessellator::loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape)
{
    if(stepFilePath.empty()) return false;
    bool fileExists = fs::exists(stepFilePath);
    if(!fileExists) return false;

    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(stepFilePath.c_str());

    return true;
}
