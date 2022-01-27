#define MAX_HOURS 48     // Maximum "hourly" forecast period, can be up 1 to 48
#define MAX_DAYS 8      // Maximum "daily" forecast periods can be 1 to 8 (Today + 7 days = 8 maximum)

/***************************************************************************************
** Description:   Structure for current weather
***************************************************************************************/
struct Currents {
  // current
  uint32_t dt = 0;
  uint32_t sunrise = 0;
  uint32_t sunset = 0;
  float    temp = 0;
  //float    feels_like = 0;
  //float    pressure = 0;
  //uint8_t  humidity = 0;
  //float    dew_point = 0;
  //uint8_t  clouds = 0;
  //float    uvi = 0;
  //uint32_t visibility = 0;
  float    wind_speed = 0;
  //float    wind_gust = 0;
  //uint16_t wind_deg = 0;
  //float    rain = 0;
  //float    snow = 0;

  // current.weather
  uint16_t id = 0;
  String   main;
  String   description;
  String   icon;

};

/***************************************************************************************
** Description:   Structure for hourly weather
***************************************************************************************/
typedef struct OW_hourly {

  // hourly
  uint32_t dt[MAX_HOURS] = { 0 };
  float    temp[MAX_HOURS] = { 0 };
  float    feels_like[MAX_HOURS] = { 0 };
  float    pressure[MAX_HOURS] = { 0 };
  uint8_t  humidity[MAX_HOURS] = { 0 };
  float    dew_point[MAX_HOURS] = { 0 };
  uint8_t  clouds[MAX_HOURS] = { 0 };
  float    wind_speed[MAX_HOURS] = { 0 };
  float    wind_gust[MAX_HOURS] = { 0 };
  uint16_t wind_deg[MAX_HOURS] = { 0 };
  float    rain[MAX_HOURS] = { 0 };
  float    snow[MAX_HOURS] = { 0 };

  // hourly.weather
  uint16_t id[MAX_HOURS] = { 0 };
  String   main[MAX_HOURS];
  String   description[MAX_HOURS];
  String   icon[MAX_HOURS];
  float    pop[MAX_HOURS];
  float    rain1h[MAX_HOURS];
} OW_hourly;

/***************************************************************************************
** Description:   Structure for daily weather
***************************************************************************************/
typedef struct OW_daily {

  // daily
  uint32_t dt[MAX_DAYS] = { 0 };  // dt
  uint32_t sunrise[MAX_DAYS] = { 0 };
  uint32_t sunset[MAX_DAYS] = { 0 };
  
  // daily.temp
  float    temp_morn[MAX_DAYS] = { 0 };
  float    temp_day[MAX_DAYS] = { 0 };
  float    temp_eve[MAX_DAYS] = { 0 };
  float    temp_night[MAX_DAYS] = { 0 };
  float    temp_min[MAX_DAYS] = { 0 };
  float    temp_max[MAX_DAYS] = { 0 };

  // daily.feels_like
  float    feels_like_morn[MAX_DAYS] = { 0 };
  float    feels_like_day[MAX_DAYS] = { 0 };
  float    feels_like_eve[MAX_DAYS] = { 0 };
  float    feels_like_night[MAX_DAYS] = { 0 };

  // daily
  float    pressure[MAX_DAYS] = { 0 };
  uint8_t  humidity[MAX_DAYS] = { 0 };
  float    dew_point[MAX_DAYS] = { 0 };
  float    wind_speed[MAX_DAYS] = { 0 };
  float    wind_gust[MAX_DAYS] = { 0 };
  uint16_t wind_deg[MAX_DAYS] = { 0 };
  uint8_t  clouds[MAX_DAYS] = { 0 };
  float    uvi[MAX_DAYS] = { 0 };
  uint32_t visibility[MAX_DAYS] = { 0 };

  float    rain[MAX_DAYS] = { 0 };
  float    snow[MAX_DAYS] = { 0 };

  // hourly.weather
  uint16_t id[MAX_DAYS] = { 0 };
  String   main[MAX_DAYS];
  String   description[MAX_DAYS];
  String   icon[MAX_DAYS];
  float    pop[MAX_DAYS];

} OW_daily;
