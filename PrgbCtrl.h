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
    /// <param name="idx">�r���C���f�b�N�X</param>s
    /// <param name="BasePer">�p�[�Z���g�l�i�f�t�H���g��100�j</param>
    PrgbCtrl(
        CProgressCtrl* prgbobj,
        const uintmax_t RngUpper,
        const uintmax_t IncVal,
        const UINT32 BasePer = 100
    );
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
    /// <returns></returns>
    void SetBarB2RColor(int perc_val);
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
    const uintmax_t m_RngUpper;     // �v���O���X�o�[�I���_
    const uintmax_t m_IncVal;   // �v���O���X�o�[����
    const UINT32 m_BasePer;     // �p�[�Z���g�l
    double m_incd;              // �v���O���X�o�[���Z��
};

