#ifndef _APNLIST_H
#define _APNLIST_H
#include "Communication.h"


const struct APN PROGMEM apnDatabase[] = {
 // indonesian provider Celullar network
    {"51001", "indosatgprs", "", ""}, // Indonesia Indosat Oreedoo
    {"51010", "M2MINTERNET", "", ""}, //Indonesia M2M Telkomsel
 //   {"51010", "internet", "", ""}, // Indonesia Telkomsel
 //   {"51010", "nb1internet", "", ""},
    {"51011", "internet", "", ""}, //Indonesia XL
    {"51089", "3gprs", "3gprs", "3gprs"}, // Indonesia 3
// vietnam provider
    {"45202","m3-world", "mms", "mms"},    //vinaphone
    {"45204", "v-internet","",""},         //viettel
    {"45201","m-wap","mms","mms"},            //mobifone
    {"45207","internet","",""},
 // Thailand provider
    {"52001", "internet","",""}, 			// AIS
    {"52000", "internet","",""},			// CAT
    {"52018", "www.dtac.co.th","",""},		// DTAC
    {"52023", "internet","",""},			// HELLO
    {"52015", "internet","",""},			// I-Mobile
    {"52099", "internet","orange","orange"}, 	// orange
    {"52001", "internet","",""},			// TH-GSM
    {"52099", "internet","true","true"},		// True
// Malaysia provider
    {"50212", "net","",""},    // Maxis
    {"50217", "net","",""},    // Maxis
    {"50213", "celcom3g","","" },      // celcom
    {"50219", "celcom3g","","" },      // celcom
    {"50216", "diginet","",""},        // digi
    {"50218", "my3g","",""},           // u mobile
// Singapore Provider
    {"52501", "e-ideas", "", ""},     // singtel
    {"52502", "e-ideas", "", ""},     // singtel
    {"52503", "sunsurf", "65",      "user123"},     // m1
    {"52505", "shppd","",""},     // starhub
    {"52506", "shppd","",""},     // starhub
    {"52508", "shppd","",""},     // starhub
    {"52507", "e-ideas", "", ""},
// china Provider
    {"46000", "cmnet", "", ""},      // China Mobile
    {"46002", "cmnet", "", ""},
    {"46007", "cmnet", "", ""},
    {"46008", "cmnet", "", ""},
    {"45412", "cmnet", "", ""},
    {"45413", "cmnet", "", ""},
    
    {"46003", "ctnet", "", ""},      // China Telecom
    {"46005", "ctnet", "", ""},
    {"46011", "ctnet", "", ""},
    {"45411", "ctnet", "", ""},
    //{"46020", "ctnet", "", ""},      // China Tietong
    {"46001", "3gnet", "", ""},      // China Unicom
    {"46006", "3gnet", "", ""},
    {"46009", "3gnet", "", ""},
    {"45407", "3gnet", "", ""},


    {"", "cmnet", "", ""}};

    
#endif