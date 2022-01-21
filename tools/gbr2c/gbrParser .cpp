#include "gbrParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define JUST_READ(var) fread((char*)&var, sizeof(var), 1, file)
#define READ(var) object_length -= (fread((char*)&var, sizeof(var), 1, file)) * sizeof(var)

namespace GbrParser
{

int GetBank(char* str) {
	char* bank_info = strstr(str, ".b");
	if(bank_info) {
		return atoi(bank_info + 2);
	}
	return 0;
}

void ExtractFileName(const char* path, char* file_name, bool include_bank) {
	const char* slash_pos = strrchr(path, '/');
	if(slash_pos == 0)
		slash_pos = strrchr(path, '\\');
	if(slash_pos != 0)
		slash_pos ++;
	else
		slash_pos = path;

	const char* dot_pos = include_bank ? strrchr(slash_pos, '.') : strchr(slash_pos, '.');
	if(dot_pos == 0) {
 		strcpy(file_name, slash_pos);
	} else {
		strncpy(file_name, slash_pos, dot_pos - slash_pos);
		file_name[dot_pos - slash_pos] = '\0';
	}
}

bool IsTileEmpty(TileSet* tile_set, int tile){
	unsigned char* data_ptr = &tile_set->data[tile_set->info.width * tile_set->info.height * tile];
	unsigned char* data_end = data_ptr + tile_set->info.width * tile_set->info.height;
	while(data_ptr != data_end) {
		if(0x3 & (*data_ptr)) {
			return false;
		}
		data_ptr ++;
	}
	return true;
}

bool LoadGBR(const char* path, GBRInfo* info) {
	FILE* file = fopen(path, "rb");
	if(!file) {
		printf("Error reading file %s", path);
		return false;
	}

	TileSet& tile_set = info->tile_set;
	TileExport& tile_export = info->tile_export;
	Palettes& palettes = info->palettes;
	TilePal& tile_pal = info->tile_pal;
	int& bank = info->bank;
	char* palette_order = info->palette_order;
	int& num_palettes = info->num_palettes;

	char marker[3];
	char version;
	JUST_READ(marker);
	JUST_READ(version);
		
	WORD object_type;
	WORD object_id;
	LONG object_length;
	while(JUST_READ(object_type)) {
		JUST_READ(object_id);
		JUST_READ(object_length);
			
		switch(object_type) {
			case OBJECT_TYPE_TILE_DATA: {
				READ(info->tile_set.info.name);
				READ(tile_set.info.width);
				READ(tile_set.info.height);
				READ(tile_set.info.count);
				READ(tile_set.info.color_set);

				int data_size = tile_set.info.width * tile_set.info.height * tile_set.info.count;
				tile_set.data = new unsigned char[data_size];
				object_length -= fread(tile_set.data, data_size, 1, file) * data_size;
				break;
			}

			case OBJECT_TYPE_TILE_EXPORT:
				READ(tile_export.tile_id);
				READ(tile_export.file_name);
				READ(tile_export.file_type);
				READ(tile_export.section_name);
				object_length -= (fread((char*)&tile_export.label_name, 20, 1, file)) * 20; //label_name must be 20 chars
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
				object_length -= fread(palettes.colors, sizeof(Palette), palettes.count, file) * sizeof(Palette);
				READ(palettes.sgb_count);
				palettes.sgb_colors = new Palette[palettes.sgb_count];
				object_length -= fread(palettes.sgb_colors, sizeof(Palette), palettes.sgb_count, file) * sizeof(Palette);
				break;

			case OBJECT_TYPE_TILEPAL:
				READ(tile_pal.id);
				READ(tile_pal.count);
				tile_pal.color_set = new LONG[tile_pal.count];
				object_length -= fread(tile_pal.color_set, sizeof(LONG), tile_pal.count, file) * sizeof(LONG);
				READ(tile_pal.sgb_count);
				tile_pal.sgb_color_set = new LONG[tile_pal.sgb_count];
				object_length -= fread(tile_pal.sgb_color_set, sizeof(LONG), tile_pal.sgb_count, file) * sizeof(LONG);
 				break;
		}
		
		if(object_length)
			fseek(file, object_length, SEEK_CUR);
	}
	fclose(file);

	//Extract bank
	//int bank = GetBank(path);
	//if(bank == 0) //for backwards compatibility, extract the bank from tile_export.name
	//	bank = GetBank(tile_export.file_name);
	//if(bank == 0)
	//	bank = tile_export.bank;
	bank = 255;

	//Adjust export file name and label name
	if(strcmp(tile_export.file_name, "Export.z80") == 0 || strcmp(tile_export.file_name, "") == 0) { //Default value
		ExtractFileName(path, tile_export.file_name, false); //Set path instead
	}

	if(strcmp(tile_export.label_name, "TileLabel") == 0 || strcmp(tile_export.label_name, "") == 0) { //Default value
		ExtractFileName(path, tile_export.label_name, false); //Set path instead
	}

	//Check tiles to export
	if(tile_export.from == 0 && tile_export.up_to == 0) {
		int tile = tile_set.info.count - 1;
		if(tile_set.info.count == 256) { //Special case for maps, the last tiles are for mapping enemies
			tile = 254;
			while(!IsTileEmpty(&tile_set, tile)) {
				tile --;
			}
		}

		while(tile > 0 && tile_export.up_to == 0) {
			unsigned char* data_ptr = &tile_set.data[tile_set.info.width * tile_set.info.height * tile];
			if(!IsTileEmpty(&tile_set, tile)) {
				tile_export.up_to = tile;
			}

			tile --;
		}
	}

	//Check palettes export
	if(!tile_export.include_colors) {
		for(int i = 0; i < palettes.count; ++i) {
			if((*(int*)&palettes.colors[i].colors[0]) != 0x0029efe0 ||
			   (*(int*)&palettes.colors[i].colors[1]) != 0x0042b939 ||
			   (*(int*)&palettes.colors[i].colors[2]) != 0x00317520 ||
			   (*(int*)&palettes.colors[i].colors[3]) != 0x002e3907) {
				tile_export.include_colors = true;
			}
		}
	}

	if(tile_export.include_colors){
		//check which palettes are being used
		for(int tile = tile_export.from; tile <= tile_export.up_to; ++ tile) {
			int pal = tile_pal.color_set[tile];
			palette_order[pal] = 0; //Mark as used
		}
		for(int i = 0, order = 0; i < 8; ++i)
		{
			if(palette_order[i] == 0)
			{
				palette_order[i] = order ++;
				num_palettes ++;
			}
		}
	}

	return true;
}

};