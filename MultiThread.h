#pragma once
#include <thread>
#include <map>
#include <string>
#define D_OUTER (100000)
#define D_INNER (5042) // FLOPS�l�����p�̒l
#define D_MAX_THRD_CNT (16)
namespace MT
{
	// �����ח��ϐݒ�
	void SetStressCtrl(const ULONG var);
	// �����ח��ώ擾
	ULONG GetStressCtrl();
	// �}���`�X���b�h�I���J�E���^�擾
	int GetthrdCounter();
	// �O���Q�Ɛ錾
	class MultiThread;
	/// <summary>
	/// CPU�𓮍삳���ăX�g���X��^����B
	/// �E�{���x���������_�����Z
	/// �E�������Z
	/// �E�����𔭐�������B
	/// </summary>
	/// <param name="csCpudlg">�}���`�X���b�h�Ǘ��N���X</param>
	/// <param name="idx">�}���`�X���b�hID</param>
	void CPU_MainStressor(
		MultiThread* thrd,
		const int idx
	);
	/// <summary>
	/// �X���b�h�X�e�[�^�X��`
	/// </summary>
	enum e_Status
	{
		st_sleep = 1,	// �ꎞ��~
		st_running = 2,	// ���s��
		st_for_xor = 3,	// �ҋ@�^���A���]�p
		st_end = 99		// �I��
	};
	/// <summary>
	/// �X���b�h�R���g���[���f�[�^
	/// </summary>
	typedef struct _st {
		_st() :
			status(e_Status::st_sleep),
			var_rnd(""),
			endflg(false),
			caltimer(0.0),
			hRndDisp(nullptr),
			hThread(nullptr)
		{}
		std::thread* hThread;	// �X���b�h�I�u�W�F�N�g
		std::thread* hRndDisp;	// �X���b�h�����\���X���b�h�I�u�W�F�N�g
		int	status;				// �����R���g���[���i0=�I���A1=�ꎞ��~�A2=���s���j
		bool endflg;
		std::string var_rnd;	// �X���b�h���̗���
		double caltimer;		// �X�g���X�������ԁinano�b=10^-9=1/1,000,000,000�j
	} _T_THREAD_CTRL_DATA;
	/// <summary>
	///  �}���`�X���b�h�Ǘ��e�[�u��
	/// </summary>
	typedef std::map<
		int,				// �X���b�hID
		_T_THREAD_CTRL_DATA	// �X���b�h�R���g���[���f�[�^
	> _T_THREAD_MGR_TBL;
	//////////////////////////////
	// �}���`�X���b�h�N���X
	//////////////////////////////
	class MultiThread
	{
	public:
		MultiThread():
			m_RunningCount(0),
			MTM_Tbl{}
		{}
	private:
		// ���ݎ��s���̃X���b�h��
		int m_RunningCount;
	public:
		// ���ݎ��s���̃X���b�h�����v���X
		void PlusCounter();
		// ���ݎ��s���̃X���b�h�����}�C�i�X
		void MinusCounter();
		// ���ݎ��s���̃X���b�h�����[���ɂ���
		void ZeroCounter();
		// ���ݎ��s���X���b�h�̃J�E���g���s��
		void SetUpdateCounter();
		// ���ݎ��s���X���b�h�̃J�E���g��ݒ肷��
		void SetUpdateCounter(int var);
		// ���ݎ��s���X���b�h�̃J�E���g���擾
		int GetUpdateCounter();
		// �Y������X���b�h��񓯊��ŋN������B
		void ExecThread(int thrdID);
		// �Y������X���b�h�̃X�e�[�^�X�ƗD��x���R���g���[������
		void CtrlThread(
			int thrdID,
			int status
		);
		// �X���b�h�̏I���t���O��ݒ�
		void SetEndFlg(int thrdID, bool var);
		// �X���b�h�̏I���t���O���擾
		bool GetEndFlg(int thrdID);
		// �X���b�h�X�e�[�^�X�ݒ�擾
		void SetStatus(int thrdID, int var);
		// �}���`�X���b�h�̃X�e�[�^�X�擾
		int GetStatus(int thrdID);
		// �X�e�[�^�X���]
		int GetReverse(int thrdID);
		// �}���`�X���b�h�e�[�u���̃T�C�Y���擾
		size_t GetMthrSize();
		// �X���b�h�|�C���^���擾
		std::thread* GetThrdPointer(int thrdID);
		// �X���b�h�̃n���h�����擾
		std::thread::native_handle_type GetThrdNativeHnd(int thrdID);
		// �}���`�X���b�h�̗�����������擾
		std::string GetRndStr(int thrdID);
		// �}���`�X���b�h�̗����������ݒ�
		void SetRndStr(
			int thrdID,
			std::string& rndstr
		);
		// �X���b�h�������Ԑݒ�
		void SetThreadTimes(int thrdID, double var);
		// �X���b�h�������Ԏ擾�i���[�v�j
		double GetThreadTimes(int thrdID);
		// �X���b�h�������Ԏ擾
		double GetCalTimer(int thrdID);
		// �}���`�X���b�h�Ǘ��e�[�u��
		_T_THREAD_MGR_TBL MTM_Tbl;
	};
}