#include "main.h"
rd_view_t *homeview = rd_view_create("Home");
void screen_init() {
    robodash::init();
    robodash::set_active_screen("main");
}