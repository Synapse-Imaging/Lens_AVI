#pragma once
#include "../FAIDefine.h"

// ============================================================
//  FAIDynamicRule.h
//
//  Defines rule structs for the dynamic FAI measurement engine.
//  P5 rules compute point-to-point distances or inner radii.
//  P7 rules compute line-to-line angles.
//
//  Rules are loaded from INI files at runtime, allowing
//  measurement items to be added without recompiling.
//  File: Data/FaiDynamicRules_{ModelName}.ini
// ============================================================

// P5 measurement type
enum class DynamicP5Type
{
    POINT_DISTANCE = 0,   // Distance between two center points (DistancePp)
    INNER_RADIUS   = 1,   // Inner circle radius of a region (InnerCircle)
};

// P5: distance / radius rule
struct FAIDynamicP5Rule
{
    int          faiId  = 0;                          // Target FAI slot (0 ~ TOTAL_FAI_NUMBER-1)
    DynamicP5Type type  = DynamicP5Type::POINT_DISTANCE;
    enMeasurePos  posA  = enMeasurePos::MEASURE_POSITION_NONE;  // Point A (or region) position
    int           idxA  = 0;                          // Point A index (0 ~ MAX_MEASURE_POINT_NUM-1)
    enMeasurePos  posB  = enMeasurePos::MEASURE_POSITION_NONE;  // Point B position (POINT_DISTANCE only)
    int           idxB  = 0;                          // Point B index
    double        multiple = 1.0;                     // Calibration multiplier
    double        offset   = 0.0;                     // Calibration offset (mm)
};

// P7: line-line angle rule
struct FAIDynamicP7Rule
{
    int          faiId     = 0;
    enMeasurePos posL1[2]  = { enMeasurePos::MEASURE_POSITION_NONE,
                               enMeasurePos::MEASURE_POSITION_NONE }; // Line1: [0]=start, [1]=end
    int          idxL1[2]  = { 0, 1 };
    enMeasurePos posL2[2]  = { enMeasurePos::MEASURE_POSITION_NONE,
                               enMeasurePos::MEASURE_POSITION_NONE }; // Line2: [0]=start, [1]=end
    int          idxL2[2]  = { 0, 1 };
    bool         bAbsAngle = true;   // If true, result = abs(angle_deg)
    double       multiple  = 1.0;
    double       offset    = 0.0;
};
