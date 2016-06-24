#pragma once
#define BEER_LAW
#define GRIDSHFT 3
//assert (1<<gridshft) == gridnum
#define GRIDNUM 8

#define REALTIMERENDER

#define SUPERSAMPLE
#ifdef SUPERSAMPLE
#define SUPERSAMPLENUM 4
#endif

#define SAMPLENUM 128
const double SMPNUM_1 = 1.0 / SAMPLENUM;