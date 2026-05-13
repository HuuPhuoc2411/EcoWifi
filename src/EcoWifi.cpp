#include "EcoWifi.h"

EcoWifiClass EcoWifi;

EcoWifiClass::EcoWifiClass() : _wifiSetupServer(80) {
    _default_num_wifis = 0;
    _current_wifi_index = 0;
    _ap_ssid = "EcoWater";
    _ap_password = "1234567890"; // changed to > 8 chars default to be safe just in case
    
    for(int i = 0; i < MAX_WIFIS; i++){
        _wifi_ssids[i] = nullptr;
        _wifi_passwords[i] = nullptr;
    }

    _hasStoredWifi = false;
    _wifiSetupMode = false;
    _reconnectAfterSave = false;
    _reconnectAfterSaveAt = 0;
    _buttonDownAt = 0;
    _buttonHoldHandled = false;
    _ledBlinkAt = 0;
    _ledBlinkState = false;

    _setupButtonPin = -1;
    _ledPin = -1;
}

void EcoWifiClass::setAPCredentials(const char* ssid, const char* password) {
    _ap_ssid = ssid;
    _ap_password = password;
}

void EcoWifiClass::addDefaultWifi(const char* ssid, const char* password) {
    if (_default_num_wifis < MAX_WIFIS) {
        _wifi_ssids[_default_num_wifis] = ssid;
        _wifi_passwords[_default_num_wifis] = password;
        _default_num_wifis++;
    }
}

String EcoWifiClass::htmlEscape(const String& text) {
  String escaped = "";
  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '&') escaped += "&amp;";
    else if (c == '<') escaped += "&lt;";
    else if (c == '>') escaped += "&gt;";
    else if (c == '"') escaped += "&quot;";
    else escaped += c;
  }
  return escaped;
}

String EcoWifiClass::jsonEscape(const String& text) {
  String escaped = "";
  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '"' || c == '\\') {
      escaped += '\\';
      escaped += c;
    } else if (c >= 32) {
      escaped += c;
    }
  }
  return escaped;
}

void EcoWifiClass::updateBlinkLed(unsigned long intervalMs, int ledPin) {
  if (ledPin == -1) return;
  if (millis() - _ledBlinkAt >= intervalMs) {
    _ledBlinkAt = millis();
    _ledBlinkState = !_ledBlinkState;
    digitalWrite(ledPin, _ledBlinkState ? HIGH : LOW);
  }
}

void EcoWifiClass::loadStoredWifi() {
  EEPROM.get(0, _storedWifi);
  _storedWifi.ssid[sizeof(_storedWifi.ssid) - 1] = '\0';
  _storedWifi.password[sizeof(_storedWifi.password) - 1] = '\0';
  _hasStoredWifi = _storedWifi.magic == WIFI_EEPROM_MAGIC && strlen(_storedWifi.ssid) > 0;
  if (_hasStoredWifi) {
    Serial.print("Loaded saved WiFi: ");
    Serial.println(_storedWifi.ssid);
  }
}

void EcoWifiClass::saveStoredWifi(const String& ssid, const String& password) {
  memset(&_storedWifi, 0, sizeof(_storedWifi));
  _storedWifi.magic = WIFI_EEPROM_MAGIC;
  ssid.toCharArray(_storedWifi.ssid, sizeof(_storedWifi.ssid));
  password.toCharArray(_storedWifi.password, sizeof(_storedWifi.password));
  EEPROM.put(0, _storedWifi);
  EEPROM.commit();
  _hasStoredWifi = true;
  _current_wifi_index = _default_num_wifis; // Reset back so we try the newly saved WiFi first
  Serial.print("Saved WiFi: ");
  Serial.println(_storedWifi.ssid);
}

int EcoWifiClass::getWifiCount() {
  return _default_num_wifis + (_hasStoredWifi ? 1 : 0);
}

const char* EcoWifiClass::getWifiSsid(int index) {
  if (index < _default_num_wifis) return _wifi_ssids[index];
  return _storedWifi.ssid;
}

const char* EcoWifiClass::getWifiPassword(int index) {
  if (index < _default_num_wifis) return _wifi_passwords[index];
  return _storedWifi.password;
}

void EcoWifiClass::startWifiSetupMode() {
  if (_wifiSetupMode) return;
  _reconnectAfterSave = false;
  Serial.println("Starting WiFi Setup Mode");
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  IPAddress netMsk(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(_ap_ssid, _ap_password);
  _dnsServer.start(DNS_PORT, "*", apIP);
  _wifiSetupServer.begin();
  _wifiSetupMode = true;
  _ledBlinkAt = 0;
  _ledBlinkState = false;
}

void EcoWifiClass::stopWifiSetupMode() {
  _dnsServer.stop();
  _wifiSetupServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  _wifiSetupMode = false;
}

bool EcoWifiClass::checkWifiSetupButtonHold() {
  if (_setupButtonPin == -1) return false;
  bool buttonPressed = digitalRead(_setupButtonPin) == LOW;

  if (!buttonPressed) {
    _buttonDownAt = 0;
    _buttonHoldHandled = false;
    return false;
  }

  if (_buttonDownAt == 0) _buttonDownAt = millis();

  if (!_buttonHoldHandled && millis() - _buttonDownAt >= WIFI_SETUP_HOLD_MS) {
    _buttonHoldHandled = true;
    startWifiSetupMode();
    return true;
  }
  return false;
}

void EcoWifiClass::handleWifiSetupRoot() {
  String page = R"rawliteral(
<!doctype html>
<html lang="vi">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>EcoWater</title>
<style>
*{box-sizing:border-box}body{margin:0;min-height:100vh;font-family:Arial,sans-serif;background:#edf7f2;color:#12352b;display:flex;align-items:center;justify-content:center;padding:20px}.app{width:min(460px,100%);background:#fff;border:1px solid #d9e9e2;border-radius:18px;box-shadow:0 18px 45px rgba(18,53,43,.14);padding:24px}h1{margin:0 0 6px;font-size:30px;letter-spacing:0}.sub{margin:0 0 22px;color:#5d776e;font-size:14px}.btn{width:100%;border:0;border-radius:12px;padding:14px 16px;font-size:16px;font-weight:700;background:#0d7d67;color:white;cursor:pointer}.btn:active{transform:translateY(1px)}.btn.secondary{margin-top:10px;background:#e8f3ee;color:#145847}.list{display:none;margin-top:18px;border-top:1px solid #e3eee9;padding-top:10px}.wifi{width:100%;display:flex;justify-content:space-between;gap:14px;align-items:center;padding:13px 4px;border:0;border-bottom:1px solid #eef4f1;background:transparent;color:#12352b;text-align:left;cursor:pointer}.wifi strong{font-size:15px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.wifi span{font-size:13px;color:#698179;white-space:nowrap}.status{min-height:22px;margin-top:14px;font-size:14px;color:#5d776e}.modal{display:none;position:fixed;inset:0;background:rgba(10,31,26,.42);align-items:center;justify-content:center;padding:18px}.box{width:min(420px,100%);background:white;border-radius:18px;padding:20px;box-shadow:0 16px 45px rgba(0,0,0,.24)}label{display:block;margin:12px 0 6px;color:#49665d;font-size:13px}input{width:100%;border:1px solid #cfe1da;border-radius:12px;padding:13px 14px;font-size:16px;color:#12352b}.row{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:14px}
</style>
</head>
<body>
<main class="app">
<h1>WiFi Setup</h1>
<p class="sub">Connect device to your WiFi network.<br><i>Kết nối thiết bị với mạng WiFi của bạn.</i></p>
<button class="btn" id="scanBtn">Scan WiFi</button>
<div class="status" id="status"></div>
<div class="list" id="list"></div>
</main>
<div class="modal" id="modal">
<form class="box" method="post" action="/save">
<h2 id="title">Enter Password</h2>
<input type="hidden" name="ssid" id="ssid">
<label for="password">Password / Mật khẩu</label>
<input id="password" name="password" type="password" autocomplete="current-password">
<div class="row">
<button class="btn secondary" type="button" id="cancel">Cancel</button>
<button class="btn" type="submit">Save</button>
</div>
</form>
</div>
<script>
const scanBtn=document.getElementById('scanBtn'),statusEl=document.getElementById('status'),list=document.getElementById('list'),modal=document.getElementById('modal'),ssid=document.getElementById('ssid'),title=document.getElementById('title'),password=document.getElementById('password');
scanBtn.onclick=async()=>{statusEl.innerHTML='Scanning WiFi...<br><i>Đang dò WiFi...</i>';list.style.display='none';list.innerHTML='';try{const res=await fetch('/scan');const networks=await res.json();statusEl.innerHTML=networks.length?'Select a network.<br><i>Chọn WiFi muốn kết nối.</i>':'No WiFi found.<br><i>Không tìm thấy WiFi nào.</i>';networks.forEach(w=>{const b=document.createElement('button');b.className='wifi';b.type='button';b.innerHTML=`<strong>${w.ssid}</strong><span>${w.rssi} dBm ${w.enc?'Secure':'Open'}</span>`;b.onclick=()=>{ssid.value=w.ssid;title.textContent=w.ssid;password.value='';modal.style.display='flex';password.focus();};list.appendChild(b);});list.style.display='block';}catch(e){statusEl.innerHTML='Failed to scan. Try again.<br><i>Không quét được WiFi. Thử lại sau.</i>';}};
document.getElementById('cancel').onclick=()=>modal.style.display='none';
modal.onclick=e=>{if(e.target===modal)modal.style.display='none'};
</script>
</body>
</html>
)rawliteral";
  _wifiSetupServer.send(200, "text/html; charset=utf-8", page);
}

void EcoWifiClass::handleWifiSetupScan() {
  int networkCount = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < networkCount; i++) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"";
    json += jsonEscape(WiFi.SSID(i));
    json += "\",\"rssi\":";
    json += WiFi.RSSI(i);
    json += ",\"enc\":";
#if defined(ESP8266)
    json += ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "false" : "true");
#elif defined(ESP32)
    json += ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "false" : "true");
#endif
    json += "}";
  }
  json += "]";
  WiFi.scanDelete();
  _wifiSetupServer.send(200, "application/json", json);
}

void EcoWifiClass::handleWifiSetupSave() {
  if (!_wifiSetupServer.hasArg("ssid")) {
    _wifiSetupServer.send(400, "text/plain", "Missing WiFi Name / Thieu ten WiFi");
    return;
  }
  String ssid = _wifiSetupServer.arg("ssid");
  String password = _wifiSetupServer.arg("password");
  ssid.trim();
  if (ssid.length() == 0 || ssid.length() > 32 || password.length() > 64) {
    _wifiSetupServer.send(400, "text/plain", "Invalid WiFi Info / Thong tin WiFi khong hop le");
    return;
  }
  saveStoredWifi(ssid, password);
  _reconnectAfterSave = true;
  _reconnectAfterSaveAt = millis() + 1500UL;

  String page = "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>WiFi Setup</title><style>body{margin:0;min-height:100vh;font-family:Arial,sans-serif;background:#edf7f2;color:#12352b;display:flex;align-items:center;justify-content:center;padding:22px}.box{max-width:420px;background:white;border:1px solid #d9e9e2;border-radius:18px;padding:24px;box-shadow:0 18px 45px rgba(18,53,43,.14)}h1{margin:0 0 10px;font-size:26px}p{line-height:1.5;color:#5d776e}</style></head><body><div class='box'><h1>Saved / Đã lưu</h1><p>Connecting to <b>";
  page += htmlEscape(ssid);
  page += "</b>. If successful, LED will stay ON.<br><i>Đang thử kết nối. Nếu thành công, đèn LED sẽ sáng liên tục.</i></p></div></body></html>";
  _wifiSetupServer.send(200, "text/html; charset=utf-8", page);
}

void EcoWifiClass::redirectToPortal() {
  _wifiSetupServer.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  _wifiSetupServer.send(302, "text/plain", "");
}

void EcoWifiClass::init(int buttonPin, int ledPin) {
  _setupButtonPin = buttonPin;
  _ledPin = ledPin;
  if (_setupButtonPin != -1) pinMode(_setupButtonPin, INPUT_PULLUP);
  if (_ledPin != -1) pinMode(_ledPin, OUTPUT);

  EEPROM.begin(sizeof(StoredWifiConfig));
  loadStoredWifi();

  _wifiSetupServer.on("/", HTTP_GET, std::bind(&EcoWifiClass::handleWifiSetupRoot, this));
  _wifiSetupServer.on("/scan", HTTP_GET, std::bind(&EcoWifiClass::handleWifiSetupScan, this));
  _wifiSetupServer.on("/save", HTTP_POST, std::bind(&EcoWifiClass::handleWifiSetupSave, this));
  _wifiSetupServer.on("/generate_204", HTTP_GET, std::bind(&EcoWifiClass::redirectToPortal, this));
  _wifiSetupServer.on("/fwlink", HTTP_GET, std::bind(&EcoWifiClass::redirectToPortal, this));
  _wifiSetupServer.on("/hotspot-detect.html", HTTP_GET, std::bind(&EcoWifiClass::handleWifiSetupRoot, this));
  _wifiSetupServer.on("/connecttest.txt", HTTP_GET, std::bind(&EcoWifiClass::redirectToPortal, this));
  _wifiSetupServer.on("/ncsi.txt", HTTP_GET, std::bind(&EcoWifiClass::redirectToPortal, this));
  _wifiSetupServer.onNotFound(std::bind(&EcoWifiClass::redirectToPortal, this));
}

bool EcoWifiClass::handle() {
  if (!_wifiSetupMode && checkWifiSetupButtonHold()) {
    return true;
  }
  if (!_wifiSetupMode) return false;

  _dnsServer.processNextRequest();
  _wifiSetupServer.handleClient();
  updateBlinkLed(LED_SETUP_BLINK_MS, _ledPin);

  if (_reconnectAfterSave && millis() >= _reconnectAfterSaveAt) {
    _reconnectAfterSave = false;
    stopWifiSetupMode();
    connect(_ledPin);
  }
  return _wifiSetupMode;
}

bool EcoWifiClass::connect(int ledPin) {
  if (ledPin != -1) _ledPin = ledPin;

  if (_wifiSetupMode) return false;

  const int totalWifis = getWifiCount();
  if (totalWifis == 0) return false;

  // Đảm bảo luôn bắt đầu thử từ mạng số 0 (các mạng WiFi được cài đặt mặc định bằng addDefaultWifi)
  _current_wifi_index = 0;

  for (int attempt = 0; attempt < totalWifis; attempt++) {
    int wifiIndex = (_current_wifi_index + attempt) % totalWifis;
    Serial.print("\nConnecting to WiFi: ");
    Serial.println(getWifiSsid(wifiIndex));

    WiFi.disconnect(); 
    delay(100); 
    WiFi.mode(WIFI_STA);
    WiFi.begin(getWifiSsid(wifiIndex), getWifiPassword(wifiIndex));

    unsigned long startedAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 10000UL) {
      updateBlinkLed(LED_RECONNECT_BLINK_MS, _ledPin);
      if (checkWifiSetupButtonHold()) return false;
      delay(10);
    }

    if (WiFi.status() == WL_CONNECTED) {
      _current_wifi_index = wifiIndex;
      if (_ledPin != -1) digitalWrite(_ledPin, HIGH);
      Serial.print("\nConnected to WiFi with IP: ");
      Serial.println(WiFi.localIP());
      return true;
    }
  }

  _current_wifi_index = (_current_wifi_index + 1) % totalWifis;
  return false;
}

void EcoWifiClass::maintain(int ledPin) {
  if (ledPin != -1) _ledPin = ledPin;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    _current_wifi_index = 0; // Đặt thủ công về 0 để ép nó ưu tiên dò lại từ các WiFi mặc định trước
    WiFi.disconnect(); 
    delay(100);
    connect(_ledPin);
  }
}
