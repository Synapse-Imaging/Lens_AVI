/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Hash_Codes.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Iurii Kim
 * @version 1.0
 * @date 10/21/2024 05:55:30 PM
 */
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
#pragma once
#include <vector>

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

//class FWMDLL_API KEY_CODE;

struct KEY_BUFF
{
	std::vector<std::string> *sysinfo;
	unsigned int *key_check_sum;
	int key_check_sum_pos = 0;
};


class KEY_CODE
{
public:
	KEY_CODE();
	~KEY_CODE();

	void Get_KEY_BUFF(KEY_BUFF& kb);
	void Save_First_Hash(std::string file_name);
	void Create_Final_Hash(std::string file_name);
	unsigned int Create_HASH(std::string& hash_code, std::string hash_code0 = "");
	bool HASH_Verification(std::string file_name);
	void Get_System_Dir(std::string& sys_dir_name);

private:
	KEY_BUFF key_buff;

	void SHA256_GET(std::string input, std::string& out);
	std::string Exec(const char* cmd);
	int Get_HDD_SN(std::string& hdd_sn);
	void Get_HASH_DLL(std::string& hash_code);
	void Get_SYSINFO();
	unsigned int HASH_CheckSum(std::string s);
	unsigned int HASH_Get(std::string& hash_code, int type);
	unsigned int HASH_Get(const std::string str, std::string& hash_code);


};

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */