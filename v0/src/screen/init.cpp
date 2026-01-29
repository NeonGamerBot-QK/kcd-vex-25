#include "robodash/api.h"
#include "main.h"
#include "intake/main.hpp"

extern pros::Motor intakeMotor;

double odom_get_x();
double odom_get_y();
double odom_get_theta_deg();

rd_view_t *homeview = rd_view_create("Home");
rd_view_t *logview = rd_view_create("Log");
rd_view_t *selector_view = rd_view_create("Auton Select");
static lv_obj_t *stat_label = nullptr;
static lv_obj_t *map_canvas = nullptr;
static lv_obj_t *position_label = nullptr;
static lv_obj_t *log_ta = nullptr;

int selected_auton = 1;

void screen_log(const char* fmt, ...) {
	if (log_ta == nullptr) return;
	
	va_list args;
	va_start(args, fmt);
	char buf[128];
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	lv_textarea_add_text(log_ta, buf);
	lv_textarea_add_text(log_ta, "\n");
}

static void draw_field_map(lv_obj_t *canvas) {
	lv_canvas_fill_bg(canvas, lv_color_hex(0x404040), LV_OPA_COVER);
	
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.bg_color = lv_color_hex(0x808080);
	rect_dsc.border_color = lv_color_white();
	rect_dsc.border_width = 1;
	
	lv_canvas_draw_rect(canvas, 5, 5, 110, 110, &rect_dsc);
	
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = lv_color_hex(0x606060);
	line_dsc.width = 1;
	
	lv_point_t h_line[] = {{5, 60}, {115, 60}};
	lv_canvas_draw_line(canvas, h_line, 2, &line_dsc);
	lv_point_t v_line[] = {{60, 5}, {60, 115}};
	lv_canvas_draw_line(canvas, v_line, 2, &line_dsc);
}

static void draw_robot_position(lv_obj_t *canvas, double x, double y, double theta) {
	int map_x = 60 + (int)(x * 0.8);
	int map_y = 60 - (int)(y * 0.8);
	
	map_x = (map_x < 5) ? 5 : (map_x > 115) ? 115 : map_x;
	map_y = (map_y < 5) ? 5 : (map_y > 115) ? 115 : map_y;
	
	lv_draw_rect_dsc_t robot_dsc;
	lv_draw_rect_dsc_init(&robot_dsc);
	robot_dsc.bg_color = lv_color_hex(0x00FF00);
	robot_dsc.radius = LV_RADIUS_CIRCLE;
	
	lv_canvas_draw_rect(canvas, map_x - 5, map_y - 5, 10, 10, &robot_dsc);
	
	int dir_x = map_x + (int)(10 * cos(theta * 3.14159 / 180.0));
	int dir_y = map_y - (int)(10 * sin(theta * 3.14159 / 180.0));
	
	lv_draw_line_dsc_t dir_dsc;
	lv_draw_line_dsc_init(&dir_dsc);
	dir_dsc.color = lv_color_hex(0xFF0000);
	dir_dsc.width = 2;
	
	lv_point_t dir_line[] = {{(lv_coord_t)map_x, (lv_coord_t)map_y}, {(lv_coord_t)dir_x, (lv_coord_t)dir_y}};
	lv_canvas_draw_line(canvas, dir_line, 2, &dir_dsc);
}

void update_stats_task(void* param) {
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(120, 120)];
	if (map_canvas != nullptr) {
		lv_canvas_set_buffer(map_canvas, cbuf, 120, 120, LV_IMG_CF_TRUE_COLOR);
	}
	
	while (true) {
		if (stat_label != nullptr) {
			double battery = pros::battery::get_capacity();
			double dt_temp_left = 0.0;
			double intake_temp = intakeMotor.get_temperature();
			
			char buf[256];
			
			#ifdef SHOW_COLOR_ON_SCREEN
			double hue = colorSensor.get_hue();
			snprintf(buf, sizeof(buf), 
				"Batt: %.0f%% DT: %.0fC\nIntk: %.0fC Hue: %.0f\nMode: %s\nSort: %s",
				battery,
				dt_temp_left,
				intake_temp,
				hue,
				pros::competition::is_autonomous() ? "Auto" : "Driver",
				getIntakeModeName()
			);
			#else
			snprintf(buf, sizeof(buf), 
				"Batt: %.0f%% DT: %.0fC\nIntk: %.0fC\nMode: %s\nSort: %s",
				battery,
				dt_temp_left,
				intake_temp,
				pros::competition::is_autonomous() ? "Auto" : "Driver",
				getIntakeModeName()
			);
			#endif
			
			lv_label_set_text(stat_label, buf);
			
			double x = odom_get_x();
			double y = odom_get_y();
			double theta = odom_get_theta_deg();
			
			if (position_label != nullptr) {
				char pos_buf[64];
				snprintf(pos_buf, sizeof(pos_buf), "X:%.1f Y:%.1f H:%.0f", x, y, theta);
				lv_label_set_text(position_label, pos_buf);
			}
			
			if (map_canvas != nullptr) {
				draw_field_map(map_canvas);
				draw_robot_position(map_canvas, x, y, theta);
			}
		}
		pros::delay(200);
	}
}

void screen_deinit() {
	rd_view_del(homeview);
}

// Forward declaration for autonomous function
void autonomous();

// Callback for the "Run Auton" button
static void run_auton_btn_cb(lv_event_t *e) {
	// Only run if not connected to competition control
	if (!pros::competition::is_connected()) {
		// Run autonomous in a separate task to avoid blocking the UI
		pros::Task auton_task([]() {
			autonomous();
		}, "Auton Test");
	}
}

void init_homeview() {
	stat_label = lv_label_create(rd_view_obj(homeview));
	lv_label_set_text(stat_label, "Ready");
	lv_obj_align(stat_label, LV_ALIGN_LEFT_MID, 10, -30);
	
	map_canvas = lv_canvas_create(rd_view_obj(homeview));
	lv_obj_align(map_canvas, LV_ALIGN_RIGHT_MID, -10, 0);
	
	position_label = lv_label_create(rd_view_obj(homeview));
	lv_label_set_text(position_label, "X:0 Y:0 H:0");
	lv_obj_align(position_label, LV_ALIGN_LEFT_MID, 10, 30);
	
	// "Run Auton" button for testing when not in competition
	lv_obj_t *auton_btn = lv_btn_create(rd_view_obj(homeview));
	lv_obj_set_size(auton_btn, 100, 40);
	lv_obj_align(auton_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
	lv_obj_add_event_cb(auton_btn, run_auton_btn_cb, LV_EVENT_CLICKED, nullptr);
	
	lv_obj_t *auton_btn_lbl = lv_label_create(auton_btn);
	lv_label_set_text(auton_btn_lbl, "Run Auton");
	lv_obj_center(auton_btn_lbl);
	
	static pros::Task stats_task(update_stats_task, nullptr, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Stats");
}

void init_logview() {
	log_ta = lv_textarea_create(rd_view_obj(logview));
	lv_obj_set_size(log_ta, 460, 220);
	lv_obj_align(log_ta, LV_ALIGN_CENTER, 0, 0);
	lv_textarea_set_text(log_ta, "System Ready.\n");
	lv_textarea_set_cursor_pos(log_ta, LV_TEXTAREA_CURSOR_LAST);
}

void auton_screen_init() {
	rd_view_alert(homeview, "Auton Mode");
}

void init_auton_selector() {
	lv_obj_t *title = lv_label_create(rd_view_obj(selector_view));
	lv_label_set_text(title, "Auton");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
	
	static const char* names[] = {"Red L", "Red R", "Blue L", "Blue R", "Skills", "None"};
	
	int y_pos = 40;
	for (int i = 0; i < 6; i++) {
		lv_obj_t *btn = lv_btn_create(rd_view_obj(selector_view));
		lv_obj_set_size(btn, 200, 35);
		lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y_pos);
		lv_obj_add_event_cb(btn, [](lv_event_t *e) {
			selected_auton = (int)(intptr_t)lv_event_get_user_data(e);
			rd_view_focus(homeview);
		}, LV_EVENT_CLICKED, (void*)(intptr_t)i);
		
		lv_obj_t *lbl = lv_label_create(btn);
		lv_label_set_text(lbl, names[i]);
		lv_obj_center(lbl);
		
		y_pos += 38;
	}
}

int get_selected_auton() {
	return selected_auton;
}

void screen_init() {
	init_auton_selector();
	init_homeview();
	init_logview();
	rd_view_focus(selector_view);
}
