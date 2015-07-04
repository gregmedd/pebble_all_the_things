#include <pebble.h>

#ifndef COUNTRIES_H
#define COUNTRIES_H

// Country list - used as index into later arrays
enum {
  UNITED_STATES = 0,
  DENMARK,
  CANADA
};

static const char *CountryNames[] = {
  "United States",
  "Denmark",
  "Canada"
};

static const uint32_t CountryMaps[] = {
  RESOURCE_ID_US_MAP,
  RESOURCE_ID_US_MAP,
  RESOURCE_ID_US_MAP
};

static const uint8_t CountryRects[][4] = {
  {0, 68, 144, 90},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

#define GET_COUNTRY_GRECT(country) GRect(CountryRects[country][0], CountryRects[country][1], \
                                        CountryRects[country][2], CountryRects[country][3])

static const unsigned int DefaultCountry = UNITED_STATES;

#endif