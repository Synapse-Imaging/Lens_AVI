/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      PhStereo.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 10/16/2024 11:29:37 AM
 */

/////////////////////////////////////////////
//    Currently used images list		/////
///////////////// 20.11.2024 ////////////////
//1.	Specular Vision
//A.Combined 2D
//B.Specular
//C.Phase X
//D.Phase Y
//E.Curvature1
//F.Curvature2
//
//2.	Diffused Vision
//A.Combined 2D
//B.Curvature1
//C.Curvature2

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once
#include <vector>
#include "Enscrypt.h"

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
#ifdef EXPORT_DLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API S_Universal_AVI;


class S_Universal_AVI
{
	bool m_Verification;


public:
	S_Universal_AVI(std::string key_file_name);
	~S_Universal_AVI();


	bool CUDASetDevice(int device);

	void IsGPUAvailable(std::vector<std::string>* gpu_available);

	// Diffuse
	void Diffuse_CUDA(UINT8 *pIm1x, UINT8 *pIm2x, UINT8 *pIm3x, UINT8 *pIm4x, UINT8 *pImResi, UINT8 *pImC2D,
		//float *pGauss1D, float *pGaussDer1D, 
		float sigma, float mult, float add,
		int width, int height);
	void Diffuse_Res_CUDA(UINT8 *pIm1x, UINT8 *pIm2x, UINT8 *pIm3x, UINT8 *pIm4x, UINT8 *pImResi, UINT8 *pImC2D,
		/*float *pGauss1D, float *pGaussDer1D,*/ float sigma, float mult, float add,
		int width, int height, int res);
	void Diffuse_CUDA_GetCurvature(UINT8 *pImResf, int width, int height, float Hp, float Lp);
	void Diffuse_CUDA_GetAlbedo(UINT8 *pImResi, int width, int height);
	void Diffuse_CUDA_GetSlopeX(float *pImRes, int width, int height);
	void Diffuse_CUDA_GetSlopeY(float *pImRes, int width, int height);

	void PhStereo_CPU(UINT8 *pIm1x, UINT8 *pIm2x, UINT8 *pIm3x, UINT8 *pIm4x,
		float *pImRes, float *pImIX, float *pImIY,
		float *pGauss1D, float *pGaussDer1D, int kernelsz, float mult, float add,
		int width, int height);

	// Specular
	void Specular_CPU(UINT8* image1, UINT8* image2, UINT8* image3, UINT8* image4, UINT8* image1y, UINT8* image2y, UINT8* image3y, UINT8* image4y,
		int width, int height, float* imagePh, UINT8* imageFC, float* imagePhy, UINT8* imageFCy, int psx, int psy, double texd, double teyd);

	int Specular_CUDA(UINT8* pIm1x, UINT8* pIm2x, UINT8* pIm3x, UINT8* pIm4x, UINT8* pIm1y, UINT8* pIm2y, UINT8* pIm3y, UINT8* pIm4y,
		float* pImPhx, UINT8* pImFCx, float* pImPhy, UINT8* pImFCy,
		UINT8* pImResi,
		//float *pGauss1D, float *pGaussDer1D,
		int width, int height,
		int psx, int psy, double texd, double teyd,
		float sigma, float mult, float add);
	void Specular_CUDA_GetCurvature(UINT8 *pImResi, int width, int height, float Hp, float Lp);
	void Specular_CUDA_GetPhaseX(float *pImResi, int width, int height);
	void Specular_CUDA_GetPhaseY(float *pImResi, int width, int height);
	void Specular_CUDA_GetAlbedo(UINT8 *pImResi, int width, int height);
	void Specular_CUDA_GetSpec(UINT8 *pImResi, int width, int height);

	
	void Resize_Image(UINT8 *pIn, UINT8 *pOut, int widthIn, int heightIn, int widthOut, int heightOut);

private:

	void PhStereoS(UINT8* image1, UINT8* image2, UINT8* image3, UINT8* image4, int width, int height, float* imageIX, float* imageIY);
	void convolve1Dh(float* In, float* Out, int nX, int nY, float* kernel, int kSize);
	void convolve1Dv(float* In, float* Out, int nX, int nY, float* kernel, int kSize);


};

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

