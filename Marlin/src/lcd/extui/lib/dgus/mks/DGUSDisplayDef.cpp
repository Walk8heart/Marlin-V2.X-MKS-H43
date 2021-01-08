/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* DGUS VPs changed by George Fu in 2019 for Marlin */

#include "../../../../../inc/MarlinConfigPre.h"

#if ENABLED(DGUS_LCD_UI_MKS)

#include "DGUSDisplayDef.h"
#include "../DGUSDisplay.h"
#include "../DGUSScreenHandler.h"

#include "../../../../../module/temperature.h"
#include "../../../../../module/motion.h"
#include "../../../../../module/planner.h"

#include "../../../ui_api.h"
#include "../../../../marlinui.h"

#if ENABLED(HAS_STEALTHCHOP)
    #include "../../../../src/module/stepper/trinamic.h"
#endif

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
uint16_t distanceToMove = 10;
#endif

#if ENABLED(DGUS_LCD_UI_MKS)
    uint16_t distanceMove = 1;
    float distanceFilament = 10;
    uint16_t FilamentSpeed = 15;
    float ZOffset_distance = 0.1;
    float mesh_adj_distance = 0.1;
    float Z_distance = 0.1;

    int16_t level_1_x_point = 20;
    int16_t level_1_y_point = 20;

    int16_t level_2_x_point = 20;
    int16_t level_2_y_point = 20;

    int16_t level_3_x_point = 20;
    int16_t level_3_y_point = 20;

    int16_t level_4_x_point = 20;
    int16_t level_4_y_point = 20;
    int16_t level_5_x_point = X_MAX_POS / 2;
    int16_t level_5_y_point = Y_MAX_POS / 2;

    uint16_t tim_h;
    uint16_t tim_m;
    uint16_t tim_s;

    uint16_t x_park_pos = 20;
    uint16_t y_park_pos = 20;
    uint16_t z_park_pos = 10;

    uint16_t min_ex_temp = 0;

    float z_offset_add = 0;

    #if ENABLED(SENSORLESS_HOMING)
        uint16_t tmc_x_step = 0;
        uint16_t tmc_y_step = 0;
        uint16_t tmc_z_step = 0;
    #else
        uint16_t tmc_x_step = 0;
        uint16_t tmc_y_step = 0;
        uint16_t tmc_z_step = 0;
    #endif

    EX_FILAMENT_DEF ex_filament;
    RUNOUT_MKS_DEF runout_mks;
    NOZZLE_PARK_DEF nozzle_park_mks;
#endif

const uint16_t VPList_Boot[] PROGMEM = {
    VP_MARLIN_VERSION,
    0x0000};

const uint16_t VPList_Main[] PROGMEM = {
/* VP_M117, for completeness, but it cannot be auto-uploaded. */
#if HOTENDS >= 1
    VP_T_E0_Is, VP_T_E0_Set, VP_E0_STATUS,
#endif
#if HOTENDS >= 2
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if HAS_HEATED_BED
    VP_T_Bed_Is, VP_T_Bed_Set, VP_BED_STATUS,
#endif
#if HAS_FAN
    VP_Fan0_Percentage, VP_FAN0_STATUS,
#endif
    VP_XPos, VP_YPos, VP_ZPos,
    VP_Fan0_Percentage,
    VP_Feedrate_Percentage,
#if ENABLED(LCD_SET_PROGRESS_MANUALLY)
    VP_PrintProgress_Percentage,
#endif
    0x0000};

// const uint16_t VPList_Temp[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   0x0000
// };

// const uint16_t VPList_Status[] PROGMEM = {
//   /* VP_M117, for completeness, but it cannot be auto-uploaded */
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   #if HAS_FAN
//     VP_Fan0_Percentage,
//   #endif
//   VP_XPos, VP_YPos, VP_ZPos,
//   VP_Fan0_Percentage,
//   VP_Feedrate_Percentage,
//   VP_PrintProgress_Percentage,
//   0x0000
// };

// const uint16_t VPList_Status2[] PROGMEM = {
//   /* VP_M117, for completeness, but it cannot be auto-uploaded */
//   #if HOTENDS >= 1
//     VP_Flowrate_E0,
//   #endif
//   #if HOTENDS >= 2
//     VP_Flowrate_E1,
//   #endif
//   VP_PrintProgress_Percentage,
//   VP_PrintTime,
//   0x0000
// };

// const uint16_t VPList_Preheat[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   0x0000
// };

// const uint16_t VPList_ManualMove[] PROGMEM = {
//   VP_XPos, VP_YPos, VP_ZPos,
//   0x0000
// };

// const uint16_t VPList_ManualExtrude[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//   #endif
//   VP_EPos,
//   0x0000
// };

// const uint16_t VPList_FanAndFeedrate[] PROGMEM = {
//   VP_Feedrate_Percentage, VP_Fan0_Percentage,
//   0x0000
// };

// const uint16_t VPList_SD_FlowRates[] PROGMEM = {
//   VP_Flowrate_E0, VP_Flowrate_E1,
//   0x0000
// };

// const uint16_t VPList_Filament_heating[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//     VP_E0_FILAMENT_LOAD_UNLOAD,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//     VP_E1_FILAMENT_LOAD_UNLOAD,
//   #endif
//   0x0000
// };

// const uint16_t VPList_Filament_load_unload[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_E0_FILAMENT_LOAD_UNLOAD,
//   #endif
//   #if HOTENDS >= 2
//     VP_E1_FILAMENT_LOAD_UNLOAD,
//   #endif
//   0x0000
// };

// const uint16_t VPList_SDFileList[] PROGMEM = {
//   VP_SD_FileName0, VP_SD_FileName1, VP_SD_FileName2, VP_SD_FileName3, VP_SD_FileName4,
//   0x0000
// };

// const uint16_t VPList_SD_PrintManipulation[] PROGMEM = {
//   VP_PrintProgress_Percentage, VP_PrintTime,
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   #if HAS_FAN
//     VP_Fan0_Percentage,
//     #if FAN_COUNT > 1
//       VP_Fan1_Percentage,
//     #endif
//   #endif
//   VP_Flowrate_E0,
//   0x0000
// };

// const uint16_t VPList_SDPrintTune[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set, VP_Flowrate_E0,
//   #endif
//   #if HOTENDS >= 2
//     VP_T_E1_Is, VP_T_E1_Set, VP_Flowrate_E1,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   VP_Feedrate_Percentage,
//   VP_SD_Print_ProbeOffsetZ,
//   0x0000
// };

// const uint16_t VPList_StepPerMM[] PROGMEM = {
//   VP_X_STEP_PER_MM,
//   VP_Y_STEP_PER_MM,
//   VP_Z_STEP_PER_MM,
//   #if HOTENDS >= 1
//     VP_E0_STEP_PER_MM,
//   #endif
//   #if HOTENDS >= 2
//     VP_E1_STEP_PER_MM,
//   #endif
//   0x0000
// };

// const uint16_t VPList_PIDE0[] PROGMEM = {
//   #if ENABLED(PIDTEMP)
//     VP_E0_PID_P,
//     VP_E0_PID_I,
//     VP_E0_PID_D,
//   #endif
//   0x0000
// };

// const uint16_t VPList_PIDBED[] PROGMEM = {
//   #if ENABLED(PIDTEMP)
//     VP_BED_PID_P,
//     VP_BED_PID_I,
//     VP_BED_PID_D,
//   #endif
//   0x0000
// };

// const uint16_t VPList_Infos[] PROGMEM = {
//   VP_MARLIN_VERSION,
//   VP_PrintTime,
//   #if ENABLED(PRINTCOUNTER)
//     VP_PrintAccTime,
//     VP_PrintsTotal,
//   #endif
//   0x0000
// };

// const uint16_t VPList_PIDTuningWaiting[] PROGMEM = {
//   VP_WAITING_STATUS,
//   0x0000
// };

// const uint16_t VPList_FLCPreheat[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_T_E0_Is, VP_T_E0_Set,
//   #endif
//   #if HAS_HEATED_BED
//     VP_T_Bed_Is, VP_T_Bed_Set,
//   #endif
//   0x0000
// };

// const uint16_t VPList_FLCPrinting[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_SD_Print_ProbeOffsetZ,
//   #endif
//   0x0000
// };

// const uint16_t VPList_Z_Offset[] PROGMEM = {
//   #if HOTENDS >= 1
//     VP_SD_Print_ProbeOffsetZ,
//   #endif
//   0x0000
// };

/* --------------------------------------------------MKS Data Updata-------------------------------------------------- */
const uint16_t MKSList_Home[] PROGMEM = {
/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Language */
    // VP_HOME_Dis,

    0x0000};

const uint16_t MKSList_Setting[] PROGMEM = {
/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Language */
    VP_Setting_Dis,
    0x0000};

const uint16_t MKSList_Tool[] PROGMEM = {
/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Language */
    VP_Tool_Dis,
    0x0000};

const uint16_t MKSList_EXTRUE[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    VP_Filament_distance,
    VP_Filament_speed,
    /* Language */

    0x0000};

const uint16_t MKSList_LEVEL[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Language */

    0x0000};

const uint16_t MKSList_MOVE[] PROGMEM = {
/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Language */
    0x0000};

const uint16_t MKSList_Print[] PROGMEM = {
/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,
    /* Print Persen*/
    VP_PrintProgress_Percentage,

    VP_PrintTime,

    VP_Flowrate_E0,
    VP_Flowrate_E1,
    VP_Feedrate_Percentage,

    VP_PrintTime_H,
    VP_PrintTime_M,
    VP_PrintTime_S,

    VP_XPos,
    VP_YPos,
    VP_ZPos,

    /* Language */

    0x0000};

const uint16_t MKSList_SD_File[] PROGMEM = {

    VP_SD_FileName0, VP_SD_FileName1,
    VP_SD_FileName2, VP_SD_FileName3,
    VP_SD_FileName4, VP_SD_FileName5,
    VP_SD_FileName6, VP_SD_FileName7,
    VP_SD_FileName8, VP_SD_FileName9,

    0x0000};

const uint16_t MKSList_TempOnly[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    0x0000};

const uint16_t MKSList_Pluse[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    /* Pluse */
    VP_X_STEP_PER_MM,
    VP_Y_STEP_PER_MM,
    VP_Z_STEP_PER_MM,
    VP_E0_STEP_PER_MM,
    VP_E1_STEP_PER_MM,

    0x0000};

const uint16_t MKSList_MaxSpeed[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    /* Pluse */

    VP_X_MAX_SPEED,
    VP_Y_MAX_SPEED,
    VP_Z_MAX_SPEED,
    VP_E0_MAX_SPEED,
    VP_E1_MAX_SPEED,

    0x0000};

const uint16_t MKSList_MaxAcc[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    /* ACC */
    VP_ACC_SPEED,
    VP_X_ACC_MAX_SPEED,
    VP_Y_ACC_MAX_SPEED,
    VP_Z_ACC_MAX_SPEED,
    VP_E0_ACC_MAX_SPEED,
    VP_E1_ACC_MAX_SPEED,

    0x0000};

const uint16_t MKSList_PID[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    /* PID */
    VP_E0_PID_P,
    VP_E0_PID_I,
    VP_E0_PID_D,

    0x0000};

const uint16_t MKSList_Level_Point[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    /* FAN      */
    VP_Fan0_Percentage,

    /* Level Point */
    VP_Level_Point_One_X,
    VP_Level_Point_One_Y,
    VP_Level_Point_Two_X,
    VP_Level_Point_Two_Y,
    VP_Level_Point_Three_X,
    VP_Level_Point_Three_Y,
    VP_Level_Point_Four_X,
    VP_Level_Point_Four_Y,
    VP_Level_Point_Five_X,
    VP_Level_Point_Five_Y,

    0x0000};

const uint16_t MKSList_Level_PrintConfig[] PROGMEM = {

    VP_T_E0_Set,
    VP_T_E1_Set,
    VP_T_Bed_Set,
    VP_Flowrate_E0,
    VP_Flowrate_E1,
    VP_Fan0_Percentage,
    VP_Feedrate_Percentage,

    0x0000
};

const uint16_t MKSList_PrintPauseConfig[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,

    VP_X_PARK_POS,
    VP_Y_PARK_POS,
    VP_Z_PARK_POS,

    0x0000};

const uint16_t MKSList_MotoConfig[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,

    VP_TRAVEL_SPEED,
    VP_FEEDRATE_MIN_SPEED,
    VP_T_F_SPEED,

    0x0000};

const uint16_t MKSList_EX_Config[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    VP_MIN_EX_T,VP_Min_EX_T_E,
    0x0000
};

const uint16_t MKSTMC_Config[] PROGMEM = {

/* E Temp   */
#if (EXTRUDERS >= 1)
    VP_T_E0_Is, VP_T_E0_Set,
#endif
#if (EXTRUDERS >= 2)
    VP_T_E1_Is, VP_T_E1_Set,
#endif
#if (EXTRUDERS >= 3)
    VP_T_E2_Is, VP_T_E2_Set,
#endif
#if (EXTRUDERS >= 4)
    VP_T_E3_Is, VP_T_E3_Set,
#endif
#if (EXTRUDERS >= 5)
    VP_T_E4_Is, VP_T_E4_Set,
#endif
#if (EXTRUDERS >= 6)
    VP_T_E5_Is, VP_T_E5_Set,
#endif
#if (EXTRUDERS >= 7)
    VP_T_E6_Is, VP_T_E6_Set,
#endif
#if (EXTRUDERS >= 8)
    VP_T_E7_Is, VP_T_E7_Set,
#endif
    /* HB Temp  */
    VP_T_Bed_Is, VP_T_Bed_Set,
    VP_MIN_EX_T,

    VP_TMC_X_STEP,
    VP_TMC_Y_STEP,
    VP_TMC_Z_STEP,
    VP_TMC_X1_Current,
    VP_TMC_Y1_Current,
    VP_TMC_X_Current,
    VP_TMC_Y_Current,
    VP_TMC_Z_Current,
    VP_TMC_E0_Current,
    VP_TMC_E1_Current,
    VP_TMC_Z1_Current,

    0x0000
};

const uint16_t MKSAuto_Level[] PROGMEM = {

    VP_MESH_LEVEL_POINT_DIS,
    VP_ZPos,
    0x0000
};


const uint16_t MKSOffset_Config[] PROGMEM = {

        /* E Temp   */
    #if (EXTRUDERS >= 1)
        VP_T_E0_Is, VP_T_E0_Set,
    #endif
    #if (EXTRUDERS >= 2)
        VP_T_E1_Is, VP_T_E1_Set,
    #endif
    #if (EXTRUDERS >= 3)
        VP_T_E2_Is, VP_T_E2_Set,
    #endif
    #if (EXTRUDERS >= 4)
        VP_T_E3_Is, VP_T_E3_Set,
    #endif
    #if (EXTRUDERS >= 5)
        VP_T_E4_Is, VP_T_E4_Set,
    #endif
    #if (EXTRUDERS >= 6)
        VP_T_E5_Is, VP_T_E5_Set,
    #endif
    #if (EXTRUDERS >= 7)
        VP_T_E6_Is, VP_T_E6_Set,
    #endif
    #if (EXTRUDERS >= 8)
        VP_T_E7_Is, VP_T_E7_Set,
    #endif

    VP_OFFSET_X,
    VP_OFFSET_Y,
    VP_OFFSET_Z,

    0x0000
};


const uint16_t MKSBabyStep[] PROGMEM = {

    VP_ZOffset_DE_DIS,
    0x0000
};


const uint16_t MKSList_About[] PROGMEM = {

    /* Marlin version */
    VP_MARLIN_VERSION,
    /* H43 Version */
    VP_MKS_H43_VERSION,
    VP_MKS_H43_UpdataVERSION,

    0x0000
};

/* Page data updata */
const struct VPMapping VPMap[] PROGMEM = {
#if 0
  { DGUSLCD_SCREEN_BOOT, VPList_Boot },
  { DGUSLCD_SCREEN_MAIN, VPList_Main },
  { DGUSLCD_SCREEN_TEMPERATURE, VPList_Temp },
  { DGUSLCD_SCREEN_STATUS, VPList_Status },
  { DGUSLCD_SCREEN_STATUS2, VPList_Status2 },
  { DGUSLCD_SCREEN_PREHEAT, VPList_Preheat },
  { DGUSLCD_SCREEN_MANUALMOVE, VPList_ManualMove },
  { DGUSLCD_SCREEN_MANUALEXTRUDE, VPList_ManualExtrude },
  { DGUSLCD_SCREEN_FILAMENT_HEATING, VPList_Filament_heating },
  { DGUSLCD_SCREEN_FILAMENT_LOADING, VPList_Filament_load_unload },
  { DGUSLCD_SCREEN_FILAMENT_UNLOADING, VPList_Filament_load_unload },
  { DGUSLCD_SCREEN_SDPRINTMANIPULATION, VPList_SD_PrintManipulation },
  { DGUSLCD_SCREEN_SDFILELIST, VPList_SDFileList },
  { DGUSLCD_SCREEN_SDPRINTTUNE, VPList_SDPrintTune },
  { DGUSLCD_SCREEN_WAITING, VPList_PIDTuningWaiting },
  { DGUSLCD_SCREEN_FLC_PREHEAT, VPList_FLCPreheat },
  { DGUSLCD_SCREEN_FLC_PRINTING, VPList_FLCPrinting },
  { DGUSLCD_SCREEN_Z_OFFSET, VPList_Z_Offset },
  { DGUSLCD_SCREEN_STEPPERMM, VPList_StepPerMM },
  { DGUSLCD_SCREEN_PID_E, VPList_PIDE0 },
  { DGUSLCD_SCREEN_PID_BED, VPList_PIDBED },
  { DGUSLCD_SCREEN_INFOS, VPList_Infos },
  { 0 , nullptr } // List is terminated with an nullptr as table entry.
#else
    {MKSLCD_SCREEN_BOOT, VPList_Boot},                          // Boot Page to show logo  0
    {MKSLCD_SCREEN_HOME, MKSList_Home},                         // Home, Page 1
    {MKSLCD_SCREEN_SETTING, MKSList_Setting},                   // Setting, Page 2
    {MKSLCD_SCREEM_TOOL, MKSList_Tool},                         // Page 3
    {MKSLCD_SCREEN_EXTRUDE_P1, MKSList_EXTRUE},                 // Page 4
    {MKSLCD_SCREEN_EXTRUDE_P2, MKSList_EXTRUE},                 // Page 11
    {MKSLCD_PAUSE_SETTING_EX, MKSList_EXTRUE},                  // Page 57
    {MKSLCD_PAUSE_SETTING_EX2, MKSList_EXTRUE},                 // Page 61
    {MKSLCD_SCREEN_LEVEL, MKSList_LEVEL},                       // Page 5
    {MKSLCD_SCREEN_MOVE, MKSList_MOVE},                         // Page 6
    {MKSLCD_SCREEN_PRINT, MKSList_Print},                       // Page 7
    {MKSLCD_SCREEN_PAUSE, MKSList_Print},                       // Page 26
    {MKSLCD_SCREEN_CHOOSE_FILE, MKSList_SD_File},               // Page 15
    {MKSLCD_SCREEN_MOTOR_PLUSE, MKSList_Pluse},                 // Page 51
    {MKSLCD_SCREEN_MOTOR_SPEED, MKSList_MaxSpeed},              // Page 55
    {MKSLCD_SCREEN_MOTOR_ACC_MAX, MKSList_MaxAcc},              // Page 53
    {MKSLCD_SCREEN_LEVEL_DATA, MKSList_Level_Point},            // Page 48
    {MKSLCD_PrintPause_SET, MKSList_PrintPauseConfig},          // Page 49
    {MKSLCD_FILAMENT_DATA, MKSList_SD_File},                    // Page 50
    {MKSLCD_SCREEN_Config, MKSList_TempOnly},                   // Page 46
    {MKSLCD_SCREEN_Config_MOTOR, MKSList_MotoConfig},           // Page 47
    {MKSLCD_PID, MKSList_PID},                                  // Page 56
    {MKSLCD_ABOUT, MKSList_About},                              // Page 36
    {MKSLCD_SCREEN_PRINT_CONFIG, MKSList_Level_PrintConfig},    // Page 60
    {MKSLCD_SCREEN_EX_CONFIG, MKSList_EX_Config},               // Page 65
    {MKSLCD_SCREEN_TMC_Config, MKSTMC_Config},                  // Page 70
    {MKSLCD_AUTO_LEVEL, MKSAuto_Level},                         // Page 73
    {MKSLCD_Screen_Offset_Config, MKSOffset_Config},            // Page 30
    {MKSLCD_Screen_PMove, MKSList_MOVE},                        // Page 64
    {MKSLCD_Screen_Baby, MKSBabyStep},                          // Page 71

    // { MKSLCD_SCREEN_LEVEL_DATA, MKSList_SD_File},  //
    // { MKSLCD_SCREEN_HOME, VPList_Boot },
    {0, nullptr} // List is terminated with an nullptr as table entry.
#endif
};

const char MarlinVersion[] PROGMEM = SHORT_BUILD_VERSION;
const char H43Version[] PROGMEM = "MKS H43_V1.30";
const char Updata_Time[] PROGMEM = STRING_DISTRIBUTION_DATE;

// Helper to define a DGUS_VP_Variable for common use cases.
#define VPHELPER(VPADR, VPADRVAR, RXFPTR, TXFPTR)                       \
  {                                                                     \
    .VP = VPADR, .memadr = VPADRVAR, .size = sizeof(VPADRVAR),          \
    .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR \
  }

// Helper to define a DGUS_VP_Variable when the sizeo of the var cannot be determined automaticalyl (eg. a string)
#define VPHELPER_STR(VPADR, VPADRVAR, STRLEN, RXFPTR, TXFPTR)           \
  {                                                                     \
    .VP = VPADR, .memadr = VPADRVAR, .size = STRLEN,                    \
    .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR \
  }

const struct DGUS_VP_Variable ListOfVP[] PROGMEM = {
    // Helper to detect touch events
    VPHELPER(VP_SCREENCHANGE, nullptr, ScreenHandler.ScreenChangeHook, nullptr),
    VPHELPER(VP_SCREENCHANGE_ASK, nullptr, ScreenHandler.ScreenChangeHookIfIdle, nullptr),
#if ENABLED(SDSUPPORT)
    VPHELPER(VP_SCREENCHANGE_WHENSD, nullptr, ScreenHandler.ScreenChangeHookIfSD, nullptr),
#endif
    VPHELPER(VP_CONFIRMED, nullptr, ScreenHandler.ScreenConfirmedOK, nullptr),
    /* Back Bottun */
    VPHELPER(VP_BACK_PAGE, nullptr, &ScreenHandler.ScreenBackChange, nullptr),
    VPHELPER(VP_TEMP_ALL_OFF, nullptr, &ScreenHandler.HandleAllHeatersOff, nullptr),


    VPHELPER(VP_MOVE_X, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Y, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Z, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_HOME_ALL, nullptr, &ScreenHandler.HandleManualMove, nullptr),

#if ENABLED(DGUS_LCD_UI_MKS)
    VPHELPER(VP_X_HOME, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_Y_HOME, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_Z_HOME, nullptr, &ScreenHandler.HandleManualMove, nullptr),

    VPHELPER(VP_MOVE_DISTANCE, &distanceMove, &ScreenHandler.GetManualMovestep, nullptr),

    VPHELPER(VP_MOTOR_LOCK_UNLOK, nullptr, &ScreenHandler.HandleManualMove, nullptr),
    VPHELPER(VP_LEVEL_POINT, nullptr, &ScreenHandler.ManualAssistLeveling, nullptr),
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
    VPHELPER(VP_POWER_LOSS_RECOVERY, nullptr, &ScreenHandler.HandlePowerLossRecovery, nullptr),
#endif
    VPHELPER(VP_SETTINGS, nullptr, &ScreenHandler.HandleSettings, nullptr),
#if ENABLED(SINGLE_Z_CALIBRATION)
    VPHELPER(VP_Z_CALIBRATE, nullptr, &ScreenHandler.HandleZCalibration, nullptr),
#endif
#if ENABLED(FIRST_LAYER_CAL)
    VPHELPER(VP_Z_FIRST_LAYER_CAL, nullptr, &ScreenHandler.HandleFirstLayerCal, nullptr),
#endif
    {.VP = VP_MARLIN_VERSION, .memadr = (void *)MarlinVersion, .size = VP_MARLIN_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},
    // M117 LCD String (We don't need the string in memory but "just" push it to the display on demand, hence the nullptr
    {.VP = VP_M117, .memadr = nullptr, .size = VP_M117_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplay},
    {.VP = VP_MKS_H43_VERSION, .memadr = (void *)H43Version, .size = VP_MKS_H43_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_MKS_H43_UpdataVERSION, .memadr = (void *)Updata_Time, .size = VP_MKS_H43_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},

// Temperature Data
#if HOTENDS >= 1
    VPHELPER(VP_T_E0_Is, &thermalManager.temp_hotend[0].celsius, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_E0_Set, &thermalManager.temp_hotend[0].target, ScreenHandler.HandleTemperatureChanged, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Flowrate_E0, &planner.flow_percentage[ExtUI::extruder_t::E0], ScreenHandler.HandleFlowRateChanged, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_EPos, &destination.e, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_MOVE_E0, nullptr, &ScreenHandler.HandleManualExtrude, nullptr),
    VPHELPER(VP_E0_CONTROL, &thermalManager.temp_hotend[0].target, &ScreenHandler.HandleHeaterControl, nullptr),
    VPHELPER(VP_E0_STATUS, &thermalManager.temp_hotend[0].target, nullptr, &ScreenHandler.DGUSLCD_SendHeaterStatusToDisplay),
#if ENABLED(DGUS_PREHEAT_UI)
    VPHELPER(VP_E0_BED_PREHEAT, nullptr, &ScreenHandler.HandlePreheat, nullptr),
#endif
#if ENABLED(PIDTEMP)
    VPHELPER(VP_E0_PID_P, &thermalManager.temp_hotend[0].pid.Kp, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_E0_PID_I, &thermalManager.temp_hotend[0].pid.Ki, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_E0_PID_D, &thermalManager.temp_hotend[0].pid.Kd, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_PID_AUTOTUNE_E0, nullptr, &ScreenHandler.HandlePIDAutotune, nullptr),
#endif
#if ENABLED(DGUS_FILAMENT_LOADUNLOAD)
    VPHELPER(VP_LOAD_Filament, nullptr, &ScreenHandler.MKS_FilamentLoad, nullptr),
    VPHELPER(VP_UNLOAD_Filament, nullptr, &ScreenHandler.MKS_FilamentUnLoad, nullptr),
    VPHELPER(VP_Filament_distance, &distanceFilament, &ScreenHandler.GetManualFilament, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Filament_speed, &FilamentSpeed, &ScreenHandler.GetManualFilamentSpeed, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
#endif
#endif

#if HOTENDS >= 2
    VPHELPER(VP_T_E1_Is, &thermalManager.temp_hotend[1].celsius, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_E1_Set, &thermalManager.temp_hotend[1].target, ScreenHandler.HandleTemperatureChanged, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Flowrate_E1, &planner.flow_percentage[ExtUI::extruder_t::E1], ScreenHandler.HandleFlowRateChanged, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_MOVE_E1, nullptr, &ScreenHandler.HandleManualExtrude, nullptr),
    VPHELPER(VP_E1_CONTROL, &thermalManager.temp_hotend[1].target, &ScreenHandler.HandleHeaterControl, nullptr),
    VPHELPER(VP_E1_STATUS, &thermalManager.temp_hotend[1].target, nullptr, &ScreenHandler.DGUSLCD_SendHeaterStatusToDisplay),

#if ENABLED(DGUS_FILAMENT_LOADUNLOAD)
    VPHELPER(VP_Filament_distance, &distanceFilament, &ScreenHandler.GetManualFilament, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Filament_speed, &FilamentSpeed, &ScreenHandler.GetManualFilamentSpeed, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
#endif

#if ENABLED(PIDTEMP)
    VPHELPER(VP_PID_AUTOTUNE_E1, nullptr, &ScreenHandler.HandlePIDAutotune, nullptr),
#endif
    VPHELPER(VP_E1_FILAMENT_LOAD_UNLOAD, nullptr, &ScreenHandler.HandleFilamentOption, &ScreenHandler.HandleFilamentLoadUnload),
#endif

#if HAS_HEATED_BED
    VPHELPER(VP_T_Bed_Is, &thermalManager.temp_bed.celsius, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_Bed_Set, &thermalManager.temp_bed.target, ScreenHandler.HandleTemperatureChanged, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_BED_CONTROL, &thermalManager.temp_bed.target, &ScreenHandler.HandleHeaterControl, nullptr),
    VPHELPER(VP_BED_STATUS, &thermalManager.temp_bed.target, nullptr, &ScreenHandler.DGUSLCD_SendHeaterStatusToDisplay),

#if ENABLED(PIDTEMPBED)
    VPHELPER(VP_BED_PID_P, &thermalManager.temp_bed.pid.Kp, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_BED_PID_I, &thermalManager.temp_bed.pid.Ki, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_BED_PID_D, &thermalManager.temp_bed.pid.Kd, ScreenHandler.HandleTemperaturePIDChanged, ScreenHandler.DGUSLCD_SendTemperaturePID),
    VPHELPER(VP_PID_AUTOTUNE_BED, nullptr, &ScreenHandler.HandlePIDAutotune, nullptr),
#endif

// Fan Data
#endif
#if HAS_FAN

#if !ENABLED(DGUS_LCD_UI_MKS)
#define FAN_VPHELPER(N)                                                                                                                                    \
  VPHELPER(VP_Fan##N##_Percentage, &thermalManager.fan_speed[N], ScreenHandler.DGUSLCD_PercentageToUint8, &ScreenHandler.DGUSLCD_SendPercentageToDisplay), \
      VPHELPER(VP_FAN##N##_CONTROL, &thermalManager.fan_speed[N], &ScreenHandler.HandleFanControl, nullptr),                                               \
      VPHELPER(VP_FAN##N##_STATUS, &thermalManager.fan_speed[N], nullptr, &ScreenHandler.DGUSLCD_SendFanStatusToDisplay),
    REPEAT(FAN_COUNT, FAN_VPHELPER)
#else
#define FAN_VPHELPER(N)                                                                                                                    \
  VPHELPER(VP_Fan##N##_Percentage, &thermalManager.fan_speed[N], ScreenHandler.DGUSLCD_SetUint8, &ScreenHandler.DGUSLCD_SendFanToDisplay), \
      VPHELPER(VP_FAN##N##_CONTROL, &thermalManager.fan_speed[N], &ScreenHandler.HandleFanControl, nullptr),                               \
      VPHELPER(VP_FAN##N##_STATUS, &thermalManager.fan_speed[N], nullptr, &ScreenHandler.DGUSLCD_SendFanStatusToDisplay),
    REPEAT(FAN_COUNT, FAN_VPHELPER)
#endif

#endif

    // Feedrate
    VPHELPER(VP_Feedrate_Percentage, &feedrate_percentage, ScreenHandler.DGUSLCD_SetValueDirectly<int16_t>, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),

    // Position Data
    VPHELPER(VP_XPos, &current_position.x, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_YPos, &current_position.y, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_ZPos, &current_position.z, nullptr, ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),

    // Level Point Set
    VPHELPER(VP_Level_Point_One_X, &level_1_x_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_One_Y, &level_1_y_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Two_X, &level_2_x_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Two_Y, &level_2_y_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Three_X, &level_3_x_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Three_Y, &level_3_y_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Four_X, &level_4_x_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Four_Y, &level_4_y_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Five_X, &level_5_x_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Level_Point_Five_Y, &level_5_y_point, ScreenHandler.HandleChangeLevelPoint_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),

    // Print Progress
    VPHELPER(VP_PrintProgress_Percentage, nullptr, nullptr, ScreenHandler.DGUSLCD_SendPrintProgressToDisplay),

#if !ENABLED(DGUS_LCD_UI_MKS)
    // Print Time
    VPHELPER_STR(VP_PrintTime, nullptr, VP_PrintTime_LEN, nullptr, ScreenHandler.DGUSLCD_SendPrintTimeToDisplay),

#else
    VPHELPER_STR(VP_PrintTime, nullptr, VP_PrintTime_LEN, nullptr, ScreenHandler.DGUSLCD_SendPrintTimeToDisplay_MKS),
#endif

#if ENABLED(PRINTCOUNTER)
    VPHELPER_STR(VP_PrintAccTime, nullptr, VP_PrintAccTime_LEN, nullptr, ScreenHandler.DGUSLCD_SendPrintAccTimeToDisplay),
    VPHELPER_STR(VP_PrintsTotal, nullptr, VP_PrintsTotal_LEN, nullptr, ScreenHandler.DGUSLCD_SendPrintsTotalToDisplay),
#endif

#if ENABLED(DGUS_LCD_UI_MKS)
    VPHELPER(VP_X_STEP_PER_MM, &planner.settings.axis_steps_per_mm[X_AXIS], ScreenHandler.HandleStepPerMMChanged_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Y_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Y_AXIS], ScreenHandler.HandleStepPerMMChanged_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Z_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Z_AXIS], ScreenHandler.HandleStepPerMMChanged_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),

    VPHELPER(VP_X_MAX_SPEED, &planner.settings.max_feedrate_mm_s[X_AXIS], ScreenHandler.HandleMaxSpeedChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Y_MAX_SPEED, &planner.settings.max_feedrate_mm_s[Y_AXIS], ScreenHandler.HandleMaxSpeedChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_Z_MAX_SPEED, &planner.settings.max_feedrate_mm_s[Z_AXIS], ScreenHandler.HandleMaxSpeedChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),

    #if HOTENDS >= 1
        VPHELPER(VP_E0_MAX_SPEED, &planner.settings.max_feedrate_mm_s[E0_AXIS], ScreenHandler.HandleExtruderMaxSpeedChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    #endif
    #if HOTENDS >= 2
        VPHELPER(VP_E1_MAX_SPEED, &planner.settings.max_feedrate_mm_s[E1_AXIS], ScreenHandler.HandleExtruderMaxSpeedChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    #endif

    VPHELPER(VP_X_ACC_MAX_SPEED, (uint16_t *)&planner.settings.max_acceleration_mm_per_s2[X_AXIS], ScreenHandler.HandleMaxAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Y_ACC_MAX_SPEED, (uint16_t *)&planner.settings.max_acceleration_mm_per_s2[Y_AXIS], ScreenHandler.HandleMaxAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Z_ACC_MAX_SPEED, (uint16_t *)&planner.settings.max_acceleration_mm_per_s2[Z_AXIS], ScreenHandler.HandleMaxAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),

    #if HOTENDS >= 1
        VPHELPER(VP_E0_ACC_MAX_SPEED, (uint16_t *)&planner.settings.max_acceleration_mm_per_s2[E0_AXIS], ScreenHandler.HandleExtruderAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    #endif
    #if HOTENDS >= 2
        VPHELPER(VP_E1_ACC_MAX_SPEED, (uint16_t *)&planner.settings.max_acceleration_mm_per_s2[E1_AXIS], ScreenHandler.HandleExtruderAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    #endif

    VPHELPER(VP_TRAVEL_SPEED, (uint16_t *)&planner.settings.travel_acceleration, ScreenHandler.HandleTravelAccChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_FEEDRATE_MIN_SPEED, (uint16_t *)&planner.settings.min_feedrate_mm_s, ScreenHandler.HandleFeedRateMinChange_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_T_F_SPEED, (uint16_t *)&planner.settings.min_travel_feedrate_mm_s, ScreenHandler.HandleMin_T_F_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    VPHELPER(VP_ACC_SPEED, (uint16_t *)&planner.settings.acceleration, ScreenHandler.HandleAccChange_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),

    VPHELPER(VP_X_PARK_POS, &x_park_pos, ScreenHandler.GetParkPos_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Y_PARK_POS, &y_park_pos, ScreenHandler.GetParkPos_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Z_PARK_POS, &z_park_pos, ScreenHandler.GetParkPos_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_MIN_EX_T, &thermalManager.extrude_min_temp, ScreenHandler.HandleGetExMinTemp_MKS, ScreenHandler.DGUSLCD_SendWordValueToDisplay),

    #if ENABLED(SENSORLESS_HOMING)  // TMC SENSORLESS Setting
        #if AXIS_HAS_STEALTHCHOP(X)
            VPHELPER(VP_TMC_X_STEP, &tmc_x_step, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendTMCStepValue),
        #endif
        #if AXIS_HAS_STEALTHCHOP(Y)
            VPHELPER(VP_TMC_Y_STEP, &tmc_y_step, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendTMCStepValue),
        #endif
        #if AXIS_HAS_STEALTHCHOP(Z)
            VPHELPER(VP_TMC_Z_STEP, &tmc_z_step, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendTMCStepValue),
        #endif
    #endif

    #if HAS_TRINAMIC_CONFIG   // TMC Current Setting
        #if AXIS_IS_TMC(X)
            VPHELPER(VP_TMC_X_Current, &stepperX.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(Y)
            VPHELPER(VP_TMC_Y_Current, &stepperY.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(Z)
            VPHELPER(VP_TMC_Z_Current, &stepperZ.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(E0)
            VPHELPER(VP_TMC_E0_Current, &stepperE0.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(E1)
            VPHELPER(VP_TMC_E1_Current, &stepperE1.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(X2)
            VPHELPER(VP_TMC_X1_Current, &stepperX2.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(Y2)
            VPHELPER(VP_TMC_Y1_Current, &stepperY2.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif

        #if AXIS_IS_TMC(Z2)
            VPHELPER(VP_TMC_Z1_Current, &stepperZ2.val_mA, ScreenHandler.TMC_ChangeConfig, ScreenHandler.DGUSLCD_SendWordValueToDisplay),
        #endif
    #endif

    VPHELPER(VP_EEPROM_CTRL, nullptr, ScreenHandler.EEPROM_CTRL, nullptr),
    VPHELPER(VP_LEVEL_BUTTON, nullptr, ScreenHandler.Level_Ctrl_MKS, nullptr),
    VPHELPER(VP_LANGUAGE_CHANGE, nullptr, ScreenHandler.LanguageChange_MKS, nullptr),

    // VPHELPER(VP_SD_Print_LiveAdjustZ, nullptr, ScreenHandler.HandleLiveAdjustZ, nullptr),

    VPHELPER(VP_SD_Print_LiveAdjustZ_Confirm, nullptr, ScreenHandler.ZoffsetConfirm, nullptr),

    VPHELPER(VP_ZOffset_Distance,nullptr ,ScreenHandler.GetZoffsetDistance, nullptr),
    VPHELPER(VP_MESH_LEVEL_ADJUST, nullptr, ScreenHandler.MeshLevelDistanceConfig, nullptr),
    VPHELPER(VP_MESH_LEVEL_POINT,nullptr, ScreenHandler.MeshLevel,nullptr),
    VPHELPER(VP_MESH_LEVEL_POINT_DIS,&current_position.z,nullptr,ScreenHandler.DGUSLCD_SendFloatByStringToDisplay),
    VPHELPER(VP_Min_EX_T_E, &thermalManager.extrude_min_temp, &ScreenHandler.GetMinExtrudeTemp, &ScreenHandler.DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_AutoTurnOffSw, nullptr, &ScreenHandler.GetTurnOffCtrl, nullptr),

    #if HOTENDS >= 1
        VPHELPER(VP_E0_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(0)], ScreenHandler.HandleStepPerMMExtruderChanged_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    #endif
    #if HOTENDS >= 2
        VPHELPER(VP_E1_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(1)], ScreenHandler.HandleStepPerMMExtruderChanged_MKS, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
    #endif

#else
    VPHELPER(VP_X_STEP_PER_MM, &planner.settings.axis_steps_per_mm[X_AXIS], ScreenHandler.HandleStepPerMMChanged, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<1>),
    VPHELPER(VP_Y_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Y_AXIS], ScreenHandler.HandleStepPerMMChanged, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<1>),
    VPHELPER(VP_Z_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Z_AXIS], ScreenHandler.HandleStepPerMMChanged, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<1>),

#if HOTENDS >= 1
    VPHELPER(VP_E0_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(0)], ScreenHandler.HandleStepPerMMExtruderChanged, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<0>),
#endif
#if HOTENDS >= 2
    VPHELPER(VP_E1_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(1)], ScreenHandler.HandleStepPerMMExtruderChanged, ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<1>),
#endif
#endif

// SDCard File listing.
#if ENABLED(SDSUPPORT)
    VPHELPER(VP_SD_ScrollEvent, nullptr, ScreenHandler.DGUSLCD_SD_ScrollFilelist, nullptr),
    VPHELPER(VP_SD_FileSelected, nullptr, ScreenHandler.DGUSLCD_SD_FileSelected, nullptr),
    VPHELPER(VP_SD_FileSelectConfirm, nullptr, ScreenHandler.DGUSLCD_SD_StartPrint, nullptr),
    VPHELPER_STR(VP_SD_FileName0, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName1, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName2, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName3, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName4, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName5, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName6, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName7, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName8, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER_STR(VP_SD_FileName9, nullptr, VP_SD_FileName_LEN, nullptr, ScreenHandler.DGUSLCD_SD_SendFilename),
    VPHELPER(VP_SD_ResumePauseAbort, nullptr, ScreenHandler.DGUSLCD_SD_ResumePauseAbort, nullptr),
    VPHELPER(VP_SD_AbortPrintConfirmed, nullptr, ScreenHandler.DGUSLCD_SD_ReallyAbort, nullptr),
    VPHELPER(VP_SD_Print_Setting, nullptr, ScreenHandler.DGUSLCD_SD_PrintTune, nullptr),


    #if ENABLED(BABYSTEPPING)
        VPHELPER(VP_SD_Print_LiveAdjustZ,nullptr, ScreenHandler.HandleLiveAdjustZ, &ScreenHandler.DGUSLCD_SendFloatAsIntValueToDisplay<2>),
        VPHELPER(VP_ZOffset_DE_DIS,&z_offset_add,nullptr, &ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    #endif

#if HAS_BED_PROBE
    VPHELPER(VP_OFFSET_X, &probe.offset.x, ScreenHandler.GetOffsetValue,ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_OFFSET_Y, &probe.offset.y, ScreenHandler.GetOffsetValue,ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_OFFSET_Z, &probe.offset.z, ScreenHandler.GetOffsetValue,ScreenHandler.DGUSLCD_SendFloatAsLongValueToDisplay<2>),
#endif
#endif

#if ENABLED(DGUS_UI_WAITING)
    VPHELPER(VP_WAITING_STATUS, nullptr, nullptr, ScreenHandler.DGUSLCD_SendWaitingStatusToDisplay),
#endif

    // Messages for the User, shared by the popup and the kill screen. They cant be autouploaded as we do not buffer content.
    // {.VP = VP_MSGSTR1, .memadr = nullptr, .size = VP_MSGSTR1_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},
    // {.VP = VP_MSGSTR2, .memadr = nullptr, .size = VP_MSGSTR2_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},
    // {.VP = VP_MSGSTR3, .memadr = nullptr, .size = VP_MSGSTR3_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},
    // {.VP = VP_MSGSTR4, .memadr = nullptr, .size = VP_MSGSTR4_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplayPGM},

    {.VP = VP_MSGSTR1, .memadr = nullptr, .size = VP_MSGSTR1_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplay_Language_MKS},
    {.VP = VP_MSGSTR2, .memadr = nullptr, .size = VP_MSGSTR2_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplay_Language_MKS},
    {.VP = VP_MSGSTR3, .memadr = nullptr, .size = VP_MSGSTR3_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplay_Language_MKS},
    {.VP = VP_MSGSTR4, .memadr = nullptr, .size = VP_MSGSTR4_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &ScreenHandler.DGUSLCD_SendStringToDisplay_Language_MKS},

    VPHELPER(0, 0, 0, 0) // must be last entry.
};

// Common Button Set

#endif // DGUS_LCD_UI_MKS
