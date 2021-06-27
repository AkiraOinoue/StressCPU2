#include <thread>
#include <atlstr.h>
#include "CPUID.h"
/// <summary>
/// コンストラクタ
/// </summary>
/// <returns></returns>
CPUID::CPUID() :
	vendor{ 0 },
	brand{ 0 },
	m_PhysCoreCnt(0),
	m_LogicProcessorCnt(0),
	IsAMD_Flg{false},
	IsIntel_Flg{false}
{
	// CPUコア情報計算
	this->CalCoreInfo();
}
/// <summary>
/// CPUコア情報計算メソッド
/// </summary>
/// <param name="">なし</param>
/// <remark>
/// Sample Code by Microsoft
/// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
/// </remark>
void CPUID::CalCoreInfo(void)
{
    LPFN_GLPI lpGetLogicalProcessorInformation;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

#pragma warning(disable:6387)
// C6387 Temp_value_#4は0である可能性があります: この動作は、関数 'GetProcAddress' の指定に従っていません。
// この警告を消すため
    lpGetLogicalProcessorInformation = (LPFN_GLPI)GetProcAddress(
        GetModuleHandle("kernel32"),
        "GetLogicalProcessorInformation"
    );
    if (nullptr == lpGetLogicalProcessorInformation)
    {
        ::MessageBox(
            nullptr,
            TEXT("GetLogicalProcessorInformation is not supported."),
            "CPUコア情報",
            MB_ICONERROR
        );
        return;
    }

    while (!done)
    {
        DWORD rc = lpGetLogicalProcessorInformation(
            buffer,
            &returnLength
        );

        if (FALSE == rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer)
                    free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                if (nullptr == buffer)
                {
                    ::MessageBox(
                        nullptr,
                        TEXT("Error: Allocation failure"),
                        "CPUコア情報",
                        MB_ICONERROR
                    );
                    return;
                }
            }
            else
            {
                CString msg = "";
                msg.Format(
                    "Error %d",
                    GetLastError()
                );
                ::MessageBox(
                    nullptr,
                    msg.GetString(),
                    "CPUコア情報",
                    MB_ICONERROR
                );
                return;
            }
        }
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
    {
        switch ((int)ptr->Relationship)
        {
        case (int)RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            break;

        case (int)RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
            break;

        case (int)RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
            Cache = &ptr->Cache;
            if (Cache->Level == 1)
            {
                processorL1CacheCount++;
            }
            else if (Cache->Level == 2)
            {
                processorL2CacheCount++;
            }
            else if (Cache->Level == 3)
            {
                processorL3CacheCount++;
            }
            break;

        case (int)RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            break;

        default:
            ::MessageBox(
                nullptr,
                "Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.",
                "CPUコア情報",
                MB_ICONERROR
            );
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
    free(buffer);
    // 物理コア数
    this->m_PhysCoreCnt = processorCoreCount;
    // 論理プロセッサ数
    this->m_LogicProcessorCnt = logicalProcessorCount;
}
// Helper function to count set bits in the processor mask.
DWORD CPUID::CountSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
	DWORD i;

	for (i = 0; i <= LSHIFT; ++i)
	{
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}
/// <summary>
/// 物理コア数を取得
/// </summary>
/// <param name=""></param>
/// <returns>物理コア数</returns>
int CPUID::GetPhysCoreCnt(void)
{
	return this->m_PhysCoreCnt;
}
/// <summary>
/// 論理プロセッサ数を取得
/// </summary>
/// <param name="">なし</param>
/// <returns>論理プロセッサ数</returns>
int CPUID::GetLogicProcessorCnt(void)
{
	return this->m_LogicProcessorCnt;
}
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
/// CPUベンダー名取得
/// </summary>
/// <returns>std::string CPUベンダー名</returns>
std::string CPUID::GetVendor()
{
	this->GetCpuid(0);
	std::string retS = "";
	*(uint32_t*)&vendor[4 * 0] = this->regs.ebx;
	*(uint32_t*)&vendor[4 * 1] = this->regs.edx;
	*(uint32_t*)&vendor[4 * 2] = this->regs.ecx;
	this->vendor[sizeof(vendor) - 1] = 0;
	retS = this->vendor;
	if (retS == "AuthenticAMD")
	{
		this->IsAMD_Flg = true;
	}
	else if (retS == "GenuineIntel")
	{
		this->IsIntel_Flg = true;
	}
	return retS;
}
/// <summary>
/// GetBrand
/// CPUのブランド名を取得
/// </summary>
/// <returns>std::string CPUのブランド名</returns>
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
/// <summary>
/// Get AMD CPU FLAG
/// </summary>
/// <returns></returns>
bool CPUID::IsAMD()
{
	return this->IsAMD_Flg;
}
/// <summary>
/// Get Intel CPU FLAG
/// </summary>
/// <returns></returns>
bool CPUID::IsIntel()
{
	return this->IsIntel_Flg;
}
