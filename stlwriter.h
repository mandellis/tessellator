#ifndef STLWRITER_H
#define STLWRITER_H

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

class StlMesh_Mesh;
class Poly_Triangulation;

namespace STLWriter
{
//! begin namespace

//! ------------------------------------------------
//! Tool to get triangles from triangulation taking
//! into account face orientation and location
//! ------------------------------------------------
class TriangleAccessor
{
public:

    TriangleAccessor (const TopoDS_Face& aFace);
    int NbTriangles () const { return myNbTriangles; }
    void GetTriangle (int iTri, gp_Vec &theNormal, gp_Pnt &thePnt1, gp_Pnt &thePnt2, gp_Pnt &thePnt3);

private:

    opencascade::handle<Poly_Triangulation> myPoly;
    gp_Trsf myTrsf;
    int myNbTriangles;
    bool myInvert;
};

bool Write(const TopoDS_Shape& aShape, const char* aFileName);

//! end namespace
};
#endif // STLWRITER_H
