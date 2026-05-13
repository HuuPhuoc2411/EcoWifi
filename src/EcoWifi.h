#ifndef ECO_WIFI_H
#define ECO_WIFI_H

#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
#else
  #error "This library only supports ESP8266 and ESP32"
#endif

#include <DNSServer.h>
#include <EEPROM.h>

class EcoWifiClass {
public:
    EcoWifiClass();

    // Cài đặt tên và mật khẩu cho điểm phát WiFi (Access Point) 
    // Dùng khi thiết bị vào chế độ cài đặt. Phải gọi trước khi init().
    void setAPCredentials(const char* ssid, const char* password);

    // Thêm một WiFi mặc định vào danh sách lưu trữ (Tuỳ chọn).
    // Có thể gọi nhiều lần nếu muốn lưu nhiều WiFi mặc định.
    void addDefaultWifi(const char* ssid, const char* password);

    // Khởi tạo thư viện
    // buttonPin: Chân nối với nút nhấn để kích hoạt chế độ cài đặt
    // ledPin: (Tùy chọn) Chân nối với đèn LED. Dùng giá trị -1 nếu không có LED.
    void init(int buttonPin, int ledPin = -1);

    // Xử lý vòng lặp nội bộ (nhận yêu cầu HTTP, kiểm tra nút).
    // Cần đặt trong hàm loop().
    // Trả về true nếu thiết bị đang trong chế độ cấu hình WiFi (AP mode).
    bool handle();

    // Cố gắng kết nối với WiFi đã được cấu hình.
    // Trả về true nếu thành công.
    bool connect(int ledPin = -1);

    // Duy trì kết nối WiFi, tự động kết nối lại nếu bị ngắt.
    // Cần gọi định kì nếu cần kiểm tra & tự nối lại.
    void maintain(int ledPin = -1);

private:
    static const int MAX_WIFIS = 3;
    const char* _wifi_ssids[MAX_WIFIS];
    const char* _wifi_passwords[MAX_WIFIS];
    int _default_num_wifis;

    const char* _ap_ssid;
    const char* _ap_password;

    int _current_wifi_index;
    int _setupButtonPin;
    int _ledPin;

    struct StoredWifiConfig {
        uint32_t magic;
        char ssid[33];
        char password[65];
    };
    StoredWifiConfig _storedWifi;
    bool _hasStoredWifi;
    
    bool _wifiSetupMode;
    bool _reconnectAfterSave;
    unsigned long _reconnectAfterSaveAt;
    unsigned long _buttonDownAt;
    bool _buttonHoldHandled;
    unsigned long _ledBlinkAt;
    bool _ledBlinkState;

    DNSServer _dnsServer;
#if defined(ESP8266)
    ESP8266WebServer _wifiSetupServer;
#elif defined(ESP32)
    WebServer _wifiSetupServer;
#endif

    static const uint32_t WIFI_EEPROM_MAGIC = 0xEC0A2026;
    static const unsigned long WIFI_SETUP_HOLD_MS = 6000;
    static const unsigned long LED_RECONNECT_BLINK_MS = 150;
    static const unsigned long LED_SETUP_BLINK_MS = 300;
    static const byte DNS_PORT = 53;

    void updateBlinkLed(unsigned long intervalMs, int ledPin);
    void loadStoredWifi();
    void saveStoredWifi(const String& ssid, const String& password);
    int getWifiCount();
    const char* getWifiSsid(int index);
    const char* getWifiPassword(int index);
    void startWifiSetupMode();
    void stopWifiSetupMode();
    bool checkWifiSetupButtonHold();

    // Các hàm xử lý HTTP cần callback nên để dạng biến toàn cục hoặc wrap lại.
    // Vì ESP8266WebServer callback không thể gán vào member function dễ dàng (trừ khi dùng std::bind),
    // ta sẽ dùng std::bind.
    void handleWifiSetupRoot();
    void handleWifiSetupScan();
    void handleWifiSetupSave();
    void redirectToPortal();

    String htmlEscape(const String& text);
    String jsonEscape(const String& text);
};

extern EcoWifiClass EcoWifi;

#endif
