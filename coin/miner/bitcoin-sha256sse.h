/*######     Copyright (c) 1997-2013 Ufasoft  http://ufasoft.com  mailto:support@ufasoft.com,  Sergey Pavlov  mailto:dev@ufasoft.com #######################################
#                                                                                                                                                                          #
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;  #
# either version 3, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the      #
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU #
# General Public License along with this program; If not, see <http://www.gnu.org/licenses/>                                                                               #
##########################################################################################################################################################################*/

#pragma once

#include "bitcoin-sha256.h"


extern "C" {
/*!!!R #if defined(_WIN64) || defined(_M_IX86_FP) && _M_IX86_FP >= 2
	extern __m128i g_4sha256_k[64];	
#endif */

#if UCFG_BITCOIN_ASM
		bool _cdecl CalcSha256(UInt32 *w, UInt32 midstate[8], UInt32 init[8], DWORD_PTR npar, UInt32& pnonce);
#	if defined(_WIN64) || defined(_M_IX86_FP) && _M_IX86_FP >= 2
		bool _cdecl CalcSha256Sse(__m128i *w, __m128i midstate[8], UInt32 init[8], DWORD_PTR npar, UInt32& pnonce);
#	endif
#endif

} // "C"

namespace Coin {

#if defined(_WIN64) || defined(_M_IX86_FP) && _M_IX86_FP >= 2

#if UCFG_GNUC
	class M128I {
	public:
		M128I(__m128i v)
			:	m_v(v)
		{}

		operator __m128i() const { return m_v; }

		__m128i m_v;
	};
#else
	typedef __m128i M128I;
#endif


template<> __forceinline __m128i Expand32<__m128i>(UInt32 a) {
	return _mm_set1_epi32(a);
}

__forceinline M128I operator&(M128I a, M128I b) {
	return _mm_and_si128(a, b);
}

__forceinline M128I operator|(M128I a, M128I b) {
	return _mm_or_si128(a, b);
}

__forceinline M128I operator^(M128I a, M128I b) {
	return _mm_xor_si128(a, b);
}

__forceinline M128I operator>>(M128I a, int n) {
	return _mm_srli_epi32(a, n);
}

__forceinline M128I operator+(M128I a, M128I b) {
	return _mm_add_epi32(a, b);
}

__forceinline M128I& operator+=(M128I& a, M128I b) {
	return a = a + b;
}

__forceinline M128I AndNot(M128I a, M128I b) {
	return _mm_andnot_si128(a, b);
}

__forceinline M128I Rotr32(M128I v, int n) {
	return _mm_srli_epi32(v, n) | _mm_slli_epi32(v, 32 - n);
}

#endif // defined(_M_IX86_FP) && _M_IX86_FP >= 2

#if UCFG_BITCOIN_ASM

class SseBitcoinSha256 : public BitcoinSha256 {
	typedef BitcoinSha256 base;
public:
	void PrepareData(const void *midstate, const void *data, const void *hash1);
	bool FindNonce(UInt32& nonce);
protected:
#	if defined(_WIN64) || defined(_M_IX86_FP) && _M_IX86_FP >= 2
	__m128i m_4midstate[8];
	__m128i m_4w[32*UCFG_BITCOIN_WAY];
#	endif
};

#endif

} // Coin::
