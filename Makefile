CC = gcc
TARGET = PacmanSDL.exe # nom de l'executable

# ici t'as les chemins
SRC_DIR = src
BUILD_DIR = bin
INCLUDE_DIR = include
LIB_DIR = lib

# chemin du main
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/map.c $(SRC_DIR)/pacman.c $(SRC_DIR)/ghost.c $(SRC_DIR)/audio.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

SDL_LIBS = -lmingw32 -lSDL2main -lSDL2 # les biblio qu'on intègre au projet

# --- Flags de Compilation ---
CFLAGS = -Wall -g -I$(INCLUDE_DIR)
LDFLAGS = -L$(LIB_DIR) $(SDL_LIBS) -Wl,-subsystem,windows

# le fonctionnement de la compilation
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR): # créer dossier
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS) # pour tout lier
	$(CC) $(OBJECTS) -o $(BUILD_DIR)/$@ $(LDFLAGS)
	cp $(LIB_DIR)/SDL2.dll $(BUILD_DIR)/

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c #compile chaque fichier dans source en objet
	$(CC) $(CFLAGS) -c $< -o $@

clean: # nettoie la mémoire
	rm -f $(BUILD_DIR)/*.o $(TARGET)
	rm -rf $(BUILD_DIR)

run: all # tu éxcute l'app
	./bin/$(TARGET)

package: all
	powershell -ExecutionPolicy Bypass -File build_release.ps1

.PHONY: all clean run