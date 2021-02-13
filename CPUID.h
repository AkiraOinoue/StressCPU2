#pragma once
#include <intrin.h>
#include <stdint.h>
#include <string>

//�����v���Z�b�T/�R�A�̐����v���O�����Ō��o���邩�A�n�C�p�[�X���b�f�B���O��Windows�AMac�ALinux�ŃA�N�e�B�u���ǂ��������o���܂�
//https://www.it-swarm-ja.tech/ja/c++/%e7%89%a9%e7%90%86%e3%83%97%e3%83%ad%e3%82%bb%e3%83%83%e3%82%b5%e3%82%b3%e3%82%a2%e3%81%ae%e6%95%b0%e3%82%92%e3%83%97%e3%83%ad%e3%82%b0%e3%83%a9%e3%83%a0%e3%81%a7%e6%a4%9c%e5%87%ba%e3%81%99%e3%82%8b%e3%81%8b%e3%80%81%e3%83%8f%e3%82%a4%e3%83%91%e3%83%bc%e3%82%b9%e3%83%ac%e3%83%83%e3%83%87%e3%82%a3%e3%83%b3%e3%82%b0%e3%81%8cwindows%e3%80%81mac%e3%80%81linux%e3%81%a7%e3%82%a2%e3%82%af%e3%83%86%e3%82%a3%e3%83%96%e3%81%8b%e3%81%a9%e3%81%86%e3%81%8b%e3%82%92%e6%a4%9c%e5%87%ba%e3%81%97%e3%81%be%e3%81%99/969584174/
typedef struct
{
	unsigned eax;
	unsigned ebx;
	unsigned ecx;
	unsigned edx;
} _T_Register;
class CPUID
{
public:
	/// <summary>
	/// CPU�x���_�[���擾
	/// </summary>
	/// <returns>std::string CPU�x���_�[��</returns>
	std::string GetVendor();
	/// <summary>
	/// GetBrand
	/// CPU�̃u�����h�����擾
	/// </summary>
	/// <returns>std::string CPU�̃u�����h��</returns>
	std::string GetBrand();
#ifdef _USED
	/// <summary>
	/// �����R�A�����擾
	/// </summary>
	/// <returns></returns>
	int GetPhysCores();
#endif
	/// <summary>
	/// �R�A�����擾
	/// �i�X���b�h���j
	/// </summary>
	/// <returns></returns>
	int GetPhysCores();
private:
	//uint32_t eax, ebx, ecx, edx; // Do not change member order.
	_T_Register regs;
	void cpuID(_T_Register& regs, unsigned i);
private:
	/// <summary>
	/// CPUID�x���_���g���p
	/// </summary>
	char vendor[12 + 1];
	/// <summary>
	/// CPUID�u�����h��
	/// </summary>
	char brand[16 * 3 + 1];

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <returns></returns>
	CPUID();
private:
	/// <summary>
	/// CPUID���擾
	/// </summary>
	/// <param name="infoType">���^�C�v</param>
	void GetCpuid(int infoType);
	/// <summary>
	/// CPUID���擾
	/// </summary>
	/// <param name="infoType">���^�C�v</param>
	void GetCpuidEx(
		int infoType,
		uint32_t ecxValue
	);
	/// <summary>
	/// get_cpuid
	/// </summary>
	/// <param name="p"></param>
	/// <param name="i"></param>
	void get_cpuid(void* p, int i);
	/// <summary>
	/// get_cpuidex
	/// </summary>
	/// <param name="p"></param>
	/// <param name="i"></param>
	/// <param name="c"></param>
	void get_cpuidex(void* p, int i, int c);
};

