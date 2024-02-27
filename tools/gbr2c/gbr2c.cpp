#include <stdio.h>
#include <string.h>

#include "gbrParser.h"
using namespace GbrParser;

int main(int argc, char* argv[]) {
	int bpp = 2;

	if((argc != 3) && (argc != 5)) {
		printf("usage: gbr2c file_in.gbr export_folder [-bpp N]");
		return 1;
	}

	GBRInfo gbrInfo;
	if(!LoadGBR(argv[1], &gbrInfo)) {
		return 1;
	}

	if(argc == 5) {
		if (strcmp(argv[3], "-bpp") != 0) {
			printf("unknown argument %s", argv[3]);
			return 1;
		}
		if (strcmp(argv[4], "2") == 0) {
			bpp = 2;
		} else if (strcmp(argv[4], "4") == 0) {
			bpp = 4;
		} else {
			printf("unsupported bpp depth not in [2, 4]: %s", argv[4]);
			return 1;
		}
	}

	TileSet& tile_set = gbrInfo.tile_set;
	TileExport& tile_export = gbrInfo.tile_export;
	Palettes& palettes = gbrInfo.palettes;
	TilePal& tile_pal = gbrInfo.tile_pal;
	int& bank = gbrInfo.bank;
	char* palette_order = gbrInfo.palette_order;
	int& num_palettes = gbrInfo.num_palettes;


	char export_file_name[256]; //For both .h and .c
	char export_file[512];
	ExtractFileName(tile_export.file_name, export_file_name, false); //For backwards compatibility the header will be taken from the export filename (and not argv[1])
	sprintf(export_file, "%s/%s.h", argv[2], export_file_name); 
	FILE* file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "#ifndef TILES_%s_H\n",  tile_export.label_name);
	fprintf(file, "#define TILES_%s_H\n",  tile_export.label_name);

	if(tile_export.include_colors){
		for(int p = 0; p < palettes.count; ++ p) {
			if(palette_order[p] != -1) {
				for(int c = 0; c < 4; ++c) {
					Color color = palettes.colors[p].colors[c];
					fprintf(file, "#define %sCGBPal%dc%d %d\n", tile_export.label_name, palette_order[p], c, (color.r >> 3) | ((color.g >> 3) << 5) | ((color.b >> 3) << 10));
				}
				fprintf(file, "\n");
			}
		}
	}
	fprintf(file, "#include \"TilesInfo.h\"\n");
	fprintf(file, "extern const void __bank_%s;\n", tile_export.label_name);
	fprintf(file, "extern struct TilesInfo %s;\n", tile_export.label_name);

	fprintf(file, "#endif\n");

	fclose(file);

	ExtractFileName(argv[1], export_file_name, true);
	sprintf(export_file, "%s/%s.gbr.c", argv[2], export_file_name);
	file = fopen(export_file, "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}
	
	fprintf(file, "#pragma bank %d\n", bank);

	fprintf(file, "#include <gbdk/platform.h>\n");

	if(tile_export.include_colors){
		//Export palettes
		fprintf(file, "\n");
		fprintf(file, "const palette_color_t %s_palettes[] = {\n", tile_export.label_name);
		int col_count  = 0;
		for(int p = 0; p < palettes.count; ++p) 
		{
			if(palette_order[p] != -1)
			{
				if(palette_order[p] != 0)
					fprintf(file, ",\n");
				fprintf(file, "\t");
			
				for(int c = 0; c < 4; ++c) {
					Color color = palettes.colors[p].colors[c];
					fprintf(file, "RGB8(%d, %d, %d)", color.r, color.g, color.b);
					if(c != 3)
						fprintf(file, ", ");
					++ col_count;
				}
			}
		}
		if((bpp == 4) && (col_count % 16)) {
			fprintf(file, ",");
			for(int pad = col_count % 16; pad < 16; ++ pad) {
				if((pad % 4) == 0)
					fprintf(file, "\n\t");
				fprintf(file, "RGB8(0, 0, 0)");
				if(pad != 15)
					fprintf(file, ", ");
			}
		}
		fprintf(file, "\n};\n");

		//Export palette per tile
		fprintf(file, "\n");
		fprintf(file, "const unsigned char %sCGB[] = {\n\t", tile_export.label_name);
		for(int tile = tile_export.from, cnt = 0; tile <= tile_export.up_to; ++ tile, ++ cnt) {
			if(tile != tile_export.from)
				fprintf(file, ",");

			if((cnt) && (cnt % 8 == 0))
				fprintf(file, "\n\t");

			fprintf(file, "0x%02x", (bpp == 2) ? palette_order[tile_pal.color_set[tile]] : 0);
		}
		fprintf(file, "\n};\n");
	}

	fprintf(file, "\n");
	fprintf(file, "const unsigned char %s_tiles[] = {", tile_export.label_name);
	int line_h = tile_set.info.height == 8 ? 8 : 16;
	for(int tile = tile_export.from; tile <= tile_export.up_to; ++ tile) {
		unsigned char pal = (tile_export.include_colors) ? palette_order[tile_pal.color_set[tile]] + 1 : 1;
		for(int y = 0; y < tile_set.info.height; y += line_h) {
			for(int x = 0; x < tile_set.info.width; x += 8) {
				unsigned char* data_ptr = &tile_set.data[(tile_set.info.width * tile_set.info.height) * tile +  tile_set.info.width * y + x];
				for(int line = 0; line < line_h; ++ line) {
					unsigned char l, h, L, H;
					if(bpp == 2) {
						l = BYTE(
							BIT(data_ptr[0], 1), BIT(data_ptr[1], 1), BIT(data_ptr[2], 1), BIT(data_ptr[3], 1), 
							BIT(data_ptr[4], 1), BIT(data_ptr[5], 1), BIT(data_ptr[6], 1), BIT(data_ptr[7], 1)
						);
						h = BYTE(
							BIT(data_ptr[0], 0), BIT(data_ptr[1], 0), BIT(data_ptr[2], 0), BIT(data_ptr[3], 0), 
							BIT(data_ptr[4], 0), BIT(data_ptr[5], 0), BIT(data_ptr[6], 0), BIT(data_ptr[7], 0)
						);
					} else {
						l = BYTE(
							BIT(data_ptr[0] * pal, 1), BIT(data_ptr[1] * pal, 1), BIT(data_ptr[2] * pal, 1), BIT(data_ptr[3] * pal, 1), 
							BIT(data_ptr[4] * pal, 1), BIT(data_ptr[5] * pal, 1), BIT(data_ptr[6] * pal, 1), BIT(data_ptr[7] * pal, 1)
						);
						h = BYTE(
							BIT(data_ptr[0] * pal, 0), BIT(data_ptr[1] * pal, 0), BIT(data_ptr[2] * pal, 0), BIT(data_ptr[3] * pal, 0), 
							BIT(data_ptr[4] * pal, 0), BIT(data_ptr[5] * pal, 0), BIT(data_ptr[6] * pal, 0), BIT(data_ptr[7] * pal, 0)
						);
						L = BYTE(
							BIT(data_ptr[0] * pal, 3), BIT(data_ptr[1] * pal, 3), BIT(data_ptr[2] * pal, 3), BIT(data_ptr[3] * pal, 3), 
							BIT(data_ptr[4] * pal, 3), BIT(data_ptr[5] * pal, 3), BIT(data_ptr[6] * pal, 3), BIT(data_ptr[7] * pal, 3)
						);
						H = BYTE(
							BIT(data_ptr[0] * pal, 2), BIT(data_ptr[1] * pal, 2), BIT(data_ptr[2] * pal, 2), BIT(data_ptr[3] * pal, 2), 
							BIT(data_ptr[4] * pal, 2), BIT(data_ptr[5] * pal, 2), BIT(data_ptr[6] * pal, 2), BIT(data_ptr[7] * pal, 2)
						);
					}

					if(data_ptr != tile_set.data)
						fprintf(file, ",");
					if((line % 4) == 0)
						fprintf(file, "\n\t");
					if(x == 0 && y == 0 && line == 0) {
						if(tile != tile_export.from)
							fprintf(file, "\n\t");
						fprintf(file, "//Frame %d\n\t", tile);
					}

					if(bpp == 2) 
						fprintf(file, "0x%02x,0x%02x", h, l);
					else
						fprintf(file, "0x%02x,0x%02x,0x%02x,0x%02x", h, l, H, L);
					data_ptr += tile_set.info.width;
				}
			}
		}
	}
	fprintf(file, "\n};\n");
	
	fprintf(file, "\n#include \"TilesInfo.h\"\n");
	fprintf(file, "const void __at(%d) __bank_%s;\n", bank, tile_export.label_name);
	fprintf(file, "const struct TilesInfo %s = {\n", tile_export.label_name);
	fprintf(file, "\t.num_frames = %d, //num_tiles\n", tile_export.up_to - tile_export.from + 1);
	fprintf(file, "\t.data = %s_tiles, //tiles\n", tile_export.label_name);
	fprintf(file, "\t.num_pals = %d, //num_palettes\n", num_palettes);
	if(tile_export.include_colors) {
		fprintf(file, "\t.pals = %s_palettes, //palettes\n", tile_export.label_name);
		fprintf(file, "\t.color_data = %sCGB //CGB palette\n", tile_export.label_name);
	}
	fprintf(file, "};");
	
	fclose(file);

    return 0;
}

