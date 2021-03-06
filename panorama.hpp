


#ifndef  CV_PANORAMA_H
#define  CV_PANORAMA_H

#include "export.hpp"
#include "defines.hpp"

//#include <cmath>


//opencv, these should be put previously, otherwise errors appear
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"



#include <vector>
using namespace std;


DLL_EXPORT int SphereTo3D_center(double x, double y, double radius, double& gx, double& gy, double& gz);
DLL_EXPORT int SphereTo3D(double x, double y, double radius, double& gx, double& gy, double& gz);

DLL_EXPORT int GrdToSphere_center(double gx, double gy, double gz, double radius, double& ix, double& iy);
DLL_EXPORT int GrdToSphere(double gx, double gy, double gz, double radius, double& ix, double& iy);


//template function for "GrdToSphere"
template<typename T>
int GrdToPanoImage(T gx, T gy, T gz, T radius, T& ix, T& iy)
{

	T sita, fai; //sita-lontitude, fai-latitude

	sita = atan2( gx, gy );
	//sita = atan2( gy, gx );
	if( sita<T(0.0) ) sita += T(2.0*PI);

	fai  = atan2( sqrt(gx*gx+gy*gy), gz );
	//fai  = atan2( gz, sqrt(gx*gx+gy*gy) );
	if( fai<T(0.0) )  fai += T(PI);

	ix = radius*sita;
	iy = radius*fai;

	return 0;
}

template<typename T>
int PanoImageCenterToGrd(T ix, T iy, T radius,T& gx, T& gy, T& gz)
{
	T lon = ix/radius; 
	T lat = iy/radius; 

	gx = radius*sin(lon)*cos(lat);
	gy = radius*cos(lon)*cos(lat);
	gz = radius*sin(lat);

	return 0;
}

template<typename T>
int GrdToPanoImageCenter(T gx, T gy, T gz, T radius, T& ix, T& iy)
{

	T sita, fai; //sita-lontitude, fai-latitude

	sita = atan2( gx, gy );
	//sita = atan2( gy, gx );
	//if( sita<T(0.0) ) sita += T(2.0*PI);

	//fai  = atan2( sqrt(gx*gx+gy*gy), gz );
	fai  = atan2( gz, sqrt(gx*gx+gy*gy) );
	//if( fai<T(0.0) )  fai += T(PI);

	ix = radius*sita;
	iy = radius*fai;

	return 0;
}





//for cylinder projection
//from 3d space to cylinder projection
int GrdToCylinder(double gx, double gy, double gz, double radius, double& ix, double& iy);
//from cylinder projection to 3d space
int CylinderToGrd(double ix, double iy, double radius, double& gx, double& gy, double& gz);



/*  convert spherical panorama image into cylinder projection
*/
DLL_EXPORT int SphereToCilinder(char* infile, char* outfile);




IplImage*  PanoToPlane(IplImage* panoImage,double* direction, double vangle, double hangle);


/* panorama to plane projection
   srcImageFile: panoram file 
   outImageFile: output file
   vangle:    vertical fov angle
   hangle:    horizonal fov angle
   direction: the plane image normal direction
   focalLenRatio:  focalLen / panaram radius
   
output:
	 pR: rotation from the spherical coordinate to projection plane coordinate
	 
*/
DLL_EXPORT int PanoToPlane(char* srcImageFile, char* outImageFile, 
						double  vangle, double hangle, double* direction, double focalLenRatio, 
						double& focalLen, int& outHt, int& outWd,
						double* pR);

DLL_EXPORT int PanoToPlanes(char* srcFile, double anglestep, double vangle, 
							double hangle, double fratio, char* outpath);

//panoram image is projected to several images and save the projection matrix 
DLL_EXPORT int PanoToPlanes(int nImageIndex, char* srcFile, double anglestep,
							double vangle, double hangle, double fratio,
							double* R, double* T, vector<CameraPara>& camParas);



DLL_EXPORT IplImage*  PanoToPlane(IplImage* panoImage, double  vangle, double hangle, 
	double* direction, double focalLenRatio, 
	double& focalLen, int& outHt, int& outWd, double* pR);



//panoram image is projected to several images and save the images and projection matrix 
DLL_EXPORT int PanoToPlanes(IplImage* panoImage, double anglestep,
	double vangle, double hangle, double fratio,
	double* R, double* T, 
	vector<IplImage*>& projImages,
	vector<CameraPara>& camParas);


/* relative pose estimation for spherical panoramic images, based on 5-point algorithm, 
   written by xdh, 2015.7.8
input:
	lPts,rPts: image points in panoramic image
	radius:    the width of panoramic image
output:
	cam:       save the R and T
*/
//DLL_EXPORT int PanoramicRelativePose( vector<Point2DDouble> lPts, vector<Point2DDouble> rPts, int width, CameraPara& cam);



/* simple method: calculate the rotation matrix align 3D vector a to b  
                Ra = b
   but this method may be wrong!!!!
*/
DLL_EXPORT int CalculateAlignRotation(double* a, double* b, double* R);



/* calculate the rotation matrix align 3D vector a to b  
                Ra = b
*/
DLL_EXPORT int CalculateAlignRotation1(double* a, double* b, double* R);



/*
input: relative estimation results, projection model x = RX + T
*/
DLL_EXPORT int GeneratePanoEpipolarImage(double* R, double* T, char* leftFile, char* rightFile);


DLL_EXPORT int GeneratePanoEpipolarImageHeading(double* R, double* T, char* leftFile, char* rightFile);


DLL_EXPORT int GeneratePanoEpipolarImageHeading(double* R, double* T, IplImage* pLeftImage, IplImage* pRightImage);


DLL_EXPORT vector<Point3DDouble> GenerateEpipolarPlaneVectors(Point3DDouble normal,int num);





/*
class for panorama matching specialy: 
including re-projection, feature points detection, matching, and error removal
*/
class CIntegratedPanoMatch
{
public:
	CIntegratedPanoMatch();
	~CIntegratedPanoMatch();

	// initialize the Essential Matrix using POS information
	int InitEpipolarConstraint(CameraPara left, CameraPara right );
	
	//for occasions without POS
	int Match(IplImage* pLeftImage, IplImage* pRightImage, vector<ImagePair>& matches);

	//given POS is right
	int Match(IplImage* pLeftImage, IplImage* pRightImage, CameraPara left, CameraPara right, vector<ImagePair>& matches);

private:

	//essential matrix for panorama pairs
	double m_EM[9];
	double m_R[9];
	double m_T[3];

	//if the essential matrix has been calculated
	bool   m_IsEssentialMatrixReady;

	//
	CameraPara m_leftPanoCam, m_rightPanoCam;
};






#endif

