#pragma once
/*
	���e�X���b�h�̒P���x���Z����Ԃ̍��v����FLOPS������o��
	�@�P�b�ԂɒP���x���Z�����s����񐔁�FLOPS�iFloating-Point Operations Per Second�j
	�@TT���S�X���b�h�̒P���x���Z�I�����Ԃ̍��v�ins�j
	�@FC���P���x���Z���[�v�񐔁i�ρj
	�@FP��4�i�P�񓖂���̒P���x���Z�񐔁��Œ�j
	�@TC���S�X���b�h���i�ρj
	�@GFLOPS�i�b�j��(FC�~FP�~TC)/TT
*/
#include <mutex>
#include "StressCPUDlg.h"

namespace MT
{
	// �r������p�I�u�W�F�N�g
	extern std::mutex mtx_[D_MAX_THRD_CNT];
}
namespace SCPU
{
	// GUI�R���g���[���p�r�����b�N
	extern std::mutex g_mtx_[D_MAX_THRD_CNT];
	// GUI����p�r���I�u�W�F�N�g
	extern std::mutex mtx_scpu;
	extern std::mutex mtx_Flops;
	extern std::mutex mtx_FlopsVar;
	extern std::mutex mtx_FlopsMax;
	extern std::mutex mtx_FlopsMin;
	extern std::mutex mtx_BU_scpu;
	extern std::mutex mtx_ExecAllFlag;
	extern std::mutex mtx_scpu_MaxThrdCount;
	extern std::mutex mtx_ThreadEndFlg;
	// GUI����p�r���I�u�W�F�N�g2
	extern std::mutex mtx_scpu2;
	// �X���b�h�v�Z���x�����W�̔r���I�u�W�F�N�g
	extern std::mutex mtx_flps;
	// �P���x���Z����ԃT���v�����O���s���t���O�r���I�u�W�F�N�g
	extern std::mutex mtx_flpssmpflg;
	// �v���O���X�o�[�X�V�t���O�r��
	extern std::mutex mtx_prgbar;
	// �����\���t���O�r��
	extern std::mutex mtx_rnddisp;
	///////////////////////////////////////////////////////////////////
	/// <summary>
	/// �ꎞ�I�ɃC�x���g�����t���O��ݒ肷��
	/// </summary>
	/// <param name="lpCBtn">CButton�|�C���^</param>
	/// <param name="mtime">�C�x���g��������(ms)</param>
	void EventWait(
		CButton* lpCBtn,
		UINT32 mtime
	);
	/// <summary>
	/// FLOPS�l��MIN/MAX�Ƃ��ꂼ��̒P�ʕ�����Ԃ� 
	/// </summary>
	/// <param name="var">�擾�������Z�����</param>
	/// <param name="min_var">MIN�l</param>
	/// <param name="max_var">MAX�l</param>
	/// <param name="cunit_var">���A���^�C��FLOPS�̒P��</param>
	/// <param name="cmin_unit">MIN�l�P��</param>
	/// <param name="cmax_unit">MAX�l�P��</param>
	void GetMinMax(
		double& flops_var,
		double& min_var,
		double& max_var,
		CString& cunit_var,
		CString& cmin_unit,
		CString& cmax_unit
	);
	/// <summary>
	/// �덷�␳����
	/// </summary>
	/// <param name="real_flops">���A���^�C��FLOPS�l</param>
	/// <param name="min_max_flops">�ŏ��܂��͍ő�l</param>
	/// <param name="per">�덷���i���j</param>
	/// <returns>
	/// �덷�𒴂����ꍇ��TRUE�A
	/// �덷�͈͓��̏ꍇ��FALSE
	/// </returns>
	BOOL IsAJustFlops(
		double real_flops,
		double min_max_flops,
		double per
	);
	/// <summary>
	/// FLOPS�p�̃v���O���X�o�[�X�V�X���b�h
	/// ���s���̃X���b�h���P�ȏ゠��Ύ��s
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void FlopsProgressBarUpdate(CStressCPUDlg* strc);
	// FLOPS�\���X���b�h
	// 0.5�b���ɍX�V
	// �S�X���b�h���I������܂Ŗ����Ƀ��[�v
	void FlopsDispValueThread(CStressCPUDlg* strc);
	/// <summary>
	/// �X�g���X�X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void MultiThreadMgrSub(CStressCPUDlg* strc);
	/// <summary>
	/// �w��X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// �X���b�h�֐��Ƃ��ē��삷��
	/// </summary>
	/// <param name="thridx">�X���b�hID</param>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void ThreadRandVarDisp(
		int thridx,
		CStressCPUDlg* strc
	);
	/// <summary>
	/// �X�g���X�X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// �X���b�h�Ď��p�X���b�h�֐�
	/// ���������������E���ĕ\���G���A�ɕ\��
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void MultiThreadMgr(CStressCPUDlg* strc);
	/// <summary>
	/// �X���b�h��~�E�N����ԕ\��
	/// </summary>
	/// <param name="strc"></param>
	void ThreadStatMsg(CStressCPUDlg* strc);
	/// <summary>
	/// �S�X���b�h�I���`�F�b�N
	/// </summary>
	/// <return>bool ThreadEndFlg</return>
	bool GetThreadEnd();
	/// <summary>
	/// �S�X���b�h�I���t���O�ݒ�
	/// </summary>
	/// <param name="ThreadEndFlg">�S�X���b�h�I���t���O</param>
	void SetThreadEnd(bool var);
	/// <summary>
	/// �e�X���b�h�̌v�Z���̕��ϒl�����W�i�񐔁A����[S]�j
	/// 0.5�b���Ɍv�Z�ʂ����W���A���ϒl���擾����
	/// </summary>
	/// <param name="strc"></param>
	void FlopsMultiThreadCollectCalculationVolume(CStressCPUDlg* strc);
}
