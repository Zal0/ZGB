#include <stdio.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

#define READ(var) fread((char*)&var, sizeof(var), 1, file)

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
	char label_name[20];
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

#define BIT(V, B) (1 & (V >> B))
#define BYTE(B0, B1, B2, B3, B4, B5, B6, B7) ((B0 << 7) | (B1 << 6) | (B2 << 5) | (B3 << 4) | (B4 << 3) | (B5 << 2) | (B6 << 1) | B7)

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("usage: gbr2c file_in.gbr export_folder");
		return 1;
	}

	FILE* file = fopen(argv[1], "rb");
	if(!file) {
		printf("Error reading file");
		return 1;
	}

	TileSet tile_set;
	TileExport tile_export;
	Palettes palettes;
	TilePal tile_pal;

	char marker[3];
	char version;
	READ(marker);
	READ(version);
		
	WORD object_type;
	WORD object_id;
	LONG object_length;
	while(READ(object_type)) {
		READ(object_id);
		READ(object_length);
			
		switch(object_type) {
			case OBJECT_TYPE_TILE_DATA: {
				READ(tile_set.info.name);
				READ(tile_set.info.width);
				READ(tile_set.info.height);
				READ(tile_set.info.count);
				READ(tile_set.info.color_set);

				int data_size = tile_set.info.width * tile_set.info.height * tile_set.info.count;
				tile_set.data = new unsigned char[data_size];
				fread(tile_set.data, data_size, 1, file);
				break;
			}

			case OBJECT_TYPE_TILE_EXPORT:
				READ(tile_export.tile_id);
				READ(tile_export.file_name);
				READ(tile_export.file_type);
				READ(tile_export.section_name);
				READ(tile_export.label_name);
				READ(tile_export.bank);
				READ(tile_export.tile_array);
				READ(tile_export.format);
				READ(tile_export.counter);
				READ(tile_export.from);
				READ(tile_export.up_to);
				READ(tile_export.compression);
				READ(tile_export.include_colors);
				READ(tile_export.sgb_palettes);
				READ(tile_export.gbc_palettes);
				READ(tile_export.make_meta_tiles);
				READ(tile_export.meta_offset);
				READ(tile_export.meta_counter);
				READ(tile_export.split);
				READ(tile_export.block_size);
				READ(tile_export.sel_tab);
				break;

			case OBJECT_TYPE_PALETTES:
				READ(palettes.id);
				READ(palettes.count);
				palettes.colors = new Palette[palettes.count];
				fread(palettes.colors, sizeof(Palette), palettes.count, file);
				READ(palettes.sgb_count);
				palettes.sgb_colors = new Palette[palettes.sgb_count];
				fread(palettes.sgb_colors, sizeof(Palette), palettes.sgb_count, file);

				fseek(file, sizeof(Palette) * 4, SEEK_CUR);//It seems there is a bug here (there are still 4 palettes that need to be read)
				break;

			case OBJECT_TYPE_TILEPAL:
				READ(tile_pal.id);
				READ(tile_pal.count);
				tile_pal.color_set = new LONG[tile_pal.count];
				fread(tile_pal.color_set, sizeof(LONG), tile_pal.count, file);
				READ(tile_pal.sgb_count);
				tile_pal.sgb_color_set = new LONG[tile_pal.sgb_count];
				fread(tile_pal.sgb_color_set, sizeof(LONG), tile_pal.sgb_count, file);

				fseek(file, 8, SEEK_CUR); //8 extra bits (?¿?)
 				break;

			default:
				fseek(file, object_length, SEEK_CUR);
		}
	}
	fclose(file);

	char export_name[512];
	char* slash_pos = strrchr(argv[1], '/') + 1;
	strncpy(export_name, slash_pos, strlen(slash_pos) - 4);
	export_name[strlen(slash_pos) - 4] = '\0';

	char export_file[512];
	sprintf(export_file, "%s/%s.h", argv[2], export_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	if(tile_export.include_colors){
		for(int i = 0; i < palettes.count; ++i) {
			for(int c = 0; c < 4; ++c) {
				Color color = palettes.colors[i].colors[c];
				fprintf(file, "#define %sCGBPal%dc%d %d\n", export_name, i, c, (color.r >> 3) | ((color.g >> 3) << 5) | ((color.b >> 3) << 10));
			}
			fprintf(file, "\n");
		}
		fprintf(file, "extern unsigned char %sCGB[];\n", export_name);
		fprintf(file, "extern unsigned char %s[];\n", export_name);
	}


	fclose(file);

	sprintf(export_file, "%s/%s.c", argv[2], export_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	if(tile_export.include_colors){
		fprintf(file, "const unsigned char %sCGB[] = {\n\t", export_name);
		for(int tile = tile_export.from; tile <= tile_export.up_to; ++ tile) {
			if(tile != tile_export.from)
				fprintf(file, ",");

			fprintf(file, "0x%02x", tile_pal.color_set[tile]);
		}
		fprintf(file, "\n};\n\n");
	}

	fprintf(file, "UINT8 %s_width = %d;\n", export_name, tile_set.info.width);
	fprintf(file, "UINT8 %s_height = %d;\n", export_name, tile_set.info.height);
	fprintf(file, "const unsigned char %s[] = {", export_name);
	for(int tile = tile_export.from; tile <= tile_export.up_to; ++ tile) {
		for(int y = 0; y < tile_set.info.height; y += 16) {
			for(int x = 0; x < tile_set.info.width; x += 8) {
				unsigned char* data_ptr = &tile_set.data[(tile_set.info.width * tile_set.info.height) * tile +  tile_set.info.width * y + x];
				for(int line = 0; line < 16; ++ line) {
					unsigned char l = BYTE(
						BIT(data_ptr[0], 1), BIT(data_ptr[1], 1), BIT(data_ptr[2], 1), BIT(data_ptr[3], 1), 
						BIT(data_ptr[4], 1), BIT(data_ptr[5], 1), BIT(data_ptr[6], 1), BIT(data_ptr[7], 1)
					);
					unsigned char h = BYTE(
						BIT(data_ptr[0], 0), BIT(data_ptr[1], 0), BIT(data_ptr[2], 0), BIT(data_ptr[3], 0), 
						BIT(data_ptr[4], 0), BIT(data_ptr[5], 0), BIT(data_ptr[6], 0), BIT(data_ptr[7], 0)
					);

					if(data_ptr != tile_set.data)
						fprintf(file, ",");
					if((line % 4) == 0)
						fprintf(file, "\n\t");

					fprintf(file, "0x%02x,0x%02x", h, l);
					data_ptr += tile_set.info.width;
				}
			}
		}
	}
	fprintf(file, "\n};");
	fclose(file);

    return 0;
}

