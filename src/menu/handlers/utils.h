#ifndef E0ECB3AB_90E9_4161_BD7B_366C5C439176
#define E0ECB3AB_90E9_4161_BD7B_366C5C439176

namespace Menu {
    void tareAndSave();
    void calibrateAndSave(float realWeight);

    template<class T>
    void cap(T &x, T min, T max) {
        if (x < min) {
            x = max;
        }

        if (x > max) {
            x = min;
        }
    }
}

#endif /* E0ECB3AB_90E9_4161_BD7B_366C5C439176 */
