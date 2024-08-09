#include "SRC\TEX.H"

unsigned char textureAlpha = TEX_ALPHA;

unsigned char RGBToVGA(float r, float g, float b)
{
	float hue, sat, lit;
	float cmin, cmax, delta;

	// RGB alpha value (255, 0, 255)
	if (r == 1.0f && g == 0.0f && b == 1.0f)
		return textureAlpha;
	
	cmin = min(r, min(g, b));
	cmax = max(r, max(g, b));
	delta = cmax - cmin;

	lit = min((cmax + cmin) / 1.5f, 1.0f);

	sat = 0;
	if (cmax != 0)
		sat = delta / cmax;

	hue = 0;
	if (delta != 0.0f) {
		if (cmax == r)
			hue = fmod((g - b) / delta, 6.0f);
		else if (cmax == g)
			hue = ((b - r) / delta) + 2.0f;
		else
			hue = ((r - g) / delta) + 4.0f;
		hue /= 6.0f;
	}

	if (sat < 0.25f || lit < 0.1f) {
		lit = floor(lit * 15.0f);
		return 16 + lit;
	}
	else {
		hue = floor(fmod(hue * 24.0f + 8.0f, 24.0f));
		sat = floor((1.0f - sat) * 3.0f) * 24.0f;
		lit = floor((1.0f - lit) * 3.0f) * 24.0f * 3.0f;
		return 32 + hue + sat + lit;
	}
}

void loadPPM(Texture *tex, char *path)
{
	unsigned i, j, k;
	char c;
	unsigned char *data = NULL;
	FILE *fp;

	char line[16];

	unsigned w = 0, h = 0;
	int dp = 0;
	int ds = 0;

	float val = 0.0f, maxval = 0.0f;
	float rgb[3];

	unsigned ri = 0, di = 0;

	char ws[6];
	char hs[6];

	line[0] = 0;
	ws[0] = 0;
	hs[0] = 0;

	tex->id = -1;
	tex->w = 0;
	tex->h = 0;
	tex->data = NULL;
	
	fp = fopen(path, "r");

	if (!fp) {
		printf("Not found: %s\n", path);
		return;
	}

	for (i = 0; !feof(fp); ++i) {
		c = (char) fgetc(fp);
		
		ds = (dp ? c != ' ' : 1);

		if (c != '\n' && c != '\r' && c && ds) {
			if (j < 15) {
				line[j] = c;
				++j;
			}
			++i;
			continue;
		}
		else {
			line[j] = 0;
			j = 0;
		}

		if (line[0] != '#' && line[0] != 'P' && line[0]) {
			if (w == 0 && h == 0) { // parse dimensions
				k = 0;
				// texture width
				for (j = 0; line[j] != ' ' && line[j]; ++j) {
					ws[k] = line[j];
					++k;
				}

				ws[k] = 0;

				k = 0;
				// texture height
				for (++j; line[j] != ' ' && line[j]; ++j) {
					hs[k] = line[j];
					++k;
				}

				hs[k] = 0;

				w = atoi(ws);
				h = atoi(hs);

				data = (unsigned char *) malloc(sizeof(unsigned char) * w * h);

				if (!data) {
					printf("Can't allocate memory for texture!\n");
					fclose(fp);
					return;
				}

				j = 0;

				dp = 1;
			}
			else { // parse texture data
				if (maxval == 0.0f) {
					maxval = (float) atoi(line);

					continue; // proper data starts after
				}

				val = (float) atoi(line) / maxval;

				rgb[ri % 3] = val;
				++ri;

				if (ri % 3 == 0 && di < w * h)
					data[di++] = RGBToVGA(rgb[0], rgb[1], rgb[2]);
			}
		}
	}
	
	fclose(fp);
	
	tex->w = w;
	tex->h = h;
	tex->data = data;
}

unsigned char getTexture(Texture *tex, unsigned x, unsigned y,
				 unsigned w, unsigned h)
{
	unsigned tx = x, ty = y;
	if (w != 0 && h != 0) {
		tx = x * tex->w / w; // convert to texture coordinates
		ty = y * tex->h / h;
	}
	if (tx > tex->w || ty > tex->h) return 0;
	return tex->data[tex->w * ty + tx];
}

void createTexture(Texture *tex, unsigned w, unsigned h,
			 unsigned char c1, unsigned char c2, int type)
{
	unsigned i, j;
	unsigned char *data;
	unsigned char c = c1;

	tex->id = -1;
	tex->w = 0;
	tex->h = 0;
	tex->data = NULL;
	
	data = (unsigned char *) malloc(sizeof(unsigned char) * w * h);

	if (!data) {
		printf("Can't allocate memory for texture!\n");
		return;
	}
	
	tex->w = w;
	tex->h = h;
	
	for (i = 0; i < h; ++i) {
		for (j = 0; j < w; ++j) {
			switch (type) {
			case 0:
				c = (i % 2 == 0 ? c1 : c2);
				break;
			case 1:
				c = (j % 2 == 0 ? c1 : c2);
				break;
			case 2:
				c = ((i+j) % 2 == 0 ? c1 : c2);
				break;
			}
			data[i * w + j] = c;
		}
	}
	
	tex->data = data;
}

void printTexture(Texture *tex)
{
	unsigned i, j;
	for (i = 0; i < tex->h; ++i) {
		for (j = 0; j < tex->w; ++j)
			printf("%c (%i) ",
				 tex->data[i * tex->w + j],
				 tex->data[i * tex->w + j]);
		printf("\n");
	}
	printf("(%u x %u)", tex->w, tex->h);
}

void destroyTexture(Texture *tex)
{
	free(tex->data);
}

void createAtlas(TextureAtlas *atlas)
{
	unsigned i;
	
	atlas->num = 0;
	atlas->page = 2;
	atlas->alpha = TEX_ALPHA;

	for (i = 0; i < ATLAS_W * ATLAS_H; ++i) {
		atlas->textures[i] = NULL;
	}
}

void addAtlasTexture(TextureAtlas *atlas, Texture *tex)
{
	unsigned id = atlas->num;

	if (!tex->data)
		return;
	
	atlas->textures[id] = tex;
	atlas->textures[id]->id = id;
	++atlas->num;
}

void destroyAtlas(TextureAtlas *atlas)
{
	unsigned i;
	
	for (i = 0; i < atlas->num; ++i) {
		destroyTexture(atlas->textures[i]);
	}
}

#define W 320
#define H 240

unsigned getAtlasTextureStart(TextureAtlas *atlas, int i)
{
	unsigned id, xa, ya, x, y, tstart;
	id = atlas->textures[i] ? atlas->textures[i]->id : i;
	xa = id % ATLAS_W;
	ya = id / ATLAS_W;
	x = xa * ATLAS_TW;
	y = ya * ATLAS_TH;
	tstart = W / 4;
	tstart *= atlas->page * H + y + ATLAS_OFFS;
	tstart += x / 4;
	return tstart;
}

void loadAtlasFont(TextureAtlas *atlas, char *path)
{
	Texture *tex = atlas->textures[atlas->num];
	loadPPM(tex, path);
	//createTexture(tex, 128, 72, 0, 5, 2);
	addAtlasTexture(atlas, tex);
	tex->id = (W - tex->w) / ATLAS_TW;
	tex->id += (H - tex->h) / ATLAS_TH * ATLAS_W;
	atlas->font = tex;
}

