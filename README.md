# EcoWifi Library / Thư viện kết nối và cài đặt wifi cho các ứng dụng IOT trên ESP32 hoặc ESP8266

[![Arduino Library](https://www.ardu-badge.com/badge/EcoWifi.svg)](https://www.ardu-badge.com/EcoWifi)

Đây là một thư viện cho phép bạn sử dụng wifi dễ dàng hơn trên **ESP32** hoặc **ESP8266**, hỗ trợ tốt cho các dự án IOT.
<br>
Thư viện có các tính năng như: Sử dụng 1 nút bấm để vào chế độ cài đặt **tên** và **mật khẩu** wifi dễ dàng, tự kết nối lại wifi khi mất mạng.

This is a library that allows you to use Wi-Fi more easily on **ESP32** or **ESP8266**, providing good support for IoT projects.
<br>
The library includes features such as: One-button access to easily set the Wi-Fi **name** and **password**, and automatic reconnection to Wi-Fi after a network outage.

## 📱 Mobile WiFi Setup Guide / Hướng dẫn cài đặt WiFi trên điện thoại

**Bước 1:** *Mở cài đặt wifi trong điện thoại lên và tìm để tên wifi phát ra từ ESP mà bạn đã cài trước đó, bấm vào wifi đó và nhập mật khẩu bạn đẫ đặt trong code. Nếu sau khi kết nối wifi mà web cài đặt không tự động mở lên thì bạn bấm wifi đó 1 lần nữa thì trang web sẽ được mở lên*
<br>
<br>
**Step 1:** Open your phone's Wi-Fi settings and find the Wi-Fi network name broadcast from the ESP you previously configured. Tap on that Wi-Fi network and enter the password you set in the code. If the settings website doesn't open automatically after connecting, tap on that Wi-Fi network again and the website should open.
<br>
<br>
<img src="img/a1.png" alt="Step 1" width="300"/>

**Bước 2:** *Sau khi đã vào web cài đặt wifi thì bấm nút "Scan wifi"*
<br>
<br>
**Step 2:** *After accessing the Wi-Fi settings website, click the "Scan Wi-Fi" button.*
<br>
<br>
<img src="img/a2.jpg" alt="Step 2" width="300"/>

**Bước 3:** *ESP sẽ dò các wifi có thể kết nối được, lúc này bạn cần bấm vào wifi mà mạng muốn kết nối*
<br>
<br>
**Step 3:** *ESP will scan for available Wi-Fi networks; at this point, you need to tap on the Wi-Fi network you want to connect to.*
<br>
<br>
<img src="img/a3.jpg" alt="Step 3" width="300"/>

**Bước 4:** *Nhập mật khẩu wifi của bạn vào, bấm nút "Save" để lưu wifi*
<br>
<br>
**Step 4:** *Enter your Wi-Fi password, then click the "Save" button to save the Wi-Fi.*
<br>
<br>
<img src="img/a4.jpg" alt="Step 4" width="300"/>

**Bước 5:** *Sau khi lưu wifi bạn hãy chờ đợi một chút, nếu wifi kết nối thành công thì đèn báo sẽ không nhấp nháy nữa*
<br>
<br>
Step 5: After saving the Wi-Fi information, please wait a moment. If the Wi-Fi connection is successful, the indicator light will stop blinking.
<br>
<br>
<img src="img/a5.jpg" alt="Step 5" width="300"/>

---
## Read the instructions below/ Đọc tiếp phần hướng dẫn bên dưới (Có đoạn tiếng việt ở cuối)

## 🇺🇸 English Documentation

### Overview
**EcoWifi** is designed to simplify WiFi provisioning for IoT projects. It gracefully handles network connections, automatically reconnects when disconnected, and provides an elegant Captive Portal allowing users to configure WiFi credentials via their smartphone or PC without hardcoding them. 

### Key Features
- **Cross-Platform Compatibility:** Fully supports both ESP8266 and ESP32 with the exact same API.
- **Smart Captive Portal:** If WiFi is unavailable or the user holds the setup button, it launches a temporary Access Point (AP) and an integrated Web UI/Captive Portal for WiFi configuration.
- **EEPROM Memory Safe:** Prevents flash memory wear and fragmentation. Credentials overwrite precisely the same block, ensuring your memory remains safe no matter how many times the WiFi config is updated.
- **Multiple Fallback Networks:** Hardcode up to 3 fallback/default WiFi connections.
- **Auto-Reconnect & Status Indication:** Automatically monitors connection drops and intelligently blinks an assigned LED to show device states (Connecting, AP Mode, Connected).

### Installation
1. Go to this repository and click **Code -> Download ZIP**.
2. Open the Arduino IDE.
3. Navigate to **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Select the downloaded `.zip` file. (Or extract it manually into your `Documents/Arduino/libraries/` folder).

### How to Use & Workflow
1. Include the library: `#include <EcoWifi.h>`.
2. Configure settings in `setup()`: define the Setup AP credentials, add fallback networks, and initialize with your Button and LED pins.
3. Run `EcoWifi.handle()` and `EcoWifi.maintain()` within your `loop()`.
4. **Triggering Setup Mode:** Power up the device. If it cannot connect to the primary or fallback networks, press and hold the designated Setup Button for **6 seconds**. The Status LED will blink, entering AP Mode.
5. **Connecting:** On your smartphone/PC, connect to the new Setup AP (e.g., `EcoWater_Setup`). A web portal will automatically pop up. (If not, navigate to `192.168.4.1`). Scan for networks, enter your password, and save. The device will auto-reboot to connect.

### Example Code
Please check the `examples/BasicSetup` folder in Arduino IDE: **File** -> **Examples** -> **EcoWifi** -> **BasicSetup**.

---

## 🇻🇳 Hướng dẫn bằng Tiếng Việt (Vietnamese)

### Tổng quan
**EcoWifi** là thư viện quản lý kết nối WiFi dành cho các dự án IoT. Nó đơn giản hóa việc kết nối mạng, tự động kết nối lại khi rớt mạng, và cung cấp một trang cấu hình Web (Captive Portal) đẹp mắt để người dùng tự cài đặt WiFi bằng điện thoại hoặc máy tính mà không cần nạp lại code bằng máy tính.

### Tính năng Nổi bật
- **Hỗ trợ cả ESP8266 & ESP32:** Tự động nhận diện board và sử dụng đúng nhân hệ thống tương ứng mà không cần bạn phải thay đổi code.
- **Trang Cấu hình Captive Portal:** Khi thiết bị không thể kết nối WiFi hoặc bạn ấn giữ nút cài đặt, thiết bị sẽ phát ra một WiFi ảo. Điện thoại khi kết nối vào sẽ tự động nhảy lên trang Web cài đặt WiFi mới cực kỳ chuyên nghiệp.
- **An toàn cho Bộ nhớ EEPROM:** Tránh tình trạng phân mảnh hay làm đầy bộ nhớ Flash/EEPROM. Dữ liệu WiFi luôn được ghi đè một cách tối ưu vào một vị trí duy nhất, bảo vệ tuổi thọ của chip ngay cả khi thay đổi WiFi hàng nghìn lần.
- **Hỗ trợ Nhiều WiFi Lưu Trữ:** Bạn có thể điền trước (hardcode) tối đa 3 cấu hình WiFi để thiết bị xoay vòng thử kết nối.
- **Chỉ báo LED Thông minh:** Tích hợp chân LED để nhấp nháy báo hiệu trạng thái (Đang kết nối, Chế độ cài đặt, Đã kết nối). Tự động theo dõi và kết nối lại khi mất mạng.

### Cài đặt
1. Tải toàn bộ mã nguồn này về dưới dạng file `.zip`.
2. Mở Arduino IDE.
3. Chọn menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Chọn file `.zip` vừa tải về. (Hoặc giải nén thẳng thư mục này vào `Documents/Arduino/libraries/`).

### Quy trình sử dụng
1. Thêm thư viện: `#include <EcoWifi.h>`.
2. Gõ các lệnh cấu hình vào hàm `setup()`: thiết lập tên/mật khẩu WiFi phát ra khi cài đặt, thêm các mạng WiFi mặc định, sau đó khởi tạo chân Nút nhấn và Đèn LED.
3. Liên tục gọi hàm `EcoWifi.handle()` và `EcoWifi.maintain()` bên trong hàm `loop()`.
4. **Vào Chế độ Cài đặt:** Cắm điện. Nếu mạch không kết nối được vào WiFi nào, hãy **nhấn giữ nút Cài đặt 6 giây**. Đèn LED sẽ nháy báo hiệu đã bật chế độ phát WiFi.
5. **Cấu hình WiFi:** Dùng điện thoại/Máy tính kết nối vào WiFi của mạch phát ra (vd: `EcoWater_Setup`). Màn hình chọn WiFi sẽ tự động hiện lên (nếu không hiện, hãy vào web `192.168.4.1`). Quét mạng, điền mật khẩu và ấn Lưu. Thiết bị sẽ tự thoát và kết nối lại vào wifi mới.

### Code mẫu
Vui lòng mở file ví dụ có sẵn bằng cách vào Arduino IDE: **File** -> **Examples** -> **EcoWifi** -> **BasicSetup**.

---

