//! ----------------
//! custom includes
//! ----------------
#include "tessellator.h"
#include "meshingparameters.h"
#include "mesh.h"

//! --------
//! C++ STL
//! --------
#include <iostream>
using namespace std;
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <map>
#include <set>

//! ------------
//! OpenCascade
//! ------------
#include <STEPControl_Reader.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

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

//! --------------------
//! function: doMeshing
//! details:
//! --------------------
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <algorithm>
bool tessellator::perform()
{
    if(!m_shapeLoaded)
    {
        cout<<"tessellator::perform()->____the shape has not been loaded____"<<endl;
        return false;
    }

    //! -----------------------------------------------
    //! clear the default triangulation from the shape
    //! -----------------------------------------------
    BRepTools::Clean(m_shape);

    //! -------------------------------------
    //! set the parameters of the OCC mesher
    //! -------------------------------------
    BRepMesh_IncrementalMesh aMesher;
    aMesher.ChangeParameters().Angle = m_mp.angDefl();
    aMesher.ChangeParameters().Deflection = m_mp.linDefl();
    aMesher.ChangeParameters().Relative = m_mp.isRel();

    aMesher.ChangeParameters().AdaptiveMin = false;                 //! default option for BRepMesh_IncrementalMesh
    aMesher.ChangeParameters().InternalVerticesMode = false;        //! the meaning is clear from the documentation
    aMesher.ChangeParameters().ControlSurfaceDeflection = true;     //! to be studied
    aMesher.ChangeParameters().InParallel = true;                   //! forced to be true

    aMesher.SetShape(m_shape);
    aMesher.Perform();

    //! --------------------------------------------------
    //! retrieve the triangulation - scan the model faces
    //! --------------------------------------------------
    std::set<mesh::point> meshPointsSet;
    std::map<int,mesh::point> meshPointsMap;
    int nodeID = 0;
    TopExp_Explorer anExp;
    for(anExp.Init(m_shape,TopAbs_FACE); anExp.More(); anExp.Next())
    {
        TopoDS_Face curFace = TopoDS::Face(anExp.Current());
        if(curFace.IsNull()) continue;
        TopLoc_Location loc;
        opencascade::handle<Poly_Triangulation> aFaceTriangulation = BRep_Tool::Triangulation(curFace,loc);
        const Poly_Array1OfTriangle& triangles = aFaceTriangulation->Triangles();
        for(int triangleID = 1; triangleID<=triangles.Size(); triangleID++)
        {
            mesh::element aMeshElement;
            const Poly_Triangle& aTriangle = triangles.Value(triangleID);
            int N1,N2,N3;
            aTriangle.Get(N1,N2,N3);
            const gp_Pnt& P1 = aFaceTriangulation->Node(N1);
            const gp_Pnt& P2 = aFaceTriangulation->Node(N2);
            const gp_Pnt& P3 = aFaceTriangulation->Node(N3);

            mesh::point MP1(P1.X(),P1.Y(),P1.Z());
            if(meshPointsSet.count(MP1)==0)
            {
                MP1.setID(++nodeID);
                meshPointsMap.insert(std::make_pair(nodeID,MP1));
                aMeshElement.addNode(MP1);
            }
            else
            {
                std::find(meshPointsSet,MP1);
            }
            mesh::point MP2(P2.X(),P2.Y(),P2.Z());
            if(meshPointsSet.count(MP2)==0)
            {
                MP2.setID(++nodeID);
                meshPointsMap.insert(std::make_pair(nodeID,MP2));
                aMeshElement.addNode(MP2);
            }
            mesh::point MP3(P3.X(),P3.Y(),P3.Z());
            if(meshPointsSet.count(MP1)==0)
            {
                MP3.setID(++nodeID);
                meshPointsMap.insert(std::make_pair(nodeID,MP3));
                aMeshElement.addNode(MP3);
            }
            else
            {

            }
        }
    }
    return true;
}

