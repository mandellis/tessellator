QT += core
QT -= gui

CONFIG += c++11

TARGET = tessellator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    tessellator.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    meshingparameters.h \
    tessellator.h

INCLUDEPATH += $$PWD/OpenCascade/7.3.0/win64/vc14/inc \

LIBS += -L$$PWD/OpenCascade\7.3.0\win64\vc14\release\lib\
-lTKBin   \
-lTKBinL   \
-lTKBinTObj   \
-lTKBinXCAF   \
-lTKBO   \
-lTKBool   \
-lTKBRep   \
-lTKCAF   \
-lTKCDF   \
-lTKD3DHost   \
-lTKDCAF   \
-lTKDraw   \
-lTKernel   \
-lTKFeat   \
-lTKFillet   \
-lTKG2d   \
-lTKG3d   \
-lTKGeomAlgo   \
-lTKGeomBase   \
-lTKHLR   \
-lTKIGES   \
-lTKIVtk   \
-lTKIVtkDraw   \
-lTKLCAF   \
-lTKMath   \
-lTKMesh   \
-lTKMeshVS   \
-lTKOffset   \
-lTKOpenGl   \
-lTKPrim   \
-lTKQADraw   \
-lTKService   \
-lTKShHealing   \
-lTKStd   \
-lTKStdL   \
-lTKSTEP   \
-lTKSTEP209   \
-lTKSTEPAttr   \
-lTKSTEPBase   \
-lTKSTL   \
-lTKTObj   \
-lTKTObjDRAW   \
-lTKTopAlgo   \
-lTKTopTest   \
-lTKV3d   \
-lTKVCAF   \
-lTKViewerTest   \
-lTKVRML   \
-lTKXCAF   \
-lTKXDEDRAW   \
-lTKXDEIGES   \
-lTKXDESTEP   \
-lTKXMesh   \
-lTKXml   \
-lTKXmlL   \
-lTKXmlTObj   \
-lTKXmlXCAF   \
-lTKXSBase   \
-lTKXSDRAW
