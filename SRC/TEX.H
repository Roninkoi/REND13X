/*
 * Textures, texture atlas, image loading utilities
 */

#ifndef TEX_H
#define TEX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	unsigned id;
	unsigned char *data;
	unsigned w;
	unsigned h;
	char *path;
} Texture;

// load portable pixmap format (PPM) image
void loadPPM(Texture *tex, char *path);
unsigned char getTexture(Texture *tex, unsigned x, unsigned y,
				 unsigned w, unsigned h);
// create texture from colors c1 and c2 (striped, checker)
void createTexture(Texture *tex, unsigned w, unsigned h,
			 unsigned char c1, unsigned char c2, int type);
void destroyTexture(Texture *tex);

// convert rgb color to default VGA palette
unsigned char RGBToVGA(float r, float g, float b);

#define ATLAS_W 10
#define ATLAS_H 7
#define ATLAS_TW 32
#define ATLAS_TH 32

typedef struct {
	Texture *textures[ATLAS_W * ATLAS_H];
	unsigned num;
	unsigned char page;
	unsigned char alpha;
} TextureAtlas;

// create texture atlas containing multiple textures in VGA memory
void createAtlas(TextureAtlas *atlas);
void addAtlasTexture(TextureAtlas *atlas, Texture *tex);
// write textures to a page of VGA memory (mode X)
void writeAtlasTextures(TextureAtlas *atlas);
// get start of texture memory of texture i
unsigned getAtlasTextureStart(TextureAtlas *atlas, int i);

#endif

