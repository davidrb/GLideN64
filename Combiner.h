#ifndef COMBINER_H
#define COMBINER_H

#include "GLideN64.h"
#include "OpenGL.h"
#include "gDP.h"
#include "Types.h"

#define TEXTURE_ENV					0
#define TEXTURE_ENV_COMBINE			1
#define NV_REGISTER_COMBINERS		2
#define GLSL_COMBINE				3

/*
* G_SETCOMBINE: color combine modes
*/
/* Color combiner constants: */
#define G_CCMUX_COMBINED	0
#define G_CCMUX_TEXEL0		1
#define G_CCMUX_TEXEL1		2
#define G_CCMUX_PRIMITIVE	3
#define G_CCMUX_SHADE		4
#define G_CCMUX_ENVIRONMENT	5
#define G_CCMUX_CENTER		6
#define G_CCMUX_SCALE		6
#define G_CCMUX_COMBINED_ALPHA	7
#define G_CCMUX_TEXEL0_ALPHA	8
#define G_CCMUX_TEXEL1_ALPHA	9
#define G_CCMUX_PRIMITIVE_ALPHA	10
#define G_CCMUX_SHADE_ALPHA	11
#define G_CCMUX_ENV_ALPHA	12
#define G_CCMUX_LOD_FRACTION	13
#define G_CCMUX_PRIM_LOD_FRAC	14
#define G_CCMUX_NOISE		7
#define G_CCMUX_K4		7
#define G_CCMUX_K5		15
#define G_CCMUX_1		6
#define G_CCMUX_0		31

/* Alpha combiner constants: */
#define G_ACMUX_COMBINED	0
#define G_ACMUX_TEXEL0		1
#define G_ACMUX_TEXEL1		2
#define G_ACMUX_PRIMITIVE	3
#define G_ACMUX_SHADE		4
#define G_ACMUX_ENVIRONMENT	5
#define G_ACMUX_LOD_FRACTION	0
#define G_ACMUX_PRIM_LOD_FRAC	6
#define G_ACMUX_1		6
#define G_ACMUX_0		7

#define EncodeCombineMode( a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0,	\
	a1, b1, c1, d1,	Aa1, Ab1, Ac1, Ad1 ) \
	(u64)(((u64)(_SHIFTL( G_CCMUX_##a0, 20, 4 ) | _SHIFTL( G_CCMUX_##c0, 15, 5 ) | \
	_SHIFTL( G_ACMUX_##Aa0, 12, 3 ) | _SHIFTL( G_ACMUX_##Ac0, 9, 3 ) | \
	_SHIFTL( G_CCMUX_##a1, 5, 4 ) | _SHIFTL( G_CCMUX_##c1, 0, 5 )) << 32) | \
	(u64)(_SHIFTL( G_CCMUX_##b0, 28, 4 ) | _SHIFTL( G_CCMUX_##d0, 15, 3 ) | \
	_SHIFTL( G_ACMUX_##Ab0, 12, 3 ) | _SHIFTL( G_ACMUX_##Ad0, 9, 3 ) | \
	_SHIFTL( G_CCMUX_##b1, 24, 4 ) | _SHIFTL( G_ACMUX_##Aa1, 21, 3 ) | \
	_SHIFTL( G_ACMUX_##Ac1, 18, 3 ) | _SHIFTL( G_CCMUX_##d1, 6, 3 ) | \
	_SHIFTL( G_ACMUX_##Ab1, 3, 3 ) | _SHIFTL( G_ACMUX_##Ad1, 0, 3 )))

// Internal combiner commands
#define LOAD		0
#define SUB			1
#define MUL			2
#define ADD			3
#define INTER		4

// Internal generalized combiner inputs
#define COMBINED		0
#define TEXEL0			1
#define TEXEL1			2
#define PRIMITIVE		3
#define SHADE			4
#define ENVIRONMENT		5
#define CENTER			6
#define SCALE			7
#define COMBINED_ALPHA	8
#define TEXEL0_ALPHA	9
#define TEXEL1_ALPHA	10
#define PRIMITIVE_ALPHA	11
#define SHADE_ALPHA		12
#define ENV_ALPHA		13
#define LOD_FRACTION	14
#define PRIM_LOD_FRAC	15
#define NOISE			16
#define K4				17
#define K5				18
#define ONE				19
#define ZERO			20
#define LIGHT			21

struct CombinerOp
{
	int op;
	int param1;
	int param2;
	int param3;
};

struct CombinerStage
{
	int numOps;
	CombinerOp op[6];
};

struct Combiner
{
	int numStages;
	CombinerStage stage[2];
};

struct CombineCycle
{
	int sa, sb, m, a;
};

class OGLCombiner {
public:
	virtual void Set() = 0;
	virtual void UpdateColors() = 0;
	virtual void UpdateFBInfo() = 0;
};

struct CachedCombiner
{
	gDPCombine combine;

	OGLCombiner *compiled;
	CachedCombiner *left, *right;
};

extern struct CombinerInfo
{
	struct
	{
		WORD color, secondaryColor, alpha;
	} vertex;

	CachedCombiner *root, *current;

	int compiler;

	BOOL usesT0, usesT1, usesLOD;
} combiner;

#define SetConstant( constant, color, alpha ) \
	switch (color) \
{ \
		case PRIMITIVE: \
		constant.r = gDP.primColor.r; \
		constant.g = gDP.primColor.g; \
		constant.b = gDP.primColor.b; \
		break; \
		case ENVIRONMENT: \
		constant.r = gDP.envColor.r; \
		constant.g = gDP.envColor.g; \
		constant.b = gDP.envColor.b; \
		break; \
		case PRIMITIVE_ALPHA: \
		constant.r = gDP.primColor.a; \
		constant.g = gDP.primColor.a; \
		constant.b = gDP.primColor.a; \
		break; \
		case ENV_ALPHA: \
		constant.r = gDP.envColor.a; \
		constant.g = gDP.envColor.a; \
		constant.b = gDP.envColor.a; \
		break; \
		case PRIM_LOD_FRAC: \
		constant.r = gDP.primColor.l; \
		constant.g = gDP.primColor.l; \
		constant.b = gDP.primColor.l; \
		break; \
		case ONE: \
		constant.r = 1.0f; \
		constant.g = 1.0f; \
		constant.b = 1.0f; \
		break; \
		case ZERO: \
		constant.r = 0.0f; \
		constant.g = 0.0f; \
		constant.b = 0.0f; \
		break; \
		case LIGHT: \
		constant.r = 0.0f; \
		constant.g = 0.0f; \
		constant.b = 0.0f; \
		constant.a = 0.0f; \
		break; \
} \
	\
	switch (alpha) \
{ \
		case PRIMITIVE_ALPHA: \
		constant.a = gDP.primColor.a; \
		break; \
		case ENV_ALPHA: \
		constant.a = gDP.envColor.a; \
		break; \
		case PRIM_LOD_FRAC: \
		constant.a = gDP.primColor.l; \
		break; \
		case ONE: \
		constant.a = 1.0f; \
		break; \
		case ZERO: \
		constant.a = 0.0f; \
		break; \
}

void Combiner_Init();
void Combiner_UpdateCombineColors();
void Combiner_UpdateCombineFBInfo();
void Combiner_SetCombine( u64 mux );
void Combiner_Destroy();
void Combiner_BeginTextureUpdate();
void Combiner_EndTextureUpdate();
void Combiner_CalcLOD();
void Combiner_PostCalcLOD();

#endif

