#include <iostream>
using namespace std;

#include <experimental/filesystem>
namespace fs = experimental::filesystem;

//! ----------------
//! custom includes
//! ----------------
#include "meshingparameters.h"

void printUsage()
{
    cout<<"usage: "<<endl;
    cout<<"-i <input file>"<<endl;
    cout<<"-o <output file>"<<endl;
}

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
    //printArgumentsOK();

    //! ----------------
    //! parse extension
    //! ----------------
    int len = static_cast<int>(strlen(inputFileName));

    //! take five chars from the end
    std::string extension1;
    for(int i=5; i>0; i--)
    {
        extension1.push_back(inputFileName[len-i]);
        //cout<<extension1<<endl;
    }
    //! take four chars from the end
    std::string extension2;
    for(int i=4; i>0; i--)
    {
        extension2.push_back(inputFileName[len-i]);
        //cout<<extension2<<endl;
    }

    if(extension1!=".step" && extension1!=".STEP" && extension2!=".stp" && extension2!=".STP")
    {
        cout<<"invalid input file"<<endl;
        return false;
    }
    printArgumentsOK();

    //! -------------------------------------
    //! generate the name of the output file
    //! -------------------------------------
    outputFileName = std::string(inputFileName);
    int count = 0;
    if(extension1==".step" || extension1==".STEP") count = 5;
    if(extension2==".stp" || extension2 ==".STP") count = 4;
    for(int i=0; i<count; i++) outputFileName.pop_back();
    outputFileName.append(".stl");
    //cout<<outputFileName<<endl;
    return true;
}

int main(int argc, char *argv[])
{
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

        return 0;
    }
        break;

    case 5:
    {
        //! parse arguments "-i <input file>"
        if(strcmp(argv[1],"-i")==0 && (strcmp(argv[3],"-ld")==0 || strcmp(argv[3],"-ad")==0))
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
            printTessellatorArguments(mp,argv[2],outputFile);
        }
    }
        break;
    }
}
