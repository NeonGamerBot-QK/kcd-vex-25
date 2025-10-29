#include "main.h"
// ----------------- CONFIG -----------------
constexpr double WHEEL_DIAMETER_IN = 2.75;   // inches (tracking wheels)
constexpr double TRACK_WIDTH_IN   = 10.0;   // inches between left and right tracking wheels
constexpr double PI = 3.14159265358979323846;
constexpr int    ODOM_LOOP_MS     = 11;     // update rate

static pros::Rotation leftRot(1);
static pros::Rotation rightRot(2);

static double g_x = 0.0;
static double g_y = 0.0;
static double g_theta = 0.0;
static double prevLeftDist = 0.0;
static double prevRightDist = 0.0;

static double degToInches(double deg) {
  return (deg / 360.0) * (PI * WHEEL_DIAMETER_IN);
}


void resetOdometry(double x = 0.0, double y = 0.0, double theta_rad = 0.0) {
  // read current raw encoder positions
  double leftDeg  = leftRot.get_position();
  double rightDeg = rightRot.get_position();

  // convert to inches and set previous readings so first update delta = 0
  prevLeftDist  = degToInches(leftDeg);
  prevRightDist = degToInches(rightDeg);

  // set global pose
  g_x = x;
  g_y = y;
  g_theta = theta_rad;
}

double odom_get_x() { return g_x; }
double odom_get_y() { return g_y; }
double odom_get_theta() { return g_theta; }           // radians
double odom_get_theta_deg() { return g_theta * 180.0 / PI; }

// ---------------- ODOM TASK ----------------
void odom_task_fn(void* /*param*/) {
  // seed prev values on first run if not already seeded
  if (prevLeftDist == 0.0 && prevRightDist == 0.0) {
    double ldeg = leftRot.get_position();
    double rdeg = rightRot.get_position();
    prevLeftDist  = degToInches(ldeg);
    prevRightDist = degToInches(rdeg);
  }

  while (true) {
    // read current encoder positions (degrees -> inches)
    double leftDist  = degToInches(leftRot.get_position());
    double rightDist = degToInches(rightRot.get_position());

    // deltas since last loop
    double dL = leftDist  - prevLeftDist;
    double dR = rightDist - prevRightDist;

    // save for next loop
    prevLeftDist  = leftDist;
    prevRightDist = rightDist;

    // change in heading (radians)
    double dTheta = (dR - dL) / TRACK_WIDTH_IN;

    // distance along robot center
    double dCenter = (dR + dL) / 2.0;

    // use mid-rotation approximation for better accuracy during turns
    double thetaMid = g_theta + dTheta / 2.0;

    // convert robot-forward motion into field frame
    double dxField = dCenter * cos(thetaMid);
    double dyField = dCenter * sin(thetaMid);

    // update global pose
    g_x += dxField;
    g_y += dyField;
    g_theta += dTheta;

    // normalize theta to [-PI, PI] (optional)
    if (g_theta > PI)  g_theta -= 2.0 * PI;
    if (g_theta < -PI) g_theta += 2.0 * PI;

    pros::delay(ODOM_LOOP_MS);
  }
}

// ---------------- START TASK ----------------
// call this from initialize() or competition_initialize()
void startOdometryTask() {
  static pros::Task odomTask(odom_task_fn, nullptr, TASK_PRIORITY_DEFAULT);
}