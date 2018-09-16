#include <QtGlobal>
#include <gdal.h>
#include <osg/Version>
#include <iostream>

using namespace std;
int main(int argc, char** argv)
{
    cout<<"Qt Version: "<<QT_VERSION_STR<<endl;
    cout<<"GDAL: "<<GDAL_RELEASE_NAME<<endl;
    cout<<"OSG: "<<osgGetVersion()<<endl;
    return 0;
}
