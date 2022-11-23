//! --------
//! C++ STL
//! --------
#include <iostream>
using namespace std;

#include <experimental/filesystem>
namespace fs = experimental::filesystem;

//! ---
//! Qt
//! ---
#include <QCoreApplication>

//! ----------------
//! custom includes
//! ----------------
#include "meshingparameters.h"
#include "tessellator.h"

//! -----------------------
//! functions used by main
//! -----------------------
void printUsage();
void printArgumentsOK();
void printTessellatorArguments(const meshingParameters& parameters, std::string inputFileName, std::string outputFileName);
bool inputFileNameToOutputFileName(char* inputFileName, std::string& outputFileName);

int main(int argc, char *argv[])
{
    //! -----------------
    //! QCoreApplication
    //! -----------------
    QCoreApplication a(argc, argv);

    //! ----------------------------------------
    //! a set of default parameters for meshing
    //! ----------------------------------------
    meshingParameters mp(0.1,0.1,true);

    switch(argc)
    {
    //! ---------------------
    //! empty arguments list
    //! ---------------------
    case 1:
        printUsage();
        break;

    //! ----------------
    //! -i <input file>
    //! ----------------
    case 3:
    {
        //! parse arguments "-i <input file>"
        if(strcmp(argv[1],"-i")!=0)
        {
            cout<<"Invalid argument"<<endl;
            return 1;
        }
        printArgumentsOK();

        //! --------------------------------------
        //! generate the default output file name
        //! --------------------------------------
        std::string outputFile;
        bool isDone = inputFileNameToOutputFileName(argv[2],outputFile);
        if(!isDone) return 1;

        //! --------------------------------------------------------------------------------
        //! parse OK: print meshing parameteers and run tessellator with default parameters
        //! --------------------------------------------------------------------------------
        printTessellatorArguments(mp,argv[2],outputFile);
        std::string inputFile(argv[2]);
        tessellator aTessellator(inputFile);
        aTessellator.setMeshingParameters(mp);
        aTessellator.perform(outputFile);
        return a.exit(0);
    }
        break;

    case 5:
    {
        //! parse arguments "-i <input file>"
        if(strcmp(argv[1],"-i")==0 && (strcmp(argv[3],"-ld")==0 || strcmp(argv[3],"-ad")==0 || strcmp(argv[3],"-ir")==0))
        {
            printArgumentsOK();

            //! --------------------------------------
            //! generate the default output file name
            //! --------------------------------------
            std::string outputFile;
            bool isDone = inputFileNameToOutputFileName(argv[2],outputFile);
            if(!isDone) return 1;

            //! ----------------------------------------------------------
            //! the linear deflection is present within the argument list
            //! ----------------------------------------------------------
            if(strcmp(argv[3],"-ld")==0)
            {
                double ld = std::atof(argv[4]);
                if(ld<=0)
                {
                    cout<<"Invalid linear deflection value"<<endl;
                    return 1;
                }
                mp.setLinearDeflection(ld);
            }
            //! -----------------------------------------------------------
            //! the angular deflection is present within the argument list
            //! -----------------------------------------------------------
            if(strcmp(argv[3],"-ad")==0)
            {
                double ad = std::atof(argv[4]);
                if(ad<0)
                {
                    cout<<"Invalid angular deflection value"<<endl;
                    return 1;
                }
                mp.setAngularDeflection(ad);
            }
            if(strcmp(argv[3],"-ir")==0)
            {
                bool isRelative = true;
                if(strcmp(argv[4],"Y")==0 || strcmp(argv[4],"N")==0)
                {
                    isRelative = strcmp(argv[4],"Y")==0 ? true:false;
                }
                else
                {
                    cout<<"Invalid is relative parameter value"<<endl;
                    return 1;
                }
                mp.setIsRelative(isRelative);
            }

            //! --------------------------------------------------------------------------------
            //! parse OK: print meshing parameteers and run tessellator with default parameters
            //! --------------------------------------------------------------------------------
            printTessellatorArguments(mp,argv[2],outputFile);

            std::string inputFile(argv[2]);
            tessellator aTessellator(inputFile);
            aTessellator.setMeshingParameters(mp);
            aTessellator.perform(outputFile);
            return a.exit(0);
        }
    }
        break;

    case 7:
    {
        if(strcmp(argv[1],"-i")==0 &&
                ((strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ad")==0) ||
                 (strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ld")==0) ||
                 (strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ir")==0) ||
                 (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ld")==0) ||
                 (strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ir"))==0) ||
                 (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ad"))==0
                )
        {
            printArgumentsOK();

            //! --------------------------------------
            //! parse arguments "-i <input file>"
            //! generate the default output file name
            //! --------------------------------------
            std::string outputFile;
            bool isDone = inputFileNameToOutputFileName(argv[2],outputFile);
            if(!isDone) return 1;

            double ld = mp.linDefl();
            double ad = mp.angDefl();
            bool isRelative = mp.isRel();

            if((strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ad")==0) || (strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ld")==0))
            {
                if(strcmp(argv[3],"-ld")==0)
                {
                    //! ----------------------
                    //! set linear deflection
                    //! ----------------------
                    ld = std::atof(argv[4]);
                    if(ld<0) { cout<<"Wrong linear deflection parameter"<<endl; return 1; }
                    mp.setLinearDeflection(ld);

                    //! -----------------------
                    //! set angular deflection
                    //! -----------------------
                    ad = std::atof(argv[6]);
                    if(ad<0) { cout<<"Wrong angular deflection parameter"<<endl; return 1; }
                    mp.setAngularDeflection(ad);
                }
                else
                {
                    //! -----------------------
                    //! set angular deflection
                    //! -----------------------
                    ad = std::atof(argv[4]);
                    if(ad<0) { cout<<"Wrong angular deflection parameter"<<endl; return 1; }
                    mp.setAngularDeflection(ad);

                    //! ----------------------
                    //! set linear deflection
                    //! ----------------------
                    ld = std::atof(argv[6]);
                    if(ld<0) { cout<<"Wrong linear deflection parameter"<<endl; return 1; }
                    mp.setLinearDeflection(ld);
                }
            }
            if((strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ir")==0) || (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ld")==0))
            {
                if(strcmp(argv[3],"-ld")==0)
                {
                    //! ----------------------
                    //! set linear deflection
                    //! ----------------------
                    ld = std::atof(argv[4]);
                    if(ld<0) { cout<<"Wrong linear deflection parameter"<<endl; return 1; }
                    mp.setLinearDeflection(ld);

                    //! ----------------
                    //! set is relative
                    //! ----------------
                    if(strcmp(argv[6],"Y")!=0 && strcmp(argv[6],"N")!=0) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                    isRelative = strcmp(argv[6],"Y")==0 ? true:false;
                    mp.setIsRelative(isRelative);
                }
                else
                {
                    //! ----------------
                    //! set is relative
                    //! ----------------
                    //bool isRelative = true;
                    if(strcmp(argv[4],"Y")!=0 && strcmp(argv[4],"N")!=0) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                    isRelative = strcmp(argv[4],"Y")==0 ? true:false;
                    mp.setIsRelative(isRelative);

                    //! ----------------------
                    //! set linear deflection
                    //! ----------------------
                    ld = std::atof(argv[6]);
                    if(ld<0) { cout<<"Wrong linear deflection parameter"<<endl; return 1; }
                    mp.setLinearDeflection(ld);
                }
            }
            if((strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ir")==0) || (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ad")==0))
            {
                if(strcmp(argv[3],"-ad")==0)
                {
                    //! -----------------------
                    //! set angular deflection
                    //! -----------------------
                    //double val = 0;
                    ad = std::atof(argv[4]);
                    if(ad<0) { cout<<"Wrong angular deflection value"<<endl; return 1; }
                    mp.setAngularDeflection(ad);

                    //! ----------------
                    //! set is relative
                    //! ----------------
                    //bool isRelative = true;
                    if(strcmp(argv[6],"Y")!=0 && strcmp(argv[6],"N")) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                    isRelative = strcmp(argv[6],"Y")==0? true: false;
                    mp.setIsRelative(isRelative);
                }
                else
                {
                    //! ----------------
                    //! set is relative
                    //! ----------------
                    //bool isRelative = true;
                    if(strcmp(argv[4],"Y")!=0 && strcmp(argv[6],"N")) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                    isRelative = strcmp(argv[4],"Y")==0? true: false;
                    mp.setIsRelative(isRelative);

                    //! -----------------------
                    //! set angular deflection
                    //! -----------------------
                    //double val = 0;
                    ad = std::atof(argv[6]);
                    if(ad<0) { cout<<"Wrong angular deflection value"<<endl; return 1; }
                    mp.setAngularDeflection(ad);
                }
            }

            //! --------------------------------------------------------------------------------
            //! parse OK: print meshing parameteers and run tessellator with default parameters
            //! --------------------------------------------------------------------------------
            printTessellatorArguments(mp,argv[2],outputFile);
            std::string inputFile(argv[2]);
            tessellator aTessellator(inputFile);
            aTessellator.setMeshingParameters(mp);
            aTessellator.perform(outputFile);
            return a.exit(0);
        }
        else
        {
            cout<<"Wrong parameters list"<<endl;
            return 1;
        }
    }
        break;

    case 9:
    {
        if(strcmp(argv[1],"-i")==0 &&
                ((strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ad")==0 && strcmp(argv[7],"-ir")==0) ||
                 (strcmp(argv[3],"-ld")==0 && strcmp(argv[5],"-ir")==0 && strcmp(argv[7],"-ad")==0) ||
                 (strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ld")==0 && strcmp(argv[7],"-ir")==0) ||
                 (strcmp(argv[3],"-ad")==0 && strcmp(argv[5],"-ir")==0 && strcmp(argv[7],"-ld")==0) ||
                 (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ld")==0 && strcmp(argv[7],"-ad")==0) ||
                 (strcmp(argv[3],"-ir")==0 && strcmp(argv[5],"-ad")==0 && strcmp(argv[7],"-ld")==0)
                 )
                )
        {
            printArgumentsOK();

            //! --------------------------------------
            //! parse arguments "-i <input file>"
            //! generate the default output file name
            //! --------------------------------------
            std::string outputFile;
            bool isDone = inputFileNameToOutputFileName(argv[2],outputFile);
            if(!isDone) return 1;

            //! --------------------------
            //! set the linear deflection
            //! --------------------------
            double val = 0;
            if(strcmp(argv[3],"-ld")==0)
            {
                val = std::atof(argv[4]);
                if(val<0) { cout<<"Invalid linear deflection value"<<endl; return 1; }
            }
            if(strcmp(argv[5],"-ld")==0)
            {
                val = std::atof(argv[6]);
                if(val<0) { cout<<"Invalid linear deflection value"<<endl; return 1; }
            }
            if(strcmp(argv[7],"-ld")==0)
            {
                val = std::atof(argv[8]);
                if(val<0) { cout<<"Invalid linear deflection value"<<endl; return 1; }
            }
            mp.setLinearDeflection(val);

            //! ---------------------------
            //! set the angular deflection
            //! ---------------------------
            if(strcmp(argv[3],"-ad")==0)
            {
                val = std::atof(argv[4]);
                if(val<0) { cout<<"Invalid angular deflection value"<<endl; return 1; }
            }
            if(strcmp(argv[5],"-ad")==0)
            {
                val = std::atof(argv[6]);
                if(val<0) { cout<<"Invalid angular deflection value"<<endl; return 1; }
            }
            if(strcmp(argv[7],"-ad")==0)
            {
                val = std::atof(argv[8]);
                if(val<0) { cout<<"Invalid angular deflection value"<<endl; return 1; }
            }
            mp.setAngularDeflection(val);

            //! --------------------------
            //! set is relative parameter
            //! --------------------------
            bool isRelative = true;
            if(strcmp(argv[3],"-ir")==0)
            {
                if(strcmp(argv[4],"Y")!=0 && strcmp(argv[4],"N")!=0) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                isRelative = strcmp(argv[4],"Y")==0 ? true:false;
            }
            if(strcmp(argv[5],"-ir")==0)
            {
                if(strcmp(argv[6],"Y")!=0 && strcmp(argv[6],"N")!=0) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                isRelative = strcmp(argv[6],"Y")==0 ? true:false;
            }
            if(strcmp(argv[7],"-ir")==0)
            {
                if(strcmp(argv[8],"Y")!=0 && strcmp(argv[8],"N")!=0) { cout<<"Wrong is relative parameter"<<endl; return 1; }
                isRelative = strcmp(argv[8],"Y")==0 ? true:false;
            }
            mp.setIsRelative(isRelative);

            //! --------------------------------------------------------------------------------
            //! parse OK: print meshing parameteers and run tessellator with default parameters
            //! --------------------------------------------------------------------------------
            printTessellatorArguments(mp,argv[2],outputFile);
            std::string inputFile(argv[2]);
            tessellator aTessellator(inputFile);
            aTessellator.setMeshingParameters(mp);
            aTessellator.perform(outputFile);
            return a.exit(0);
        }
        else
        {
            cout<<"Wrong parameters list"<<endl;
            return 1;
        }
    }
        break;
    }

    //return a.exec();
}

//! ---------------------
//! function: printUsage
//! ---------------------
void printUsage()
{
    cout<<"******************************************************"<<endl;
    cout<<"usage: "<<endl;
    cout<<"-i <input file>"<<endl;
    cout<<"-ld <value> [linear deflection value - optional]"<<endl;
    cout<<"-ad <value> [angular deflection value - optional]"<<endl;
    cout<<"-ir Y/N [is relative parameter - optional]"<<endl;
    cout<<endl<<"The ouput files are: <input file>.stl (raw .stl file)"<<endl;
    cout<<"<input file>_H.stl (corrected tessellation)"<<endl;
    cout<<"******************************************************"<<endl;
}

//! ---------------------------
//! function: printArgumentsOK
//! ---------------------------
void printArgumentsOK()
{
    cout<<"arguments ok"<<endl;
}

//! ------------------------------------
//! function: printTessellatorArguments
//! details:
//! ------------------------------------
void printTessellatorArguments(const meshingParameters& parameters, std::string inputFileName, std::string outputFileName)
{
    if(!inputFileName.empty()) cout<<"Input file name: "<<inputFileName<<endl;
    if(!outputFileName.empty()) cout<<"Output file name: "<<outputFileName<<endl;
    cout<<"Meshing parameters: "<<endl;
    cout<<"Linear deflection:  "<<parameters.linDefl()<<endl;
    cout<<"Angular deflection: "<<parameters.angDefl()<<endl;
    cout<<"Is relative: "<<(parameters.isRel()==true? "Y":"N")<<endl;
}

//! ----------------------------------------
//! function: inputFileNameToOutputFileName
//! details:
//! ----------------------------------------
bool inputFileNameToOutputFileName(char* inputFileName, std::string& outputFileName)
{
    //! ---------------------------------------------
    //! check if the input file exists; if not print
    //! "the input file does not exist" and return
    //! ---------------------------------------------
    bool exist = fs::exists(inputFileName);
    if(!exist)
    {
        cout<<"the input file does not exists"<<endl;
        return false;
    }

    //! -----------------------------------------------
    //! the input file exists: now check the extension
    //! -----------------------------------------------
    cout<<"Input file found"<<endl;
    //! ----------------
    //! parse extension
    //! ----------------
    std::string ext,ifn;
    ifn = inputFileName;
    size_t dot = ifn.find_last_of(".");
    if (dot != std::string::npos)
    {
        outputFileName = ifn.substr(0, dot);
        ext  = ifn.substr(dot, ifn.size() - dot);
    }
    if(ext!=".step" && ext!=".STEP" && ext!=".stp" && ext!=".STP" && ext!=".stl" && ext!=".STL")
    {
        cout<<"invalid input file"<<endl;
        return false;
    }
    cout<<"File is "<<ext<<" file"<<endl;
    printArgumentsOK();

    //! -------------------------------------
    //! generate the name of the output file
    //! -------------------------------------
    outputFileName.append(".stl");
    return true;
}
