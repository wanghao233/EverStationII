//

#include "stdafx.h"
#include <Winuser.h>
#include <Windows.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include <wingdi.h>
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_Histogram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Histogram
CInstrumentPanel_Histogram::CInstrumentPanel_Histogram()
{
	m_nChannleDepth = HISTGRAM_SAMPLE_DEPTH;
}

CInstrumentPanel_Histogram::~CInstrumentPanel_Histogram()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_Histogram, CInstrumentPanel_Base)
	//{{AFX_MSG_MAP(CInstrumentPanel_Histogram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_Histogram::DisplayMeasureGraph(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int		i;
	CRect	rectPaint;
	int		max_count = 0;

	int		nHistGram[HISTGRAM_HORZ_DIVISION];
	memset(nHistGram, 0x00, sizeof(nHistGram));

	double fBiasLevel = (double)(m_nXPositiveMark - m_nXNegtiveMark) / HISTGRAM_HORZ_DIVISION;


	BITMAP bm;
	if ((pGraphBmp != NULL) && (pMemDC != NULL))
	{
		if (pGraphBmp->GetSafeHandle() != NULL)
		{
			pGraphBmp->GetBitmap(&bm);
			pMemDC->SelectObject(pGraphBmp);

			CRect rectPicture;

			rectPicture.left = 0;
			rectPicture.top = 0;
			rectPicture.right = bm.bmWidth;
			rectPicture.bottom = bm.bmHeight;
			pMemDC->FillRect(&rectPicture, m_pBkBrush);

			double deltX = (double)rectPicture.Width() / HISTGRAM_HORZ_DIVISION;
			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				CBrush* pPaintBrush = new CBrush;
				pPaintBrush->CreateSolidBrush(pChannel->color);
				CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);

				::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
				if (pChannel->nSampleCount > 0) {

					if (fBiasLevel > 0)
					{
						if (m_nXAxisStyle == AXIS_STYLE_MEAN_SYMMETRY)		//对称型
						{
							for (i = 0; i < pChannel->nSampleCount; i++)
							{
								int distance = pChannel->pnXSampleArray[i] - m_nMeasuredXMeanValue;
								if ((distance > m_nXNegtiveMark) && (distance < m_nXPositiveMark))
								{
									int index = (int)((distance - m_nXNegtiveMark) / fBiasLevel);
									assert((index >= 0) && (index < HISTGRAM_HORZ_DIVISION));
									nHistGram[index] ++;

									if (nHistGram[index] > max_count)
									{
										max_count = nHistGram[index];
									}
								}
							}
						}
						else if (m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)  //非对称型
						{
							for (i = 0; i < pChannel->nSampleCount; i++)
							{
								if ((pChannel->pnXSampleArray[i] > m_nXNegtiveMark) && (pChannel->pnXSampleArray[i] < m_nXPositiveMark))
								{
									int index = (int)((pChannel->pnXSampleArray[i] - m_nXNegtiveMark) / fBiasLevel);
									assert((index >= 0) && (index < HISTGRAM_HORZ_DIVISION));
									nHistGram[index] ++;

									if (nHistGram[index] > max_count)
									{
										max_count = nHistGram[index];
									}
								}
							}
						}
					}

					if (max_count > 0)
					{
						rectPaint.bottom = rectPicture.bottom;

						for (i = 0; i < HISTGRAM_HORZ_DIVISION; i++)
						{
							double ratio = 0.9 * nHistGram[i] / max_count;
							assert(ratio < 1.0);

							rectPaint.left = (int)(rectPicture.left + i * deltX);
							rectPaint.right = (int)(rectPicture.left + (i + 1)*deltX);
							rectPaint.top = rectPicture.bottom - (int)(ratio * rectPicture.Height());
							//assert(rectPaint.top < rectContainer.top);
							pMemDC->FillRect(&rectPaint, pPaintBrush);
						}
					}
				}

				m_bNeedUpdate = 0;
				::SetEvent(pChannel->hSampleAccess);

				pMemDC->SelectObject(pOldBrush);
				delete pPaintBrush;
			}
		}
	}
}

//注：这里varValue是相对于offset的标准方差
//ideaValue -- 期望接近真值的一个理想值，实际也是存在误差的估计值
//nSampleCurValue -- 样本当前测量值
//nSampleMeanValue -- 样本长期测量均值
//nSampleVarValue -- 样本长期最大偏离度
void CInstrumentPanel_Histogram::AppendSample(int ID, int sampleValue, SAMPLE_ATTRIBUTE_t* attr)
{
	int nNegtiveBias, nPositiveBias;

	if (attr != NULL)
	{
		m_nMeasuredXMeanValue = attr->mean;
		m_nMeasuredXRmsValue = attr->rms;

		if (attr->min < m_nMeasuredXMinValue)
		{
			m_nMeasuredXMinValue = attr->min;
		}
		if (attr->max > m_nMeasuredXMaxValue)
		{
			m_nMeasuredXMaxValue = attr->max;
		}

		int bRedraw = 0;
		if (m_nXAxisStyle == AXIS_STYLE_MEAN_SYMMETRY)
		{
			nNegtiveBias = m_nXNegtiveMark;
			if (attr->min < m_nXNegtiveMark)
			{
				if (attr->min > m_nXFloor)
				{
					nNegtiveBias = (int)(floor(attr->min / (double)m_nXStep) * m_nXStep);
					bRedraw = 1;
				}
			}

			nPositiveBias = m_nXPositiveMark;
			if (attr->max > m_nXPositiveMark)
			{
				if (attr->max < m_nXCeil)
				{
					nPositiveBias = (int)(ceil(attr->max / (double)m_nXStep) * m_nXStep);
					bRedraw = 1;
				}
			}

			if (bRedraw)
			{
				int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
				m_nXNegtiveMark = -bias;
				m_nXPositiveMark = bias;
			}
		}
		else if (m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
		{
			if (attr->min < m_nXNegtiveMark)
			{
				if (attr->min > m_nXFloor)
				{
					m_nXNegtiveMark = (int)(floor(attr->min / (double)m_nXStep) * m_nXStep);
					bRedraw = 1;
				}
			}
			if (attr->max > m_nXPositiveMark)
			{
				if (attr->max < m_nXCeil)
				{
					m_nXPositiveMark = (int)(ceil(attr->max / (double)m_nXStep) * m_nXStep);
					bRedraw = 1;
				}
			}
		}

		if (bRedraw)
		{
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXLeftMark, m_nXNegtiveMark);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXMidMark, (m_nXNegtiveMark + m_nXPositiveMark) / 2);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXRightMark, m_nXPositiveMark);

			DisplayBkGrid(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
			DisplayXAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
		}
	}

	CInstrumentPanel_Base::AppendXSample(ID, sampleValue);
}

