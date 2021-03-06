#pragma once


// CDlgConfigCoderDebug 对话框

class CDlgConfigCoderDebug : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfigCoderDebug)

public:
	CDlgConfigCoderDebug(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgConfigCoderDebug();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_SYSDEBUG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	// 是否允许编程者内部诊断PCR分析结果
	int m_bEnablePcrDebug;
	// 是否允许编程者内部诊断TS速率计算结果
	int m_bEnableTSRateDebug;
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
