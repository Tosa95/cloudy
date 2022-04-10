#ifndef DD27DC84_F003_437D_A78D_0AFF9DBDF234
#define DD27DC84_F003_437D_A78D_0AFF9DBDF234

#include "../../../lib/gui/graphics/ssd1306_graphics.h"
#include "../gfx_utils.h"
#include "../gui_setup.h"
#include "../menu_event.h"
#include "../menu_state.h"

namespace Menu {

class ScreenHandler {
   public:
    virtual void handleClick(MenuState &state) = 0;
    virtual void handleIncDec(Event &e, MenuState &state) = 0;
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) = 0;
    virtual int getNSelections(MenuState &state) = 0;
    virtual ~ScreenHandler(){};
};

}  // namespace Menu

#endif /* DD27DC84_F003_437D_A78D_0AFF9DBDF234 */
