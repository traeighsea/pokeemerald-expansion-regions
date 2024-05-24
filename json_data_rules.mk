# JSON files are run through jsonproc, which is a tool that converts JSON data to an output file
# based on an Inja template. https://github.com/pantor/inja


ifeq ($(OPTION_INDIVIDUAL_MAP_WILD_ENCOUNTERS),true)
	WILD_ENCOUNTERS_JSON := $(DATA_ASM_SUBDIR)/wild_encounters.json
	WILD_ENCOUNTERS_JSON_TXT := $(DATA_ASM_SUBDIR)/wild_encounters.json.txt
else
	WILD_ENCOUNTERS_JSON := $(DATA_SRC_SUBDIR)/wild_encounters.json
	WILD_ENCOUNTERS_JSON_TXT := $(DATA_SRC_SUBDIR)/wild_encounters.json.txt
endif

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/wild_encounters.h
$(DATA_SRC_SUBDIR)/wild_encounters.h: $(WILD_ENCOUNTERS_JSON) $(WILD_ENCOUNTERS_JSON_TXT)
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/wild_encounter.o: c_dep += $(DATA_SRC_SUBDIR)/wild_encounters.h

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
$(DATA_SRC_SUBDIR)/region_map/region_map_entries.h: $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/region_map.o: c_dep += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
