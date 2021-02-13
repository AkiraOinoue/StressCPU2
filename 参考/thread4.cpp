//	�_�C�A���O�{�b�N�X�Ŏw�肵���C�ӂ�CPU�ɃX���b�h������t����@���s�����ύX�\

#include <windows.h>
#include <tchar.h>
#include "resource.h"

#pragma comment(lib,"comctl32.lib")


//	�_��CPU�����擾����

int GetCPUMax(void){
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	return sys.dwNumberOfProcessors;
}

volatile int end_f = 0;	//	�X���b�h���I��������Ƃ���0�ȊO�ɐݒ�

//	�d���ׂ̃X���b�h

DWORD WINAPI  high_load(LPVOID lp){
	while (end_f == 0);
	return 0;
}

HINSTANCE hInst;

//	�_�C�A���O�{�b�N�X�v���V�[�W���[

LRESULT CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	static int cpuMax;
	static HWND* hCpuv = 0;
	TCHAR buf[16];
	static HANDLE handle = 0;	//	�X���b�h�n���h��
	static DWORD id;
	static int cpun = 0;

	switch (msg) {
	case WM_INITDIALOG:{
		cpuMax = GetCPUMax();
		hCpuv = new HWND[cpuMax];
		hCpuv[0] = GetDlgItem(hDlg, IDC_RADIOBOX000);
		int x1, y1, lx, ly;
		int id;
		RECT rect;
		POINT pt;
		GetWindowRect(hCpuv[0], &rect);
		pt.x = rect.left;
		pt.y = rect.top;
		ScreenToClient(hDlg, &pt);
		rect.left = pt.x;
		rect.top = pt.y;
		pt.x = rect.right;
		pt.y = rect.bottom;
		ScreenToClient(hDlg, &pt);
		rect.right = pt.x;
		rect.bottom = pt.y;

		x1 = rect.left;
		y1 = rect.top;
		lx = rect.right - rect.left;
		ly = rect.bottom - rect.top;
		id = IDC_RADIOBOX000;
		int cmax = 8;	// �X���b�h�ő��
		int x = 0;
		int y = 0;
		for (int n = 0; n < cpuMax; n++){
			if (0 < n){
				_stprintf_s(buf, sizeof(buf) / sizeof(TCHAR), _TEXT("%i"), n);
				hCpuv[n] = CreateWindow(
					TEXT("BUTTON"), buf,
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
					x1 + (lx)*x, y1 + ly*y, lx, ly,
					hDlg, (HMENU)(id + n), ::hInst, NULL
				);
				SendMessage(hCpuv[n], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(true, 0));
			}
			if (cmax <= ++x){
				x = 0;
				++y;
			}
		}
		SendMessage(hCpuv[0], BM_SETCHECK, BST_CHECKED, 0);
		_stprintf_s(buf, sizeof(buf) / sizeof(TCHAR), _TEXT("CPU %i"), cpun);
		SetWindowText(GetDlgItem(hDlg, IDC_LABEL), buf);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:{
			EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
			handle = CreateThread(0, 0, &high_load, 0, 0, &id);	//	�X���b�h���쐬
			SetThreadAffinityMask(handle, 1 << cpun);	//	CPU���Œ�
			return TRUE;
		}
		case IDCANCEL:{
			end_f = -1;
			if (handle){
				WaitForSingleObject(handle, INFINITE); // �X���b�h�I����҂�
				CloseHandle(handle);
			}
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		default:
			if (IDC_RADIOBOX000 <= LOWORD(wParam) || LOWORD(wParam) <= IDC_RADIOBOX000 + cpuMax - 1){
				cpun = LOWORD(wParam) - IDC_RADIOBOX000;	//	�r�b�g�ԍ�
				_stprintf_s(buf, sizeof(buf) / sizeof(TCHAR), _TEXT("CPU %i"), cpun);
				SetWindowText(GetDlgItem(hDlg, IDC_LABEL), buf);
				if (handle)
					SetThreadAffinityMask(handle, 1 << cpun);	//	CPU���Œ�
			}
			return FALSE;

		}
	default:
		return FALSE;
	}
	return TRUE;
}



int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow){
	::hInst = hCurInst;
	//	�_�C�A���O�{�b�N�X�̕\��
	DialogBox(hCurInst, TEXT("DLG1"), 0, (DLGPROC)DlgProc);

	return 0;
}
