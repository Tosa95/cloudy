#ifndef C58909A3_3F8C_4C30_AA88_FEEF8839C95C
#define C58909A3_3F8C_4C30_AA88_FEEF8839C95C

    #include <FS.h>

    namespace DataToFs {

        boolean save(const char *file, void *data, size_t sz, fs::FS &fsRef) {
            char *dptr = (char*)data;

            File fh = fsRef.open(file,"w");

            if (fh){
                for (int i = 0; i < sz; i++) {
                    fh.write(dptr[i]);
                } 
                fh.close();
                return true;
            }else{
                return false;
            } 
        }

        boolean load(const char *file, void *data, size_t sz, fs::FS &fsRef) {
            if (fsRef.exists(file)) {
                char *dptr = (char*)data;

                File fh = fsRef.open(file,"r");

                if (fh.size() != sz) {
                    return false;
                }

                char old[sz];
                memcpy(old, data, sz);

                for (int i = 0; i < sz; i++) {
                    dptr[i] = fh.read();
                }

                fh.close();

                if (dptr[0] != old[0]) {
                    memcpy(data, old, sz);
                    return false;
                } else {
                    return true;
                }

                
            } else {
                return false;
            }
        }

    }

#endif /* C58909A3_3F8C_4C30_AA88_FEEF8839C95C */
