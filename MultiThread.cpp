#include <mutex>
#include <chrono>
#include <random>
#include <time.h>
#include <atlstr.h>
#include <taskschd.h> // CPU Thread�̃X�P�W���[���H
#include <string>
#include "MultiThread.h"
namespace SCPU
{
	// �X���b�h�v�Z���x�����W�̔r���I�u�W�F�N�g
	extern std::mutex mtx_flps;
}
namespace MT
{
	// �r������p�I�u�W�F�N�g
	std::mutex mtx_[D_MAX_THRD_CNT];
	// �����ח��ϔr���I�u�W�F�N�g
	std::mutex mtx_strss;
	// �}���`�X���b�h�I���J�E���^
	int thrdCounter = 0;
	// �����ח��ϐ�
	ULONG LoopInnerVar;
	// �����ח��ϐݒ�
	void SetStressCtrl(const ULONG var)
	{
		// �����ח��ϔr���I�u�W�F�N�g
		std::lock_guard<std::mutex> lock(mtx_strss);
		LoopInnerVar = var;
	}
	// �����ח��ώ擾
	ULONG GetStressCtrl()
	{
		// �����ח��ϔr���I�u�W�F�N�g
		std::lock_guard<std::mutex> lock(mtx_strss);
		return LoopInnerVar;
	}
	// �}���`�X���b�h�I���J�E���^�擾
	int GetthrdCounter()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		return thrdCounter;
	}
	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="mrnd">�����l</param>
	/// <returns>ULONG</returns>
	ULONG GetRnd(ULONG mrnd)
	{
		static const ULONG N = 2100000000; // [500, N-1] �̗����𔭐�
		return ((mrnd % N) + 500);
	}
	/// <summary>
	/// ���׏���
	/// �{���x���Z�v���Z�b�T���������Z�v���Z�b�T�̕��׏���
	/// </summary>
	void ThreadStressor(void)
	{
		// ��������������
		std::mt19937 mt;            // �����Z���k�E�c�C�X�^��32�r�b�g��
		std::random_device rnd;     // �񌈒�I�ȗ���������

		ULONG max_count = GetStressCtrl();
		for (ULONG kk = 0; kk < D_OUTER; kk++)
		{
			// ���Z�l�𗐐�����
			mt.seed(rnd());            // �V�[�h�w��
			auto var1 = mt();
			auto var2 = mt();
			auto var3 = mt();
			double d = (double)var1 * 1.987654;
			double e = (double)d * 1.012345;
			for (ULONG ii = 0; ii < max_count; ii++)
			{
				d *= e;
				d /= e;
				d += e;
				d -= e;
			}
		}
	}
	/// <summary>
	/// CPU�𓮍삳���ăX�g���X��^����B
	/// �E�{���x���������_�����Z
	/// �E�������Z
	/// �E�����𔭐�������B
	/// </summary>
	/// <param name="thrd">�}���`�X���b�h�Ǘ��N���X</param>
	/// <param name="idx">�}���`�X���b�hID</param>
	void CPU_MainStressor(
		MultiThread* thrd,
		const int idx
	)
	{
		// ��������������
		std::mt19937 mt;            // �����Z���k�E�c�C�X�^��32�r�b�g��
		std::random_device rnd;     // �񌈒�I�ȗ���������
		// ��������
		int r;
		// �v���O���X�o�[�R���g���[�����J�n
		//thrd->
		// �o�ߎ��Ԃ�������
		for (int stat = e_Status::st_sleep;
			stat != e_Status::st_end;
			stat = thrd->GetStatus(idx)
			)
		{
			if (stat == e_Status::st_sleep)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			// ���Z�l�𗐐�����
			mt.seed(rnd());            // �V�[�h�w��
			// ���Ԍv���J�n
			auto start = std::chrono::system_clock::now();
			// ���׏���
			ThreadStressor();
			// �o�ߎ��Ԑݒ�
			auto end = std::chrono::system_clock::now();
			// �o�ߎ��Ԃ�nano�b�P�ʂŕ\��
			auto diff = end - start;
			auto worktm = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
			// �o�ߎ��Ԃ�ݒ�
			thrd->SetThreadTimes(idx, (double)worktm);
			// ��������
			r = GetRnd(mt());
			CString cvar_rnd;
			cvar_rnd.Format("%010d", r);
			// �������Y���X���b�hID�ɕۑ�
			std::string rndstr = cvar_rnd.GetString();
			thrd->SetRndStr(
				idx,
				rndstr
			);
		}
		// �I���t���O��ON
		thrd->SetEndFlg(idx, true);
	}
	// �Y������X���b�h��񓯊��ŋN������B
	void MultiThread::ExecThread(int ID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[ID]);
		// �񓯊��X���b�h�̐���
		this->MTM_Tbl[ID].hThread = new std::thread(
			MT::CPU_MainStressor,
			this,
			ID
		);
		// �X���b�h�̃n���h�������o��
		auto handle = this->MTM_Tbl[ID].hThread->native_handle();
		// CPU�̃X���b�h�Ɋ��蓖�Ă�
		SetThreadAffinityMask(
			handle,
			1 << ID
		);
	}
	// �Y������X���b�h�̃X�e�[�^�X�ƗD��x���R���g���[������
	void MultiThread::CtrlThread(
		int thrdID,
		int status
	)
	{
		// �X���b�h�̃X�e�[�^�X�ݒ�
		this->SetStatus(thrdID, status);
		// �X���b�h�|�C���^���擾
		if (this->GetThrdPointer(thrdID) == nullptr)
		{
			return;
		}
		// �w�肳�ꂽ�X���b�h�̑��ΗD��x��ύX
		int thread_pri;
		if (status == e_Status::st_running)
		{
			// �������
			// ��D��x��D��x�N���X���2�����Ⴍ�ݒ肵�܂��B
			thread_pri = THREAD_PRIORITY_LOWEST;
		}
		else if (status == e_Status::st_sleep)
		{
			// �ҋ@���		
			// ��D��x��D��x�N���X���1���������ݒ肵�܂��B
			//THREAD_PRIORITY_LOWEST;
			thread_pri = THREAD_PRIORITY_ABOVE_NORMAL;
		}
		else
		{
			// ��~���
			// ��D��x��D��x�N���X���2�����Ⴍ�ݒ肵�܂��B
			thread_pri = THREAD_PRIORITY_LOWEST;
		}
		// �Y���X���b�h�̐�L�D��x��ݒ肷��B
		SetThreadPriority(
			// �X���b�h�n���h�����擾
			this->GetThrdNativeHnd(thrdID),
			thread_pri
		);
	}
	void MultiThread::SetEndFlg(int thrdID, bool var)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].endflg = var;
		MT::thrdCounter++;
	}
	// �X���b�h�̏I���t���O���擾
	bool MultiThread::GetEndFlg(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].endflg;
	}
	// �}���`�X���b�h�̃X�e�[�^�X�擾
	int MultiThread::GetStatus(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		// �X���b�h�̃X�e�[�^�X�ݒ�
		return this->MTM_Tbl[thrdID].status;
	}
	// �X�e�[�^�X���]
	int MultiThread::GetReverse(int thrdID)
	{
		// MT::e_Status::st_sleep <---> MT::e_Status::st_running
		int st = this->GetStatus(thrdID) ^ MT::e_Status::st_for_xor;
		return st;
	}
	// �X���b�h�X�e�[�^�X�ݒ�擾
	void MultiThread::SetStatus(int thrdID, int var)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].status = var;
	}
	// �}���`�X���b�h�̗�����������擾
	std::string MultiThread::GetRndStr(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].var_rnd;
	}
	// �}���`�X���b�h�̗����������ݒ�
	void MultiThread::SetRndStr(
		int thrdID,
		std::string& rndstr
	)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].var_rnd = rndstr;
	}
	// �X���b�h�������Ԑݒ�
	// �X���b�h����ThreadStressor()��񓖂���̎��s���Ԃ�ݒ肷��
	void MultiThread::SetThreadTimes(int thrdID, double var)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].caltimer = var;
	}
	// �X���b�h�������Ԏ擾�i���[�v�j
	// ThreadStressor()��񓖂���̎��s���Ԃ�Ԃ�
	double MultiThread::GetThreadTimes(int thrdID)
	{
		// �������Ԃ��ݒ肳���܂ő҂�
		double caltimer = 0.0;
		for (; caltimer <= 0.0;)
		{
			caltimer = this->GetCalTimer(thrdID);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return caltimer;
	}
	// �X���b�h�������Ԏ擾
	// ThreadStressor()��񓖂���̎��s���Ԃ�Ԃ�
	double MultiThread::GetCalTimer(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].caltimer;
	}
	// ���ݎ��s���X���b�h�̃J�E���g���s��
	void MultiThread::SetUpdateCounter()
	{
		// �}���`�X���b�h�e�[�u���̃T�C�Y���擾
		size_t max_counter = this->GetMthrSize();
		int run_counter = 0;
		for (size_t ii = 0; ii < max_counter; ii++)
		{
			if (this->GetStatus(ii) == e_Status::st_running)
			{
				run_counter++;
			}
		}
		this->m_RunningCount = run_counter;
	}
	// �}���`�X���b�h�e�[�u���̃T�C�Y���擾
	size_t MultiThread::GetMthrSize()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		size_t max_counter = this->MTM_Tbl.size();
		return max_counter;
	}
	// �X���b�h�|�C���^���擾
	std::thread* MultiThread::GetThrdPointer(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].hThread;
	}
	// �X���b�h�̃n���h�����擾
	std::thread::native_handle_type MultiThread::GetThrdNativeHnd(int thrdID)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].hThread->native_handle();
	}
	// ���ݎ��s���X���b�h�̃J�E���g��ݒ肷��
	void MultiThread::SetUpdateCounter(int var)
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount = var;
	}
	// ���ݎ��s���X���b�h�̃J�E���g���擾
	int MultiThread::GetUpdateCounter()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		return this->m_RunningCount;
	}
	// ���ݎ��s���̃X���b�h�����v���X
	void MultiThread::PlusCounter()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount++;
	}
	// ���ݎ��s���̃X���b�h�����}�C�i�X
	void MultiThread::MinusCounter()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount--;
	}
	// ���ݎ��s���̃X���b�h�����[���ɂ���
	void MultiThread::ZeroCounter()
	{
		// �Q�Ƃ���O�Ƀ��b�N���擾����
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount = 0;
	}
}