#include "pch.h"
#include <mutex>
#include "PrgbCtrl.h"

/// <summary>
/// プログレスバーコンストラクター
/// </summary>
/// <param name="prgbobj">プログレスバーオブジェクトポインタ</param>
PrgbCtrl::PrgbCtrl(
    CProgressCtrl* prgbobj
) :
    PrgbObj(prgbobj),
    m_RngUpper(D_UPPER_VAL),
    m_IncVal(D_INC_VAL),
    m_incd(0.0),
    m_BasePer(D_BASE_PERCENT)
{
    // プログレスバーレンジ設定
    this->PrgbObj->SetRange(this->m_RngLow, this->m_BasePer);
    /// プログレスバー初期化
    this->Start();
}
/// <summary>
/// プログレスバーコンストラクター
/// </summary>
/// <param name="prgbobj">プログレスバーオブジェクトポインタ</param>
/// <param name="RngUpper">プログレスバー終了点</param>
/// <param name="IncVal">プログレスバー増分</param>
/// <param name="BasePer">パーセント値（デフォルト＝100）</param>
PrgbCtrl::PrgbCtrl(
    CProgressCtrl* prgbobj,
    const uintmax_t RngUpper,
    const uintmax_t IncVal,
    const UINT32 BasePer
) :
    PrgbObj(prgbobj),
    m_RngUpper(RngUpper),
    m_IncVal(IncVal),
    m_incd(0.0),
    m_BasePer(BasePer)
{
    // プログレスバーレンジ設定
    this->PrgbObj->SetRange(this->m_RngLow, this->m_BasePer);
    /// プログレスバー初期化
    this->Start();
}
/// <summary>
/// プログレスバー更新
/// </summary>
/// <returns>進捗率(%)</returns>
UINT32 PrgbCtrl::Inc()
{
    // 増分のパーセントを計算
    this->m_incd += (double)this->m_IncVal;
    double addval = (this->m_incd / this->m_RngUpper) * this->m_BasePer;
    UINT32 addval_int = (UINT32)addval;
    this->PrgbObj->SetPos(addval_int);
    if (this->m_incd == this->m_RngUpper)
    {
        this->m_incd = 0.0;
    }
    return addval_int;
}
/// <summary>
/// プログレスバー初期化
/// </summary>
void PrgbCtrl::Start()
{
    int Red = 0;
    int Green = 0;
    int Blue = 255;
    // プログレスバーの色設定
    this->PrgbObj->SetBarColor(RGB(Red, Green, Blue));
    // プログレスバー初期位置設定
    this->PrgbObj->SetPos(0);
    // プログレスバー加算値初期化
    this->m_incd = 0.0;
}
/// <summary>
/// プログレスバー終了
/// </summary>
void PrgbCtrl::End()
{
    // プログレスバー初期位置設定
    this->PrgbObj->SetPos(100);
}
/// <summary>
/// バーカラーを設定
/// 初期値青から赤に変更する
/// </summary>
/// <param name="perc_val">進捗率(%)</param>
/// <returns></returns>
void PrgbCtrl::SetBarB2RColor(int perc_val)
{
    int Red = 255;
    int Green = 0;
    int Blue = 255;
    // 赤設定
    Red = Red * perc_val / 100;
    // 青設定
    Blue = Blue - (Blue * perc_val / 100);
    // プログレスバーの色設定
    this->PrgbObj->SetBarColor(RGB(Red, Green, Blue));
}