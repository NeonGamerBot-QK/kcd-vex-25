#include "robodash/api.h"
#include "main.h"

extern pros::Motor intakeMotor;

double odom_get_x();
double odom_get_y();
double odom_get_theta_deg();

rd_view_t *homeview = rd_view_create("Home");
rd_view_t *selector_view = rd_view_create("Auton Select");
static lv_obj_t *stat_label = nullptr;
static lv_obj_t *map_canvas = nullptr;
static lv_obj_t *position_label = nullptr;

int selected_auton = 0;

static void draw_field_map(lv_obj_t *canvas) {
	lv_canvas_fill_bg(canvas, lv_color_hex(0x404040), LV_OPA_COVER);
	
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.bg_color = lv_color_hex(0x808080);
	rect_dsc.border_color = lv_color_white();
	rect_dsc.border_width = 2;
	
	lv_canvas_draw_rect(canvas, 5, 5, 200, 200, &rect_dsc);
	
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = lv_color_hex(0x606060);
	line_dsc.width = 1;
	
	lv_point_t h_line[] = {{5, 105}, {205, 105}};
	lv_canvas_draw_line(canvas, h_line, 2, &line_dsc);
	lv_point_t v_line[] = {{105, 5}, {105, 205}};
	lv_canvas_draw_line(canvas, v_line, 2, &line_dsc);
}

static void draw_robot_position(lv_obj_t *canvas, double x, double y, double theta) {
	int map_x = 105 + (int)(x * 1.5);
	int map_y = 105 - (int)(y * 1.5);
	
	map_x = (map_x < 5) ? 5 : (map_x > 205) ? 205 : map_x;
	map_y = (map_y < 5) ? 5 : (map_y > 205) ? 205 : map_y;
	
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
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(210, 210)];
	if (map_canvas != nullptr) {
		lv_canvas_set_buffer(map_canvas, cbuf, 210, 210, LV_IMG_CF_TRUE_COLOR);
	}
	
	while (true) {
		if (stat_label != nullptr) {
			double battery = pros::battery::get_capacity();
			double dt_temp_left = 0.0;
			double intake_temp = intakeMotor.get_temperature();
			
			char buf[256];
			snprintf(buf, sizeof(buf), 
				"Battery: %.0f%%\n"
				"DT Temp: %.0fC\n"
				"Intake: %.0fC\n"
				"Mode: %s",
				battery,
				dt_temp_left,
				intake_temp,
				pros::competition::is_autonomous() ? "Auto" : "Driver"
			);
			lv_label_set_text(stat_label, buf);
			
			double x = odom_get_x();
			double y = odom_get_y();
			double theta = odom_get_theta_deg();
			
			if (position_label != nullptr) {
				char pos_buf[128];
				snprintf(pos_buf, sizeof(pos_buf), "X: %.1f  Y: %.1f\nHeading: %.0f°", x, y, theta);
				lv_label_set_text(position_label, pos_buf);
			}
			
			if (map_canvas != nullptr) {
				draw_field_map(map_canvas);
				draw_robot_position(map_canvas, x, y, theta);
			}
		}
		pros::delay(100);
	}
}

void screen_deinit() {
	rd_view_del(homeview);
}

void init_homeview() {
	stat_label = lv_label_create(rd_view_obj(homeview));
	lv_label_set_text(stat_label, "Waiting for Competition...");
	lv_obj_align(stat_label, LV_ALIGN_LEFT_MID, 10, -30);
	
	map_canvas = lv_canvas_create(rd_view_obj(homeview));
	lv_obj_align(map_canvas, LV_ALIGN_RIGHT_MID, -10, 0);
	
	position_label = lv_label_create(rd_view_obj(homeview));
	lv_label_set_text(position_label, "X: 0.0  Y: 0.0\nHeading: 0°");
	lv_obj_align(position_label, LV_ALIGN_LEFT_MID, 10, 30);
	
	static pros::Task stats_task(update_stats_task, nullptr, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Stats");
}

void auton_screen_init() {
	rd_view_alert(homeview, "Auton Mode");
}

void init_auton_selector() {
	rd_view_t *selector = selector_view;
	
	lv_obj_t *selector_label = lv_label_create(rd_view_obj(selector));
	lv_label_set_text(selector_label, "Select Autonomous:");
	lv_obj_align(selector_label, LV_ALIGN_TOP_MID, 0, 20);
	
	const char* auton_names[] = {
		"Move forward", 
		"Do nothing",
		"Blue side",
		"Red side",
		"Skills!"
	};
	
	lv_obj_t *btn_container = lv_obj_create(rd_view_obj(selector));
	lv_obj_set_size(btn_container, 400, 180);
	lv_obj_align(btn_container, LV_ALIGN_CENTER, 0, 20);
	lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	
	for (int i = 0; i < 6; i++) {
		lv_obj_t *btn = lv_btn_create(btn_container);
		lv_obj_set_size(btn, 180, 50);
		lv_obj_add_event_cb(btn, [](lv_event_t *e) {
			int auton_idx = (int)(intptr_t)lv_event_get_user_data(e);
			selected_auton = auton_idx;
			rd_view_focus(homeview);
		}, LV_EVENT_CLICKED, (void*)(intptr_t)i);
		
		lv_obj_t *label = lv_label_create(btn);
		lv_label_set_text(label, auton_names[i]);
		lv_obj_center(label);
	}
	
	lv_obj_t *selected_label = lv_label_create(rd_view_obj(selector));
	lv_label_set_text(selected_label, "Current: None");
	lv_obj_align(selected_label, LV_ALIGN_BOTTOM_MID, 0, -10);
}

int get_selected_auton() {
	return selected_auton;
}

void screen_init() {
	init_auton_selector();
	init_homeview();
	rd_view_focus(selector_view);
}
