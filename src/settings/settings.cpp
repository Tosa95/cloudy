#include "settings.h"

#include "data_to_fs.h"
#include "fs/filesystem.h"

Settings settings;

void saveSettings() {
    DataToFs::save("/settings.bin", &settings, sizeof(settings),
                   *FileSystem::mainFS);
}

void loadSettings() {
    DataToFs::load("/settings.bin", &settings, sizeof(settings),
                   *FileSystem::mainFS);
}