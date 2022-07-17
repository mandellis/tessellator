//! ----------------
//! custom includes
//! ----------------
#include "tessellator.h"
#include "meshingparameters.h"

//! --------
//! C++ STL
//! --------
#include <iostream>
using namespace std;
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
    cout<<"tessellator::tessellator()->____constructor called____"<<endl;

    //! ---------------------------------------------------------------------------------
    //! init meshing parameters - use the default constructor of meshingParameters class
    //! ---------------------------------------------------------------------------------
    m_mp = meshingParameters();

    //! ------------------------------------------------------------------
    //! load the step file (loadStepFile check also if the path is empty)
    //! meshing, healing, and all the subsequents operations can be done
    //! if m_shapeLoaded == true
    //! ------------------------------------------------------------------
    bool isDone = this->loadStepFile(shapeFileFullPath,m_shape);
    isDone == true? m_shapeLoaded = true : m_shapeLoaded = false;
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

//! -------------------------------------------------
//! function: readStepFile
//! details:  check if 1) the file path is not empty
//!                    2) the file exists
//!                    3) the step file can be read
//! -------------------------------------------------
bool tessellator::loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape)
{
    cout<<"tessellator::loadStepFile()->____function called____"<<endl;

    //! -------------------
    //! preliminary checks
    //! -------------------
    if(stepFilePath.empty()) return false;
    bool fileExists = fs::exists(stepFilePath);
    if(!fileExists) return false;

    //! ------------------------
    //! opencascade step reader
    //! ------------------------
    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(stepFilePath.c_str());
    if(stat != IFSelect_RetDone)
    {
        cout<<"Error in reading the step file"<<endl;
        return false;
    }

    //! ---------------------------------------------------------
    //! diagnostic messages - use if you want to redirect stdout
    //! ---------------------------------------------------------
    //IFSelect_PrintCount mode = IFSelect_CountByItem;
    //reader.PrintCheckLoad(false,mode);

    int NbRoots = reader.TransferRoots();
    cout<<"Number of roots: "<<NbRoots<<endl;

    //! ---------------------------------------------
    //! force the limit on the number of shapes to 1
    //! ---------------------------------------------
    int NbShapes = reader.NbShapes();
    if(NbShapes>1)
    {
        cout<<"The step file contains more than one shape"<<endl;
        return false;
    }

    //! --------------------------------
    //! transfer a shape by rank number
    //! --------------------------------
    const int rank = 1;
    aShape = reader.Shape(rank);
    if(aShape.IsNull())
    {
        cout<<"Null step shape"<<endl;
        return false;
    }
    return true;
}

