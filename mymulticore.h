//
// mymulticore.h
//
#ifndef _mymulticore_h
#define _mymulticore_h

#include <circle/screen.h>
#include <circle/multicore.h>
#include <circle/timer.h>
#include <circle/gpiopin.h>
#include <circle/types.h>

class CMyMultiCoreClass : public CMultiCoreSupport
{
public:
	CMyMultiCoreClass (CTimer *pTimer, CScreenDevice *pScreen);
	~CMyMultiCoreClass (void);

	void Run (unsigned nCore);

private:
	void Core0Proc (void);
	void Core3Proc (void);

private:
	CTimer *m_pTimer;

	void read_dht11_dat();
	void Mandelbrot();

	CScreenDevice *m_pScreen;

};

#endif
