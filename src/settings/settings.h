#ifndef E49F14F1_5768_4527_8A2D_6B2D71AD744A
#define E49F14F1_5768_4527_8A2D_6B2D71AD744A

    #include <string.h>
    #include <stdint.h>
    #include <Arduino.h>

    #define CONTAINER_ID_SIZE 16
    #define MAX_CONTAINERS 100
    #define N_FOODS 3
    #define MAX_PEOPLE_PER_FOOD 6

    struct StrainGauge {
        int64_t offset = 1413002;
        float scale = 540700.0 / 322.0;
    };

    struct ContainerData {
        char id[CONTAINER_ID_SIZE];
        float weight;

        ContainerData(const char *id, float weight): weight(weight) {
            strcmp(this->id, id);
        }

        ContainerData() {
            setFree();
        }

        void setFree() {
            id[0] = '\0';
        }

        void setId(const char *id) {
            strcpy(this->id, id);
        }

        bool isFree() {
            return id[0] == '\0';
        }
    };

    struct Containers {
        ContainerData data[MAX_CONTAINERS];

        Containers() {
            for (int i = 0; i < MAX_CONTAINERS; i++) {
                data[i].setFree();
            }
        }

        bool hasContainer(const char *id) {
            return getContainer(id) != nullptr;
        }

        ContainerData *getContainer(const char *id) {
            for (int i = 0; i < MAX_CONTAINERS; i++) {
                if (strcmp(data[i].id, id) == 0) {
                    return &(data[i]);
                }
            }

            return nullptr;
        }

        ContainerData *getFreeContainer() {
            for (int i = 0; i < MAX_CONTAINERS; i++) {
                if (data[i].id[0] == '\0') {
                    return &(data[i]);
                }
            }

            return nullptr;
        }
    };

    struct FoodData {
        int nPeople = 2;
        int weights[MAX_PEOPLE_PER_FOOD];

        float getTotalWeight(int fromPerson = 0) {
            float res = 0;
            for (int i = fromPerson; i < nPeople; i++) {
                res += weights[i];
            }
            return res;
        }
    };

    struct Settings {

        char version = 2;

        StrainGauge strainGauge;
        Containers containers;
        FoodData foodData[N_FOODS];

        Settings() {
            for (int i = 0; i < N_FOODS; i++) {
                for (int j = 0; j < MAX_PEOPLE_PER_FOOD; j++) {
                    foodData[i].weights[j] = 100;
                }
            }
        }

    };

    extern Settings settings;

    void saveSettings();

    void loadSettings();

#endif /* E49F14F1_5768_4527_8A2D_6B2D71AD744A */
