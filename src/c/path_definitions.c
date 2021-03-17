#include <pebble.h>

// Path defs for OV face on aplite, basalt, diorite
static const GPathInfo PATH_OV_STATUS_LIGHT = {
  .num_points = 8,
  .points = (GPoint []) {
    {14, 0}, {130, 0}, {101, 83}, {101, 85}, {130, 168},
    {14, 168}, {43, 85}, {43, 83}
    }
};
static const GPathInfo PATH_OV_LEFT_MARKING_BG = {
  .num_points = 6,
  .points = (GPoint []) {
    {0, 0}, {18, 0}, {47, 83}, {47, 85}, {18,168}, {0, 168}
    }
};
static const GPathInfo PATH_OV_LEFT_MARKING = {
  .num_points = 9,
  .points = (GPoint []) {
    {0, 0}, {14, 0}, {43, 83}, {43, 85}, {14, 168}, {0, 168},
    {0, 137}, {19, 84}, {0, 31}
    }
};
static const GPathInfo PATH_OV_RIGHT_MARKING_BG = {
  .num_points = 6,
  .points = (GPoint []) {
    {126, 0}, {144, 0}, {144, 168}, {126, 168}, {97, 85}, {97, 83}
    }
};
static const GPathInfo PATH_OV_RIGHT_MARKING = {
  .num_points = 9,
  .points = (GPoint []) {
    {130, 0}, {144, 0}, {144, 31}, {125, 84}, {144, 137}, {144, 168},
    {130, 168}, {101, 85}, {101, 83}
    }
};

// Path defs for OV face on chalk
static const GPathInfo PATH_OV_STATUS_LIGHT_ROUND = {
  .num_points = 14,
  .points = (GPoint []) {
    {47, 35}, {65, 24}, {90, 20}, {114, 24}, {133, 35},
    {112, 88}, {112, 92}, {134, 144}, {114, 156}, {90, 160},
    {65, 155}, {46, 144}, {68, 92}, {68, 88}
    }
};

static const GPathInfo PATH_OV_LEFT_MARKING_BG_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {3, 0}, {33, 0}, {68, 88}, {68, 92}, {33, 180},
    {3, 180}, {39, 90}
    }
};

static const GPathInfo PATH_OV_LEFT_MARKING_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {7, 0}, {29, 0}, {64, 88}, {64, 92}, {29, 180},
    {7, 180}, {43, 90}
    }
};

static const GPathInfo PATH_OV_RIGHT_MARKING_BG_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {147, 0}, {177, 0}, {141, 90}, {177, 180}, {147, 180},
    {112, 92}, {112, 88}
    }
};

static const GPathInfo PATH_OV_RIGHT_MARKING_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {151, 0}, {173, 0}, {137, 90}, {173, 180}, {151, 180},
    {116, 92}, {116, 88}
    }
};

// Path defs for classic face on aplite, basalt, diorite
static const GPathInfo PATH_CLASSIC_STATUS_LIGHT = {
  .num_points = 8,
  .points = (GPoint []) {
    {24, 35}, {72, 14}, {120, 36}, {98, 84}, {121, 132},
    {72, 154}, {24, 132}, {46, 84}
    }
};

static const GPathInfo PATH_CLASSIC_LEFT_CARET = {
  .num_points = 7,
  .points = (GPoint []) {
    {21, 37}, {32, 35}, {53, 83}, {53, 85}, {32, 133},
    {22, 131}, {41, 84}
    }
};

static const GPathInfo PATH_CLASSIC_RIGHT_CARET = {
  .num_points = 7,
  .points = (GPoint []) {
    {112, 35}, {123, 37}, {103, 84}, {123, 131}, {112, 133},
    {91, 85}, {91, 83}
    }
};

// Path defs for classic face on chalk
static const GPathInfo PATH_CLASSIC_STATUS_LIGHT_ROUND = {
  .num_points = 8,
  .points = (GPoint []) {
    {29, 0}, {151, 0}, {116, 88}, {116, 92}, {151, 180},
    {29, 180}, {64, 92}, {64, 88}
    }
};

static const GPathInfo PATH_CLASSIC_LEFT_CARET_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {7, 0}, {29, 0}, {64, 88}, {64, 92},{29, 180},
    {7, 180}, {43, 90}
    }
};

static const GPathInfo PATH_CLASSIC_RIGHT_CARET_ROUND = {
  .num_points = 7,
  .points = (GPoint []) {
    {151, 0}, {173, 0}, {137, 90}, {173, 180}, {151, 180},
    {116, 92}, {116, 88}
    }
};