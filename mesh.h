#ifndef MESH_H
#define MESH_H

#include "macro.h"

//! --------
//! C++ STL
//! --------
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

template <class T>
class simplex
{
protected:

    std::vector<T> m_components;
    int m_id;

public:

    //! constructor
    simplex(const std::vector<T>& components = std::vector<T>(), int id = -1):
        m_components(components),m_id(id){}

    //! copy constructor
    simplex(const std::vector<T>& other)
    {
        m_components = other.m_components;
        m_id = other.m_id;
    }

    //! operator =
    simplex operator = (const simplex& other)
    {
        m_id = other.m_id;
        m_components = other.m_components;
        return *this;
    }

    //! operator ==
    virtual bool operator == (const simplex& other) = 0;

    //! operator !=
    virtual bool operator != (const simplex& other) = 0;

    //! get component
    T comp(int n) const { return m_components[n]; }

    //! get the number of components
    int NbComps() const { return (int)m_components.size(); }

    //! write
    virtual void write(std::ofstream& os)=0;

    //! read
    virtual void read(std::ifstream& is)=0;
};

//! ----------------
//! class meshPoint
//! ----------------
class meshPoint: public simplex<real>
{

public:

    //! constructor
    meshPoint(const std::vector<real>& components = std::vector<real>(), int id = -1):
        simplex(components,id){}

    //! operator ==
    virtual bool operator == (const simplex<real>& other) override
    {
        if(comp(0) == other.comp(0) && comp(1) == other.comp(1) && other.comp(2) == other.comp(2))
            return true;
        return false;
    }

    //! operator !=
    virtual bool operator != (const simplex<real>& other) override
    {
        if(comp(0) != other.comp(0) || comp(1) != other.comp(1) || other.comp(2) != other.comp(2))
            return true;
        return false;
    }

    //! write
    virtual void write(std::ofstream &os) override
    {
        os<<m_id<<std::endl;
        os<<m_components.size()<<std::endl;
        for(int j=0; j<NbComps();j++) os<<comp(j)<<std::endl;
    }

    //! read
    virtual void read(std::ifstream& is) override
    {
        is>>m_id;
        int NbComps = 0;
        is>>NbComps;
        //cout<<NbComps<<endl;
        for(int j=0; j<NbComps; j++)
        {
            real coord;
            is>>coord;
            m_components.push_back(coord);
            //cout<<coord<<endl;
        }
    }

    //! to screen
    void toScreen()
    {
        cout<<m_id<<"(";
        for(int i=0; i<m_components.size()-1; i++) cout<<m_components[i]<<", ";
        cout<<m_components[m_components.size()-1]<<")"<<endl;
    }
};

//! ------------------
//! class meshElement
//! ------------------
class meshElement: public simplex<meshPoint>
{

public:

    //! constructor
    meshElement(const std::vector<meshPoint>& components = std::vector<meshPoint>(), int id = -1):
        simplex(components,id){}

    //! operator ==
    virtual bool operator == (const simplex<meshPoint>& other)
    {
        if(NbComps()!=other.NbComps()) return false;
        for(int j=0; j<NbComps(); j++) if(comp(j)!= other.comp(j)) return false;
        return true;
    }

    //! operator !=
    virtual bool operator != (const simplex<meshPoint>& other)
    {
        if(NbComps()!=other.NbComps()) return true;
        for(int j=0; j<NbComps(); j++) if(comp(j)!= other.comp(j)) return true;
        return false;

    }

    //! write
    virtual void write(std::ofstream &os) override
    {
        os<<m_id<<std::endl;
        os<<m_components.size()<<std::endl;
        for(meshPoint p : m_components) p.write(os);
    }

    //! read
    virtual void read(std::ifstream &is) override
    {
        is>>m_id;
        int NbComps = 0;
        is>>NbComps;
        //cout<<NbComps<<endl;
        for(int j=0; j<NbComps; j++)
        {
            meshPoint p;
            p.read(is);
            m_components.push_back(p);
            //p.toScreen();
        }
    }

    //! to screen
    void toScreen()
    {
        cout<<m_id<<endl<<"{"<<endl;
        for(int i=0; i<m_components.size(); i++) m_components[i].toScreen();
        cout<<"}"<<endl;

    }
};

#endif // MESH_H
