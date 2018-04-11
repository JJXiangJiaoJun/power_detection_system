#ifndef _GETFFT_H
#define _GETFFT_H
#include "sys.h"

typedef struct 
{
		float UA_Harmonic[22];
		float UB_Harmonic[22];
		float UC_Harmonic[22];
		float IA_Harmonic[22];
		float IB_Harmonic[22];
		float IC_Harmonic[22];
		float IN_Harmonic[22];
}Harmonic_StructureDef;
//extern struct Harmonic_StructureDef Harmonic;
//µ×²ã¹¦ÄÜº¯Êý
void getFFT(Harmonic_StructureDef *Harmonic);

#endif