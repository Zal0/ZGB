#include <stdio.h>
#include "gbrParser.h"
#include "lodepng/lodepng.h"

using namespace GbrParser;

void ExtractFileName(char* path, char* file_name, bool include_bank) {
	char* slash_pos = strrchr(path, '/');
	if(slash_pos == 0)
		slash_pos = strrchr(path, '\\');
	if(slash_pos != 0)
		slash_pos ++;
	else
		slash_pos = path;

	char* dot_pos = include_bank ? strrchr(slash_pos, '.') : strchr(slash_pos, '.');
	if(dot_pos == 0) {
 		strcpy(file_name, slash_pos);
	} else {
		strncpy(file_name, slash_pos, dot_pos - slash_pos);
		file_name[dot_pos - slash_pos] = '\0';
	}
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("usage: gbr2png file_in.gbr export_folder");
		return 1;
	}

	GBRInfo gbrInfo;
	if(!LoadGBR(argv[1], &gbrInfo)) {
		return 1;
	}

	TileSet& tile_set = gbrInfo.tile_set;
	TileExport& tile_export = gbrInfo.tile_export;
	Palettes& palettes = gbrInfo.palettes;
	TilePal& tile_pal = gbrInfo.tile_pal;
	int& bank = gbrInfo.bank;
	char* palette_order = gbrInfo.palette_order;
	int& num_palettes = gbrInfo.num_palettes;

	char export_file_name[256]; 
	char export_file[512];
	ExtractFileName(argv[1], export_file_name, true);
	//sprintf(export_file_name, "%s", tile_export.label_name, bank);
	sprintf(export_file, "%s/%s.gbr.png", argv[2], export_file_name);

	int num_tiles_to_export = tile_export.up_to - tile_export.from  + 1;
	unsigned char* png_data = new unsigned char[tile_set.info.width * tile_set.info.height * num_tiles_to_export];
	int line_h = tile_set.info.height == 8 ? 8 : 16;
	for(int tile = tile_export.from; tile <= tile_export.up_to; ++ tile) {
		LONG source_palette_idx = tile_pal.color_set[tile];
		char palette_idx = palette_order[source_palette_idx];
		for(int y = 0; y < tile_set.info.height; y ++) {
			for(int x = 0; x < tile_set.info.width; x ++) {
				unsigned char* data_ptr = &tile_set.data[(tile_set.info.width * tile_set.info.height) * tile + tile_set.info.width * y + x];
				unsigned char* png_ptr = &png_data[(tile_set.info.width * tile_set.info.height) * (tile - tile_export.from) + tile_set.info.width * y + x];

				*png_ptr = *data_ptr;
				if(tile_export.include_colors)
					*png_ptr += (palette_idx * 4);
			}
		}
	}

	lodepng::State state;
#define ADD_PALETTE(R, G, B, A) lodepng_palette_add(&state.info_png.color, R, G, B, A); lodepng_palette_add(&state.info_raw, R, G, B, A)
	if(tile_export.include_colors){
		for(int p = 0; p < palettes.count; ++p) {
			if(palette_order[p] != -1)
			{
				for(int c = 0; c < 4; ++c) {
					Color color = palettes.colors[p].colors[c];
					ADD_PALETTE(color.r, color.g, color.b, c == 0 ? 0 : 255);
				}
			}
		}
	} else {
		ADD_PALETTE(255, 255, 255,   0);
		ADD_PALETTE(170, 170, 170, 255);
		ADD_PALETTE( 85,  85,  85, 255);
		ADD_PALETTE(  0,   0,   0, 255);
	}

  state.info_png.color.colortype = LCT_PALETTE; 
  state.info_png.color.bitdepth = 8;
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  state.encoder.auto_convert = 0; //we specify ourselves exactly what output PNG color mode we want

	std::vector<unsigned char> buffer;
	unsigned error = lodepng::encode(buffer, png_data, tile_set.info.width, tile_set.info.height * num_tiles_to_export, state);
	if(error)
	{
		printf("encoder error %d %s\n", error, lodepng_error_text(error));
		return 1;
	}

	error = lodepng::save_file(buffer, export_file);
	if(error)
	{
		printf("encoder error %d %s\n", error, lodepng_error_text(error));
		return 1;
	}

	//Load meta
	sprintf(export_file, "%s.meta", argv[1]);
	FILE* file = fopen(export_file, "r");
	if(!file)
	{
		//No meta, let's create one
		file = fopen(export_file, "w");
		if(!file)
		{
			printf("Error creating meta");
			return 1;
		}

		fprintf(file, "-px %d -py %d -pw %d -ph %d", 0, 0, tile_set.info.width, tile_set.info.height);
		fclose(file);

		file = fopen(export_file, "r");
		if(!file)
		{
			printf("Error opening meta");
			return 1;
		}
	}

	char meta_line[255];
	fgets(meta_line, 255, file);
	fclose(file);

	//Export meta file for png2mtspr
	sprintf(export_file, "%s/%s.gbr.png.meta", argv[2], export_file_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "-sh %d -b %d %s", tile_set.info.height, 255, meta_line);
	fclose(file);

	return 0;
}

