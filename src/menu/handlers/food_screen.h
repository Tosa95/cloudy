#ifndef B3D5B1B8_03B7_4FA6_B48C_2A01E4F8EF1D
#define B3D5B1B8_03B7_4FA6_B48C_2A01E4F8EF1D

#include <functional>

#include "../../sensors/all_sensors.h"
#include "../../settings/settings.h"
#include "screen_handler.h"
#include "utils.h"

namespace Menu {

const int rowH = 16;

enum FoodSubScreen : int { TABLE = 0, START = 1, DISTRIBUTION = 2 };

class FoodTableScreenHandler : public ScreenHandler {
   private:
    int foodN;

    std::function<void(FoodSubScreen)> setSubScreen;

    int getCellColumn(int i) { return i % 3; }

    int getCellRow(int i) { return i / 3; }

    int getCellX(int column) {
        switch (column) {
            case 0:
                return 0;
            case 1:
                return 42;
            case 2:
                return 86;
        }
        return 0;
    }

    int getCellWidth(int column) {
        switch (column) {
            case 0:
                return 42;
            case 1:
                return 44;
            case 2:
                return 42;
        }
        return 0;
    }

    int getCellY(int row) { return row * rowH; }

    int getCellHeight(int row) { return rowH; }

   public:
    FoodTableScreenHandler(int foodN) : foodN(foodN) {}

    virtual void handleClick(MenuState &state) {
        if (state.focus < 1 + settings.foodData[foodN].nPeople) {
            if (state.selected) {
                saveSettings();
            }
            state.selected = !state.selected;
        }

        if (state.focus == settings.foodData[foodN].nPeople + 1) {
            setSubScreen(FoodSubScreen::START);
        }

        if (state.focus == settings.foodData[foodN].nPeople + 2) {
            setSubScreen(FoodSubScreen::DISTRIBUTION);
        }
    }

    virtual void handleIncDec(Event &e, MenuState &state) {
        if (state.focus == 0) {
            settings.foodData[foodN].nPeople += (int)e;
            cap(settings.foodData[foodN].nPeople, 1, MAX_PEOPLE_PER_FOOD);
        }

        if (state.focus > 0 &&
            state.focus < settings.foodData[foodN].nPeople + 1) {
            switch (e) {
                case Event::DECREMENT:
                    settings.foodData[foodN].weights[state.focus - 1] -= 1;
                    break;
                case Event::INCREMENT:
                    settings.foodData[foodN].weights[state.focus - 1] += 1;
                    break;
            }
        }
    }
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        int nPeople = settings.foodData[foodN].nPeople;
        int *weights = settings.foodData[foodN].weights;

        g.setFontSize(1);
        sprintf(buffer, "FOOD%d", foodN + 1);
        g.drawStringCentered(0, 1, 48, rowH, buffer);

        sprintf(buffer, "people: %d", nPeople);
        drawStringWRect(0, buffer, 48, 1, WIDTH - 48, rowH, g, state);

        if (state.focus >= 1 && state.focus <= nPeople) {
            g.setFontSize(2);
            sprintf(buffer, "P%d: %dg", state.focus, weights[state.focus - 1]);
            drawStringWRect(state.focus, buffer, 0, rowH, WIDTH, rowH * 2, g,
                            state);
            g.setFontSize(1);
        } else {
            for (int i = 0; i < nPeople; i++) {
                int column = getCellColumn(i);
                int row = getCellRow(i);

                sprintf(buffer, "P%d:%d", i + 1, weights[i]);
                drawStringWRect(i + 1, buffer, getCellX(column),
                                getCellY(row) + rowH + 1, getCellWidth(column),
                                getCellHeight(column), g, state);
            }
        }

        drawStringWRect(nPeople + 1, "START", 0, 3 * rowH + 1, 64, rowH, g,
                        state);
        drawStringWRect(nPeople + 2, "DIST", 64, 3 * rowH + 1, 64, rowH, g,
                        state);
    }

    virtual int getNSelections(MenuState &state) {
        return 1 + settings.foodData[foodN].nPeople + 2;
    }

    void setSetSubScreen(std::function<void(FoodSubScreen)> setSubScreen) {
        this->setSubScreen = setSubScreen;
    }
};

class StartFoodScreenHandler : public ScreenHandler {
   private:
    int foodN;
    std::function<void(FoodSubScreen)> setSubScreen;

   public:
    StartFoodScreenHandler(int foodN) : foodN(foodN) {}

    virtual void handleClick(MenuState &state) {
        setSubScreen(FoodSubScreen::TABLE);
    }

    virtual void handleIncDec(Event &e, MenuState &state) {}

    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        float currentWeight = Sensors::compoundStrainGauge.getWeight();
        float totalWeight = settings.foodData[foodN].getTotalWeight();

        if (currentWeight >= totalWeight) {
            g.setColor(WHITE);
            g.fillRect(0, 0, WIDTH, HEIGHT);
            g.setColor(BLACK);
        }

        sprintf(buffer, "FOOD%d --> START", foodN + 1);
        g.drawStringCentered(0, 1, WIDTH, rowH, buffer);

        g.setFontSize(2);

        sprintf(buffer, "%3.0f/%3.0f", currentWeight, totalWeight);
        g.drawStringCentered(0, rowH, WIDTH, rowH * 2, buffer);

        g.setFontSize(1);

        drawStringWRect(0, "OK", 0, HEIGHT - rowH, WIDTH, rowH, g, state);

        g.setColor(WHITE);
    }

    virtual int getNSelections(MenuState &state) { return 1; }

    void setSetSubScreen(std::function<void(FoodSubScreen)> setSubScreen) {
        this->setSubScreen = setSubScreen;
    }
};

class DistributionFoodScreenHandler : public ScreenHandler {
   private:
    int foodN;
    std::function<void(FoodSubScreen)> setSubScreen;
    float initialWeight = 0;
    float stageWeight = 0;
    int person = -1;
    float conversionFactor = 0;

    float getPersonWeightOriginal() {
        return settings.foodData[foodN].weights[person];
    }

    float getConversionFactor() {
        float total = settings.foodData[foodN].getTotalWeight(person);
        return stageWeight / total;
    }

    float getConversionFactorReal() {
        float total = settings.foodData[foodN].getTotalWeight(0);
        return initialWeight / total;
    }

    float getPersonWeightProportional() {
        return getConversionFactor() * settings.foodData[foodN].weights[person];
    }

   public:
    DistributionFoodScreenHandler(int foodN) : foodN(foodN) {}

    virtual void handleClick(MenuState &state) {
        if (person == -1) {
            initialWeight = Sensors::compoundStrainGauge.getWeight();
        }
        stageWeight = Sensors::compoundStrainGauge.getWeight();

        person++;

        if (person >= settings.foodData[foodN].nPeople) {
            person = -1;
            setSubScreen(FoodSubScreen::TABLE);
        }
    }
    virtual void handleIncDec(Event &e, MenuState &state) {}
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        float weight = Sensors::compoundStrainGauge.getWeight();
        float deliveredWeight = stageWeight - weight;
        float conversionFactor = getConversionFactorReal();
        float originalWeight = weight / conversionFactor;

        if (deliveredWeight >= getPersonWeightProportional() && person >= 0) {
            g.setColor(WHITE);
            g.fillRect(0, 0, WIDTH, HEIGHT);
            g.setColor(BLACK);
        }

        g.setFontSize(1);

        if (person >= 0) {
            sprintf(buffer, "FOOD%d --> P%d (%.0fg)", foodN + 1, person + 1,
                    getPersonWeightOriginal());
        } else {
            sprintf(buffer, "FOOD%d --> DISTR", foodN + 1);
        }

        g.drawStringCentered(0, 1, WIDTH, rowH, buffer);

        g.setFontSize(2);

        if (person == -1) {
            sprintf(buffer, "%.2fg", weight);
        } else {
            sprintf(buffer, "%3.0f/%3.0f", deliveredWeight,
                    getPersonWeightProportional());
        }
        g.drawStringCentered(0, rowH, WIDTH, rowH * 2, buffer);

        // if (person >= 0) {
        //     sprintf(buffer, "RAW: %.1fg / %.1fg",
        //             stageWeight / conversionFactor - originalWeight,
        //             getPersonWeightOriginal());
        //     g.drawStringCentered(0, rowH * 2, WIDTH, rowH, buffer);
        // }

        g.setFontSize(1);

        if (person == -1) {
            sprintf(buffer, "BEGIN");
        } else if (person >= 0 &&
                   person < settings.foodData[foodN].nPeople - 1) {
            sprintf(buffer, "NEXT");
        } else {
            sprintf(buffer, "END");
        }

        drawStringWRect(0, buffer, 0, HEIGHT - rowH, WIDTH, rowH, g, state);

        g.setColor(WHITE);
    }

    virtual int getNSelections(MenuState &state) { return 1; }

    void setSetSubScreen(std::function<void(FoodSubScreen)> setSubScreen) {
        this->setSubScreen = setSubScreen;
    }
};

class FoodScreenHandler : public ScreenHandler {
   private:
    FoodTableScreenHandler table;
    StartFoodScreenHandler start;
    DistributionFoodScreenHandler distribution;

    ScreenHandler *current;

   public:
    FoodScreenHandler(int foodN)
        : table(foodN), start(foodN), distribution(foodN) {
        std::function<void(FoodSubScreen)> setSubScreen =
            [=](FoodSubScreen dest) {
                switch (dest) {
                    case FoodSubScreen::TABLE:
                        current = &table;
                        break;
                    case FoodSubScreen::START:
                        current = &start;
                        break;
                    case FoodSubScreen::DISTRIBUTION:
                        current = &distribution;
                        break;
                }
            };

        current = &table;

        table.setSetSubScreen(setSubScreen);
        start.setSetSubScreen(setSubScreen);
        distribution.setSetSubScreen(setSubScreen);
    }

    virtual void handleClick(MenuState &state) { current->handleClick(state); }
    virtual void handleIncDec(Event &e, MenuState &state) {
        current->handleIncDec(e, state);
    }
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        current->draw(g, state);
    }

    virtual int getNSelections(MenuState &state) {
        return current->getNSelections(state);
    }
};

}  // namespace Menu

#endif /* B3D5B1B8_03B7_4FA6_B48C_2A01E4F8EF1D */
