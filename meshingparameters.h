#ifndef MESHINGPARAMETERS_H
#define MESHINGPARAMETERS_H

class meshingParameters
{
private:

    double linearDeflection;
    double angularDeflection;
    bool isRelative;

public:

    //! ------------
    //! constructor
    //! ------------
    explicit meshingParameters(double aLinearDeflection = 0.1, double anAngularDeflection = 0.1, bool aIsRelative = true):
        linearDeflection(aLinearDeflection),
        angularDeflection(anAngularDeflection),
        isRelative(aIsRelative)
    {
    }

    //! -----------------
    //! copy constructor
    //! -----------------
    meshingParameters(const meshingParameters& other)
    {
        linearDeflection = other.linearDeflection;
        angularDeflection = other.angularDeflection;
        isRelative = other.isRelative;
    }

    //! -----------
    //! operator =
    //! -----------
    meshingParameters operator = (const meshingParameters& other)
    {
        linearDeflection = other.linearDeflection;
        angularDeflection = other.angularDeflection;
        isRelative = other.isRelative;
        return *this;
    }

    //! ------------
    //! operator ==
    //! ------------
    bool operator ==(const meshingParameters& other)
    {
        if(linearDeflection==other.linearDeflection &&  angularDeflection ==other.angularDeflection
                && isRelative == other.isRelative)
            return true;
        return false;
    }

    //! ------------
    //! operator !=
    //! ------------
    bool operator !=(const meshingParameters& other)
    {
        if(linearDeflection!=other.linearDeflection ||
                angularDeflection !=other.angularDeflection || isRelative != other.isRelative)
            return true;
        return false;
    }

    //! -----------------
    //! access functions
    //! -----------------
    inline double linDefl() const { return linearDeflection; }
    inline double angDefl() const { return angularDeflection; }
    inline bool isRel() const { return isRelative; }

    inline void setLinearDeflection(double val) { linearDeflection = val; }
    inline void setAngularDeflection(double val) { angularDeflection = val; }
    inline void setIsRelative(bool val) { isRelative = val; }

    void setParameters(double aLinDeflection, double anAngDeflection, bool aIsRelative)
    {
        linearDeflection = aLinDeflection;
        angularDeflection = anAngDeflection;
        isRelative = aIsRelative;
    }

};

#endif // MESHINGPARAMETERS_H
