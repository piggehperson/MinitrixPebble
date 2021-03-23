#include <pebble.h>

// Path defs for OV face on aplite, basalt, diorite
static const GPathInfo PATH_OV_STATUS_LIGHT = {
  .num_points = 8,
  .points = (GPoint []) {
    {12, 0}, {132, 0}, {103, 83}, {103, 85}, {132, 168},
    {12, 168}, {41, 85}, {41, 83}
    }
};
static const GPathInfo PATH_OV_LEFT_MARKING_BG = {
  .num_points = 6,
  .points = (GPoint []) {
    {-1, 0}, {18, 0}, {47, 83}, {47, 85}, {18,168}, {-1, 168}
    }
};
static const GPathInfo PATH_OV_LEFT_MARKING = {
  .num_points = 9,
  .points = (GPoint []) {
    {-1, 0}, {13, 0}, {42, 83}, {42, 85}, {13, 168}, {-1, 168},
    {-1, 137}, {18, 84}, {-1, 31}
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
    {27, 0}, {153, 0}, {118, 88}, {118, 92}, {153, 180},
    {27, 180}, {62, 92}, {62, 88}
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