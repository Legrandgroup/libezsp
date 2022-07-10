/**
 * @file cluster-attribute.h
 *
 * @brief Representes all attributes for all cluster.
 */

#pragma once

#include <map>
#include <iostream>
#include <cassert>

namespace NSEZSP {
    std::map<int32_t, std::string> ELECTRICAL_MESUREMENT_ATTRIBUTE = {
        { 0x0300, "Non-phase Specific, ACFrequency : " },
        { 0x0301, "Non-phase Specific, ACFrequencyMin : " },
        { 0x0302, "Non-phase Specific, ACFrequencyMax : " },
        { 0x0303, "Non-phase Specific, NeutralCurrent : " },
        { 0x0304, "Non-phase Specific, TotalActivePower : " },
        { 0x0305, "Non-phase Specific, TotalReactivePower : " },
        { 0x0306, "Non-phase Specific, TotalApparentPower : " },
        { 0x0307, "Non-phase Specific, Measured1stHarmonicCurrent : " },
        { 0x0308, "Non-phase Specific, Measured3rdHarmonicCurrent : " },
        { 0x0309, "Non-phase Specific, Measured5thHarmonicCurrent : " },
        { 0x030A, "Non-phase Specific, Measured7thHarmonicCurrent : " },
        { 0x030B, "Non-phase Specific, Measured9thHarmonicCurrent : " },
        { 0x030C, "Non-phase Specific, Measured11thHarmonicCurrent : " },
        { 0x030D, "Non-phase Specific, MeasuredPhase1stHarmonicCurrent : " },
        { 0x030E, "Non-phase Specific, MeasuredPhase3rdHarmonicCurrent : " },
        { 0x030F, "Non-phase Specific, MeasuredPhase5thHarmonicCurrent : " },
        { 0x0310, "Non-phase Specific, MeasuredPhase7thHarmonicCurrent : " },
        { 0x0311, "Non-phase Specific, MeasuredPhase9thHarmonicCurrent : " },
        { 0x0312, "Non-phase Specific, MeasuredPhase11thHarmonicCurrent : " },

        { 0x0500, "Phase A, Reserved" },
        { 0x0501, "Phase A, LineCurrent : " },
        { 0x0502, "Phase A, ActiveCurrent : " },
        { 0x0503, "Phase A, ReactiveCurrent : " },
        { 0x0504, "Phase A, RMSVoltage : " },
        { 0x0505, "Phase A, RMSVoltage : " },
        { 0x0506, "Phase A, RMSVoltageMin : " },
        { 0x0507, "Phase A, RMSVoltageMax : " },
        { 0x0508, "Phase A, RMSCurrent : " },
        { 0x0509, "Phase A, RMSCurrentMin : " },
        { 0x050A, "Phase A, RMSCurrentMax : " },
        { 0x050B, "Phase A, ActivePower : " },
        { 0x050C, "Phase A, ActivePowerMin : " },
        { 0x050D, "Phase A, ActivePowerMax : " },
        { 0x050E, "Phase A, ReactivePower : " },
        { 0x050F, "Phase A, ApparentPower : " },
        { 0x0510, "Phase A, PowerFactor : " },
        { 0x0511, "Phase A, AverageRMSVoltageMeasurementPeriod : " },
        { 0x0512, "Phase A, AverageRMSOverVoltageCounter : " },
        { 0x0513, "Phase A, AverageRMSUnderVoltageCounter : " },
        { 0x0514, "Phase A, RMSExtremeOverVoltagePeriod : " },
        { 0x0515, "Phase A, RMSExtremeUnderVoltagePeriod : " },
        { 0x0516, "Phase A, RMSVoltageSagPeriod : " },
        { 0x0517, "Phase A, RMSVoltageSwellPeriod : " },


        { 0x0901, "Phase B, LineCurrentPhB : " },
        { 0x0902, "Phase B, ActiveCurrentPhB : " },
        { 0x0903, "Phase B, ReactiveCurrentPhB : " },
        { 0x0904, "Phase B, Reserved" },
        { 0x0905, "Phase B, RMSVoltagePhB : " },
        { 0x0906, "Phase B, RMSVoltageMinPhB : " },
        { 0x0907, "Phase B, RMSVoltageMaxPhB : " },
        { 0x0908, "Phase B, RMSCurrentPhB : " },
        { 0x0909, "Phase B, RMSCurrentMinPhB : " },
        { 0x090A, "Phase B, RMSCurrentMaxPhB : " },
        { 0x090B, "Phase B, ActivePowerPhB : " },
        { 0x090C, "Phase B, ActivePowerMinPhB : " },
        { 0x090D, "Phase B, ActivePowerMaxPhB : " },
        { 0x090E, "Phase B, ReactivePowerPhB : " },
        { 0x090F, "Phase B, ApparentPowerPhB : " },
        { 0x0910, "Phase B, PowerFactorPhB : " },
        { 0x0911, "Phase B, AverageRMSVoltageMeasurementPeriodPhB : " },
        { 0x0912, "Phase B, AverageRMSOverVoltageCounterPhB : " },
        { 0x0913, "Phase B, AverageRMSUnderVoltageCounterPhB : " },
        { 0x0914, "Phase B, RMSExtremeOverVoltagePeriodPhB : " },
        { 0x0915, "Phase B, RMSExtremeUnderVoltagePeriodPhB : " },
        { 0x0916, "Phase B, RMSVoltageSagPeriodPhB : " },
        { 0x0917, "Phase B, RMSVoltageSwellPeriodPhB : " },


        { 0x0A01, "Phase C, LineCurrentPhC : " },
        { 0x0A02, "Phase C, ActiveCurrentPhC : " },
        { 0x0A03, "Phase C, ReactiveCurrentPhC : " },
        { 0x0A05, "Phase C, RMSVoltagePhC : " },
        { 0x0A06, "Phase C, RMSVoltageMinPhC : " },
        { 0x0A07, "Phase C, RMSVoltageMaxPhC : " },
        { 0x0A08, "Phase C, RMSCurrentPhC : " },
        { 0x0A09, "Phase C, RMSCurrentMinPhC : " },
        { 0x0A0A, "Phase C, RMSCurrentMaxPhC : " },
        { 0x0A0B, "Phase C, ActivePowerPhC : " },
        { 0x0A0C, "Phase C, ActivePowerMinPhC : " },
        { 0x0A0D, "Phase C, ActivePowerMaxPhC : " },
        { 0x0A0E, "Phase C, ReactivePowerPhC : " },
        { 0x0A0F, "Phase C, ApparentPowerPhC : " },
        { 0x0A10, "Phase C, PowerFactorPhC : " },
        { 0x0A11, "Phase C, AverageRMSVoltageMeasurementPeriodPhC : " },
        { 0x0A12, "Phase C, AverageRMSOverVoltageCounterPhC : " },
        { 0x0A13, "Phase C, AverageRMSUnderVoltageCounterPhC : " },
        { 0x0A14, "Phase C, RMSExtremeOverVoltagePeriodPhC : " },
        { 0x0A15, "Phase C, RMSExtremeUnderVoltagePeriodPhC : " },
        { 0x0A16, "Phase C, RMSVoltageSagPeriodPhC : " },
        { 0x0A17, "Phase C, RMSVoltageSwellPeriodPhC : " },
    };
}