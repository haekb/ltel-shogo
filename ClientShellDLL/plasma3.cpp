

#include "plasma.h"


int g_PlasmaSizeX = 16;
int g_PlasmaSizeY = 16;


void cs_RandomizePolyGrid(ClientDE *pClientDE, HLOCALOBJ hPolyGrid)
{
}



void cs_PrecalculateData(ClientDE *pClientDE, HLOCALOBJ hPolyGrid)
{
	DDWORD width, height, x, y;
	char *pData, *pCur;
	DVector *pColorTable;


	// Randomize the poly grid values.
	pClientDE->GetPolyGridInfo(hPolyGrid, &pData, &width, &height, &pColorTable);
	for(y=0; y < height; y++)
	{
		pCur = pData + (y*width);

		for(x=0; x < width; x++)
		{
			*pCur++ = (char)rand();
		}
	}
}


typedef struct
{
	DVector m_Color;
	int m_Index;
} ColorRamp;


ColorRamp g_ColorRamps[] = 
{
	0.0f, 0.0f, 0.0f, 0,
	255.0f, 255.0f, 255.0f, 256
};

#define NUM_COLORRAMPS() (sizeof(g_ColorRamps) / sizeof(ColorRamp))


void cs_SetPolyGridPalette(ClientDE *pClientDE, HLOCALOBJ hPolyGrid)
{
	float t;
	DDWORD width, height;
	char *pData;
	DVector *pColorTable, color1, color2;
	int ramp, i, index1, index2;

	// Randomize the poly grid values.
	pClientDE->GetPolyGridInfo(hPolyGrid, &pData, &width, &height, &pColorTable);

	// Make the color table go from white to black.
	for(ramp=0; ramp < NUM_COLORRAMPS()-1; ramp++)
	{
		index1 = g_ColorRamps[ramp].m_Index;
		index2 = g_ColorRamps[ramp+1].m_Index;
		VEC_COPY(color1, g_ColorRamps[ramp].m_Color);
		VEC_COPY(color2, g_ColorRamps[ramp+1].m_Color);

		for(i=index1; i < index2; i++)
		{
			t = (float)(i - index1) / (index2 - index1);
			VEC_LERP(pColorTable[i], color1, color2, t);
		}
	}
}


