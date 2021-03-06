
// EverStationII.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "EverStationII.h"
#include "MainFrm.h"

#include "ChildFrm.h"
//#include "EverStationIIDoc.h"
//#include "EverStationIIView.h"
#include "./Magic_TS/TSMagicView.h"
#include "./Magic_TS/TSMagicDoc.h"
#include "./Magic_PS/PSMagicView.h"
#include "./Magic_PS/PSMagicDoc.h"
#include "./Magic_ES/ESMagicView.h"
#include "./Magic_ES/ESMagicDoc.h"
#include "./Magic_YUV/YUVMagicView.h"
#include "./Magic_YUV/YUVMagicDoc.h"
#include "./DlgConfigCoderDebug.h"
#include "./DlgConfigWinpcap.h"
#include "./DlgConfigTSInput.h"

#include "libs_Utilities\Include\XStream_Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2D(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3D(x64).lib")
#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x64/wpcap(x64).lib")
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XMLD(x64).lib")
//#pragma comment(lib, "HAL/HAL_DBMS/lib/HAL_DBMSD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_SectionD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacketD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PSPacket/lib/MPEG_PSPacketD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacketD(x64).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x64).lib")
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPVD(x64).lib")
#pragma comment(lib, "libs_ES/ES_H264/lib/ES_H264D(x64).lib")
#pragma comment(lib, "libs_ES/ES_AVS/lib/ES_AVSD(x64).lib")
#pragma comment(lib, "libs_ES/ES_AVS2/lib/ES_AVS2D(x64).lib")
#pragma comment(lib, "libs_ES/ES_AAC/lib/ES_AACD(x64).lib")
#pragma comment(lib, "libs_ES/ES_MPA/lib/ES_MPAD(x64).lib")
#pragma comment(lib, "libs_ES/ES_AC3/lib/ES_AC3D(x64).lib")
#pragma comment(lib, "libs_ES/ES_DRA/lib/ES_DRAD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TransportStream/lib/MiddleWare_TransportStreamD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_ESDecoder/lib/MiddleWare_ESDecoderD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_Utilities/lib/MiddleWare_UtilitiesD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_PayloadSplicer/lib/MiddleWare_TS_PayloadSplicerD(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_DBases/lib/MiddleWare_TS_DBasesD(x64).lib")
#else
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2D(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3D(x32).lib")
#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x32/wpcap(x32).lib")
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XMLD(x32).lib")
//#pragma comment(lib, "HAL/HAL_DBMS/lib/HAL_DBMSD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_SectionD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacketD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PSPacket/lib/MPEG_PSPacketD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacketD(x32).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x32).lib")
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPVD(x32).lib")
#pragma comment(lib, "libs_ES/ES_AVS/lib/ES_AVSD(x32).lib")
#pragma comment(lib, "libs_ES/ES_AVS2/lib/ES_AVS2D(x32).lib")
#pragma comment(lib, "libs_ES/ES_H264/lib/ES_H264D(x32).lib")
#pragma comment(lib, "libs_ES/ES_AAC/lib/ES_AACD(x32).lib")
#pragma comment(lib, "libs_ES/ES_MPA/lib/ES_MPAD(x32).lib")
#pragma comment(lib, "libs_ES/ES_AC3/lib/ES_AC3D(x32).lib")
#pragma comment(lib, "libs_ES/ES_DRA/lib/ES_DRAD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TransportStream/lib/MiddleWare_TransportStreamD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_ESDecoder/lib/MiddleWare_ESDecoderD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_Utilities/lib/MiddleWare_UtilitiesD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_PayloadSplicer/lib/MiddleWare_TS_PayloadSplicerD(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_DBases/lib/MiddleWare_TS_DBasesD(x32).lib")

#endif

#else

#ifdef _WIN64
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3(x64).lib")
#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x64/wpcap(x64).lib")
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XML(x64).lib")
//#pragma comment(lib, "HAL/HAL_DBMS/lib/HAL_DBMS(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_Section(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacket(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PSPacket/lib/MPEG_PSPacket(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacket(x64).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x64).lib")
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPV(x64).lib")
#pragma comment(lib, "libs_ES/ES_H264/lib/ES_H264(x64).lib")
#pragma comment(lib, "libs_ES/ES_AVS/lib/ES_AVS(x64).lib")
#pragma comment(lib, "libs_ES/ES_AVS2/lib/ES_AVS2(x64).lib")
#pragma comment(lib, "libs_ES/ES_AAC/lib/ES_AAC(x64).lib")
#pragma comment(lib, "libs_ES/ES_MPA/lib/ES_MPA(x64).lib")
#pragma comment(lib, "libs_ES/ES_AC3/lib/ES_AC3(x64).lib")
#pragma comment(lib, "libs_ES/ES_DRA/lib/ES_DRA(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TransportStream/lib/MiddleWare_TransportStream(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_ESDecoder/lib/MiddleWare_ESDecoder(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_Utilities/lib/MiddleWare_Utilities(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_PayloadSplicer/lib/MiddleWare_TS_PayloadSplicer(x64).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_DBases/lib/MiddleWare_TS_DBases(x64).lib")
#else
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3(x32).lib")
#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x32/wpcap(x32).lib")
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XML(x32).lib")
//#pragma comment(lib, "HAL/HAL_DBMS/lib/HAL_DBMS(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_Section(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacket(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PSPacket/lib/MPEG_PSPacket(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacket(x32).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x32).lib")
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPV(x32).lib")
#pragma comment(lib, "libs_ES/ES_H264/lib/ES_H264(x32).lib")
#pragma comment(lib, "libs_ES/ES_AVS/lib/ES_AVS(x32).lib")
#pragma comment(lib, "libs_ES/ES_AVS2/lib/ES_AVS2(x32).lib")
#pragma comment(lib, "libs_ES/ES_AAC/lib/ES_AAC(x32).lib")
#pragma comment(lib, "libs_ES/ES_MPA/lib/ES_MPA(x32).lib")
#pragma comment(lib, "libs_ES/ES_AC3/lib/ES_AC3(x32).lib")
#pragma comment(lib, "libs_ES/ES_DRA/lib/ES_DRA(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TransportStream/lib/MiddleWare_TransportStream(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_ESDecoder/lib/MiddleWare_ESDecoder(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_Utilities/lib/MiddleWare_Utilities(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_PayloadSplicer/lib/MiddleWare_TS_PayloadSplicer(x32).lib")
#pragma comment(lib, "MiddleWare/MiddleWare_TS_DBases/lib/MiddleWare_TS_DBases(x32).lib")
#endif

#endif

// CEverStationIIApp

BEGIN_MESSAGE_MAP(CEverStationIIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CEverStationIIApp::OnAppAbout)
	// 基于文件的标准文档命令
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	//ON_COMMAND(ID_FILE_SAVE, &CEverStationIIApp::OnFileSave)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(IDC_BTN_TSMAGIC, &CEverStationIIApp::OnBtnTSMagic)
	ON_COMMAND(IDC_BTN_PSMAGIC, &CEverStationIIApp::OnBtnPSMagic)
	ON_COMMAND(IDC_BTN_YUVMAGIC, &CEverStationIIApp::OnBtnYUVMagic)
	ON_COMMAND(IDC_BTN_ESMAGIC, &CEverStationIIApp::OnBtnESMagic)
	ON_COMMAND(ID_MENU_SETUP_SYS_DEBUG, &CEverStationIIApp::OnMenuSetupCoderDebug)
	ON_COMMAND(ID_MENU_SETUP_WINPCAP_DEBUG, &CEverStationIIApp::OnMenuSetupWinpcapDebug)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_SYS_DEBUG, &CEverStationIIApp::OnUpdateMenuSetupCoderDebug)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_WINPCAP_DEBUG, &CEverStationIIApp::OnUpdateMenuSetupWinpcapDebug)
	ON_COMMAND(ID_MENU_SETUP_TS_INPUT, &CEverStationIIApp::OnMenuSetupTsInput)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_TS_INPUT, &CEverStationIIApp::OnUpdateMenuSetupTsInput)
END_MESSAGE_MAP()


// CEverStationIIApp 构造

CEverStationIIApp::CEverStationIIApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("EverStationII.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CEverStationIIApp 对象

CEverStationIIApp theApp;


#include "thirdparty_libs/SQLite3/Include/sqlite3.h"  

int user_callback(void* param, int nColCount, char** pColValue, char** pColName)
{
	char pszMsg[256];
	//CString s;
	for (int i = 0; i<nColCount; i++)
	{
		sprintf_s(pszMsg, sizeof(pszMsg), "<col=%d>%s:%s\n", i, pColName[i], pColValue[i]);
		//	s += pColName[i];
		//	s += ":";
		//	s += pColValue[i];
		//	s += "\t";
		TRACE("user_callback: %s", pszMsg);
	}
	//s += "\n";
	//TRACE("user_callback: %s", s.GetBuffer());
	//cout << s << endl;
	return 0;
}

int sqlite3_test()
{
	char	pszDbFile[MAX_PATH];

	char	pszExeFile[MAX_PATH];
	char	exeDrive[3];
	char	pszDbaseDir[MAX_PATH];
	GetModuleFileName(NULL, pszExeFile, MAX_PATH);
	exeDrive[0] = pszExeFile[0];
	exeDrive[1] = pszExeFile[1];
	exeDrive[2] = '\0';

	sprintf_s(pszDbaseDir, sizeof(pszDbaseDir), "%s\\~EverStationII\\dbase", exeDrive);
	BuildDirectory(pszDbaseDir);

	sprintf_s(pszDbFile, sizeof(pszDbFile), "%s\\EverStationII.db", pszDbaseDir);

	char strSql[256];

	sqlite3* pdb;
	int nResult = sqlite3_open(pszDbFile, &pdb);
	if (nResult != SQLITE_OK)
	{
		//TRACE("打开数据库失败：%s\n", sqlite3_errmsg(pdb));
		//cout << "打开数据库失败：" << sqlite3_errmsg(db) << endl;

		return 0;
	}
	else
	{
		//TRACE("数据库打开成功!\n");
		//cout << "数据库打开成功" << endl;
	}

	char* errmsg;
	nResult = sqlite3_exec(pdb, "create table APP(id integer primary key autoincrement,name varchar(100))", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("创建表失败(errmsg)：%s\n", errmsg);
		//TRACE("创建表失败(sqlite3_errmsg)：%s\n", sqlite3_errmsg(pdb));
		sqlite3_free(errmsg);
	}

	//CString strSql;
	//for (int i = 0; i<100; i++)
	//{
	//	strSql += "insert into fuck values(null,'caonima');";
	//}
	//TRACE(strSql);

	//cout << strSql << endl;
	//nResult = sqlite3_exec(pdb, strSql.GetBuffer(), NULL, NULL, &errmsg);
	nResult = sqlite3_exec(pdb, "insert into APP values(null,'packets');", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		//TRACE("插入数据失败(errmsg)：%s", errmsg);
		//TRACE("插入数据失败(sqlite3_errmsg)：%s", sqlite3_errmsg(pdb));
	}

	strcpy_s(strSql, sizeof(strSql), "select * from APP");
	nResult = sqlite3_exec(pdb, strSql, user_callback, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("读数据表(errmsg)：%s\n", errmsg);
		//cout << errmsg << endl;
		sqlite3_free(errmsg);
		return 0;
	}

	sqlite3_close(pdb);
	return 0;
}

// CEverStationIIApp 初始化

BOOL CEverStationIIApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("码流分析工作站"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	//CMultiDocTemplate* pDocTemplate;
	//m_pDocTemplate = new CMultiDocTemplate(IDR_EverStationIITYPE,
	//	RUNTIME_CLASS(CEverStationIIDoc),
	//	RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
	//	RUNTIME_CLASS(CEverStationIIView));
	//if (!m_pDocTemplate)
	//	return FALSE;
	//AddDocTemplate(pDocTemplate);

	m_pDocTemplateForTS = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CTSMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CTSMagicView));

	m_pDocTemplateForPS = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CPSMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPSMagicView));

	m_pDocTemplateForES = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CESMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CESMagicView));

	m_pDocTemplateForYUV = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CYUVMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CYUVMagicView));

	//m_pTSMagicFrame = NULL;
	//m_pYUVMagicFrame = NULL;
	//m_pESMagicFrame = NULL;
	//m_pPSMagicFrame = NULL;

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	//OnBtnYUVMagic();
	//OnBtnESMagic();
	OnBtnTSMagic();
	//OnBtnPSMagic();

	m_bRuning = 0;

	//sqlite3_test();

	return TRUE;
}

int CEverStationIIApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	if (m_pDocTemplateForTS != nullptr)
	{
		delete m_pDocTemplateForTS;
		m_pDocTemplateForTS = NULL;
	}
	if (m_pDocTemplateForPS != nullptr)
	{
		delete m_pDocTemplateForPS;
		m_pDocTemplateForPS = NULL;
	}
	if (m_pDocTemplateForES != nullptr)
	{
		delete m_pDocTemplateForES;
		m_pDocTemplateForES = NULL;
	}
	if (m_pDocTemplateForYUV != nullptr)
	{
		delete m_pDocTemplateForYUV;
		m_pDocTemplateForYUV = NULL;
	}

	return CWinAppEx::ExitInstance();
}

// CEverStationIIApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SIZE()
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CEverStationIIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CEverStationIIApp 自定义加载/保存方法

void CEverStationIIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CEverStationIIApp::LoadCustomState()
{
}

void CEverStationIIApp::SaveCustomState()
{
}

// CEverStationIIApp 消息处理程序



void CEverStationIIApp::OnBtnTSMagic()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL	bNew = FALSE;

	//if (m_pTSMagicFrame == NULL)
	//{
	//	bNew = TRUE;
	//}
	//else
	//{
	//	if (!::IsWindow(m_pTSMagicFrame->GetSafeHwnd()))
	//	{
	//		bNew = TRUE;
	//	}
	//}

	//if (bNew)
	//{
	//	m_pTSMagicFrame = (CTSMagicFrame*)(((CMainFrame*)m_pMainWnd)->CreateNewChild(RUNTIME_CLASS(CTSMagicFrame), IDR_TSMAGIC, m_hMDIMenu, m_hMDIAccel));
	//}
	//else
	//{
	//	m_pTSMagicFrame->ActivateFrame(SW_MAXIMIZE);
	//}

	 //注册应用程序的文档模板。  文档模板
	 //将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForTS != nullptr)
	{
		POSITION docPos = m_pDocTemplateForTS->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForTS->OpenDocumentFile(NULL);
			pDoc->SetTitle("MPEG2/DVB TS流分析");
		}
		else
		{
		}
	}
}

void CEverStationIIApp::OnBtnYUVMagic()
{
	// TODO: Add your control notification handler code here
	//BOOL	bNew = FALSE;

	//if (m_pYUVMagicFrame == NULL)
	//{
	//	bNew = TRUE;
	//}
	//else
	//{
	//	if (!::IsWindow(m_pYUVMagicFrame->GetSafeHwnd()))
	//	{
	//		bNew = TRUE;
	//	}
	//}

	//if (bNew)
	//{
	//	m_pYUVMagicFrame = (CYUVMagicFrame*)(((CMainFrame*)m_pMainWnd)->CreateNewChild(RUNTIME_CLASS(CYUVMagicFrame), IDR_YUVMAGIC, m_hMDIMenu, m_hMDIAccel));
	//}
	//else
	//{
	//	m_pYUVMagicFrame->ActivateFrame(SW_MAXIMIZE);
	//}

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForYUV != nullptr)
	{
		POSITION docPos = m_pDocTemplateForYUV->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForYUV->OpenDocumentFile(NULL);
			pDoc->SetTitle("YUV序列分析");
		}
		else
		{
		}
	}
}

void CEverStationIIApp::OnBtnESMagic()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL	bNew = FALSE;

	//if (m_pESMagicFrame == NULL)
	//{
	//	bNew = TRUE;
	//}
	//else
	//{
	//	if (!::IsWindow(m_pESMagicFrame->GetSafeHwnd()))
	//	{
	//		bNew = TRUE;
	//	}
	//}

	//if (bNew)
	//{
	//	m_pESMagicFrame = (CESMagicFrame*)(((CMainFrame*)m_pMainWnd)->CreateNewChild(RUNTIME_CLASS(CESMagicFrame), IDR_ESMAGIC, m_hMDIMenu, m_hMDIAccel));
	//}
	//else
	//{
	//	m_pESMagicFrame->ActivateFrame(SW_MAXIMIZE);
	//}

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForES != nullptr)
	{
		POSITION docPos = m_pDocTemplateForES->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForES->OpenDocumentFile(NULL);
			pDoc->SetTitle("ES流分析");
		}
		else
		{
		}
	}
}

void CEverStationIIApp::OnBtnPSMagic()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL	bNew = FALSE;

	//if (m_pPSMagicFrame == NULL)
	//{
	//	bNew = TRUE;
	//}
	//else
	//{
	//	if (!::IsWindow(m_pPSMagicFrame->GetSafeHwnd()))
	//	{
	//		bNew = TRUE;
	//	}
	//}

	//if (bNew)
	//{
	//	m_pPSMagicFrame = (CPSMagicFrame*)(((CMainFrame*)m_pMainWnd)->CreateNewChild(RUNTIME_CLASS(CPSMagicFrame), IDR_PSMAGIC, m_hMDIMenu, m_hMDIAccel));
	//}
	//else
	//{
	//	m_pPSMagicFrame->ActivateFrame(SW_MAXIMIZE);
	//}

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForPS != nullptr)
	{
		POSITION docPos = m_pDocTemplateForPS->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForPS->OpenDocumentFile(NULL);
			pDoc->SetTitle("PS流分析");
		}
		else
		{
		}
	}
}

int	CEverStationIIApp::GetRunningState()
{
	return m_bRuning;
}

int CEverStationIIApp::SetRunningState(int state)
{
	m_bRuning = state;
	return m_bRuning;
}

void CEverStationIIApp::OnMenuSetupCoderDebug()
{
	// TODO: 在此添加命令处理程序代码
#ifdef _DEBUG
	CDlgConfigCoderDebug dlgSysDebug;
	if (dlgSysDebug.DoModal() == IDOK)
	{

	}
#endif // 
}


void CEverStationIIApp::OnMenuSetupWinpcapDebug()
{
	// TODO: 在此添加命令处理程序代码
	CDlgConfigWinpcap dlgWinpcap;
	if (dlgWinpcap.DoModal() == IDOK)
	{

	}
}


void CEverStationIIApp::OnMenuSetupTsInput()
{
	// TODO: 在此添加命令处理程序代码
	CDlgConfigTSInput dlgTSInput;
	if (dlgTSInput.DoModal() == IDOK)
	{

	}
}

void CEverStationIIApp::OnUpdateMenuSetupCoderDebug(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
#ifdef _DEBUG
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
#else
	pCmdUI->Enable(false);
#endif
}


void CEverStationIIApp::OnUpdateMenuSetupWinpcapDebug(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
}

void CEverStationIIApp::OnUpdateMenuSetupTsInput(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	char pszModuleFileName[256];
	char* pszExeName = NULL;

	GetModuleFileNameA(NULL, pszModuleFileName, sizeof(pszModuleFileName));

	char*		 pdest;
	int			 idx;

	pdest = strchr(pszModuleFileName, '\\');
	while (pdest != NULL)
	{
		idx = (int)(pdest - pszModuleFileName);
		pdest = strchr(pszModuleFileName + idx + 1, '\\');
	}
	pszExeName = pszModuleFileName + idx + 1;

	//CString strAppName = AfxGetAppName();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_APPNAME_EVERSTATIONII);

	pWnd->SetWindowText(pszExeName);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

