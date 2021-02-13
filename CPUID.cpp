#include "CPUID.h"
//#include <boost/thread.hpp>
#include <thread>
/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <returns></returns>
CPUID::CPUID() :
	vendor{ 0 },
	brand{ 0 }
{}
/// <summary>
/// get_cpuid
/// </summary>
/// <param name="p"></param>
/// <param name="i"></param>
void CPUID::get_cpuid(void* p, int i)
{
	::__cpuid((int*)p, i);
}
/// <summary>
/// get_cpuidex
/// </summary>
/// <param name="p"></param>
/// <param name="i"></param>
/// <param name="c"></param>
void CPUID::get_cpuidex(void* p, int i, int c)
{
	::__cpuidex((int*)p, i, c);
}
void CPUID::GetCpuid(int infoType)
{
	this->get_cpuid(&this->regs, infoType);
}
void CPUID::GetCpuidEx(int infoType, uint32_t ecxValue)
{
	this->get_cpuidex(&this->regs, infoType, ecxValue);
}
void CPUID::cpuID(_T_Register& regs, unsigned i)
{
	::__cpuid((int*)&regs, (int)i);
}
/// <summary>
/// �R�A�����擾
/// </summary>
/// <returns></returns>
int CPUID::GetPhysCores()
{
	return std::thread::hardware_concurrency();
}
#ifdef _USED
/// <summary>
/// �����R�A�����擾
/// </summary>
/// <returns>�����R�A��</returns>
/// <remark>AMD RYZEN5 1600AF�ł͕����R�A��12�ƂȂ��Ă��܂��i���g�p�j
/// </remark>
int CPUID::GetPhysCores()
{
	int physcore = 0;
	// Logical core count per CPU
	_T_Register regs;
	::memset(&regs, NULL, sizeof(regs));
	this->cpuID(regs, 1);
	//unsigned logical = (regs[1] >> 16) & 0xff; // EBX[23:16]
	//unsigned cores = logical;
	auto cpuVendor = this->GetVendor();
	if (cpuVendor == "GenuineIntel")
	{
		// Get DCP cache info
		this->cpuID(regs, 4);
		physcore = ((regs.eax >> 26) & 0x3f) + 1; // EAX[31:26] + 1
	}
	else if (cpuVendor == "AuthenticAMD") {
		// Get NC: Number of CPU cores - 1
		this->cpuID(regs, 0x80000008);
		physcore = ((unsigned)(regs.ecx & 0xff)) + 1; // ECX[7:0] + 1
	}
	//physcore = cores;
	return physcore;
}
#endif
/// <summary>
/// CPU�x���_�[���擾
/// </summary>
/// <returns>std::string CPU�x���_�[��</returns>
std::string CPUID::GetVendor()
{
	this->GetCpuid(0);
	std::string retS = "";
	*(uint32_t*)&vendor[4 * 0] = this->regs.ebx;
	*(uint32_t*)&vendor[4 * 1] = this->regs.edx;
	*(uint32_t*)&vendor[4 * 2] = this->regs.ecx;
	this->vendor[sizeof(vendor) - 1] = 0;
	retS = this->vendor;
	return retS;
}
/// <summary>
/// GetBrand
/// CPU�̃u�����h�����擾
/// </summary>
/// <returns>std::string CPU�̃u�����h��</returns>
std::string CPUID::GetBrand()
{
	std::string retS = "";
	for (int i = 0; i < 3; ++i) {
		this->GetCpuid(i + 0x80000002);
		*(uint32_t*)&brand[16 * i + 4 * 0] = this->regs.eax;
		*(uint32_t*)&brand[16 * i + 4 * 1] = this->regs.ebx;
		*(uint32_t*)&brand[16 * i + 4 * 2] = this->regs.ecx;
		*(uint32_t*)&brand[16 * i + 4 * 3] = this->regs.edx;
	};
	this->brand[sizeof(brand) - 1] = 0;
	retS = this->brand;
	return retS;
}
