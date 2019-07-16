#include <stdio.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int LONG;
typedef unsigned int INTEGER;
typedef unsigned char BYTE;

#define READ(var) fread((char*)&var, sizeof(var), 1, file)

enum ObjectTypes {
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

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("usage: gbrmc file_in.gbm export_folder");
		return 1;
	}

	FILE* file = fopen(argv[1], "rb");
	if(!file) {
		printf("Error reading file");
		return 1;
	}

	ObjectHeader object_header;
	Map map;
	MapTileData* map_tiles_data;

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
				}
 				break;
			}

			default:
				fseek(file, object_header.length, SEEK_CUR);
				break;
		}
	}
	fclose(file);

	char export_name[512];
	char* slash_pos = strrchr(argv[1], '/') + 1;
	strncpy(export_name, slash_pos, strlen(slash_pos) - 4);
	export_name[strlen(slash_pos) - 4] = '\0';

	char export_file[512];
	sprintf(export_file, "%s/%s.c", argv[2], export_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "UINT8 %s_width = %d;\n", export_name, map.width);
	fprintf(file, "UINT8 %s_height = %d;\n", export_name, map.height);
	fprintf(file, "const unsigned char %s[] = {", export_name);
	for(int i = 0; i < map.width * map.height; ++i) {
		if(i != 0)
			fprintf(file, ",");
		if((i % 10) == 0)
			fprintf(file, "\n\t");

		fprintf(file, "0x%02x", map_tiles_data[i].tile_number);
	}
	fprintf(file, "\n};");

    return 0;
}

