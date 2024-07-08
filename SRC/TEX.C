#include "SRC\TEX.H"

unsigned char RGBToVGA(float r, float g, float b)
{
	float hue, sat, lit;
	float cmin, cmax, delta;
	
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
	int i, j, k;
	unsigned char *rawdata = NULL;
	unsigned char *data = NULL;
	float *rgbdata = NULL;
	FILE *fp;

	unsigned char *word;

	int w = 0, h = 0;
	int dp = 0;
	int ds = 0;

	float val = 0.0f, maxval = 0.0f;
	float r, g, b;

	int ri = 0;

	char ws[6];
	char hs[6];
				
	fp = fopen(path, "r");

	if (!fp) {
		printf("Not found: %s\n", path);
		fclose(fp);
		return;
	}

	rawdata = calloc(TEX_MAX, sizeof(unsigned char *));

	if (!rawdata) {
		printf("Can't allocate memory for texture!\n");
		fclose(fp);
		return;
	}
	
	fread(rawdata, 1, TEX_MAX, fp);

	fclose(fp);

	word = (unsigned char *) malloc(sizeof(unsigned char) * 64);

	if (!word)
		return;

	for (i = 0; rawdata[i] && i < TEX_MAX; ++i) {
		ds = (dp ? rawdata[i] != ' ' : 1);

		for (j = 0; rawdata[i] != '\n' && rawdata[i] != '\r' && rawdata[i] && ds; ++j) {
			word[j] = rawdata[i];
			++i;
		}

		word[j] = 0;

		if (word[0] != '#' && word[0] != 'P') {
			if (w == 0 && h == 0) { // parse dimensions
				k = 0;
				for (j = 0; word[j] != ' ' && word[j]; ++j) { // texture width
					ws[k] = word[j];
					++k;
				}

				ws[k] = 0;

				k = 0;
				for (j = 0; word[j] != ' ' && word[j]; ++j) { // texture height
					hs[k] = word[j];
					++k;
				}

				hs[k] = 0;

				w = atoi(ws);
				h = atoi(hs);

				rgbdata = (float *) malloc(sizeof(float) * w * h * 3);

				if (!rgbdata) {
					free(rawdata);
					free(word);
					return;
				}

				dp = 1;
			}
			else { // parse texture data
				if (maxval == 0.0f) {
					maxval = (float) atoi(word);

					continue; // proper data starts after
				}

				val = (float) atoi(word) / maxval;

				if (ri < w * h * 3)
					rgbdata[ri] = val;

				++ri;
			}
		}
	}
	
	free(rawdata);
	free(word);

	data = (unsigned char *) malloc(sizeof(unsigned char) * w * h);

	if (!data) {
		free(rgbdata);
		return;
	}

	for (i = 0; i < w * h; ++i) {
		r = rgbdata[i * 3];
		g = rgbdata[i * 3 + 1];
		b = rgbdata[i * 3 + 2];

		data[i] = RGBToVGA(r, g, b);
	}

	free(rgbdata);

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

	tex->w = 0;
	tex->h = 0;
	tex->data = NULL;
	data = (unsigned char *) malloc(sizeof(unsigned char) * w * h);

	if (!data)
		return;
	
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
			printf("%c (%i) ", tex->data[i * tex->w + j], tex->data[i * tex->w + j]);
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
	atlas->alpha = 5;

	for (i = 0; i < ATLAS_W * ATLAS_H; ++i) {
		atlas->textures[i] = NULL;
	}
}

void addTexture(TextureAtlas *atlas, Texture *tex)
{
	unsigned id = atlas->num;

	if (!tex->data)
		return;
	
	atlas->textures[id] = tex;
	atlas->textures[id]->id = id;
	++atlas->num;
}

