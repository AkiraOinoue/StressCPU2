
// StressCPU.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//
/*
# StressCPU
Stress And Flops CPU</br>
Windows Application</br>
32bit Application</br>
Created by Visual Studio 2019 / C++ / MFC</br>
running need to install: https://www.microsoft.com/ja-JP/download/details.aspx?id=52685</br>
*/
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CStressCPUApp:
// このクラスの実装については、StressCPU.cpp を参照してください
//

class CStressCPUApp : public CWinApp
{
public:
	CStressCPUApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CStressCPUApp theApp;
