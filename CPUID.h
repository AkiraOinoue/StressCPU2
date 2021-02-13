#pragma once
#include <intrin.h>
#include <stdint.h>
#include <string>

//物理プロセッサ/コアの数をプログラムで検出するか、ハイパースレッディングがWindows、Mac、Linuxでアクティブかどうかを検出します
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
	/// CPUベンダー名取得
	/// </summary>
	/// <returns>std::string CPUベンダー名</returns>
	std::string GetVendor();
	/// <summary>
	/// GetBrand
	/// CPUのブランド名を取得
	/// </summary>
	/// <returns>std::string CPUのブランド名</returns>
	std::string GetBrand();
#ifdef _USED
	/// <summary>
	/// 物理コア数を取得
	/// </summary>
	/// <returns></returns>
	int GetPhysCores();
#endif
	/// <summary>
	/// コア数を取得
	/// （スレッド数）
	/// </summary>
	/// <returns></returns>
	int GetPhysCores();
private:
	//uint32_t eax, ebx, ecx, edx; // Do not change member order.
	_T_Register regs;
	void cpuID(_T_Register& regs, unsigned i);
private:
	/// <summary>
	/// CPUIDベンダ名拡張用
	/// </summary>
	char vendor[12 + 1];
	/// <summary>
	/// CPUIDブランド名
	/// </summary>
	char brand[16 * 3 + 1];

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <returns></returns>
	CPUID();
private:
	/// <summary>
	/// CPUID情報取得
	/// </summary>
	/// <param name="infoType">情報タイプ</param>
	void GetCpuid(int infoType);
	/// <summary>
	/// CPUID情報取得
	/// </summary>
	/// <param name="infoType">情報タイプ</param>
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

