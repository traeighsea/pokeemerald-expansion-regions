# Map JSON data

MAPS_DIR = $(DATA_ASM_SUBDIR)/maps
LAYOUTS_DIR = $(DATA_ASM_SUBDIR)/layouts

MAP_DIRS := $(dir $(wildcard $(MAPS_DIR)/*/map.json))
MAP_CONNECTIONS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/connections.inc,$(MAP_DIRS))
MAP_EVENTS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/events.inc,$(MAP_DIRS))
MAP_HEADERS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/header.inc,$(MAP_DIRS))

# The following vars and build targets are optionally used only if using json for mapgrid data
LAYOUTS_MAP_BINS := $(wildcard $(LAYOUTS_DIR)/*/map.bin)
LAYOUTS_BORDER_BINS := $(wildcard $(LAYOUTS_DIR)/*/border.bin)

# The following vars and build targets are optionally used only if using json for metatiles data
TILESETS_METATILES_BINS := $(wildcard $(DATA_ASM_SUBDIR)/tilesets/primary/*/metatiles.bin) $(wildcard $(DATA_ASM_SUBDIR)/tilesets/secondary/*/metatiles.bin)
TILESETS_METATILE_ATTRIBUTES_BINS := $(wildcard $(DATA_ASM_SUBDIR)/tilesets/primary/*/metatile_attributes.bin) $(wildcard $(DATA_ASM_SUBDIR)/tilesets/secondary/*/metatile_attributes.bin)

ifeq ($(OPTION_LAYOUT_MAPGRIDS_USE_JSON),true)
LAYOUTS_MAP_JSONS := $(wildcard $(LAYOUTS_DIR)/*/map.json)
LAYOUTS_BORDER_JSONS := $(wildcard $(LAYOUTS_DIR)/*/border.json)

mapgrid-bins-generated: $(LAYOUTS_BORDER_BINS) $(LAYOUTS_MAP_BINS)
$(LAYOUTS_DIR)/%/border.bin: $(LAYOUTS_DIR)/%/border.json
	$(JSON2BIN) mapgrid $<
$(LAYOUTS_DIR)/%/map.bin: $(LAYOUTS_DIR)/%/map.json
	$(JSON2BIN) mapgrid $<
endif

ifeq ($(OPTION_TILESET_METATILES_USE_JSON),true)
TILESETS_METATILES_JSONS := $(wildcard $(DATA_ASM_SUBDIR)/tilesets/primary/*/metatiles.json) $(wildcard $(DATA_ASM_SUBDIR)/tilesets/secondary/*/metatiles.json)
TILESETS_METATILE_ATTRIBUTES_JSONS := $(wildcard $(DATA_ASM_SUBDIR)/tilesets/primary/*/metatile_attributes.json) $(wildcard $(DATA_ASM_SUBDIR)/tilesets/secondary/*/metatile_attributes.json)

metatile-bins-generated: $(TILESETS_METATILES_BINS) $(TILESETS_METATILE_ATTRIBUTES_BINS)
%/metatiles.bin: %/metatiles.json
	$(JSON2BIN) metatiles $<
%/metatile_attributes.bin: %/metatile_attributes.json
	$(JSON2BIN) metatile_attributes $<
endif

# Note the mapgrid-bins-generated and metatile-bins-generated only get created when the option is toggled
$(DATA_ASM_SUBDIR)/maps.s: $(mapgrid-bins-generated) $(metatile-bins-generated) 

$(DATA_ASM_BUILDDIR)/maps.o: $(DATA_ASM_SUBDIR)/maps.s $(LAYOUTS_DIR)/layouts.inc $(LAYOUTS_DIR)/layouts_table.inc $(MAPS_DIR)/headers.inc $(MAPS_DIR)/groups.inc $(MAPS_DIR)/connections.inc $(MAP_CONNECTIONS) $(MAP_HEADERS)
	$(PREPROC) $< charmap.txt | $(CPP) -I include - | $(AS) $(ASFLAGS) -o $@
$(DATA_ASM_BUILDDIR)/map_events.o: $(DATA_ASM_SUBDIR)/map_events.s $(MAPS_DIR)/events.inc $(MAP_EVENTS)
	$(PREPROC) $< charmap.txt | $(CPP) -I include - | $(AS) $(ASFLAGS) -o $@

$(MAPS_DIR)/%/header.inc: $(MAPS_DIR)/%/map.json
	$(MAPJSON) map emerald $< $(LAYOUTS_DIR)/layouts.json
$(MAPS_DIR)/%/events.inc: $(MAPS_DIR)/%/header.inc ;
$(MAPS_DIR)/%/connections.inc: $(MAPS_DIR)/%/events.inc ;

$(MAPS_DIR)/groups.inc: $(MAPS_DIR)/map_groups.json
	$(MAPJSON) groups emerald $<
$(MAPS_DIR)/connections.inc: $(MAPS_DIR)/groups.inc ;
$(MAPS_DIR)/events.inc: $(MAPS_DIR)/connections.inc ;
$(MAPS_DIR)/headers.inc: $(MAPS_DIR)/events.inc ;
include/constants/map_groups.h: $(MAPS_DIR)/headers.inc ;

$(LAYOUTS_DIR)/layouts.inc: $(LAYOUTS_DIR)/layouts.json
	$(MAPJSON) layouts emerald $<
$(LAYOUTS_DIR)/layouts_table.inc: $(LAYOUTS_DIR)/layouts.inc ;
include/constants/layouts.h: $(LAYOUTS_DIR)/layouts_table.inc ;

# This is a migration script you can run to convert data/layouts/*/border.bin and data/layouts/*/map.bin bin files to json
.PHONY: run-layout-mapgrids-bin-to-json
run-layout-mapgrids-bin-to-json:
	$(BIN2JSON) rse mapgrid $(LAYOUTS_BORDER_BINS) $(LAYOUTS_MAP_BINS)

# This is a migration script you can run to convert data/tilesets/*/*/metatiles.bin and data/tilesets/*/*/metatile_attributes.bin bin files to json
.PHONY: run-tileset-metatiles-bin-to-json
run-tileset-metatiles-bin-to-json:
	$(BIN2JSON) rse metatiles $(TILESETS_METATILES_BINS)
	$(BIN2JSON) rse metatile_attributes $(TILESETS_METATILE_ATTRIBUTES_BINS)