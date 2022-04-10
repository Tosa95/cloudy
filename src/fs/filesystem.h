#ifndef C899840E_6858_4989_9239_5C23496393E8
#define C899840E_6858_4989_9239_5C23496393E8

    #include <FS.h>

    namespace FileSystem {
        extern fs::FS *mainFS;
        extern fs::FS *externalFS;

        void init();
    }

#endif /* C899840E_6858_4989_9239_5C23496393E8 */
