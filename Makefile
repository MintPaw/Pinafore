GAME_NAME=pinafore
WIN_LIBS=lib/win64
WIN_INCS=include/win64

MONTAGE=magick montage
ASEPRITE=/d/_tools/Aseprite-v1.1.9-dev/aseprite.exe
T_CONVERTER="/c/Program Files/OpenToonz 1.2/tconverter"
BUILD_DIR=../build

all:
	$(MAKE) b
	$(MAKE) r

ifneq (, $(findstring MSYS_NT, $(shell uname))) # -------------------------------------------------- Windows
API_TRACE_DIR=d:/_tools/apitrace/x64/bin/ # Must have trailing slash

b:
	-mkdir bin
	# $(MAKE) buildAssets
	set -o pipefail; \
		cmd /c "cl -MTd -Zi -EHsc -nologo -I$(WIN_INCS) $(WIN_LIBS)/*.lib winmm.lib opengl32.lib src/main.cpp -Fdbin/$(GAME_NAME).pdb -Fobin/$(GAME_NAME).obj -link /DEBUG -out:bin/$(GAME_NAME).exe" | tee errors.err
	cp $(WIN_LIBS)/SDL2d.dll bin
	cp $(WIN_LIBS)/glew32d.dll bin
	cp $(WIN_LIBS)/OpenAl32.dll bin/OpenAl32.dll
	# cp $(WIN_LIBS)/libcurl.dll bin
	# cp $(WIN_LIBS)/libcurl-d.dll bin
	cp -r assets bin

r:
	@echo .
	@echo .
	@echo .
	@(cd bin; ./$(GAME_NAME).exe)

debugC:
	cmd /c "devenv bin\$(GAME_NAME).exe"

pack:
	cd bin;\
		rm -f $(GAME_NAME).zip; \
		zip -r $(GAME_NAME).zip $(GAME_NAME).exe assets *.dll

dist:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	cp -r bin/$(GAME_NAME).exe bin/assets bin/*.dll $(BUILD_DIR)
endif # ------------------------------------------------------------------------------------------ End Windows

ifeq ($(shell uname), Linux) # ------------------------------------------------------------------- Linux
API_TRACE_DIR=

b:
	$(MAKE) buildAssets
	# g++ bin/libs.o src/one.cpp \
	# 	-g -Wall -Wno-unused-variable -Wno-sign-compare -Wno-strict-aliasing -Wno-trigraphs \
	# 	-isystem include/linux -isystem include/json -o bin/$(GAME_NAME) -lGL -lGLEW -lopenal -lSDL2

r:
	@echo .
	@echo .
	@echo .
	@(cd bin; ./$(GAME_NAME))

debugC:
	(cd bin; gdb $(GAME_NAME))

endif # ------------------------------------------------------------------------------------------ End Linux

buildAssets:
	rm -rf bin/assets
	
	cd raw; \
		texturepacker --basic-sort-by Name --disable-rotation --max-width 2048 --max-height 2048 --data ../assets/sprites.spr --sheet ../assets/sprites.png --format mintExporter --padding 0 --png-opt-level 1 sprites
	dos2unix assets/sprites.spr
	$(MONTAGE) -mode concatenate -channel rgba -background transparent -tile 8x raw/tiles/*.png assets/tileset.png

debugGl:
	rm -f bin/gl.trace
	cmd /c "$(API_TRACE_DIR)apitrace trace -o bin/gl.trace -v --api gl bin/$(GAME_NAME).exe"
	cmd /c "$(API_TRACE_DIR)qapitrace bin/gl.trace"
