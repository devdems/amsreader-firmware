/**
 * @copyright Utilitech AS 2023
 * License: Fair Source
 * 
 */

#ifndef _PRICESERVICE_H
#define _PRICESERVICE_H

#include <vector>

#include "TimeLib.h"
#include "Timezone.h"
#include "RemoteDebug.h"
#include "AmsConfiguration.h"
#include "EntsoeA44Parser.h"

#if defined(ESP8266)
	#include <ESP8266HTTPClient.h>
#elif defined(ESP32) // ARDUINO_ARCH_ESP32
	#include <HTTPClient.h>
#else
	#warning "Unsupported board type"
#endif

#define SSL_BUF_SIZE 512

#define PRICE_DIRECTION_IMPORT 0x01
#define PRICE_DIRECTION_EXPORT 0x02
#define PRICE_DIRECTION_BOTH 0x03

#define PRICE_DAY_MO 0x01
#define PRICE_DAY_TU 0x02
#define PRICE_DAY_WE 0x04
#define PRICE_DAY_TH 0x08
#define PRICE_DAY_FR 0x10
#define PRICE_DAY_SA 0x12
#define PRICE_DAY_SU 0x14

#define PRICE_TYPE_FIXED 0x00
#define PRICE_TYPE_ADD 0x01
#define PRICE_TYPE_PCT 0x02

struct PriceConfig {
    char name[32];
    uint8_t direction;
    uint8_t days;
    uint32_t hours;
    uint8_t type;
    uint16_t value;
};

struct PricePart {
    char name[32];
    char description[32];
    uint16_t value;
};

class PriceService {
public:
    PriceService(RemoteDebug*);
    void setup(PriceServiceConfig&);
    bool loop();

    char* getToken();
    char* getCurrency();
    char* getArea();
    char* getSource();
    float getValueForHour(uint8_t direction, int8_t hour);
    float getValueForHour(uint8_t direction, time_t ts, int8_t hour);

    float getEnergyPriceForHour(uint8_t direction, time_t ts, int8_t hour);

    std::vector<PriceConfig>& getPriceConfig();
    void setPriceConfig(uint8_t index, PriceConfig &priceConfig);

    PricePart getPricePart(uint8_t index);

    int16_t getLastError();

    bool load();
    bool save();

private:
    RemoteDebug* debugger;
    PriceServiceConfig* config = NULL;
    HTTPClient* http = NULL;

    uint8_t currentDay = 0, currentHour = 0;
    uint8_t tomorrowFetchMinute = 15; // How many minutes over 13:00 should it fetch prices
    uint8_t nextFetchDelayMinutes = 15;
    uint64_t lastTodayFetch = 0;
    uint64_t lastTomorrowFetch = 0;
    uint64_t lastCurrencyFetch = 0;
    PricesContainer* today = NULL;
    PricesContainer* tomorrow = NULL;

    std::vector<PriceConfig> priceConfig;

    Timezone* tz = NULL;

    static const uint16_t BufferSize = 256;
    char* buf;

    bool hub = false;
    uint8_t* key = NULL;
    uint8_t* auth = NULL;

    float currencyMultiplier = 0;

    int16_t lastError = 0;

    PricesContainer* fetchPrices(time_t);
    bool retrieve(const char* url, Stream* doc);
    float getCurrencyMultiplier(const char* from, const char* to, time_t t);

    void debugPrint(byte *buffer, int start, int length);
};
#endif
