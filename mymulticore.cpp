//
// mymulticore.cpp
//
#include "mymulticore.h"

#include <circle/screen.h>

#include <circle/gpiopin.h>
#include <circle/types.h>
#include <circle/logger.h>
#include <circle/memory.h>

int dht11_dat[5] = { 0, 0, 0, 0, 0};
#define MAXTIMINGS	85
#define DHTPIN		4

CMyMultiCoreClass::CMyMultiCoreClass (CTimer *pTimer, CScreenDevice *pScreen)
:	CMultiCoreSupport (CMemorySystem::Get ()),
	m_pTimer (pTimer),
	m_pScreen(pScreen)
{
}

CMyMultiCoreClass::~CMyMultiCoreClass (void)
{
	m_pScreen = 0;
}

void CMyMultiCoreClass::Run (unsigned nCore)
{
	switch (nCore)
	{
	case 0:
		Core0Proc ();
		break;

	case 3:
		Core3Proc ();
		break;
	}
}

void CMyMultiCoreClass::Core0Proc (void)
{
	while (1)
	{
		read_dht11_dat();
		m_pTimer->MsDelay(1000);
	}
}

void CMyMultiCoreClass::Core3Proc (void)
{
	m_pTimer->MsDelay (1);
	while (1)
	{
		Mandelbrot();
	}
}

void CMyMultiCoreClass::read_dht11_dat()
{
	unsigned char laststate = HIGH;
	unsigned char counter   = 0;
	unsigned char j			= 0, i;
	float f;

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	CGPIOPin m_GPIO4 (DHTPIN, GPIOModeOutput); 

	m_GPIO4.Write(LOW);
	m_pTimer->MsDelay(18);
	m_GPIO4.Write(HIGH);
	m_pTimer->usDelay(40);


	m_GPIO4.SetMode(GPIOModeInput);
	for(i = 0; i < MAXTIMINGS; i++)
	{
		counter = 0;
		while(m_GPIO4.Read() == laststate)
		{
			counter++;
			m_pTimer->usDelay(1);
			if(counter == 255)
			{
				break;
			}
		}
		laststate = m_GPIO4.Read();

		if((i >= 4) && (i % 2 == 0))
		{
			dht11_dat[ j / 8 ] <<=1;
			if(counter > 16)
				{
					dht11_dat[ j / 8 ] |= 1;
				}
			j++;
		}
	}

	if ( (j >= 40) && 
		 (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)))
	{
		f = dht11_dat[2]*9. / 5. + 32;
		CLogger::Get() -> Write("DATE",LogDebug, "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n", 
									               dht11_dat[0],dht11_dat[1],dht11_dat[2],dht11_dat[3],f);
	}
	else
	{
		CLogger::Get() -> Write("DATA NOT FOUND",LogDebug, "       ERROR     ");
	}
}

void CMyMultiCoreClass::Mandelbrot()
{
	float x1 = -2.0, 
		  x2 =  1.0, 
		  y1 = -1.0,
		  y2 =  1.0;
	unsigned nMaxIteration = 5000, 
			 nPosY0        =    0,
			 nHeight = m_pScreen->GetHeight();

	float dx = (x2-x1) / m_pScreen->GetWidth ();
	float dy = (y2-y1) / nHeight;

	float y0 = y1;

			for (unsigned nPosY = nPosY0; nPosY < nPosY0+nHeight; nPosY++, y0 += dy)
	{
		float x0 = x1;
		for (unsigned nPosX = 0; nPosX < m_pScreen->GetWidth (); nPosX++, x0 += dx)
		{
			float x = 0.0;
			float y = 0.0;
			unsigned nIteration = 0;
			for (; x*x+y*y < 2*2 && nIteration < nMaxIteration; nIteration++)
			{
				float xtmp = x*x - y*y + x0;
				y = 2*x*y + y0;
				x = xtmp;
			}

			#if DEPTH == 8
				TScreenColor Color = (TScreenColor) (nIteration * 3 / nMaxIteration);
			#elif DEPTH == 16
				TScreenColor Color = (TScreenColor) (nIteration * 65535 / nMaxIteration);
			Color++;
			#else
				#error DEPTH must be 8 or 16
			#endif
			m_pScreen->SetPixel (nPosX, nPosY, Color);
		}
	}
}
