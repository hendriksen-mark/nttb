#include "WiFiHandler.h"
#include "Display.h"
#include <sstream>
#include <DebugLog.h>

String SSID = "ZTTV" + String(random(1000, 9999));
String PASS = "ZTTV" + String(random(1000, 9999));

WifiHandler::WifiHandler():
	debugDisplay(nullptr)
{
	
}

WifiHandler::~WifiHandler()
{
}

void WifiHandler::start()
{
	WiFi.onEvent(staticEventHandler);
	
	// Set AP to not claim it functions as a gateway
	WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(), IPAddress(255, 255, 255, 0));
	WiFi.mode(wifi_mode_t::WIFI_MODE_APSTA);
	WiFi.hostname(SSID);
	WiFi.softAP(SSID, PASS, 5, 0, 1);
	WiFi.begin(SSID, PASS);
}

void WifiHandler::setDebugDisplay(Display* display)
{
	debugDisplay = display;
}

void WifiHandler::eventHandler(arduino_event_t* event)
{
	LOG_DEBUG(eventIdToString(event->event_id));
	if (debugDisplay == nullptr)
		return;

	switch (event->event_id)
	{
	case ARDUINO_EVENT_WIFI_AP_START:
	case ARDUINO_EVENT_WIFI_READY:
	case ARDUINO_EVENT_WIFI_STA_START:
		debugDisplay->print("Maak AP:\n" + SSID + "\n" + PASS, debugDisplay->colors.white);//switch between text and qrcode
		debugDisplay->create("WIFI:S:" + SSID + ";T:WPA;P:" + PASS + ";H:false;;");
		break;

	case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
		WiFi.enableSTA(true);
		if(WiFi.SSID() == ""){
			WiFi.begin(SSID, PASS);
		}
		break;

	case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
		WiFi.enableAP(true);
		if(WiFi.softAPSSID() == ""){
			WiFi.softAP(SSID, PASS, 5, 0, 1);
		}
		break;

	case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
	case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
		WiFi.enableSTA(false);
		debugDisplay->print("Gebruik IP:\n" + WiFi.softAPIP().toString(), debugDisplay->colors.white);
		break;

	case ARDUINO_EVENT_WIFI_STA_CONNECTED:
	case ARDUINO_EVENT_WIFI_STA_GOT_IP:
		WiFi.enableAP(false);
		debugDisplay->print("Gebruik IP:\n" + WiFi.localIP().toString(), debugDisplay->colors.white);
		break;

	default:
		debugDisplay->printWrapped(eventIdToString(event->event_id));
		break;
	}
}

void WifiHandler::staticEventHandler(arduino_event_t* event)
{
	WifiHandler::get().eventHandler(event);
}

WifiHandler& WifiHandler::get()
{
	if (instance == nullptr)
	{
		instance = std::make_unique<WifiHandler>();
	}
	return *instance;
}

const char* WifiHandler::eventIdToString(arduino_event_id_t id)
{
	switch(id)
	{
		case ARDUINO_EVENT_WIFI_READY: return "WF Ready";
		case ARDUINO_EVENT_WIFI_SCAN_DONE: return "WF Scan Done";
		case ARDUINO_EVENT_WIFI_STA_START: return "STA Start";
		case ARDUINO_EVENT_WIFI_STA_STOP: return "STA Stop";
		case ARDUINO_EVENT_WIFI_STA_CONNECTED: return "STA Connected";
		case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: return "STA Disconnected";
		case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: return "STA Auth Changed";
		case ARDUINO_EVENT_WIFI_STA_GOT_IP: return "STA GOT IP";
		case ARDUINO_EVENT_WIFI_STA_GOT_IP6: return "STA GOT IP6";
		case ARDUINO_EVENT_WIFI_STA_LOST_IP: return "STA LOST IP";
		case ARDUINO_EVENT_WIFI_AP_START: return "AP Start";
		case ARDUINO_EVENT_WIFI_AP_STOP: return "AP Stop";
		case ARDUINO_EVENT_WIFI_AP_STACONNECTED: return "AP Client Connected";
		case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: return "AP Client Disconnected";
		case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED: return "IP assigned";
		case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED: return "Probe reqrecved";
		case ARDUINO_EVENT_WIFI_AP_GOT_IP6: return "AP Got IP6";
		case ARDUINO_EVENT_WIFI_FTM_REPORT: return "FTM Report";
		case ARDUINO_EVENT_ETH_START: return "ETH Start";
		case ARDUINO_EVENT_ETH_STOP: return "ETH Stop";
		case ARDUINO_EVENT_ETH_CONNECTED: return "ETH Connected";
		case ARDUINO_EVENT_ETH_DISCONNECTED: return "ETH Disconnected";
		case ARDUINO_EVENT_ETH_GOT_IP: return "ETH Got IP";
		case ARDUINO_EVENT_ETH_GOT_IP6: return "ETH Got IP6";
		case ARDUINO_EVENT_WPS_ER_SUCCESS: return "WPS ER Success";
		case ARDUINO_EVENT_WPS_ER_FAILED: return "WPS ER Failed";
		case ARDUINO_EVENT_WPS_ER_TIMEOUT: return "WPS ER Timeout";
		case ARDUINO_EVENT_WPS_ER_PIN: return "WPS ER PIN";
		case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP: return "WPS ER PBC Overlap";
		case ARDUINO_EVENT_SC_SCAN_DONE: return "SC Scan Done";
		case ARDUINO_EVENT_SC_FOUND_CHANNEL: return "SC Found Channel";
		case ARDUINO_EVENT_SC_GOT_SSID_PSWD: return "SC Got SSID Pwd";
		case ARDUINO_EVENT_SC_SEND_ACK_DONE: return "SC Send Ack Done";
		case ARDUINO_EVENT_PROV_INIT: return "PROV Init";
		case ARDUINO_EVENT_PROV_DEINIT: return "PROV Deinit";
		case ARDUINO_EVENT_PROV_START: return "PROV Start";
		case ARDUINO_EVENT_PROV_END: return "PROV End";
		case ARDUINO_EVENT_PROV_CRED_RECV: return "PROV Cred Recv";
		case ARDUINO_EVENT_PROV_CRED_FAIL: return "PROV Cred Fail";
		case ARDUINO_EVENT_PROV_CRED_SUCCESS: return "PROV Cred Success";
		default: return "unknown event id";
	}
}

std::unique_ptr<WifiHandler> WifiHandler::instance = nullptr;
