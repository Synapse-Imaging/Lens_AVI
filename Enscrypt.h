/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Enskrypt.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 10/28/2024 11:52:45 AM
 */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
#ifdef EXPORT_DLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API ENSCRYPT;
using namespace std;

struct DLL_API ENSRYPT_KEY_BLOB
{
	BYTE *pbKeyBlob;
	DWORD dwBlobLen;
};

struct DLL_API ENSRYPT_SIGNATURE
{
	BYTE *pbSignature;
	DWORD dwSigLen;
};


class ENSCRYPT
{

public:
	ENSCRYPT(int type);
public:
	HMODULE InstanceHandle;

	void Read_Signature(string file_name);
	bool Verification();
	int Verify_Signature();

	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Сохраняем уникальные признаки для последующего формирования HASH
	 */
	void Save_First_Hash(string file_name);

#define _MASTER_KEY
#ifdef _MASTER_KEY
public:
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Читаем уникальные признаки для последующего формирования HASH
	 *		Прочитать файл с уникальными признаками
	 *		Сформировать HASH
	 *		Сохранить в файл
	 *		TODO: certutil -hashfile S_Universal_AVI.dll md5
	 */
	bool Save_Final_Hash(string file_name, string szContainer);
#else

#endif // #ifdef _MASTER_KEY
private:
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Получить дамп с Открытым Ключем и подписью
	*/
	int Get_Signature(string data, LPCSTR szContainer, ENSRYPT_KEY_BLOB* key_blob, ENSRYPT_SIGNATURE* signature);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Проверка подписи
	*/
	int Verify_Signature(string data, ENSRYPT_KEY_BLOB* key_blob, ENSRYPT_SIGNATURE* signature);
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------

	/** ---------------------------------------------------------------------------
	* @Summary
	*
	*/
	void Get_SYSINFO(std::vector<std::string> &sysinfo);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Вычислен ие HASH
	*/
	void SHA256_GET(std::string input, std::string& out);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Выполнение командной строки
	*/
	string Exec(const char* cmd);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Получить серийный номер HDD
	*/
	int Get_HDD_SN(std::string& hdd_sn);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Получить HASH HDD
	*/
	void Get_HDD_HASH(string& hash_code);
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Получить HASH DLL
	 */
	void Get_HASH_DLL(string& hash_code);
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Системная директория
	 */
	void Get_System_Dir(string& sys_dir_name);


};




/* ************************************************************************** */
/* Section: Function Prototypes                                               */
/* ************************************************************************** */


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

