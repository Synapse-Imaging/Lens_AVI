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
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
#pragma once
#include <vector>
#include "KEY_CODE.h"

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
#if defined(USE_DLL)
#ifdef FWMDLL_EXPORTS
#define FWMDLL_API __declspec(dllexport)
#else
#define FWMDLL_API __declspec(dllimport)
#endif
#else
#define FWMDLL_API
#endif

class FWMDLL_API PH_STEREO;


class PH_STEREO
{

	KEY_BUFF key_buff;
	bool m_Verification;

public:
	PH_STEREO(std::string key_file_name);
	~PH_STEREO();


	KEY_BUFF kb() const { return key_buff; };

	bool CUDASetDevice(int device);

	void IsGPUAvailable(std::vector<std::string>* gpu_available);

	void PhStereo_CUDA(UINT8 *pIm1x, UINT8 *pIm2x, UINT8 *pIm3x, UINT8 *pIm4x, UINT8 *pImResi, UINT8 *pImC2D,
		float *pGauss1D, float *pGaussDer1D, int kernelsz, float mult, float add,
		int width, int height);

	void PhStereo_CPU(UINT8 *pIm1x, UINT8 *pIm2x, UINT8 *pIm3x, UINT8 *pIm4x,
		float *pImRes, float *pImIX, float *pImIY,
		float *pGauss1D, float *pGaussDer1D, int kernelsz, float mult, float add,
		int width, int height);

private:

	void PhStereoS(UINT8* image1, UINT8* image2, UINT8* image3, UINT8* image4, int width, int height, float* imageIX, float* imageIY);
	void convolve1Dh(float* In, float* Out, int nX, int nY, float* kernel, int kSize);
	void convolve1Dv(float* In, float* Out, int nX, int nY, float* kernel, int kSize);


};

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

