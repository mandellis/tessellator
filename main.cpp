#include "mesh.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    meshPoint P1({0.1f,0.2f,0.3f},123);
    meshPoint P2({0.11f,0.21f,0.31f},123);
    meshPoint P3({0.12f,0.22f,0.32f},123);

    ofstream os("C:\\Work\\Repositories\\3ntr\\tessellator_build\\test\\test.txt");
    //P1.write(os);
    //P2.write(os);
    //P3.write(os);

    meshElement E1({P1,P2,P3},100);
    E1.write(os);


    ifstream is("C:\\Work\\Repositories\\3ntr\\tessellator_build\\test\\test.txt");
    //meshPoint P;
    //P.read(is);
    meshElement e;
    e.read(is);
    e.toScreen();

    return 0;
}
