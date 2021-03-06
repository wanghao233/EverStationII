// DlgConfigWinpcap.cpp: 实现文件
//

#include "stdafx.h"
#include "EverStationII.h"
#include "DlgConfigWinpcap.h"
#include "afxdialogex.h"

#include <assert.h>

#include "libs_Utilities\Include\XStream_Utilities.h"

// CDlgConfigWinpcap 对话框
U32 WINPCAP_Debug_udp_dump(LPVOID pParam)
{
	CDlgConfigWinpcap* pDlg = (CDlgConfigWinpcap*)pParam;

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	char pszTick[128];
	char errbuf[4096];
	//int inum;
	pcap_t *adhandle;
	u_int netmask;
	char packet_filter[] = "ip and udp";
	struct bpf_program fcode;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		//		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
	}
	else
	{
		int			 nSel = pDlg->GetSelectedDevice();
		assert(nSel >= 0);

		for (d = alldevs, i = 0; i < nSel; d = d->next, i++);

		/* Open the adapter */
		if ((adhandle = pcap_open_live(d->name,	// name of the device
			65536,			// portion of the packet to capture. 
							// 65536 grants that the whole packet will be captured on all the MACs.
			1,				// promiscuous mode (nonzero means promiscuous)
			1000,			// read timeout
			errbuf			// error buffer
		)) == NULL)
		{
			pDlg->AppendLog(NULL, errbuf);
			//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
			/* Free the device list */
			//pcap_freealldevs(alldevs);
			//return -1;
		}

		/* Check the link layer. We support only Ethernet for simplicity. */
		if (pcap_datalink(adhandle) != DLT_EN10MB)
		{
			//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
			pDlg->AppendLog(NULL, "This program works only on Ethernet networks.");
			/* Free the device list */
			//pcap_freealldevs(alldevs);
			//return -1;
		}

		if (d->addresses != NULL)
			/* Retrieve the mask of the first address of the interface */
			netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
		else
			/* If the interface is without addresses we suppose to be in a C class network */
			netmask = 0xffffff;


		//compile the filter
		if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
		{
			pDlg->AppendLog(NULL, "Unable to compile the packet filter. Check the syntax.\n");
			//fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
			/* Free the device list */
			//pcap_freealldevs(alldevs);
			//return -1;
		}

		//set the filter
		if (pcap_setfilter(adhandle, &fcode) < 0)
		{
			pDlg->AppendLog(NULL, "Error setting the filter.\n");
			//fprintf(stderr, "\nError setting the filter.\n");
			///* Free the device list */
			//pcap_freealldevs(alldevs);
			//return -1;
		}

		//		printf("\nlistening on %s...\n", d->description);
		sprintf_s(errbuf, sizeof(errbuf), "listening on %s...\n", d->description);
		pDlg->AppendLog(NULL, errbuf);

		/* At this point, we don't need any more the device list. Free it */
		//pcap_freealldevs(alldevs);

		/* start the capture */
		//pcap_loop(adhandle, 0, packet_handler, NULL);

		time_t local_tv_sec;
		struct tm ltime;
		char timestr[16];

		while (pDlg->m_nDiagnosing)
		{
			int res = pcap_next_ex(adhandle, &header, &pkt_data);
			if (res >= 0)
			{
				if (res > 0)
				{
					/* convert the timestamp to readable format */
					local_tv_sec = header->ts.tv_sec;
					localtime_s(&ltime, &local_tv_sec);
					strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);

					/* print timestamp and length of the packet */
					sprintf_s(pszTick, sizeof(pszTick), "%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

					/* print pkt timestamp and pkt len */
					//sprintf_s(pszTick, sizeof(pszTick), "%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);

					/* Print the packet */
					for (i = 0; i < (int)header->caplen; i++)
					{
						sprintf_s(errbuf + i*3, 4, "%.2x ", pkt_data[i]);
					}
					sprintf_s(errbuf + i * 3, 3, "\n");
					//if ((i % LINE_LEN) == 0) printf("\n");

					pDlg->AppendLog(pszTick, errbuf);
				}
			}
			else
			{
				break;
			}
		}

		pcap_close(adhandle);

		sprintf_s(errbuf, sizeof(errbuf), "exit capturing on %s...\n", d->description);
		pDlg->AppendLog(NULL, errbuf);

		/* At this point, we don't need any more the device list. Free it */
		pcap_freealldevs(alldevs);
	}

	return 0;
}

IMPLEMENT_DYNAMIC(CDlgConfigWinpcap, CDialogEx)

CDlgConfigWinpcap::CDlgConfigWinpcap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_WINPCAP, pParent)
{
	m_palldevs = NULL;
	m_nDeviceCount = 0;
}

CDlgConfigWinpcap::~CDlgConfigWinpcap()
{
}

void CDlgConfigWinpcap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PCAP_DEVICES, m_listDevices);
	DDX_Control(pDX, IDC_LIST_PCAP_DEBUG_LOG, m_listLogs);
}


BEGIN_MESSAGE_MAP(CDlgConfigWinpcap, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_PCAP_DIAGNOSE, &CDlgConfigWinpcap::OnClickedBtnPcapDiagnose)
	ON_BN_CLICKED(IDOK, &CDlgConfigWinpcap::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConfigWinpcap 消息处理程序


BOOL CDlgConfigWinpcap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rectDlg;
	GetClientRect(&rectDlg);

	// TODO:  在此添加额外的初始化
	CRect rect;
	m_listDevices.GetClientRect(&rect);
	m_listDevices.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	//m_listDevices.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_listDevices.InsertColumn(0, "", LVCFMT_CENTER, 30, -1);					//ICON

	int width_devices = (int)((rect.Width() - 30) * 0.6);
	int width_notes = rect.Width() - 30 - width_devices;

	m_listDevices.InsertColumn(1, "设备名称", LVCFMT_LEFT, width_devices, -1);					//ICON
	m_listDevices.InsertColumn(2, "注释", LVCFMT_LEFT, width_notes, -1);

	m_listLogs.GetClientRect(&rect);
	m_listLogs.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_listLogs.InsertColumn(0, "", LVCFMT_CENTER, 20, -1);					//ICON
	m_listLogs.InsertColumn(1, "序  号", LVCFMT_LEFT, 80, -1);					//ICON
	m_listLogs.InsertColumn(2, "时间戳", LVCFMT_LEFT, 160, -1);

	int width_event = rect.Width() - 260;
	m_listLogs.InsertColumn(3, "事件描述", LVCFMT_LEFT, width_event, -1);

	//pcap_if_t *alldevs;
	pcap_if_t *d;
	//int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list */
	if (pcap_findalldevs(&m_palldevs, errbuf) == -1)
	{
//		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		AppendLog(NULL, errbuf);
	}
	else
	{
		char		 pszExeFile[MAX_PATH];
		char		 pszExePath[MAX_PATH];
		char		 pszIniFile[MAX_PATH];
		char		 pszIFSelected[MAX_PATH];
		int			 len;

		GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
		len = GetModulePathLength(pszExeFile);
		assert(len > 0);

		memcpy(pszExePath, pszExeFile, len);
		pszExePath[len] = '\0';
		sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszExePath);

		GetPrivateProfileStringA((LPCSTR)"winpcap", (LPCSTR)"网卡名称", NULL, pszIFSelected, sizeof(pszIFSelected), (LPCSTR)pszIniFile);

		/* Print the list */
		int			 nSel = -1;
		m_nDeviceCount = 0;
		for (d = m_palldevs; d; d = d->next)
		{
//			printf("%d. %s", ++i, d->name);
			//AppendLog(d->name);

			if (strcmp(d->name, pszIFSelected) == 0)
			{
				nSel = m_nDeviceCount;
			}
			else
			{
			}

			if (d->description)
			{
//				printf(" (%s)\n", d->description);
				//AppendLog(d->description);
				AppendDevice(d->name, d->description);
			}
			else
			{
//				printf(" (No description available)\n");
				AppendDevice(d->name, "No description available");
			}

			m_nDeviceCount++;
		}

		if (m_nDeviceCount == 0)
		{
//			printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
			AppendLog(NULL, "No interfaces found! Make sure WinPcap is installed.");
		}
		else
		{
			if (nSel == -1)
			{
				nSel = m_nDeviceCount - 1;

				int i;
				for (d = m_palldevs, i = 0; i < nSel; d = d->next, i++);

				WritePrivateProfileString("winpcap", "网卡名称", d->name, pszIniFile);
				WritePrivateProfileString("winpcap", "网卡描述", d->description, pszIniFile);
			}

			if (nSel >= 0)
			{
				m_listDevices.SetCheck(nSel);
			}
		}

		UpdateData(false);

		/* At this point, we don't need any more the device list. Free it */
		//pcap_freealldevs(alldevs);

		m_nDiagnosing = 0;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgConfigWinpcap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CDlgConfigWinpcap::AppendLog(char* pszTime, char* pszMessage)
{
	int nCount;

	nCount = m_listLogs.GetItemCount();
	m_listLogs.InsertItem(nCount, "");
	m_listLogs.SetItemText(nCount, 2, pszTime);
	m_listLogs.SetItemText(nCount, 3, pszMessage);
}

void CDlgConfigWinpcap::AppendDevice(char* pszDevice, char* pszDescription)
{
	int nCount;

	nCount = m_listDevices.GetItemCount();
	m_listDevices.InsertItem(nCount, "");
	m_listDevices.SetItemText(nCount, 1, pszDevice);
	m_listDevices.SetItemText(nCount, 2, pszDescription);
}

int CDlgConfigWinpcap::GetSelectedDevice(void)
{
	int count = m_listDevices.GetItemCount();
	int nSel = -1;
	for (int item = 0; item < count; item++)
	{
		//		int state = m_listDevices.GetItemState(item, LVIS_SELECTED);
		if (m_listDevices.GetCheck(item))
		{
			nSel = item;
			break;
		}
	}

	return nSel;
}

void CDlgConfigWinpcap::OnClickedBtnPcapDiagnose()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pWnd = GetDlgItem(IDC_BTN_PCAP_DIAGNOSE);
	CString strCmd;
	pWnd->GetWindowTextA(strCmd);

	if (strCmd == "诊断")
	{
		pWnd->SetWindowTextA("停止");

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(false);

		pWnd = GetDlgItem(IDCANCEL);
		pWnd->EnableWindow(false);

		m_nDiagnosing = 1;
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WINPCAP_Debug_udp_dump, this, 0, 0);
	}
	else
	{
		pWnd->SetWindowTextA("诊断");

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow();

		pWnd = GetDlgItem(IDCANCEL);
		pWnd->EnableWindow();

		m_nDiagnosing = 0;
	}
}

BOOL CDlgConfigWinpcap::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	pcap_freealldevs(m_palldevs);
	m_nDeviceCount = 0;

	return CDialogEx::DestroyWindow();
}


void CDlgConfigWinpcap::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	int count = m_listDevices.GetItemCount();
	int nSel = -1;
	for (int item = 0; item < count; item++)
	{
//		int state = m_listDevices.GetItemState(item, LVIS_SELECTED);
		if (m_listDevices.GetCheck(item))
		{
			nSel = item;

			char	pszExeFile[MAX_PATH];
			char	pszExePath[MAX_PATH];
			char	pszIniFile[MAX_PATH];
			int		len;

			GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
			len = GetModulePathLength(pszExeFile);
			assert(len > 0);

			memcpy(pszExePath, pszExeFile, len);
			pszExePath[len] = '\0';
			sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszExePath);

			int i;
			pcap_if_t *d;
			for (d = m_palldevs, i = 0; i < nSel; d = d->next, i++);

			WritePrivateProfileString("winpcap", "网卡名称", d->name, pszIniFile);
			WritePrivateProfileString("winpcap", "网卡描述", d->description, pszIniFile);

			break;
		}
	}

	CDialogEx::OnOK();
}
