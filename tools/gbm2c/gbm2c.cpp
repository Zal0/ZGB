#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "gbrParser.h"

typedef unsigned short WORD;
typedef unsigned int LONG;
typedef unsigned int INTEGER;
typedef unsigned char BYTE;

#define READ(var) fread((char*)&var, sizeof(var), 1, file)

enum GBMObjectTypes {
	OBJECT_TYPE_PRODUCER = 0x1,
	OBJECT_TYPE_MAP = 0x2,
	OBJECT_TYPE_MAP_TILE_DATA = 0x3,
	OBJECT_TYPE_MAP_PROPERTIES = 0x4,
	OBJECT_TYPE_MAP_PROPERTY_DATA = 0x5,
	OBJECT_TYPE_MAP_DEFAULT_PROPERTY_VALUE = 0x6,
	OBJECT_TYPE_MAP_SETTINGS = 0x7,
	OBJECT_TYPE_MAP_PROPERTY_COLORS = 0x8,
	OBJECT_TYPE_MAP_EXPORT_SETTINGS = 0x9,
	OBJECT_TYPE_MAP_EXPORT_PROPERTIES = 0xA,
	OBJECT_TYPE_DELETED = 0xFFFF
};

struct ObjectHeader {
	char marker[6];
	WORD type;
	WORD id;
	WORD master_id;
	LONG crc;
	LONG length;
};

bool ReadHeader(ObjectHeader* header, FILE* file) {
	if(READ(header->marker))
	{
		READ(header->type);
		READ(header->id);
		READ(header->master_id);
		READ(header->crc);
		READ(header->length);

		return 1;
	}
	return 0;
}

struct Map {
	char name[128];
	INTEGER width;
	INTEGER height;
	INTEGER prop_count;
	char tile_file[256];
	INTEGER tile_count;
	INTEGER prop_color_count;
};

struct MapTileData {
	WORD tile_number;
	unsigned char gbc_palette;
	unsigned char sgb_palette;
	bool h_flip;
	bool v_flip;
};

struct MapExportSettings {
	char file_name[256];
	BYTE file_type;
	char section_name[39];
	char label_name[128];//[40];
	BYTE bank;
	WORD plane_count;
	WORD plane_order;
	WORD map_layout;
	bool split;
	INTEGER split_size;
	bool split_bank;
	BYTE sel_tab;
	WORD prop_count;
	WORD tile_offset;
};

void GetGBRPath(char* gbm_path, char* tile_file, char* gbr_path)
{
	char tile_file_name[256];
	GbrParser::ExtractFileName(tile_file, tile_file_name, false);

	char* slash_pos = strrchr(gbm_path, '/');
	if(slash_pos == 0)
		slash_pos = strrchr(gbm_path, '\\');
	if(slash_pos != 0)
		slash_pos ++;
	else
		slash_pos = gbm_path;

	char path[256];
	strncpy(path, gbm_path, slash_pos - gbm_path);
	path[slash_pos - gbm_path] = '\0';
	sprintf(gbr_path, "%s%s.gbr", path, tile_file_name);
}

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("usage: gbm2c file_in.gbm export_folder");
		return 1;
	}

	FILE* file = fopen(argv[1], "rb");
	if(!file) {
		printf("Error reading file %s", argv[1]);
		return 1;
	}

	ObjectHeader object_header;
	Map map;
	MapTileData* map_tiles_data;
	MapExportSettings map_export_settings;
	bool export_attributes = false;

	char marker[3];
	char version;
	READ(marker);
	READ(version);

	while(ReadHeader(&object_header, file)) {
		switch(object_header.type) {
			case OBJECT_TYPE_MAP:
				READ(map.name);
				READ(map.width);
				READ(map.height);
				READ(map.prop_count);
				READ(map.tile_file);
				READ(map.tile_count);
				READ(map.prop_color_count);
				break;

			case OBJECT_TYPE_MAP_TILE_DATA: {
				INTEGER num_tiles = map.width * map.height;
				INTEGER record = 0;
				unsigned char* record_ptr = (unsigned char*)(void*)&record;
				map_tiles_data = new MapTileData[num_tiles];
				for(INTEGER i = 0; i < num_tiles; ++i) {
					fread(&record_ptr[2], 1, 1, file);
					fread(&record_ptr[1], 1, 1, file);
					fread(&record_ptr[0], 1, 1, file);

					map_tiles_data[i].tile_number = 0x3FF & record;
					map_tiles_data[i].gbc_palette = 0x1F & (record >> 10);
					map_tiles_data[i].sgb_palette = 0x7 & (record >> 16);
					map_tiles_data[i].h_flip = 0x1 & (record >> 22);
					map_tiles_data[i].v_flip = 0x1 & (record >> 23);

					if(map_tiles_data[i].gbc_palette != 0 || map_tiles_data[i].h_flip || map_tiles_data[i].v_flip || map_tiles_data[i].tile_number > 255) {
						export_attributes = true;
					}
				}
				int pending = object_header.length - num_tiles * 3;
				if(pending)
					fseek(file, pending, SEEK_CUR);
 				break;
			}

			case OBJECT_TYPE_MAP_EXPORT_SETTINGS:
				READ(map_export_settings.file_name);
				READ(map_export_settings.file_type);
				READ(map_export_settings.section_name);
				//READ(map_export_settings.label_name);
				(fread((char*)&map_export_settings.label_name, 40, 1, file)); //label_name must be 40 chars
				READ(map_export_settings.bank);
				READ(map_export_settings.plane_count);
				READ(map_export_settings.plane_order);
				READ(map_export_settings.map_layout);
				READ(map_export_settings.split);
				READ(map_export_settings.split_size);
				READ(map_export_settings.split_bank);
				READ(map_export_settings.sel_tab);
				READ(map_export_settings.prop_count);
				READ(map_export_settings.tile_offset);
				break;

			default:
				fseek(file, object_header.length, SEEK_CUR);
				break;
		}
	}
	fclose(file);

	GbrParser::GBRInfo gbrInfo;
	if(export_attributes) {
		char gbr_path[256];
		GetGBRPath(argv[1], map.tile_file, gbr_path);
		if(!GbrParser::LoadGBR(gbr_path, &gbrInfo)) {
			printf("Error reading gbr file %s", map.tile_file);
			return 1;
		}
	}

	//Extract bank
	/*int bank = GetBank(argv[1]);
	if(bank == 0) //for backwards compatibility, extract the bank from tile_export.name
		bank = GetBank(map_export_settings.file_name);
	if(bank == 0)
		bank = map_export_settings.bank;*/
	int bank = 255;

	//Adjust export file name and label name
	if(strcmp(map_export_settings.file_name, "") == 0) { //Default value
		GbrParser::ExtractFileName(argv[1], map_export_settings.file_name, false);  //Set argv[1] instead
	}

	if(strcmp(map_export_settings.label_name, "") == 0) { //Default value
		GbrParser::ExtractFileName(argv[1], map_export_settings.label_name, false);  //Set argv[1] instead
	}
	GbrParser::Replace(map_export_settings.label_name, ' ', '_'); //Ensure the label_name doesn't contain any spaces

	char export_file_name[256]; //For both .h and .c
	char export_file[512];
	GbrParser::ExtractFileName(map_export_settings.file_name, export_file_name, false); //For backwards compatibility the header will be taken from the export filename (and not argv[1])
	sprintf(export_file, "%s/%s.h", argv[2], export_file_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "#ifndef MAP_%s_H\n",  map_export_settings.label_name);
	fprintf(file, "#define MAP_%s_H\n",  map_export_settings.label_name);
	
	fprintf(file, "#define %sWidth %d\n", map_export_settings.label_name, map.width);
    fprintf(file, "#define %sHeight %d\n", map_export_settings.label_name, map.height);

	fprintf(file, "#include \"MapInfo.h\"\n");
	fprintf(file, "extern unsigned char bank_%s;\n", map_export_settings.label_name);
	fprintf(file, "extern struct MapInfo %s;\n", map_export_settings.label_name);

	fprintf(file, "#endif\n");

	fclose(file);

	GbrParser::ExtractFileName(argv[1], export_file_name, true);
	sprintf(export_file, "%s/%s.gbm.c", argv[2], export_file_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}
	
	fprintf(file, "#pragma bank %d\n", bank);

	fprintf(file, "#include <gbdk/platform.h>\n");

	fprintf(file, "const unsigned char %s_map[] = {", map_export_settings.label_name);
	for(INTEGER i = 0; i < map.width * map.height; ++i) {
		if(i != 0)
			fprintf(file, ",");
		if((i % 10) == 0)
			fprintf(file, "\n\t");

		fprintf(file, "0x%02x", map_tiles_data[i].tile_number);
	}
	fprintf(file, "\n};\n");

	if(export_attributes) {
		fprintf(file, "const unsigned char %s_attributes[] = {", map_export_settings.label_name);
		for(INTEGER i = 0; i < map.width * map.height; ++i) {
			if(i != 0)
				fprintf(file, ",");
			if((i % 10) == 0)
				fprintf(file, "\n\t");

			MapTileData data = map_tiles_data[i];
			bool vram_bank = data.tile_number > 255;
			//gbc_palette 0 is used by GBMB to indicate Default palette (the one stores in the tile)
			unsigned char pal_idx = (data.gbc_palette == 0 ? gbrInfo.palette_order[gbrInfo.tile_pal.color_set[data.tile_number]] : data.gbc_palette - 1);
			fprintf(file, "0x%02x", pal_idx | (vram_bank << 3) | (data.h_flip << 5) | (data.v_flip << 6));
		}
		fprintf(file, "\n};\n");
	}

	char tile_file[256];
	GbrParser::ExtractFileName(map.tile_file, tile_file, false);
	fprintf(file, "#include \"TilesInfo.h\"\n");
	fprintf(file, "extern const void __bank_%s;\n", tile_file);
	fprintf(file, "extern const struct TilesInfo %s;\n", tile_file);
	fprintf(file, "\n");

	fprintf(file, "#include \"MapInfo.h\"\n");

	fprintf(file, "const void __at(%d) __bank_%s;\n", bank, map_export_settings.label_name);
	fprintf(file, "const struct MapInfo %s = {\n", map_export_settings.label_name);
	fprintf(file, "\t%s_map, //map\n", map_export_settings.label_name);
	fprintf(file, "\t%d, //width\n", map.width);
	fprintf(file, "\t%d, //height\n", map.height);
	if(export_attributes) {
		fprintf(file, "\t%s_attributes, //attributes\n", map_export_settings.label_name);
	} else {
		fprintf(file, "\t%s, //attributes\n", "0");
	}
	fprintf(file, "\tBANK(%s), //tiles bank\n", tile_file);
	fprintf(file, "\t&%s, //tiles info\n", tile_file);
	fprintf(file, "};");

	return 0;
}

