#include "robodash/api.h"
#include "main.h"

// rd_view_t *homeview;
rd_view_t *homeview = rd_view_create("Home");
void screen_init() {
    // lv_init();
    // init_homeview();
}
void screen_deinit() {
    rd_view_del(homeview);
}

void init_homeview() {
    rd_view_alert(homeview, "Hello from RoboDash!");
}