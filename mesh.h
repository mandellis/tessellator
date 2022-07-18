#ifndef MESH_H
#define MESH_H

//! ----------------
//! custom includes
//! ----------------
#include "macro.h"

//! --------
//! C++ STL
//! --------
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include <algorithm>

namespace mesh
{
enum elementType
{
    trig3,
    trig6,
    quad4,
    quad8,
    penta5,
    penta10,
    esa6,
    esa12,
    epta7,
    epta14,
    tet4,
    tet10,
    hexa8,
    hexa20,
    pyram5,
    pyram13,
    prism6,
    prism15
};

//! ----------------------
//! class:   point
//! details: a mesh point
//! ----------------------
class point
{
private:

    real m_x,m_y,m_z;
    int m_id;
    size_t m_hashCode;

public:

    //! constructor
    point(real x = 0, real y = 0, real z = 0, int id = 0):
        m_x(x), m_y(y), m_z(z), m_id(id)
    {
        m_hashCode = 0;
        hash_combine<real>(m_hashCode,m_x);
        hash_combine<real>(m_hashCode,m_y);
        hash_combine<real>(m_hashCode,m_z);
    }

    //! copy contructor
    point(const point& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
        m_z = other.m_z;
        m_id = other.m_id;
    }

    //! operator =
    point operator = (const point& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
        m_z = other.m_z;
        m_id = other.m_id;
        return *this;
    }

    //! operator == (exact equality)
    bool operator == (const point& other)
    {
        if(m_x == other.m_x && m_y == other.m_y && m_z == other.m_z) return true;
        return false;
    }

    //! operator !=
    bool operator != (const point& other)
    {
        if(m_x != other.m_x || m_y != other.m_y || m_z != other.m_z) return true;
        return false;
    }

    //! operator <
    bool operator < (const point& other) const
    {
        size_t seed1 = 0, seed2 = 0;
        hash_combine<real>(seed1,m_x);
        hash_combine<real>(seed1,m_y);
        hash_combine<real>(seed1,m_z);
        hash_combine<real>(seed2,other.m_x);
        hash_combine<real>(seed2,other.m_y);
        hash_combine<real>(seed2,other.m_z);
        if(seed1<seed2) return true;
        return false;
    }

    //! very close to
    bool veryCloseTo(const point& other, real tolerance = 1e-6)
    {
        real dist = sqrt(pow(m_x-other.m_x,2)+pow(m_y-other.m_y,2)+pow(m_z-other.m_z,2));
        if(dist<= tolerance) return true;
        return false;
    }

    //! persistance: write to stream
    void write(std::ofstream &os)
    {
        os<<m_id<<"\t"<<m_x<<"\t"<<m_y<<"\t"<<m_z<<endl;
    }

    //! persistance: read from stream
    bool read(std::ifstream& is)
    {
        std::string line;
        std::getline(is,line);
        if(sscanf_s(line.c_str(),"%d%lf%lf%lf",&m_id,&m_x,&m_y,&m_z)==4) return true;
        return false;
    }

    //! access functions
    inline int id() const { return m_id; }
    inline real x() const { return m_x; }
    inline real y() const { return m_y; }
    inline real z() const { return m_z; }
    inline size_t hashCode() const { return m_hashCode; }
    inline void setID(int id) { m_id = id; }
};

//! ------------------------
//! class:   element
//! details: a mesh element
//! ------------------------
class element
{
private:

    std::vector<mesh::point> m_points;
    int m_id;

public:

    //! constructor
    element(const std::vector<mesh::point>& points = std::vector<mesh::point>() , int id = 0):
        m_points(points),m_id(id)
    {
    }

    //! copy constructor
    element(const element& other)
    {
        m_points = other.m_points;
        m_id = other.m_id;
    }

    //! operator =
    element operator = (const element& other)
    {
        m_points = other.m_points;
        m_id = other.m_id;
        return *this;
    }

    //! operator == (based on coordinates)
    //! this operator does not feel the ordering
    bool operator == (const element& other)
    {
        if(m_points.size()!=other.m_points.size()) return false;
        std::vector<mesh::point> vecPoints1(m_points);
        std::sort(vecPoints1.begin(),vecPoints1.end());
        std::vector<mesh::point> vecPoints2(other.m_points);
        std::sort(vecPoints2.begin(),vecPoints2.end());

        for(size_t i = 0; i<vecPoints1.size(); i++) if(vecPoints1[i]!=vecPoints2[i]) return false;
        return false;
    }

    //! addNode
    void addNode(const mesh::point& aPoint)
    {
        m_points.push_back(aPoint);
    }

    //! persistance: write to stream
    void write(std::ofstream& os)
    {
        os<<m_id<<endl;                                                     //! write id
        os<<m_points.size()<<endl;                                          //! write the number of points
        for(size_t i = 0; i<m_points.size(); i++) m_points[i].write(os);    //! write the points
    }

    //! persistance: read from stream
    bool read(std::ifstream& is)
    {
        m_points.clear();
        std::string line;
        std::getline(is,line);
        if(sscanf_s(line.c_str(),"%d",&m_id)!=1) return false;                //! read the id
        std::getline(is,line);
        int NbPoints = 0;
        if(sscanf_s(line.c_str(),"%d",&NbPoints)!=1) return false;            //! read the number of points
        for(int i=0; i<NbPoints; i++)
        {
            mesh::point P;
            P.read(is);
            m_points.push_back(P);
        }
        return true;
    }
};
}

#endif // MESH_H
