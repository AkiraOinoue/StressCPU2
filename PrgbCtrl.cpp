#include "pch.h"
#include <mutex>
#include "PrgbCtrl.h"

/// <summary>
/// �v���O���X�o�[�R���X�g���N�^�[
/// </summary>
/// <param name="prgbobj">�v���O���X�o�[�I�u�W�F�N�g�|�C���^</param>
PrgbCtrl::PrgbCtrl(
    CProgressCtrl* prgbobj
) :
    PrgbObj(prgbobj),
    m_RngUpper(D_UPPER_VAL),
    m_IncVal(D_INC_VAL),
    m_incd(0.0),
    m_BasePer(D_BASE_PERCENT)
{
    // �v���O���X�o�[�����W�ݒ�
    this->PrgbObj->SetRange(this->m_RngLow, this->m_BasePer);
    /// �v���O���X�o�[������
    this->Start();
}
/// <summary>
/// �v���O���X�o�[�R���X�g���N�^�[
/// </summary>
/// <param name="prgbobj">�v���O���X�o�[�I�u�W�F�N�g�|�C���^</param>
/// <param name="RngUpper">�v���O���X�o�[�I���_</param>
/// <param name="IncVal">�v���O���X�o�[����</param>
/// <param name="BasePer">�p�[�Z���g�l�i�f�t�H���g��100�j</param>
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
    // �v���O���X�o�[�����W�ݒ�
    this->PrgbObj->SetRange(this->m_RngLow, this->m_BasePer);
    /// �v���O���X�o�[������
    this->Start();
}
/// <summary>
/// �v���O���X�o�[�X�V
/// </summary>
/// <returns>�i����(%)</returns>
UINT32 PrgbCtrl::Inc()
{
    // �����̃p�[�Z���g���v�Z
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
/// �v���O���X�o�[������
/// </summary>
void PrgbCtrl::Start()
{
    int Red = 0;
    int Green = 0;
    int Blue = 255;
    // �v���O���X�o�[�̐F�ݒ�
    this->PrgbObj->SetBarColor(RGB(Red, Green, Blue));
    // �v���O���X�o�[�����ʒu�ݒ�
    this->PrgbObj->SetPos(0);
    // �v���O���X�o�[���Z�l������
    this->m_incd = 0.0;
}
/// <summary>
/// �v���O���X�o�[�I��
/// </summary>
void PrgbCtrl::End()
{
    // �v���O���X�o�[�����ʒu�ݒ�
    this->PrgbObj->SetPos(100);
}
/// <summary>
/// �o�[�J���[��ݒ�
/// �����l����ԂɕύX����
/// </summary>
/// <param name="perc_val">�i����(%)</param>
/// <returns></returns>
void PrgbCtrl::SetBarB2RColor(int perc_val)
{
    int Red = 255;
    int Green = 0;
    int Blue = 255;
    // �Ԑݒ�
    Red = Red * perc_val / 100;
    // �ݒ�
    Blue = Blue - (Blue * perc_val / 100);
    // �v���O���X�o�[�̐F�ݒ�
    this->PrgbObj->SetBarColor(RGB(Red, Green, Blue));
}