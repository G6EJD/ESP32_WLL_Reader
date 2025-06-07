/*
  Davis WeatherLink reader using API V2
  Go to weatherlink at https://www.weatherlink.com/ then login with your account credentials for Davis
  Select your account on the right (persion icon)
  Note (or generate first) your API V2 api-key and api-Secret key
  Populate  the crednetials tab in the IDE with your wifi and api key data
  Version-1
  ####################################################################################################################################
  This software, the ideas and concepts is Copyright (c) David Bird 2025. All rights to this software are reserved.

  Any redistribution or reproduction of any part or all of the contents in any form is prohibited other than the following:
  1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
  2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
  3. You may not, except with my express written permission, distribute or commercially exploit the content.
  4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.

  The above copyright ('as annotated') notice and this permission notice shall be included in all copies or substantial portions of the Software and where the
  software use is visible to an end-user.

  THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT. FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY
  OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  See more at http://www.dsbird.org.uk
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include "credentials.h"

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
}

void loop() {
  HTTPClient http;
  Serial.print("\n[HTTP] begin...\n");
  String url = "https://api.weatherlink.com/v2/current/" + device_id + "?api-key=" + api_key;
  http.begin(url);  //HTTP
  Serial.print("[HTTP] GET...\n");
  http.addHeader("X-Api-Secret", api_secret);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String Response = http.getString();
      Serial.println(Response);
      DecodeJson(Response);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  delay(60000 * 2);
}

void DecodeJson(String json) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  JsonObject data0 = doc["sensors"][0]["data"][0];
  JsonObject data1 = doc["sensors"][1]["data"][0];
  JsonObject data2 = doc["sensors"][2]["data"][0];
  JsonObject data3 = doc["sensors"][3]["data"][0];
  // Get all the David WLL 6100 data

  // Json section [0]
  Serial.println();
  Serial.println("JSON Section [0]");
  float temp_in       = ((float)data0["temp_in"] - 32) / 1.8;
  int   tz_offset     = (int)data0["tz_offset"];
  float heat_index_in = ((float)data0["heat_index_in"] - 32) / 1.8;
  float dew_point_in  = ((float)data0["dew_point_in"] - 32) / 1.8;
  float hum_in        = (float)data0["hum_in"];
  Serial.println("   Temperature In: " + String(temp_in, 1) + "'C");
  Serial.println(" Time Zone Offset: " + String(tz_offset) + "-secs");
  Serial.println("    Heat Index In: " + String(heat_index_in, 1) + "'C");
  Serial.println("     Dew Point In: " + String(dew_point_in, 1) + "'C");
  Serial.println("      Humidity In: " + String(hum_in, 1) + "'C");

  // Json section [1] - a selection
  Serial.println();
  Serial.println("JSON Section [1]");
  int battery_voltage = (int)data1["battery_voltage"];
  int wifi_rssi       = (int)data1["wifi_rssi"];
  String ip_address   = data1["ip_v4_address"];
  Serial.println("   Battery Volts: " + String(battery_voltage));
  Serial.println("       WiFi Rssi: " + String(wifi_rssi));
  Serial.println("      IP Address: " + String(ip_address));

// Json section [2] - a selection
  Serial.println();
  Serial.println("JSON Section [2]");
  float bar_absolute  = (float)data2["bar_absolute"] * 33.86389;  // converted to hpa
  float bar_sea_level = (float)data2["bar_sea_level"] * 33.86389; // converted to hpa
  float bar_offset    = (float)data2["bar_offset"] * 33.86389;    // converted to hpa
  float bar_trend     = (float)data2["bar_trend"] * 33.86389;     // converted to hpa
  Serial.println("   Barometer Abs: " + String(bar_absolute, 1) + "hPa");   // Value used by most METO systems
  Serial.println(" Barom Sea level: " + String(bar_sea_level, 1) + "hPa"); // Usual value used
  Serial.println("Barometer Offset: " + String(bar_offset, 2) + "hPa");
  Serial.println(" Barometer Trend: " + String(bar_trend, 2) + "hPa");
    
  // Json section [3]
  Serial.println();
  Serial.println("JSON Section [3]");
  float temp = ((float)data3["temp"] - 32) / 1.8;
  float hum = data3["hum"];
  float pressure = (float)data2["bar_absolute"] * 33.86389;
  float pressureSL = (float)data2["bar_sea_level"] * 33.86389;
  float pressure_trend = (float)data3["bar_trend"] * 33.86389;
  float wind_dir = data3["wind_dir_last"];
  float wind_speed_hi = data3["wind_speed_hi_last_2_min"];
  float wind_chill = ((float)data3["wind_chill"] - 32) / 1.8;
  float thw_index = ((float)data3["thw_index"] - 32) / 1.8;
  float thsw_index = ((float)data3["thsw_index"] - 32) / 1.8;
  float heat_index = ((float)data3["heat_index"] - 32) / 1.8;
  float wet_bulb = ((float)data3["wet_bulb"] - 32) / 1.8;
  float dew_point = ((float)data3["dew_point"] - 32) / 1.8;
  float rainfall_daily = data3["rainfall_daily_mm"];
  float rainfall_monthly = data3["rainfall_monthly_mm"];
  float rainfall_yearly = data3["rainfall_year_mm"];
  float rain_rate = data3["rain_rate_hi_mm"];
  float solar_rad = data3["solar_rad"];
  Serial.println("      Temperature: " + String(temp, 1) + "'C");
  Serial.println("         Humidity: " + String(hum, 0) + "%");
  Serial.println("         Pressure: " + String(pressure, 1) + "hPa");   // Value used by most METO systems
  Serial.println("       PressureSL: " + String(pressureSL, 1) + "hPa"); // Usual value used
  Serial.println("   Pressure Trend: " + String(pressure_trend, 2) + "hPa");
  Serial.println("        Dew Point: " + String(dew_point, 1) + "'C");
  Serial.println("       Wind Speed: " + String(wind_speed_hi, 1) + "mph");
  Serial.println("         Wind Dir: " + String(wind_dir));
  Serial.println("       Wind Chill: " + String(wind_chill, 1) + "'C");
  Serial.println("        THW Index: " + String(thw_index, 1) + "'C");
  Serial.println("       THSW Index: " + String(thsw_index, 1) + "'C");
  Serial.println("       Heat Index: " + String(heat_index, 1) + "'C");
  Serial.println("         Wet Bulb: " + String(wet_bulb,1) + "'C");
  Serial.println("  Rain Fall Daily: " + String(rainfall_daily, 1) + "mm");
  Serial.println("Rain Fall Monthly: " + String(rainfall_monthly, 1) + "mm");
  Serial.println(" Rain Fall Yearly: " + String(rainfall_yearly, 1) + "mm");
  Serial.println("        Rain Rate: " + String(rain_rate, 1) + "mm");
  Serial.println("  Solar Radiation: " + String(solar_rad, 0) + "W/M");
}

/*
{
  "station_id_uuid": "0838fcc9-de45-43cf-87eb-33368be02a87",
  "sensors": [
    {
      "lsid": 414278,
      "data": [
        {
          "temp_in": 74.3,
          "tz_offset": 3600,
          "heat_index_in": 74.1,
          "dew_point_in": 53.1,
          "ts": 1749292246,
          "hum_in": 47.5
        }
      ],
      "sensor_type": 243,
      "data_structure_type": 12
    },
    {
      "lsid": 414276,
      "data": [
        {
          "battery_voltage": 4697,
          "wifi_rssi": -70,
          "network_error": null,
          "ip_v4_gateway": "192.168.0.1",
          "bluetooth_version": null,
          "bgn": null,
          "firmware_version": 1636079858,
          "tz_offset": 3600,
          "local_api_queries": 54910,
          "rx_bytes": 10185677,
          "health_version": 1,
          "radio_version": 621020416,
          "ip_address_type": 1,
          "link_uptime": 760718,
          "input_voltage": 4637,
          "tx_bytes": 203308430,
          "ip_v4_netmask": "255.255.255.0",
          "rapid_records_sent": 323000,
          "uptime": 850642,
          "touchpad_wakeups": 78,
          "ip_v4_address": "192.168.0.101",
          "bootloader_version": 1550707628,
          "espressif_version": 1534381024,
          "dns_type_used": null,
          "network_type": 1,
          "ts": 1749292200
        }
      ],
      "sensor_type": 504,
      "data_structure_type": 15
    },
    {
      "lsid": 414277,
      "data": [
        {
          "bar_absolute": 29.591,
          "tz_offset": 3600,
          "bar_sea_level": 29.735,
          "bar_offset": -0.001,
          "bar_trend": 0.002,
          "ts": 1749292246
        }
      ],
      "sensor_type": 242,
      "data_structure_type": 12
    },
    {
      "lsid": 414281,
      "data": [
        {
          "rx_state": 0,
          "wind_speed_hi_last_2_min": 5,
          "hum": 86.8,
          "wind_dir_at_hi_speed_last_10_min": 180,
          "wind_chill": 59.4,
          "rain_rate_hi_last_15_min_clicks": 0,
          "thw_index": 59.4,
          "wind_dir_scalar_avg_last_10_min": 233,
          "rain_size": 2,
          "uv_index": null,
          "tz_offset": 3600,
          "wind_speed_last": 6,
          "rainfall_last_60_min_clicks": 0,
          "wet_bulb": 56.9,
          "rainfall_monthly_clicks": 89,
          "wind_speed_avg_last_10_min": 1.62,
          "wind_dir_at_hi_speed_last_2_min": 254,
          "rainfall_daily_in": 0.09448819,
          "wind_dir_last": 220,
          "rainfall_daily_mm": 2.4,
          "rain_storm_last_clicks": 32,
          "tx_id": 1,
          "rain_storm_last_start_at": 1748929980,
          "rain_rate_hi_clicks": 0,
          "rainfall_last_15_min_in": 0,
          "rainfall_daily_clicks": 12,
          "dew_point": 55.5,
          "rainfall_last_15_min_mm": 0,
          "rain_rate_hi_in": 0,
          "rain_storm_clicks": 56,
          "rain_rate_hi_mm": 0,
          "rainfall_year_clicks": 1076,
          "rain_storm_in": 0.44094488,
          "rain_storm_last_end_at": 1749034861,
          "rain_storm_mm": 11.2,
          "wind_dir_scalar_avg_last_2_min": 251,
          "heat_index": 59.4,
          "rainfall_last_24_hr_in": 0.09448819,
          "rainfall_last_60_min_mm": 0,
          "trans_battery_flag": 0,
          "rainfall_last_60_min_in": 0,
          "rain_storm_start_time": 1749101221,
          "rainfall_last_24_hr_mm": 2.4,
          "rainfall_year_in": 8.472441,
          "wind_speed_hi_last_10_min": 9,
          "rainfall_last_15_min_clicks": 0,
          "rainfall_year_mm": 215.2,
          "wind_dir_scalar_avg_last_1_min": 258,
          "temp": 59.4,
          "wind_speed_avg_last_2_min": 2.62,
          "solar_rad": 253,
          "rainfall_monthly_mm": 17.8,
          "rain_storm_last_mm": 6.4,
          "wind_speed_avg_last_1_min": 2.56,
          "thsw_index": 62.3,
          "rainfall_monthly_in": 0.7007874,
          "rain_rate_last_mm": 0,
          "rain_rate_last_clicks": 0,
          "rainfall_last_24_hr_clicks": 12,
          "rain_storm_last_in": 0.2519685,
          "rain_rate_last_in": 0,
          "rain_rate_hi_last_15_min_mm": 0,
          "rain_rate_hi_last_15_min_in": 0,
          "ts": 1749292246
        }
      ],
      "sensor_type": 43,
      "data_structure_type": 10
    }
  ],
  "generated_at": 1749292783,
  "station_id": 112555
}
*/
