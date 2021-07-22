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
    /// �v���O���X�o�[�R���X�g���N�^�[
    /// </summary>
    /// <param name="prgbobj">�v���O���X�o�[�I�u�W�F�N�g�|�C���^</param>
    PrgbCtrl(CProgressCtrl* prgbobj);
    /// <summary>
    /// �v���O���X�o�[�R���X�g���N�^�[
    /// </summary>
    /// <param name="prgbobj">�v���O���X�o�[�I�u�W�F�N�g�|�C���^</param>
    /// <param name="RngUpper">�v���O���X�o�[�I���_</param>
    /// <param name="IncVal">�v���O���X�o�[����</param>
    /// <param name="BasePer">�p�[�Z���g�l�i�f�t�H���g��100�j</param>
    PrgbCtrl(
        CProgressCtrl* prgbobj,
        const uintmax_t RngUpper,
        const uintmax_t IncVal,
        const UINT32 BasePer = 100
    );
    /// <summary>
    /// FLOPS�lMAX�l�ݒ�
    /// </summary>
    /// <param name="flops_max">FLOPS��MAX�l</param>
    void SetFlopsRange(double flops_max);
    /// <summary>
    /// FLOPS�l�ݒ�
    /// </summary>
    /// <param name="flops">FLOPS�l</param>
    void SetFlops(double flops);
    /// <summary>
    /// �v���O���X�o�[�X�V�̃X�e�b�v��
    /// </summary>
    /// <param name="var"></param>
    void SetStep(int var);
    /// <summary>
    /// �v���O���X�o�[�X�V
    /// </summary>
    /// <returns>�i����(%)</returns>
    UINT32 Inc();
    /// <summary>
    /// �o�[�J���[��ݒ�
    /// �����l����ԂɕύX����
    /// </summary>
    /// <param name="perc_val">�i����(%)</param>
    /// <returns>�Ȃ�</returns>
    void SetBarB2RColor(int perc_val);
    /// <summary>
    /// �o�[�J���[��ݒ�
    /// �����l�΂���I�����W�ɕύX����
    /// </summary>
    /// <param name="perc_val">�i����(%)</param>
    /// <returns>�Ȃ�</returns>
    void SetBarG2OColor(int perc_val);
    /// <summary>
    /// �v���O���X�o�[�I��
    /// </summary>
    void End();
    /// <summary>
    /// �v���O���X�o�[������
    /// </summary>
    void Start();
    // �v���O���X�o�[�I�u�W�F�N�g
    CProgressCtrl* PrgbObj;   // �v���O���X�o�[�I�u�W�F�N�g�|�C���^
private:
    const short m_RngLow = 0;   // �v���O���X�o�[�J�n�_
    uintmax_t m_RngUpper;     // �v���O���X�o�[�I���_
    uintmax_t m_IncVal;   // �v���O���X�o�[����
    const UINT32 m_BasePer;     // �p�[�Z���g�l
    double m_incd;              // �v���O���X�o�[���Z��
};

