#ifndef GBR_PARSER_H
#define GBR_PARSER_H

typedef unsigned short WORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

namespace GbrParser
{
enum ObjectTypes {
	OBJECT_TYPE_PRODUCER = 0x1,
	OBJECT_TYPE_TILE_DATA = 0x2,
	OBJECT_TYPE_TILE_SETTINGS = 0x3,
	OBJECT_TYPE_TILE_EXPORT = 0x4,
	OBJECT_TYPE_TILE_IMPORT = 0x5,
	OBJECT_TYPE_PALETTES = 0xD,
	OBJECT_TYPE_TILEPAL = 0xE,
	OBJECT_TYPE_DELETED = 0xFF
};

struct TileSet{
	struct Info {
		char name[30];
		WORD width;
		WORD height;
		WORD count;
		unsigned char color_set[4];
	};
	Info info;
	unsigned char* data;
};

struct TileExport {
	WORD tile_id;
	char file_name[128];
	BYTE file_type;
	char section_name[20];
	char label_name[128];//[20]
	BYTE bank;
	bool tile_array;
	BYTE format;
	BYTE counter;
	WORD from;
	WORD up_to;
	BYTE compression;
	bool include_colors;
	BYTE sgb_palettes;
	BYTE gbc_palettes;
	bool make_meta_tiles;
	LONG meta_offset;
	BYTE meta_counter;
	bool split;
	LONG block_size;
	BYTE sel_tab;
};

struct Color {
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

struct Palette {
	Color colors[4];
};

struct Palettes {
	WORD id;
	WORD count;
	Palette* colors;
	WORD sgb_count;
	Palette* sgb_colors;
};

struct TilePal {
	WORD id;
	WORD count;
	LONG* color_set;
	WORD sgb_count;
	LONG* sgb_color_set;
};

struct GBRInfo {
	TileSet tile_set;
	TileExport tile_export;
	Palettes palettes;
	TilePal tile_pal;
	int bank;

	char palette_order[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	int num_palettes = 0;
};

void ExtractFileName(const char* path, char* file_name, bool include_bank);
bool LoadGBR(const char* path, GBRInfo* info);

};

#define BIT(V, B) (1 & (V >> B))
#define BYTE(B0, B1, B2, B3, B4, B5, B6, B7) ((B0 << 7) | (B1 << 6) | (B2 << 5) | (B3 << 4) | (B4 << 3) | (B5 << 2) | (B6 << 1) | B7)

#endif