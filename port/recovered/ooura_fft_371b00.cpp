// @flags /O2
// Takuya Ooura's general-purpose FFT package (fftsg.c, the split-radix
// version of the late 1990s, in single precision), linked in with Bink: the
// four top-level pieces in 0x371b00-0x372020.
//   0x371b00  cftfsub(n, a, ip, nw, w): the forward complex FFT
//   0x371c30  cftbsub: the backward one
//   0x371d60  rdft(n, isgn, a, ip, w): the real FFT (stdcall)
//   0x371ea0  ddct(n, isgn, a, ip, w): the discrete cosine transform
//             (stdcall)
// The rest of the package (classified with Bink, not recovered) is reached
// through h2_retail_<address>. Retail passes much of it in registers (the
// whole program was optimised together); the parameters below are in the
// package's own order, which whoever recovers those must keep:
//   0x36d450 makewt(nw, ip, w)        0x36d5a0 makect(nc, ip, c)
//   0x36e200 cftf1st(n, a, w)         0x36ea60 cftb1st(n, a, w)
//   0x3719c0 cftrec1(n, a, nw, w)     0x371a60 cftrec2(n, a, nw, w)
//   0x371120 cftexp1(n, a, nw, w)     0x3710b0 cftfx41(n, a, nw, w)
//   0x36d620 bitrv2(n, ip, a)         0x36da60 bitrv2conj(n, ip, a)
//   0x36fc20 cftf161(a, w)            0x370800 cftf081(a, w)
//   0x36df20 bitrv216(a)              0x36e000 bitrv216neg(a)
//   0x36e130 bitrv208(a)              0x36e180 bitrv208neg(a)
//   0x370c10 cftf040(a)               0x370cb0 cftb040(a)
//   0x370d50 rftfsub(n, a, nc, c)     0x370e10 rftbsub(n, a, nc, c)
//   0x370ed0 dctsub(n, a, nc, c)
// In retail, 0x371b00 and 0x371c30 take a in ecx and w in eax with n, ip
// and nw on the stack.

extern "C" void h2_retail_36d450(int nw, int *ip, float *w);
extern "C" void h2_retail_36d5a0(int nc, int *ip, float *c);
extern "C" void h2_retail_36e200(int n, float *a, float *w);
extern "C" void h2_retail_36ea60(int n, float *a, float *w);
extern "C" void h2_retail_3719c0(int n, float *a, int nw, float *w);
extern "C" void h2_retail_371a60(int n, float *a, int nw, float *w);
extern "C" void h2_retail_371120(int n, float *a, int nw, float *w);
extern "C" void h2_retail_3710b0(int n, float *a, int nw, float *w);
extern "C" void h2_retail_36d620(int n, int *ip, float *a);
extern "C" void h2_retail_36da60(int n, int *ip, float *a);
extern "C" void h2_retail_36fc20(float *a, float *w);
extern "C" void h2_retail_370800(float *a, float *w);
extern "C" void h2_retail_36df20(float *a);
extern "C" void h2_retail_36e000(float *a);
extern "C" void h2_retail_36e130(float *a);
extern "C" void h2_retail_36e180(float *a);
extern "C" void h2_retail_370c10(float *a);
extern "C" void h2_retail_370cb0(float *a);
extern "C" void h2_retail_370d50(int n, float *a, int nc, float *c);
extern "C" void h2_retail_370e10(int n, float *a, int nc, float *c);
extern "C" void h2_retail_370ed0(int n, float *a, int nc, float *c);

// the 4-point butterfly both directions end with
static __forceinline void cftx020_371b00(float *a)
{
	float const x0r = a[0] - a[2];
	float const x0i = a[1] - a[3];
	a[0] = a[0] + a[2];
	a[1] = a[3] + a[1];
	a[2] = x0r;
	a[3] = x0i;
}

// @retail 0x371b00
void __cdecl cftfsub_371b00(int n, float *a, int *ip, int nw, float *w)
{
	if (n > 32)
	{
		int const m = n >> 2;
		h2_retail_36e200(n, a, &w[nw - m]);
		if (n > 512)
		{
			h2_retail_3719c0(m, a, nw, w);
			h2_retail_371a60(m, &a[m], nw, w);
			h2_retail_3719c0(m, &a[2 * m], nw, w);
			h2_retail_3719c0(m, &a[3 * m], nw, w);
		}
		else if (m > 32)
		{
			h2_retail_371120(n, a, nw, w);
		}
		else
		{
			h2_retail_3710b0(n, a, nw, w);
		}
		h2_retail_36d620(n, ip, a);
	}
	else if (n > 8)
	{
		if (n == 32)
		{
			h2_retail_36fc20(a, &w[nw - 8]);
			h2_retail_36df20(a);
		}
		else
		{
			h2_retail_370800(a, w);
			h2_retail_36e130(a);
		}
	}
	else if (n == 8)
	{
		h2_retail_370c10(a);
	}
	else if (n == 4)
	{
		cftx020_371b00(a);
	}
}

// @retail 0x371c30
void __cdecl cftbsub_371c30(int n, float *a, int *ip, int nw, float *w)
{
	if (n > 32)
	{
		int const m = n >> 2;
		h2_retail_36ea60(n, a, &w[nw - m]);
		if (n > 512)
		{
			h2_retail_3719c0(m, a, nw, w);
			h2_retail_371a60(m, &a[m], nw, w);
			h2_retail_3719c0(m, &a[2 * m], nw, w);
			h2_retail_3719c0(m, &a[3 * m], nw, w);
		}
		else if (m > 32)
		{
			h2_retail_371120(n, a, nw, w);
		}
		else
		{
			h2_retail_3710b0(n, a, nw, w);
		}
		h2_retail_36da60(n, ip, a);
	}
	else if (n > 8)
	{
		if (n == 32)
		{
			h2_retail_36fc20(a, &w[nw - 8]);
			h2_retail_36e000(a);
		}
		else
		{
			h2_retail_370800(a, w);
			h2_retail_36e180(a);
		}
	}
	else if (n == 8)
	{
		h2_retail_370cb0(a);
	}
	else if (n == 4)
	{
		cftx020_371b00(a);
	}
}

// @retail 0x371d60
void __stdcall rdft_371d60(int n, int isgn, float *a, int *ip, float *w)
{
	int nw = ip[0];
	if (n > (nw << 2))
	{
		nw = n >> 2;
		h2_retail_36d450(nw, ip, w);
	}
	int nc = ip[1];
	if (n > (nc << 2))
	{
		nc = n >> 2;
		h2_retail_36d5a0(nc, ip, w + nw);
	}
	if (isgn >= 0)
	{
		if (n > 4)
		{
			cftfsub_371b00(n, a, ip + 2, nw, w);
			h2_retail_370d50(n, a, nc, w + nw);
		}
		else if (n == 4)
		{
			cftfsub_371b00(n, a, ip + 2, nw, w);
		}
		float const xi = a[0] - a[1];
		a[0] = a[0] + a[1];
		a[1] = xi;
	}
	else
	{
		a[1] = (a[0] - a[1]) * 0.5f;
		a[0] = a[0] - a[1];
		if (n > 4)
		{
			h2_retail_370e10(n, a, nc, w + nw);
			cftbsub_371c30(n, a, ip + 2, nw, w);
		}
		else if (n == 4)
		{
			cftbsub_371c30(n, a, ip + 2, nw, w);
		}
	}
}

// @retail 0x371ea0
void __stdcall ddct_371ea0(int n, int isgn, float *a, int *ip, float *w)
{
	int nw = ip[0];
	if (n > (nw << 2))
	{
		nw = n >> 2;
		h2_retail_36d450(nw, ip, w);
	}
	int nc = ip[1];
	if (n > nc)
	{
		nc = n;
		h2_retail_36d5a0(nc, ip, w + nw);
	}
	if (isgn < 0)
	{
		float const xr = a[n - 1];
		for (int j = n - 2; j >= 2; j -= 2)
		{
			a[j + 1] = a[j] - a[j - 1];
			a[j] = a[j - 1] + a[j];
		}
		a[1] = a[0] - xr;
		a[0] = xr + a[0];
		if (n > 4)
		{
			h2_retail_370e10(n, a, nc, w + nw);
			cftbsub_371c30(n, a, ip + 2, nw, w);
		}
		else if (n == 4)
		{
			cftbsub_371c30(n, a, ip + 2, nw, w);
		}
	}
	h2_retail_370ed0(n, a, nc, w + nw);
	if (isgn >= 0)
	{
		if (n > 4)
		{
			cftfsub_371b00(n, a, ip + 2, nw, w);
			h2_retail_370d50(n, a, nc, w + nw);
		}
		else if (n == 4)
		{
			cftfsub_371b00(n, a, ip + 2, nw, w);
		}
		float const xr = a[0] - a[1];
		a[0] = a[1] + a[0];
		for (int j = 2; j < n; j += 2)
		{
			a[j - 1] = a[j] - a[j + 1];
			a[j] = a[j + 1] + a[j];
		}
		a[n - 1] = xr;
	}
}
