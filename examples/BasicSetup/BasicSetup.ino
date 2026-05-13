#include <EcoWifi.h>

// Configure the pins for the push button to enter Wi-Fi setup mode and the LED to indicate Wi-Fi connection status.
// Cài đặt chân cho nút nhấn để vào chế độ cài wiif và LED báo trạng thái kết nối wifi
const int WIFI_SETUP_BUTTON_PIN = 0; 
const int WIFI_STATUS_LED_PIN = 2;   

void setup() {
  // Initialize serial communication
  // Khởi tạo giao tiếp Serial
  Serial.begin(115200);
  Serial.println("\nSystem starting...");

  // Optional: Set custom AP credentials for WiFi setup mode
  // Tùy chọn: Đặt tên và mật khẩu cho điểm phát WiFi khi cài đặt
  EcoWifi.setAPCredentials("EcoWater_Setup", "12345678");

  // Optional: Add default hardcoded WiFi networks to connect to initially (max 3 networks)
  // Tùy chọn: Thêm các mạng WiFi mặc định để kết nối ban đầu (tối đa 3 mạng)
  // If not added, it will only use saved networks in EEPROM or start AP mode.
  // If you don't want to add this default Wi-Fi, you don't need to call the EcoWifi.addDefaultWifi() function.
  // Nếu không thêm, thư viện sẽ chỉ dùng mạng đã lưu trong bộ nhớ hoặc mở chế độ chờ cài đặt (AP).
  // Nếu không muốn thêm wifi mặc định này thì không cần gọi hàm EcoWifi.addDefaultWifi()
  EcoWifi.addDefaultWifi("My_Home_Network", "password123");
  EcoWifi.addDefaultWifi("Backup_Network", "password456");
  // EcoWifi.addDefaultWifi("Another_Network", "password789");

  // Initialize the library with button and LED pins (LED pin is optional)
  // Khởi tạo thư viện với chân nút nhấn và chân LED (chân LED có thể bỏ qua nếu không dùng)
  // Example without LED / Ví dụ nếu không dùng LED: EcoWifi.init(WIFI_SETUP_BUTTON_PIN);
  EcoWifi.init(WIFI_SETUP_BUTTON_PIN, WIFI_STATUS_LED_PIN);

  // Try to connect to configured or saved WiFi networks
  // Thử kết nối tới các mạng WiFi đã cấu hình hoặc đã lưu
  if (EcoWifi.connect()) {
    Serial.println("WiFi connected successfully!");
  } else {
    Serial.println("WiFi connection failed. Press and hold button to enter setup mode.");
  }
}

void loop() {
  // Must be called repeatedly to handle button press and web setup portal
  // Phải được gọi liên tục để xử lý nút nhấn và trang web cài đặt
  if (EcoWifi.handle()) {
    // Stop running other application code while in AP setup mode
    // Dừng chạy code của ứng dụng khác khi đang ở chế độ cài đặt AP
    return;
  }

  // Maintain connection, automatically reconnect if WiFi drops
  // Duy trì kết nối, tự động kết nối lại nếu rớt mạng WiFi
  EcoWifi.maintain();

  // Your application logic goes here
  // Code ứng dụng của bạn đặt ở đây
}
