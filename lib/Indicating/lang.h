#ifndef LANG_H
#define LANG_H

#include <avr/pgmspace.h>

#define EN 0
#define ID 1

#define INIT 0
#define CALIBRATION 1
#define YES 2
#define DELETE 3
#define PLEASE_WAIT 4
#define SHUTING_DOWN 5
#define TURNING_ON 6
#define WARNING_BATTERY_LOW 7
#define PLEASE_CHARGE_BATTERY 8
#define PLEASE_TAP_ID 9
#define HOLD_TO_SEND 10
#define SEND_DATA 11
#define DATA_SENDING_QUEUE_FULL 12
#define TAP_ID_CARD 13
#define PUT_PROBE_PH_TO_7 14
#define PUT_PROBE_PH_TO_10 15
#define PH_CALIBRATION_DONE 16
#define EC_PROBE_DRY 17
#define PUT_PROBE_EC_TO_12880 18
#define PUT_PROBE_EC_TO_80000 19
#define EC_CALIBRATION_DONE 20
#define DAMP_THE_PROBE_DO 21
#define DO_CALIBRATION_DONE 22
#define PUT_PROBE_ORP_TO_255 23
#define ORP_CALIBRATION_DONE 24
#define SENDING_DELAYED_DATA 25
#define DELAYED_DATA_DELETED 26
#define FAILED_RESTART 27
#define CONNECTION_FAILED 28
#define SEND_DATA_WITHOUT_INTERNET 29
#define CANT_IDENTIFY_ID_CARD 30
#define SEND_DELAYED_DATA 31
#define CALIBRATION_WITH_DOTS 32
#define PORTABLE_VERSION 33
#define CONTINOUS_VERSION 34
#define SD_CARD_DETACHED 35
#define SD_CARD_FAILED 36
#define PH_CALIBRATION 37
#define EC_CALIBRATION 38
#define DO_CALIBRATION 39
#define BACK 40
#define PUT_PROBE_PH_TO_4_10 41
#define CHARGER_PLUGED       42
#define CHARGER_RELEASED     43
#define STATUS_NONE          44
#define STATUS_EXIST         45
#define PH_CALIBRATION_CANCELLED 46
#define PH_CALIBRATION_FAILED    47
#define PLEASE_CHECK_SENSOR      48

#define TEMP_CALIBRATION         49
#define TEMP_REF_PLUG_IN         50
#define FAILED                   51

#define LANGUAGE                 52

const char en_init[] PROGMEM = "Init";
const char en_calibration[] PROGMEM = "Calibration";
const char en_yes[] PROGMEM = "Yes";
const char en_delete[] PROGMEM = "Delete";
const char en_please_wait[] PROGMEM = " Please wait...";
const char en_shuting_down[] PROGMEM = "                 Shuting down...";
const char en_turning_on[] PROGMEM = "                  Turning on...";
const char en_warning_battery_low[] PROGMEM = "    Warning!      The battery       is low";
const char en_please_charge_battery[] PROGMEM = "Please charge the battery first!";
const char en_please_tap_id[] PROGMEM = "                 Please tap the      ID card!";
const char en_hold_to_send[] PROGMEM = "                Hold to send the    data..";
const char en_send_data[] PROGMEM = "                Sending the data";
const char en_data_sending_queue_full[] PROGMEM = "                  Data sending    queue is full!";
const char en_tap_id_card[] PROGMEM = "Tap ID card";
const char en_put_probe_ph_to_7[] PROGMEM = "                Put probe into  pH 7 solution->";
const char en_put_probe_ph_to_10[] PROGMEM = "                Put probe into  pH 10 solution->";
const char en_ph_calibration_done[] PROGMEM = "                 pH calibration     is done";
const char en_ec_probe_dry[] PROGMEM = "Let the EC probe on dry condi-  tion";
const char en_put_probe_ec_to_12880[] PROGMEM = "                 Put probe into 12800 solution->";
const char en_put_probe_ec_to_80000[] PROGMEM = "                 Put probe into 80000 solution->";
const char en_ec_calibration_done[] PROGMEM = "                 EC calibration     is done";
const char en_damp_the_probe_do[] PROGMEM = " Damp the probe, let it in the       air->";
const char en_do_calibration_done[] PROGMEM = "                 DO calibration     is done";
const char en_put_probe_orp_to_255[] PROGMEM = " Put probe into 255 solution ";
const char en_orp_calibration_done[] PROGMEM = "                 ORP calibration    is done";
const char en_sending_delayed_data[] PROGMEM = "                 Sending delayed      data";
const char en_delayed_data_deleted[] PROGMEM = "                Delayed data has been deleted";
const char en_failed_restart[] PROGMEM = "    Failed!     restart or check your sim card       credit";
const char en_connection_failed[] PROGMEM = "   Connection    failed, check    your sim card      credit";
const char en_send_data_without_internet[] PROGMEM = "  Send the data without internet  connection";
const char en_cant_identify_id_card[] PROGMEM = "Can't identify the ID card";
const char en_send_delayed_data[] PROGMEM = " Send %d delayed  data?";
const char en_calibration_with_dots[] PROGMEM = " Calibration...";
const char en_portable_version[] PROGMEM = "Ver %s";
const char en_continous_version[] PROGMEM = "Ver %sC";
const char en_sd_card_detached[] PROGMEM = "Init SD Detached";
const char en_sd_card_failed[] PROGMEM = "Init SD Failed";
const char en_ph_calibration[] PROGMEM = "pH Calibration";
const char en_ec_calibration[] PROGMEM = "EC Calibration";
const char en_do_calibration[] PROGMEM = "DO Calibration";
const char en_back[] PROGMEM = "Back";
const char en_put_probe_ph_to_4_10[] PROGMEM = "                Put probe into  pH 4/10 solution->";
const char en_charger_pluged[] PROGMEM = "      Power            is            charging";
const char en_charger_released[] PROGMEM = "                    Charger's        released";
const char en_status_none[] PROGMEM ="none";
const char en_status_exist[] PROGMEM="exist";
const char en_ph_calibration_cancelled[] PROGMEM = "                pH calibration      is            cancelled";
const char en_ph_calibration_failed[] PROGMEM = "                pH calibration         is            failed";
const char en_please_check_sensor[] PROGMEM = " Please check %s   Sensor !";
const char en_temp_calibration[] PROGMEM = "Temp Calibration";
const char en_plug_ref_temperature [] PROGMEM = "                 Please plug in ref-temperature     sensor !";
const char en_failed  [] PROGMEM ="failed";
const char language_en[]PROGMEM = "Language";

const char id_init[] PROGMEM = "Init";
const char id_calibration[] PROGMEM = "Kalibrasi";
const char id_yes[] PROGMEM = "Ya";
const char id_delete[] PROGMEM = "Hapus";
const char id_please_wait[] PROGMEM = " Harap tunggu..";
const char id_shuting_down[] PROGMEM = "            Sedang    mematikan      ....";
const char id_turning_on[] PROGMEM = "                   Menyalakan         ....";
const char id_warning_battery_low[] PROGMEM = "                   Perhatian!    Baterai lemah.";
const char id_please_charge_battery[] PROGMEM = "  Isi baterai,   pengukuran ber-  lanjut saat     baterai cukup.";
const char id_please_tap_id[] PROGMEM = "                   Tempelkan       kartu ID!";
const char id_hold_to_send[] PROGMEM = "                  Tahan untuk   mengirim data...";
const char id_send_data[] PROGMEM = "                Mengirim data...";
const char id_data_sending_queue_full[] PROGMEM = "                Pengiriman data     penuh";
const char id_tap_id_card[] PROGMEM = "Tempel kartu";
const char id_put_probe_ph_to_7[] PROGMEM = "                Masukkan ke     larutan pH 7.00               ->";
const char id_put_probe_ph_to_10[] PROGMEM = "                Masukkan ke     larutan pH 10                 ->";
const char id_ph_calibration_done[] PROGMEM = "  Kalibrasi pH      selesai";
const char id_ec_probe_dry[] PROGMEM = "                Keringkan probe EC                            ->";
const char id_put_probe_ec_to_12880[] PROGMEM = "                Masukkan probe  ke larutan 12880              ->";
const char id_put_probe_ec_to_80000[] PROGMEM = "                Masukkan probe  ke larutan 80000              ->";
const char id_ec_calibration_done[] PROGMEM = "  Kalibrasi EC      selesai";
const char id_damp_the_probe_do[] PROGMEM = "                Basahkan Sensor,Biarkan di udara              ->";
const char id_do_calibration_done[] PROGMEM = "  Kalibrasi DO      selesai";
const char id_put_probe_orp_to_255[] PROGMEM = "                Masukkan probe ke larutan 225              ->";
const char id_orp_calibration_done[] PROGMEM = " Kalibrasi ORP     selesai";
const char id_sending_delayed_data[] PROGMEM = "                  Mengirim data  yang tertunda.";
const char id_delayed_data_deleted[] PROGMEM = "                  Data tertunda  telah dihapus.";
const char id_failed_restart[] PROGMEM = "                 Gagal! Restart  atau isi pulsa";
const char id_connection_failed[] PROGMEM = " Koneksi gagal, periksa paket   internet anda";
const char id_send_data_without_internet[] PROGMEM = "                 Mengarahkan ke   server tanpa    Internet...";
const char id_cant_identify_id_card[] PROGMEM = "ID tidak dikenali";
const char id_send_delayed_data[] PROGMEM = "Kirim %d data tertunda?";
const char id_calibration_with_dots[] PROGMEM = "Kalibrasi...";
const char id_portable_version[] PROGMEM = "Versi %s";
const char id_continous_version[] PROGMEM = "Versi %sC";
const char id_sd_card_detached[] PROGMEM = "Init SD Lepas";
const char id_sd_card_failed[] PROGMEM = "Init SD Gagal";
const char id_ph_calibration[] PROGMEM = "Kalibrasi pH";
const char id_ec_calibration[] PROGMEM = "Kalibrasi EC";
const char id_do_calibration[] PROGMEM = "Kalibrasi DO";
const char id_back[] PROGMEM = "Kembali";
const char id_put_probe_ph_to_4_10[] PROGMEM = "                Masukkan ke     larutan pH 4/10               ->";
const char id_charger_pluged[] PROGMEM = "                  Mengisi daya";
const char id_charger_released[] PROGMEM = "                   Daya telah      dilepaskan";
const char id_status_none[] PROGMEM ="tiada";
const char id_status_exist[] PROGMEM="ada";
const char id_ph_calibration_cancelled[] PROGMEM = " Kalibrasi pH      dibatalkan";
const char id_ph_calibration_failed[] PROGMEM = " Kalibrasi pH      gagal";
const char id_please_check_sensor[] PROGMEM = " Mohon periksa   Sensor %s !";
const char id_temp_calibration[] PROGMEM = "Kalibrasi Temp";
const char id_plug_ref_temperature [] PROGMEM = "             Mohon menghubungkan ref. sensor !";
const char id_failed  [] PROGMEM ="gagal";
const char language_id[]PROGMEM = "Bahasa";

const char* const trans[][100] PROGMEM= {
  {
    en_init,
    en_calibration,
    en_yes,
    en_delete,
    en_please_wait,
    en_shuting_down,
    en_turning_on,
    en_warning_battery_low,
    en_please_charge_battery,
    en_please_tap_id,
    en_hold_to_send,
    en_send_data,
    en_data_sending_queue_full,
    en_tap_id_card,
    en_put_probe_ph_to_7,
    en_put_probe_ph_to_10,
    en_ph_calibration_done,
    en_ec_probe_dry,
    en_put_probe_ec_to_12880,
    en_put_probe_ec_to_80000,
    en_ec_calibration_done,
    en_damp_the_probe_do,
    en_do_calibration_done,
    en_put_probe_orp_to_255,
    en_orp_calibration_done,
    en_sending_delayed_data,
    en_delayed_data_deleted,
    en_failed_restart,
    en_connection_failed,
    en_send_data_without_internet,
    en_cant_identify_id_card,
    en_send_delayed_data,
    en_calibration_with_dots,
    en_portable_version,
    en_continous_version,
    en_sd_card_detached,
    en_sd_card_failed,
    en_ph_calibration,
    en_ec_calibration,
    en_do_calibration,
    en_back,
    en_put_probe_ph_to_4_10,
    en_charger_pluged,
    en_charger_released,
    en_status_none,
    en_status_exist,
    en_ph_calibration_cancelled,
    en_ph_calibration_failed,
    en_please_check_sensor,
    en_temp_calibration,
    en_plug_ref_temperature,
    en_failed,
    language_en

  },
  {
    id_init,
    id_calibration,
    id_yes,
    id_delete,
    id_please_wait,
    id_shuting_down,
    id_turning_on,
    id_warning_battery_low,
    id_please_charge_battery,
    id_please_tap_id,
    id_hold_to_send,
    id_send_data,
    id_data_sending_queue_full,
    id_tap_id_card,
    id_put_probe_ph_to_7,
    id_put_probe_ph_to_10,
    id_ph_calibration_done,
    id_ec_probe_dry,
    id_put_probe_ec_to_12880,
    id_put_probe_ec_to_80000,
    id_ec_calibration_done,
    id_damp_the_probe_do,
    id_do_calibration_done,
    id_put_probe_orp_to_255,
    id_orp_calibration_done,
    id_sending_delayed_data,
    id_delayed_data_deleted,
    id_failed_restart,
    id_connection_failed,
    id_send_data_without_internet,
    id_cant_identify_id_card,
    id_send_delayed_data,
    id_calibration_with_dots,
    id_portable_version,
    id_continous_version,
    id_sd_card_detached,
    id_sd_card_failed,
    id_ph_calibration,
    id_ec_calibration,
    id_do_calibration,
    id_back,
    id_put_probe_ph_to_4_10,
    id_charger_pluged,
    id_charger_released,
    id_status_none,
    id_status_exist,
    id_ph_calibration_cancelled,
    id_ph_calibration_failed,
    id_please_check_sensor,
    id_temp_calibration,
    id_plug_ref_temperature,
    id_failed,
    language_id
  }
};

#endif
