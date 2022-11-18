//! ----------------
//! custom includes
//! ----------------
#include "stlwriter.h"
using namespace STLWriter;

//! ------------
//! OpenCascade
//! ------------
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//! ---------------------------
//! function: TriangleAccessor
//! details:
//! ---------------------------
STLWriter::TriangleAccessor::TriangleAccessor(const TopoDS_Face& aFace)
{
    TopLoc_Location aLoc;
    myPoly = BRep_Tool::Triangulation (aFace, aLoc);
    myTrsf = aLoc.Transformation();
    myNbTriangles = (myPoly.IsNull() ? 0 : myPoly->Triangles().Length());
    myInvert = (aFace.Orientation() == TopAbs_REVERSED);
    if (myTrsf.IsNegative()) myInvert = ! myInvert;
}

//! ----------------------
//! function: GetTriangle
//! details:
//! ----------------------
void STLWriter::TriangleAccessor::GetTriangle(int iTri, gp_Vec &theNormal, gp_Pnt &thePnt1, gp_Pnt &thePnt2, gp_Pnt &thePnt3)
{
    int iNode1, iNode2, iNode3;
    myPoly->Triangles()(iTri).Get (iNode1, iNode2, iNode3);
    thePnt1 = myPoly->Nodes()(iNode1);
    thePnt2 = myPoly->Nodes()(myInvert ? iNode3 : iNode2);
    thePnt3 = myPoly->Nodes()(myInvert ? iNode2 : iNode3);

    //! apply transormation if not identity
    if (myTrsf.Form() != gp_Identity)
    {
        thePnt1.Transform (myTrsf);
        thePnt2.Transform (myTrsf);
        thePnt3.Transform (myTrsf);
    }

    //! compute normal
    theNormal = (thePnt2.XYZ() - thePnt1.XYZ()) ^ (thePnt3.XYZ() - thePnt1.XYZ());
    Standard_Real aNorm = theNormal.Magnitude();
    if (aNorm > gp::Resolution()) theNormal /= aNorm;
}

//! ----------------
//! function: Write
//! details:
//! ----------------
bool STLWriter::Write(const TopoDS_Shape& theShape, const char *theFileName)
{
    FILE* aFile = fopen(theFileName, "wb");
    if (aFile==nullptr) return false;
    fprintf (aFile, "solid shape\n");
    for (TopExp_Explorer exp (theShape, TopAbs_FACE); exp.More(); exp.Next())
    {
        TriangleAccessor aTool (TopoDS::Face (exp.Current()));
        for (int iTri = 1; iTri <= aTool.NbTriangles(); iTri++)
        {
            gp_Vec aNorm;
            gp_Pnt aPnt1, aPnt2, aPnt3;
            aTool.GetTriangle (iTri, aNorm, aPnt1, aPnt2, aPnt3);
            fprintf (aFile,
                     " facet normal %.6f %.6f %.6f\n"
                     "   outer loop\n"
                     "     vertex %.6f %.6f %.6f\n"
                     "     vertex %.6f %.6f %.6f\n"
                     "     vertex %.6f %.6f %.6f\n"
                     "   endloop\n"
                     " endfacet\n",
                     aNorm.X(), aNorm.Y(), aNorm.Z(),
                     aPnt1.X(), aPnt1.Y(), aPnt1.Z(),
                     aPnt2.X(), aPnt2.Y(), aPnt2.Z(),
                     aPnt3.X(), aPnt3.Y(), aPnt3.Z()
                     );
        }
    }
    fprintf (aFile, "endsolid shape\n");
    fclose (aFile);
    return true;
}
