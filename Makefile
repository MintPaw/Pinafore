GAME_NAME=Pinafore
RAW_ASSETS_DIR=raw
WIN_LIBS=lib/win64
WIN_INCS=include/win64

MONTAGE=magick montage
ASEPRITE=
ifneq ("$(wildcard /d/_tools/Aseprite-v1.1.9-dev/aseprite.exe)","")
	ASEPRITE=/d/_tools/Aseprite-v1.1.9-dev/aseprite.exe
else ifneq ("$(wildcard /d/Programs/Aseprite-v1.1.9-dev/Aseprite-v1.1.9-dev/aseprite.exe)","")
	ASEPRITE=/d/Programs/Aseprite-v1.1.9-dev/Aseprite-v1.1.9-dev/aseprite.exe
endif

WIN_API_TRACE_DIR=d:/_tools/apitrace/x64/bin

PYTHON3=/c/Users/MintPaw/AppData/Local/Programs/Python/Python37-32/python.exe

all:
	$(MAKE) b
	$(MAKE) r

ifneq (, $(findstring MSYS_NT, $(shell uname))) # -------------------------------------------------- Windows

b:
	-mkdir bin
	set -o pipefail; \
		cmd /c "cl -MTd -Zi -EHsc -nologo -I$(WIN_INCS) $(WIN_LIBS)/*.lib shcore.lib winmm.lib opengl32.lib src/main.cpp -Fdbin/$(GAME_NAME).pdb -Fobin/$(GAME_NAME).obj -link /DEBUG -out:bin/$(GAME_NAME).exe" | tee errors.err
	cp $(WIN_LIBS)/*.dll bin

bRelease:
	-mkdir bin
	set -o pipefail; \
		cmd /c "cl -MT -nologo -I$(WIN_INCS) $(WIN_LIBS)/*.lib winmm.lib shcore.lib winmm.lib opengl32.lib src/main.cpp -Fdbin/$(GAME_NAME).pdb -Fobin/$(GAME_NAME).obj -link /NODEFAULTLIB:msvcrt -out:bin/$(GAME_NAME).exe" | tee errors.err
	cp $(WIN_LIBS)/*.dll bin

r:
	@echo .
	@echo .
	@echo .
	@(cd bin; ./$(GAME_NAME).exe)

EMSCRIPTEN_DIR_WIN := D:\_tools/_sdks/emsdk-portable/emscripten/1.37.21
bhtml5:
	-mkdir bin/webgl
	echo Do it manually

rhtml5:
	cp $(RAW_ASSETS_DIR)/emscriptenTemplate.html bin/webgl/index.html
	clear; cd bin/webgl; start 'http://127.0.0.1:8000' & $(PYTHON3) -m http.server


debugC:
	cmd /c "devenv bin\$(GAME_NAME).exe"

pack:
	$(MAKE) bRelease
	cd bin; \
		cp ../assets . -r; \
		rm -f $(GAME_NAME).zip; \
		zip -r $(GAME_NAME).zip $(GAME_NAME).exe assets *.dll; \
		rm -rf assets

dist:
	$(MAKE) pack
	rm -rf ../dist
	unzip bin/$(GAME_NAME).zip -d ../dist
endif # ------------------------------------------------------------------------------------------ End Windows

ifeq ($(shell uname), Linux) # ------------------------------------------------------------------- Linux

b:
	echo Not set supported
	# $(MAKE) buildAssets
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
	find $(RAW_ASSETS_DIR)/sprites -name "*.png" -delete
	for file in `find $(RAW_ASSETS_DIR)/sprites/ -type f -name "*.ase"`; do \
		$(ASEPRITE) -b "$$file" --filename-format '{path}/{title}_{frame000}.{extension}' --save-as "$${file%.*}.png"& \
		done; \
		wait;
	texturepacker --basic-sort-by Name --disable-rotation --max-width 4096 --max-height 4096 --data assets/sprites.spr --sheet assets/sprites.png --format mintExporter --padding 10 --premultiply-alpha $(RAW_ASSETS_DIR)/sprites
	dos2unix assets/sprites.spr
	dos2unix assets/catalogs/*
	# cp $(RAW_ASSETS_DIR)/tileset.png assets
	# cp $(RAW_ASSETS_DIR)/injectedCatalogs/* assets/catalogs
	
	rm -rf assets/audio
	for file in `find $(RAW_ASSETS_DIR)/audio/ -type f -name "*.wav"`; do \
		baseFileName=`basename $$file`; \
		baseFileName="$${baseFileName%.*}"; \
		endDir=`dirname $$file`; \
		endDir=assets/"$${endDir#*/}"; \
		endFile=$$endDir/$$baseFileName.ogg; \
		echo $$file to $$endFile; \
		mkdir -p $$endDir; \
		ffmpeg -i $$file -ar 44100 -loglevel error -qscale:a 2 $$endFile -y& \
	done; \
	wait

debugGl:
	rm -f bin/gl.trace
	cmd /c "$(WIN_API_TRACE_DIR)/apitrace trace -o bin/gl.trace -v --api gl bin/$(GAME_NAME).exe"
	cmd /c "$(WIN_API_TRACE_DIR)/qapitrace bin/gl.trace"

clean:
	rm -rf bin
