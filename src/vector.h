#pragma once

typedef struct LatLon {
  /** Degrees north (+90) to south (-90) */
  float lat;

  /** Degrees east (+180) to west (-180) */
  float lon;
} LatLon;

typedef union Vec2 {
  struct {
    float x;
    float y;
  };
  float data[2];
} Vec2;

typedef union Vec3 {
  struct {
    float x;
    float y;
    float z;
  };
  float data[3];
} Vec3;
