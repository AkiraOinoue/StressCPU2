#include "framework.h"
#include "SCPU.h"
#include "StressCPUDlg.h"
namespace SCPU
{
	// GUI�R���g���[���p�r�����b�N
	std::mutex g_mtx_[D_MAX_THRD_CNT];
	// GUI����p�r���I�u�W�F�N�g
	std::mutex mtx_scpu;
	// �X���b�h�v�Z���x�����W�̔r���I�u�W�F�N�g
	std::mutex mtx_flps;
	// �P���x���Z����ԃT���v�����O���s���t���O�r���I�u�W�F�N�g
	std::mutex mtx_flpssmpflg;
	// �v���O���X�o�[�X�V�t���O�r��
	std::mutex mtx_prgbar;
	// �����\���t���O�r��
	std::mutex mtx_rnddisp;
	// �S�X���b�h�I���t���O
	// true=�S�X���b�h�I��, false=�S�X���b�h�ғ���
	bool ThreadEndFlg;
	/// <summary>
	/// �w��X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// �X���b�h�֐��Ƃ��ē��삷��
	/// </summary>
	/// <param name="thridx">�X���b�hID</param>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void ThreadRandVarDisp(
		int thridx,
		CStressCPUDlg* strc
	)
	{
		std::string svar = "";
		SCPU::SetThreadEnd(false);
		int stat;
		bool PrgRunFlg = true;
		for (; GetThreadEnd() == false;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			stat = strc->hmultiThrd->GetStatus(thridx);
			if (stat == MT::e_Status::st_sleep)
			{
				// �v���O���X�o�[�\���I��
				if (PrgRunFlg == false)
				{
					strc->PrgBarEnd(thridx);
					// �X���b�h�����\������CPU�x���_�[���\��
					strc->SetStVector(thridx, strc->GetCpuVend().c_str());
					PrgRunFlg = true;
				}
				continue;
			}
			else if (stat == MT::e_Status::st_running)
			{
				// �����\��ON
				if (strc->GetThreadRndDispFlg() == TRUE)
				{
					// �e�X�g���X�������擾
					svar = strc->hmultiThrd->GetRndStr(thridx);
					// �e�����\���̈�ɕ\��
					strc->SetStVector(thridx, svar.c_str());
				}
				// �v���O���X�o�[�X�VON
				if (strc->GetPrgBarUpdateFlg() == TRUE)
				{
					// �v���O���X�o�[�\��
					if (PrgRunFlg == true)
					{
						strc->PrgBarRun(thridx);
						PrgRunFlg = false;
					}
					// �v���O���X�o�[�X�V
					strc->PrgBarUpdate(thridx);
				}
			}
		}
	}
	/// <summary>
	/// �X�g���X�X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void MultiThreadMgrSub(CStressCPUDlg* strc)
	{
		std::string svar = "";
		// �ő�ΏۃX���b�h���擾
		int max_thrd = strc->GetMaxThrdCount();
		for (int ii = 0; ii < max_thrd; ii++)
		{
			// �X���b�h�����\���p�X���b�h�֐��N��
			strc->hmultiThrd->MTM_Tbl[ii].hRndDisp = new std::thread(
				SCPU::ThreadRandVarDisp,
				ii,
				strc
			);
			// �Y���X���b�h�̐�L�D��x��ݒ肷��B
			// ��D��x��D��x�N���X���2�����Ⴍ�ݒ肵�܂��B
			auto thread_pri = THREAD_PRIORITY_LOWEST;
			SetThreadPriority(
				strc->hmultiThrd->GetThrdNativeHnd(ii),
				thread_pri
			);
		}
	}
	/// <summary>
	/// �X�g���X�X���b�h�̗�����GUI�R���g���[���ɕ\������
	/// �X���b�h�Ď��p�X���b�h�֐�
	/// ���������������E���ĕ\���G���A�ɕ\��
	/// </summary>
	/// <remark>CStressCPUDlg�̃R���X�g���N�^�[����Ă΂��
	/// </remark>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void MultiThreadMgr(CStressCPUDlg* strc)
	{
		static bool RunFlg = false;
		for (;;)
		{
			// �ꊇ���s���ꂽ
			if (RunFlg == false &&
				strc->GetExecAllFlag() == TRUE)
			{
				// �X�g���X�X���b�h�̗�����GUI�R���g���[���ɕ\������
				// �P��̂ݎ��s
				MultiThreadMgrSub(strc);
				RunFlg = true;
			}
			// �ꊇ�ҋ@���
			/// �S�X���b�h�I���`�F�b�N
			if (GetThreadEnd() == true)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	}
	/// <summary>
	/// �X���b�h��~�E�N����ԕ\��
	/// </summary>
	/// 0.5�b�Ԋu�Łu�X���b�h�N���\�v
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void ThreadStatMsg(CStressCPUDlg* strc)
	{
		// 0.5�b�Ԋu�Łu�X���b�h�N���\�v
		CString StMsg = "";
		int disp_flg = 1; // 1=�\���A0=��
		for (;;)
		{
			if (disp_flg)
			{
				StMsg = "�X���b�h��";
			}
			else
			{
				StMsg = "�N���\";
			}
			// �X���b�h��ԕ\���ݒ�
			strc->SetST_ThreadStatus(StMsg);
			// �\���҂�
			// 0.5�b���ɏo��
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			disp_flg ^= 1; // 1/0 ���]
			/// �S�X���b�h�I���`�F�b�N
			if (GetThreadEnd() == true)
			{
				// �X���b�h��ԕ\���ݒ�
				strc->SetST_ThreadStatus("�X���b�h�͒�~��");
				break;
			}
		}
	}
	/// <summary>
	/// �S�X���b�h�I���`�F�b�N
	/// </summary>
	/// <return>bool ThreadEndFlg</return>
	bool GetThreadEnd()
	{
		// GUI����p�r���I�u�W�F�N�g
		std::lock_guard<std::mutex> lock(mtx_scpu);
		return ThreadEndFlg;
	}
	/// <summary>
	/// �S�X���b�h�I���t���O�ݒ�
	/// </summary>
	/// <param name="ThreadEndFlg">�S�X���b�h�I���t���O</param>
	void SetThreadEnd(bool var)
	{
		// GUI����p�r���I�u�W�F�N�g
		std::lock_guard<std::mutex> lock(mtx_scpu);
		ThreadEndFlg = var;
	}
	/// <summary>
	/// �e�X���b�h����ThreadStressor()�̌v�Z�������Ԃ̕��ϒl���Z�o�i����[nano]�j
	/// 1m�b���Ɏ��W���A���ϒl���擾����
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void FlopsMultiThreadCollectCalculationVolume(CStressCPUDlg* strc)
	{
		const auto max_count = strc->GetMaxThrdCount();
		// �I���t���O�𔻒�
		for (;SCPU::GetThreadEnd() == false;)
		{
			// �T���v�����O���s���t���OON���H
			if (strc->GetFlpsSamplingFlg() == false)
			{
				// 1m�b�҂�
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			// �e���s���X���b�h�̃X�g���X�������Ԃ��W�v���ĕ��ϒl���Z�o
			// ���s�X���b�h�������ԍ��v�l
			double flpsTm = 0.0;
			for (int ii = 0; ii < max_count; ii++)
			{
				// ���s���̃X���b�h�̂ݏ���Ԃ��̎�
				if (strc->hmultiThrd->GetStatus(ii) == MT::e_Status::st_running)
				{
					// �X���b�h�������Ԏ擾
					flpsTm += strc->hmultiThrd->GetThreadTimes(ii);
				}
				// 1m�b�҂�
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			// ���s�X���b�h��
			int ExecThread = strc->hmultiThrd->GetUpdateCounter();
			// ���s���X���b�h���Ŋ���
			flpsTm /= (double)ExecThread;
			// ���ϒl����Ԃ�ݒ�
			strc->SetFlpsCalTimerEven(flpsTm);
			// �T���v�����O���s���t���OOFF
			strc->SetFlpsSamplingFlg(false);
		}
	}
	/// <summary>
	/// FLOPS�\���X���b�h
	/// 0.3�b���ɍX�V
	/// �S�X���b�h���I������܂Ŗ����Ƀ��[�v
	/// </summary>
	/// <param name="strc">CStressCPUDlg�|�C���^</param>
	void FlopsDispValueThread(CStressCPUDlg* strc)
	{
		float fps_max = 0.0;
		CString cfps_max = "";
		float fps_min = 0.0;
		CString cfps_min = "";
		// ���s�X���b�h���ێ�
		static int st_RunThreadCount;
		// �P���x���Z����ԃT���v�����O���s���t���O��ON
		// FlopsMultiThreadCollectCalculationVolume()��FlpsSamplingFlg���Q�Ƃ���A
		// �T���v�����O�������OFF�ɂ����
		strc->SetFlpsSamplingFlg(true);
		for (; SCPU::GetThreadEnd() == false;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			// �P���x���Z����ԃT���v�����O���s����FLOPS�v�Z�͂��Ȃ�
			if (strc->GetFlpsSamplingFlg() == true)
			{
				continue;
			}
			// �ғ����̃X���b�h�����邩
			if (strc->GetExecAllFlag() == TRUE)
			{
				//*******************************************************************
				// FLOPS�v�Z
				//*******************************************************************
				// ���s�X���b�h���擾
				auto RunThreadCount = strc->hmultiThrd->GetUpdateCounter();
				// ���s�X���b�h�����ύX���ꂽ��MAX/MIN���N���A
				if (st_RunThreadCount != RunThreadCount)
				{				
					// MAX/MIN���N���A
					double tmpFlops = -1.0;
					CString tmpCvar = "";
					GetMinMax(
						tmpFlops,
						tmpFlops,
						tmpFlops,
						tmpCvar,
						tmpCvar,
						tmpCvar
					);
					// ���s�X���b�h���ۊ�
					st_RunThreadCount = RunThreadCount;
				}
				// �����ח��ϐ�(MT::GetStressCtrl()) x �O�����[�v�� x �X���b�h��
				double d_inner = MT::GetStressCtrl();
				double d_outer = D_OUTER;
				double d_RunThreadCount = RunThreadCount;
				auto Flops = d_inner * d_outer * d_RunThreadCount;
				// nano(1/1,000,000,000)�b / �X���b�h���̕��Ϗ�������
				// �T���v�����O�����ɗv�������Ԃ͔��X���邽�ߖ�������
				auto nSecond = strc->GetFlpsCalTimerEven();
				// GFLOPS�ϊ��@1nano(1/1000,000,000)�b�Ԃɉ��񉉎Z���ł��邩
				if (nSecond > 0.0)
				{
					Flops /= nSecond;
				}
				else
				{
					Flops = 0.0;
				}
				//*******************************************************************
				// MIN/MAX �Ƃ��ꂼ��̒P�ʂ�Ԃ�
				// �P�ʕ\���ύX
				//*******************************************************************
				double MinVar = 0.0;
				double MaxVar = 0.0;
				CString	cFPUnit = "GFLOPS";
				CString cMinUnit = "GFLOPS";
				CString cMaxUnit = "GFLOPS";
				GetMinMax(
					Flops,
					MinVar,
					MaxVar,
					cFPUnit,
					cMinUnit,
					cMaxUnit
				);
				CString cvar = "";
				// ���A���^�C��FLOPS�\�����ɏo��
				cvar.Format("%7.2f", Flops);
				strc->SetSTFlops(cvar, strc->m_ST_Flops);
				strc->SetSTFlops(cFPUnit, strc->m_ST_FPUNIT);
				// MIN�lFLOPS�\�����ɏo��
				cvar.Format("%7.2f", MinVar);
				strc->SetSTFlops(cvar, strc->m_ST_FlopsMIN);
				strc->SetSTFlops(cMinUnit, strc->m_ST_FPUNIT_MIN);
				// MAX�lFLOPS�\�����ɏo��
				cvar.Format("%7.2f", MaxVar);
				strc->SetSTFlops(cvar, strc->m_ST_FlopsMAX);
				strc->SetSTFlops(cMaxUnit, strc->m_ST_FPUNIT_MAX);
			}
			// �P���x���Z����ԃT���v�����O���s���t���O��ON
			// FlopsMultiThreadCollectCalculationVolume()��FlpsSamplingFlg���Q�Ƃ���A
			// �T���v�����O�������OFF�ɂ����
			strc->SetFlpsSamplingFlg(true);
		}
	}
	/// <summary>
	/// �덷�␳����
	/// </summary>
	/// <param name="real_flops">���A���^�C��FLOPS�l</param>
	/// <param name="min_max_flops">�ŏ��܂��͍ő�l</param>
	/// <param name="per">�덷���i���j</param>
	/// <returns>
	/// �덷���i���j�𒴂����ꍇ��TRUE�A
	/// �덷���i���j�͈͓��̏ꍇ��FALSE
	/// </returns>
	BOOL IsAJustFlops(
		double real_flops,
		double min_max_flops,
		double per
	)
	{
		BOOL retvB = FALSE; // �덷�͈͂̏ꍇ��FALSE��Ԃ�
		// real_flops��min_max_flops�̍��̐�Βl�����
		auto diff = ::abs(real_flops - min_max_flops);
		// ����real_flops�̌덷���𒴂����ꍇ��TRUE
		auto rates = (diff / real_flops) * 100;
		retvB = (per < rates) ? TRUE : FALSE;
		return retvB;
	}
	/// <summary>
	/// FLOPS�l��MIN/MAX�Ƃ��ꂼ��̒P�ʕ�����Ԃ�
	/// ���A���^�C��FLOPS��MAX�y��MIN���������ꂽ��
	/// ��U���A���^�C��FLOPS�l�ɐݒ肷��B
	/// ���A���^�C��FLOPS�Ƃ̍���50%�ȏ�̏ꍇ��
	/// ���A���^�C��FLOPS�l�ɐݒ肷��
	/// </summary>
	/// <param name="flops_var">�擾�������Z�����</param>
	/// <param name="min_var">MIN�l</param>
	/// <param name="max_var">MAX�l</param>
	/// <param name="str_real_var">���A���^�C��FLOPS�̒P��</param>
	/// <param name="str_min_unit">MIN�l�P��</param>
	/// <param name="str_max_unit">MAX�l�P��</param>
	void GetMinMax(
		double& flops_var,
		double& min_var,
		double& max_var,
		CString& str_real_var,
		CString& str_min_unit,
		CString& str_max_unit
	)
	{
		// �덷�␳��(%)
		// ���A���^�C��FLOPS�l��MAX�l�y��MIN�l�̌덷����12.0%�𒴂����ꍇ�͋����I��
		// ���A���^�C��FLOPS�l�ɐݒ肵�܂��B
		static const double ajRate = 12.0;
		// ��r�p�̒l
		static double MaxVar = 0.0;
		static double MinVar = 0.0;
		// �\���p�̒l�ƒP��
		static double FlpsMaxVar = 0.0;
		static double FlpsMinVar = 0.0;
		static CString cMaxUnit = "GFLOPS";
		static CString cMinUnit = "GFLOPS";
		auto var = flops_var;
		// MAX/MIN�N���A
		if (flops_var < 0)
		{
			MaxVar = 0.0;
			MinVar = 0.0;
			FlpsMaxVar = 0.0;
			FlpsMinVar = 0.0;
			cMaxUnit = "GFLOPS";
			cMinUnit = "GFLOPS";
			return;
		}
		// �P�ʂ����߂�
		CString cunit;
		cunit = "GFLOPS";
		if (var > 950.0)
		{
			cunit = "TFLOPS";
			var /= 1000.0;
		}
		if (var > 950.0)
		{
			cunit = "PFLOPS";
			var /= 1000.0;
		}
		str_real_var = cunit;
		// �����ݒ�
		if (MaxVar == 0.0)
		{
			MaxVar = flops_var;
			FlpsMinVar = var;
		}
		if (MinVar == 0.0)
		{
			MinVar = flops_var;
			FlpsMinVar = var;
		}
		// MAX���傫����
		if (flops_var >= MaxVar)
		{
			MaxVar = flops_var;
			FlpsMaxVar = var;
			// �P�ʕ\���ύX
			cMaxUnit = cunit;
		}
		// MIN��菬������
		if (flops_var < MinVar)
		{
			MinVar = flops_var;
			FlpsMinVar = var;
			// �P�ʕ\���ύX
			cMinUnit = cunit;
		}
		// ���A���^�C��FLOPS�Ƃ̍���50%�ȏ�̏ꍇ��
		// ���A���^�C��FLOPS�l�ɐݒ肷��
		if (TRUE == IsAJustFlops(flops_var, MinVar, ajRate))
		{
			MinVar = flops_var;
			FlpsMinVar = var;
		}
		if (TRUE == IsAJustFlops(flops_var, MaxVar, ajRate))
		{
			MaxVar = flops_var;
			FlpsMaxVar = var;
		}
		// ���A���^�C��FLOPS
		flops_var = var;
		str_real_var = cunit;
		// MIN�lFLOPS
		min_var = FlpsMinVar;
		str_min_unit = cMinUnit;
		// MAX�lFLOPS
		max_var = FlpsMaxVar;
		str_max_unit = cMaxUnit;
	}
	///////////////////////////////////////////////////////////////////
	/// <summary>
	/// �ꎞ�I�ɃC�x���g�����t���O��ݒ肷��
	/// </summary>
	/// <param name="lpCBtn">CButton�|�C���^</param>
	/// <param name="mtime">�C�x���g��������(ms)</param>
	void EventWait(
		CButton* lpCBtn,
		UINT32 mtime
	)
	{
		lpCBtn->EnableWindow(FALSE);
		std::this_thread::sleep_for(std::chrono::milliseconds(mtime));
		lpCBtn->EnableWindow(TRUE);
	}
}
