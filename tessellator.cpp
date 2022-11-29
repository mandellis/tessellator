//! ----------------
//! custom includes
//! ----------------
#include "tessellator.h"
#include "meshingparameters.h"
#include "mesh.h"
#include "stlwriter.h"
//#include "networkcheck.h"

//! --------
//! C++ STL
//! --------
#include <iostream>
using namespace std;
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <map>
#include <set>
#include <memory>

//! ------------
//! OpenCascade
//! ------------
#include <STEPControl_Reader.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

//! ---
//! Qt
//! ---
#include <QProcess>
#include <QCoreApplication>
#include <QFile>
#include <QDate>
//#include <qnetwork.h>
//#include <QNetworkAddressEntry>
//#include <qhostaddress.h>
//#include <qtcpsocket.h>
//#include <qnetworkinterface.h>
/*
 * future implementation of mesh in memory
 *
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
*/

//! ----------------------
//! function: constructor
//! details:
//! ----------------------
tessellator::tessellator(const std::string& shapeFileFullPath)
{
    //! ---------------------------------------------------------------------------------
    //! init meshing parameters - use the default constructor of meshingParameters class
    //! ---------------------------------------------------------------------------------
    m_mp = meshingParameters();

    //! ------------------------------------------------------------------
    //! import the input file - STL or STEP
    //! ------------------------------------------------------------------
    formatType = this->import(shapeFileFullPath);

    //! ------------------------------------------------------------------
    //! meshing, healing, and all the subsequents operations can be done
    //! if m_shapeLoaded == true
    //! ------------------------------------------------------------------
    switch (formatType) {
    case format::STL:
        m_shapeLoaded = true;
        break;
    case format::STEP:
        m_shapeLoaded = true;
        break;
    case format::NONE:
        m_shapeLoaded = false;
        break;
    default:
        m_shapeLoaded = false;
        break;
    }
}

//! -------------------------------
//! function: import
//! details:
//! -------------------------------
tessellator::format tessellator::import(const std::string& fp)
{

    /*
     * this is intended to protect the software
     * the executable will run only if the network is connected
     * if the network is connected the machine IP, the time and date
     * will be sent to www.solvosrl.com: usage will be monitored
     *//*
    network n;
    bool connected = n.check();
    if(!connected) { cout<<"Network is not connected"<<endl; }
    cout<<"Network is connected"<<endl;
*//*
    QTcpSocket dnsTestSocket;
    QString localIP="127.0.0.1";    //fall back
    QString googleDns = "8.8.8.83";  //try google DNS or sth. else reliable first
    dnsTestSocket.connectToHost(googleDns, 53);
    if (dnsTestSocket.waitForConnected(3000))
    {
        localIP = dnsTestSocket.localAddress().toString();
    }
    else
    {
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        {
            QString guessedGatewayAddress = address.toString().section( ".",0,2 ) + ".1";

            if (address.protocol() == QAbstractSocket::IPv4Protocol
                    && address != QHostAddress(QHostAddress::LocalHost)
                    )
            {
                dnsTestSocket.connectToHost(guessedGatewayAddress, 53);
                if (dnsTestSocket.waitForConnected(3000))
                {
                    localIP = dnsTestSocket.localAddress().toString();
                    break;
                }
            }
        }
    }*/
    //! --------------------------------------
    //! check date and stop program execution
    //! --------------------------------------
    int y,m,d;
    y = 2022;
    m=12;
    d=31;
    QDate targetDate = QDate::QDate(y,m,d);
    bool canExecute = this->blockProgram(targetDate);
    if(!canExecute) return format::NONE;

    //! ---------------------------------------------------
    //! absolute input file path and output file directory
    //! ---------------------------------------------------
    m_absoluteInputFilePath = fs::absolute(fs::path(fp)).string();

    std::stringstream ss(m_absoluteInputFilePath);
    std::vector<std::string> chunkList;
    std::string chunk;
    while(std::getline(ss, chunk, '\\')) chunkList.push_back(chunk);
    int NbChunks = static_cast<int>(chunkList.size());
    int NbChars = static_cast<int>(chunkList.at(NbChunks-1).size());

    m_outputFileDirectory = m_absoluteInputFilePath;
    for(int i=0; i<NbChars; i++) m_outputFileDirectory.pop_back();

       std::string ext;
       size_t dot = fp.find_last_of(".");
       if (dot != std::string::npos)
           ext  = fp.substr(dot, fp.size() - dot);
      if( ext.compare(".stl")==0) {
           return format::STL;
       }
       if(ext.compare(".stp")==0 ||
               ext.compare(".step")==0 ||
               ext.compare(".STEP")==0||
               ext.compare(".STP")==0){
           bool isDone = this->loadStepFile(fp,m_shape);
           if (isDone) return format::STEP;
           else return format::NONE;
       }
       else
           return format::NONE; ;
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

//! -----------------------------
//! function: read AdMesh Output
//! details:
//! -----------------------------
bool tessellator::readOutput(std::string& filePath){

    ifstream of;
    of.open(filePath,ios::in);
    std::string astring = "============== Size ==============";

    if(of.is_open())
    {
        int count=0;
        string tp;
        int pos=0;
        while(getline(of,tp)){
            count++;
            if(!tp.find(astring))
                pos=count;
        }
        if(pos==0) return false;
        //! return at beginning of the file
        of.clear();
        of.seekg(0,ios::beg);
        //! reset counter
        count = 0;
        while(getline(of,tp)){
            count++;
            if(count<pos) continue;
            cout<<count<<" "<<tp<<endl;
        }
        of.close();


        try {
            fs::remove(filePath);
        }
        catch(const fs::filesystem_error& err) {
            std::cout << "filesystem error: " << err.what() << '\n';
        }
            return true;
    }
    else return false;
}
//! -------------------------------------------------
//! function: readStepFile
//! details:  check if 1) the file path is not empty
//!                    2) the file exists
//!                    3) the step file can be read
//! -------------------------------------------------
bool tessellator::loadStepFile(const std::string& stepFilePath, TopoDS_Shape& aShape)
{
    cout<<"loadStepFile()->____function called____"<<endl;

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
    else cout<<"Step file OK"<<endl;

    //! -------------------
    //! transfer the shape
    //! -------------------
    aShape = reader.OneShape();
    if(aShape.IsNull())
    {
        cout<<"Null step shape"<<endl;
        return false;
    }
    return true;
}

//! -------------------------------------------------
//! function: block Program
//! details:  check the current date and block the
//!           program if current date past the
//!           tagert date
//! -------------------------------------------------
bool tessellator::blockProgram(QDate &targetDate)
{
    QDate curDate = QDate::currentDate();
    if(curDate < targetDate)
    return true;
    else return false;
}


//! ------------------
//! function: perform
//! details:
//! ------------------
bool tessellator::perform(const std::string& outputFileName)
{
    if(!m_shapeLoaded)
    {
        cout<<"____the shape has not been loaded____"<<endl;
        return false;
    }

    //! Absolute file path
    std::string absoluteOutputFilePath = m_outputFileDirectory+outputFileName;
    cout<<"Absolute output file path: "<<absoluteOutputFilePath<<endl;

    switch (formatType) {
    case format::STL:
        cout<<"Working on STL file: skip stl generation"<<endl;
        break;
    case format::STEP:
    {
        cout<<"Working on STEP file: stl extraction"<<endl;

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

        /*{
            //! --------------------------------------------------
            //! retrieve the triangulation - scan the model faces
            //! --------------------------------------------------
            std::set<mesh::point> meshPointsSet;
            std::map<int,mesh::point> meshPointsMap;
            std::set<mesh::element> meshElementsSet;

            int nodeID = 0;
            TopExp_Explorer anExp;
            for(anExp.Init(m_shape,TopAbs_FACE); anExp.More(); anExp.Next())
            {
                TopoDS_Face curFace = TopoDS::Face(anExp.Current());
                if(curFace.IsNull()) continue;
                TopLoc_Location loc;
                const opencascade::handle<Poly_Triangulation>& aFaceTriangulation = BRep_Tool::Triangulation(curFace,loc);
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
                    std::set<mesh::point>::iterator it = meshPointsSet.find(MP1);
                    if(it==meshPointsSet.end())
                    {
                        MP1.setID(++nodeID);
                        meshPointsMap.insert(std::make_pair(nodeID,MP1));
                        aMeshElement.addNode(MP1);
                    }
                    else
                    {
                        aMeshElement.addNode(*it);
                    }
                    mesh::point MP2(P2.X(),P2.Y(),P2.Z());
                    it = meshPointsSet.find(MP2);
                    if(it==meshPointsSet.end())
                    {
                        MP2.setID(++nodeID);
                        meshPointsMap.insert(std::make_pair(nodeID,MP2));
                        aMeshElement.addNode(MP2);
                    }
                    else
                    {
                        aMeshElement.addNode(*it);
                    }
                    mesh::point MP3(P3.X(),P3.Y(),P3.Z());
                    it = meshPointsSet.find(MP3);
                    if(it==meshPointsSet.end())
                    {
                        MP3.setID(++nodeID);
                        meshPointsMap.insert(std::make_pair(nodeID,MP3));
                        aMeshElement.addNode(MP3);
                    }
                    else
                    {
                        aMeshElement.addNode(*it);
                    }

                    //! check if the element already exists

                }
            }
        }*/

        //! ----------------------------------
        //! Write the .stl file as is on disk
        //! ----------------------------------
        STLWriter::Write(m_shape,absoluteOutputFilePath.c_str());
    }
        break;
    default:
        break;
    }

    //! -----------------------------------------------------------
    //! locate ADMesh and run it with QProcess or system:
    //! generate the output file name and write the stl file.
    //! If you want to perform a custom set of checks using ADMesh
    //!
    //! --exact --nearby --tolerance=<tolerance value>
    //! --iterations=<number of iterations> --exact
    //! --normal-directions --normal-values
    //! -----------------------------------------------------------

    std::string pathOfExecutable = QCoreApplication::applicationDirPath().toStdString();
    std::string pathOfADMesh = pathOfExecutable+"\\tex.exe";
    std::string absoluteOutputFilePath_H = absoluteOutputFilePath;
    for(int i=0; i<4; i++) absoluteOutputFilePath_H.pop_back();
    absoluteOutputFilePath_H += "_H.stl";
    cout<<"Full path of the corrected tessellation: "<<absoluteOutputFilePath_H<<endl;

    //! -------------
    //! use QProcess
    //! -------------
    std::string admeshout = "out.txt";

    std::shared_ptr<QProcess> healingProcess = std::make_shared<QProcess>();
    healingProcess->setProgram(QString::fromStdString(pathOfADMesh));
    cout<<"Absolute input file path: "<<m_absoluteInputFilePath<<endl;
    QStringList arguments;
    arguments<<QString::fromStdString(absoluteOutputFilePath)<<QString("-a")<<QString::fromStdString(absoluteOutputFilePath_H);
    healingProcess->setArguments(arguments);
    //! redirect adMesh output to ext file for futher reading
    healingProcess->setStandardOutputFile(QString::fromStdString(admeshout));
    cout<<"Mesh healing started"<<endl;
    healingProcess->start();
    bool isDone = healingProcess->waitForFinished(-1);
    std::string out;
    out = (isDone==0)? "Mesh healing failed " : "Mesh healing completed ";

    //! read AdMesh out and print some stats to console
    bool isFinished=false;
    if(isDone){
        isFinished = this->readOutput(pathOfExecutable+"\\"+admeshout);
        cout<<out<<endl;
    }
    else{
        return isDone;
        cout<<out<<endl;
    }
    return isFinished;

    //! -----------------------------------------------
    //! use system - redirect ADMesh output to console
    //! -----------------------------------------------
    //std::string command = pathOfADMesh+" "+absoluteOutputFilePath+" -a "+absoluteOutputFilePath_H;
    //int exitCode = system(command.c_str());
    //return (exitCode == 0? true: false);
}
