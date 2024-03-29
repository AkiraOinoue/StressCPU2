#pragma once
#include <afxcmn.h>
#include <cstdint>
#define D_UPPER_VAL 100
#define D_INC_VAL 10
#define D_BASE_PERCENT 100
class PrgbCtrl
{
public:
    /// <summary>
    /// プログレスバーコンストラクター
    /// </summary>
    /// <param name="prgbobj">プログレスバーオブジェクトポインタ</param>
    PrgbCtrl(CProgressCtrl* prgbobj);
    /// <summary>
    /// プログレスバーコンストラクター
    /// </summary>
    /// <param name="prgbobj">プログレスバーオブジェクトポインタ</param>
    /// <param name="RngUpper">プログレスバー終了点</param>
    /// <param name="IncVal">プログレスバー増分</param>
    /// <param name="BasePer">パーセント値（デフォルト＝100）</param>
    PrgbCtrl(
        CProgressCtrl* prgbobj,
        const uintmax_t RngUpper,
        const uintmax_t IncVal,
        const UINT32 BasePer = 100
    );
    /// <summary>
    /// FLOPS値MAX値設定
    /// </summary>
    /// <param name="flops_max">FLOPSのMAX値</param>
    void SetFlopsRange(double flops_max);
    /// <summary>
    /// FLOPS値設定
    /// </summary>
    /// <param name="flops">FLOPS値</param>
    void SetFlops(double flops);
    /// <summary>
    /// プログレスバー更新のステップ数
    /// </summary>
    /// <param name="var"></param>
    void SetStep(int var);
    /// <summary>
    /// プログレスバー更新
    /// </summary>
    /// <returns>進捗率(%)</returns>
    UINT32 Inc();
    /// <summary>
    /// プログレスバー更新
    /// </summary>
    /// <param name="var">折り返し点</param>
    /// <returns>進捗率(%)</returns>
    UINT32 Inc(double var);
    /// <summary>
    /// バーカラーを設定
    /// 初期値青から赤に変更する
    /// </summary>
    /// <param name="perc_val">進捗率(%)</param>
    /// <returns>なし</returns>
    void SetBarB2RColor(int perc_val);
    /// <summary>
    /// バーカラーを設定
    /// 初期値緑からオレンジに変更する
    /// </summary>
    /// <param name="perc_val">進捗率(%)</param>
    /// <returns>なし</returns>
    void SetBarG2OColor(int perc_val);
    /// <summary>
    /// プログレスバー終了
    /// </summary>
    void End();
    /// <summary>
    /// プログレスバー初期化
    /// </summary>
    void Start();
    // プログレスバーオブジェクト
    CProgressCtrl* PrgbObj;   // プログレスバーオブジェクトポインタ
private:
    const short m_RngLow = 0;   // プログレスバー開始点
    uintmax_t m_RngUpper;     // プログレスバー終了点
    uintmax_t m_IncVal;   // プログレスバー増分
    const UINT32 m_BasePer;     // パーセント値
    double m_incd;              // プログレスバー加算分
};

