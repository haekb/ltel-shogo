
#include "plasma.h"


short g_SinTable[256];
DBYTE *g_DistanceGrid;

#define PLASMA_SIZE	16
#define PLASMA_MASK 15

int g_PlasmaSizeX = PLASMA_SIZE;
int g_PlasmaSizeY = PLASMA_SIZE;

float g_fWaveOffsets[2][2]; // [Wave #][X/Y];

void cs_PrecalculateData(ClientDE *pClientDE, HLOCALOBJ hPolyGrid)
{
	DDWORD width, height;
	int halfWidth, halfHeight, x, y;
	char *pData;
	DVector *pColorTable;
	float scale, val, maxDistSqr, testDist, t;
	int i;


	g_fWaveOffsets[0][0] = 10.0f;
	g_fWaveOffsets[0][1] = 4.0f;
	g_fWaveOffsets[1][0] = 13.0f;
	g_fWaveOffsets[1][1] = 14.0f;


	// Create the (scaled) sin table.
	scale = (MATH_CIRCLE / 255.0f) * 27.0f;
	for(i=0; i < 256; i++)
	{
		val = (float)i * scale;
		g_SinTable[i] = (short)(sin(val) * 128.0f * .20f);
	}


	// Fill in the distance grid.
	pClientDE->GetPolyGridInfo(hPolyGrid, &pData, &width, &height, &pColorTable);


	g_DistanceGrid = (DBYTE*)malloc(width*height);
	halfWidth = width >> 1;
	halfHeight = height >> 1;
	maxDistSqr = (float)(halfWidth*halfWidth + halfHeight*halfHeight) + 1.0f;
	for(y=0; y < (int)height; y++)
	{
		for(x=0; x < (int)width; x++)
		{
			testDist = (float)((x-halfWidth)*(x-halfWidth) + (y-halfHeight)*(y-halfHeight));
			t = testDist / maxDistSqr;
			g_DistanceGrid[y*height+x] = (DBYTE)(t * 255.0f);
		}
	}
}

void cs_RandomizePolyGrid(ClientDE *pClientDE, HLOCALOBJ hPolyGrid)
{
	DDWORD width, height, x, y;
	char *pData, *pCur;
	DVector *pColorTable;
	DBYTE waveOffsets[2][2];
	DBYTE xOffset0, xOffset1, yOffset0, yOffset1;
	DBYTE *pDistPos0, *pDistPos1;

	g_fWaveOffsets[0][0] += ((float)rand() / RAND_MAX) * 5.0f * pClientDE->GetFrameTime();
	g_fWaveOffsets[0][1] += ((float)rand() / RAND_MAX) * 12.0f * pClientDE->GetFrameTime();
	g_fWaveOffsets[1][0] += ((float)rand() / RAND_MAX) * 14.0f * pClientDE->GetFrameTime();
	g_fWaveOffsets[1][1] += ((float)rand() / RAND_MAX) * 8.0f * pClientDE->GetFrameTime();
	waveOffsets[0][0] = (DBYTE)(g_fWaveOffsets[0][0] + 0.5f);
	waveOffsets[0][1] = (DBYTE)(g_fWaveOffsets[0][1] + 0.5f);
	waveOffsets[1][0] = (DBYTE)(g_fWaveOffsets[1][0] + 0.5f);
	waveOffsets[1][1] = (DBYTE)(g_fWaveOffsets[1][1] + 0.5f);

	// Randomize the poly grid values.
	pClientDE->GetPolyGridInfo(hPolyGrid, &pData, &width, &height, &pColorTable);

	yOffset0 = waveOffsets[0][1];
	yOffset1 = waveOffsets[1][1];

	for(y=0; y < height; y++)
	{
		pCur = pData + (y*width);
		
		xOffset0 = waveOffsets[0][0];
		xOffset1 = waveOffsets[1][0];
		
		pDistPos0 = &g_DistanceGrid[((y+waveOffsets[0][1]) & PLASMA_MASK)*width];
		pDistPos1 = &g_DistanceGrid[((y+waveOffsets[1][1]) & PLASMA_MASK)*width];

		for(x=0; x < width; x++)
		{
			//*pCur++ = g_SinTable[g_DistanceGrid[y*width+x]+count];
			//*pCur++ = (char)(g_SinTable[pDistPos0[xOffset0]] + g_SinTable[pDistPos1[xOffset1]]);
			//*pCur++ = (char)(g_SinTable[(*pDistPos0 + xOffset0) & 255] + 
				//g_SinTable[(*pDistPos1 + xOffset1) & 255]);
			//*pCur++ = (char)(g_SinTable[(*pDistPos + xOffset0) & 255]);

			*pCur++ = (char)(g_SinTable[xOffset0] + g_SinTable[xOffset1] + 
				g_SinTable[yOffset0] + g_SinTable[yOffset1]);
			
			++xOffset0;
			++xOffset1;
		}

		++yOffset0;
		++yOffset1;
	}
}


typedef struct
{
	DVector m_Color;
	int m_Index;
} ColorRamp;


ColorRamp g_ColorRamps[] = 
{
	230.0f, 100.0f, 21.0f, 0,
	255.0f, 0.0f, 0.0f, 128,
	255.0f, 255.0f, 0.0f, 192,
	255.0f, 0.0f, 128.0f, 256
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


