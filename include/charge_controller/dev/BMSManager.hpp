#ifndef EXAMPLE_BMSMANAGER_HPP
#define EXAMPLE_BMSMANAGER_HPP

#include <stdint.h>
#include <EVT/io/CAN.hpp>
#include <EVT/io/GPIO.hpp>
#include <Canopen/co_obj.h>

#include <charge_controller/Logger.h>
#include <Canopen/co_pdo.h>

//RPDO0-1 settings
// 0: RPDO number in index and total number of sub indexes.
// 1: The COB-ID to receive PDOs from.
// 2: transmission trigger
#define GEN_PACK_RPDO_CONFIG(NUM, ID)                                        \
    {                                                                       \
        .Key = CO_KEY(0x1400+2*NUM, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 3,                                              \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1400+2*NUM, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) CO_COBID_TPDO_DEFAULT(0) + ID,                  \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1400+2*NUM, 2, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 0xFE,                                           \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1401+2*NUM, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 3,                                              \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1401+2*NUM, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) CO_COBID_TPDO_DEFAULT(0) + ID + 1,              \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1401+2*NUM, 2, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 0xFE,                                           \
    },


// RPDO0-1 mapping, determines the PDO messages to send when TPDO1 is triggered
// 0: The number of PDO messages associated with the TPDO
// 1: Link to the first PDO message
// n: Link to the nth PDO message
#define GEN_PACK_RPDO_MAP(NUM)                                               \
    {                                                                       \
        .Key = CO_KEY(0x1600+2*NUM, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 5,                                              \
    },                                                                      \
    { /* Battery Voltage  */                                                \
        .Key = CO_KEY(0x1600+2*NUM, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 1, 16),                                 \
    },                                                                      \
    { /* Min Cell Voltage  */                                               \
        .Key = CO_KEY(0x1600+2*NUM, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 2, 16),                                 \
    },                                                                      \
    { /* Min Cell Voltage ID  */                                            \
        .Key = CO_KEY(0x1600+2*NUM, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 3, 8),                                  \
    },                                                                      \
    { /* Max Cell Voltage  */                                               \
        .Key = CO_KEY(0x1600+2*NUM, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 4, 16),                                 \
    },                                                                      \
    { /* Max Cell Voltage ID  */                                            \
        .Key = CO_KEY(0x1600+2*NUM, 5, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 5, 8),                                  \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x1601+2*NUM, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),        \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) 7,                                              \
    },                                                                      \
    { /* Current - Unused  */                                               \
        .Key = CO_KEY(0x1601+2*NUM, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x5000, 0, 8),                                      \
    },                                                                      \
    { /* Pack Min Temp  */                                                  \
        .Key = CO_KEY(0x1601+2*NUM, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 7, 8),                                  \
    },                                                                      \
    { /* Pack Max Temp  */                                                  \
        .Key = CO_KEY(0x1601+2*NUM, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 8, 8),                                  \
    },                                                                      \
    { /* SOC - Unused  */                                                   \
        .Key = CO_KEY(0x1601+2*NUM, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x5000, 0, 8),                                      \
    },                                                                      \
    { /* State  */                                                          \
        .Key = CO_KEY(0x1601+2*NUM, 5, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x2100+NUM, 10, 8),                                 \
    },                                                                      \
    { /*RecapActualAllowed - Unused  */                                     \
        .Key = CO_KEY(0x1601+2*NUM, 6, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x5000, 0, 8),                                      \
    },                                                                      \
    { /*DischargeActualAllowed - Unused  */                                 \
        .Key = CO_KEY(0x1601+2*NUM, 7, CO_UNSIGNED32 | CO_OBJ_D__R_),       \
        .Type = nullptr,                                                    \
        .Data = CO_LINK(0x5000, 0, 8),                                      \
    },

#define GEN_PACK_DATA(NUM, VAR)                                             \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 1, CO_UNSIGNED16 | CO_OBJ___PR_),         \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.batteryVoltage,                            \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 2, CO_UNSIGNED16 | CO_OBJ___PR_),         \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.minCellVoltage,                            \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 3, CO_UNSIGNED8 | CO_OBJ___PR_),          \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.minCellVoltageID,                          \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 4, CO_UNSIGNED16 | CO_OBJ___PR_),         \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.maxCellVoltage,                            \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 5, CO_UNSIGNED8 | CO_OBJ___PR_),          \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.maxCellVoltageID,                          \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 7, CO_SIGNED8 | CO_OBJ___PR_),            \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.batteryPackMinTemp,                        \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 8, CO_SIGNED8 | CO_OBJ___PR_),            \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.batteryPackMaxTemp,                        \
    },                                                                      \
    {                                                                       \
        .Key = CO_KEY(0x2100+NUM, 10, CO_UNSIGNED8 | CO_OBJ___PR_),         \
        .Type = nullptr,                                                    \
        .Data = (uintptr_t) &VAR.status,                                    \
    },

namespace IO = EVT::core::IO;

class BMSManager {
    static constexpr uint32_t TOTAL_VOLTAGE_ID = 0x2101;
    static constexpr uint32_t STATE_ID = 0x2102;
public:
    enum class BMSStatus {
        /// When the BMS is powered on
        START = 0,
        /// When the BMS fails startup sequence
        INITIALIZATION_ERROR = 1,
        /// When the system is waiting for settings to be sent to the BMS
        FACTORY_INIT = 2,
        /// When the BMS is actively sending settings over to the BQ
        TRANSFER_SETTINGS = 3,
        /// When the BMS is ready for charging / discharging
        SYSTEM_READY = 4,
        /// When the system is running in a low power mode
        DEEP_SLEEP = 5,
        /// When a fault is detected during normal operation
        UNSAFE_CONDITIONS_ERROR = 6,
        /// When the BMS is on the bike and delivering power
        POWER_DELIVERY = 7,
        /// When the BMS is handling charging the battery pack
        CHARGING = 8
    };

    BMSManager(IO::CAN &can, IO::GPIO *bmsOK[]);

//    void updateBMSData(IO::CANMessage &message);
//
//    int getBattVoltage() { return millivolts; };
//
//    int getBattTemperature() { return 0; };
//
//    BMSStatus getBMSState() { return state; };
//
//    int getStateOfCharge();

    bool isConnected(uint8_t packNum);

    uint8_t numConnected();

    bool faultDetected(uint8_t packNum);

    bool isCharging(uint8_t packNum);

    bool isReady(uint8_t packNum);

    int16_t getBatteryVoltage(uint8_t packNum);

    int16_t getMinCellVoltage(uint8_t packNum);

    uint8_t getMinCellVoltageID(uint8_t packNum);

    int16_t getMaxCellVoltage(uint8_t packNum);

    uint8_t getMaxCellVoltageID(uint8_t packNum);

    int8_t getBatteryMinTemp(uint8_t packNum);

    int8_t getBatteryMaxTemp(uint8_t packNum);

    BMSStatus getStatus(uint8_t packNum);

    void printDebug();

    void update();

    /**
     * Get a pointer to the start of the object dictionary
     *
     * @return Pointer to the start of the object dictionary
     */
    CO_OBJ_T* getObjectDictionary();

    /**
     * Get the number of elements in the object dictionary.
     *
     * @return The number of elements in the object dictionary
     */
    uint8_t getNumElements();

    static constexpr uint8_t MAX_BMS_PACKS = 1;
private:
    /**
     * State for representing the BMS is in not in an OK state to charge/discharge
     */
    static constexpr EVT::core::IO::GPIO::State BMS_OK =
            EVT::core::IO::GPIO::State::HIGH;

    /**
     * State for representing the BMS is in not in an OK state to charge/discharge
     */
    static constexpr EVT::core::IO::GPIO::State BMS_NOT_OK =
            EVT::core::IO::GPIO::State::LOW;

    IO::CAN *can = nullptr;
    IO::GPIO *bmsOK[MAX_BMS_PACKS];
//    IO::GPIO &bmsOK2;

//    uint32_t millivolts = 0; //TODO Replace
//    uint32_t temperature = 5; //TODO Replace
//    BMSState state = BMSState::START; //TODO Replace

    struct BMSData {
        int16_t batteryVoltage;
        int16_t minCellVoltage;
        uint8_t minCellVoltageID;
        int16_t maxCellVoltage;
        uint8_t maxCellVoltageID;

        int8_t batteryPackMinTemp;
        int8_t batteryPackMaxTemp;
        BMSStatus status;
    };

    struct BMSConfig {
        uint8_t node_ID;
        bool isConnected;
        BMSData data;
    };

    struct BMSConfig packs[MAX_BMS_PACKS] = {
            {0x05, false},
//            {0x24, false}
    };

    BMSData lastValues[MAX_BMS_PACKS];

    uint8_t dummy = 0;

    static constexpr uint8_t NODE_ID = 0x0F;
    static constexpr uint16_t OBJECT_DICTIONARY_SIZE = 36;

    CO_OBJ_T objectDictionaryBMS[OBJECT_DICTIONARY_SIZE + 1] = {
        // Sync ID, defaults to 0x80
        {
            .Key = CO_KEY(0x1005, 0, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x80,
        },

        //Heartbeat Producer
//        {
//            .Key = CO_KEY(0x1017, 0, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 500,
//        },

        // Information about the hardware, hard coded sample values for now
        // 1: Vendor ID
        // 2: Product Code
        // 3: Revision Number
        // 4: Serial Number
        {
            .Key = CO_KEY(0x1018, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x10},
        {
            .Key = CO_KEY(0x1018, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x11,
        },
        {
            .Key = CO_KEY(0x1018, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x12,
        },
        {
            .Key = CO_KEY(0x1018, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x13,
        },

        // SDO CAN message IDS.
        // 1: Client -> Server ID, default is 0x600 + NODE_ID
        // 2: Server -> Client ID, default is 0x580 + NODE_ID
        {
            .Key = CO_KEY(0x1200, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x600 + NODE_ID,
        },
        {
            .Key = CO_KEY(0x1200, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
            .Type = nullptr,
            .Data = (uintptr_t) 0x580 + NODE_ID,
        },

//        //RPDO0 settings
//        // 0: RPDO number in index and total number of sub indexes.
//        // 1: The COB-ID to receive PDOs from.
//        // 2: transmission trigger
//        {
//            .Key = CO_KEY(0x1400, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 3,
//        },
//        {
//            .Key = CO_KEY(0x1400, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) CO_COBID_TPDO_DEFAULT(0) + packs[0].node_ID,
//        },
//        {
//            .Key = CO_KEY(0x1400, 2, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 0xFE,
//        },
//
//        //RPDO1 settings
//        // 0: RPDO number in index and total number of sub indexes.
//        // 1: The COB-ID to receive PDOs from.
//        // 2: transmission trigger
//        {
//            .Key = CO_KEY(0x1401, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 3,
//        },
//        {
//            .Key = CO_KEY(0x1401, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) CO_COBID_TPDO_DEFAULT(0) + packs[0].node_ID + 1,
//        },
//        {
//            .Key = CO_KEY(0x1401, 2, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 0xFE,
//        },
        //PRDO0-1 for Pack 1
        GEN_PACK_RPDO_CONFIG(0,packs[0].node_ID)

//        // RPDO0 mapping, determines the PDO messages to send when TPDO1 is triggered
//        // 0: The number of PDO messages associated with the TPDO
//        // 1: Link to the first PDO message
//        // n: Link to the nth PDO message
//        {
//            .Key = CO_KEY(0x1600, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 5,
//        },
//        { // Battery Voltage
//            .Key = CO_KEY(0x1600, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 1, 16),
//        },
//        { // Min Cell Voltage
//            .Key = CO_KEY(0x1600, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 2, 16),
//        },
//        { // Min Cell Voltage ID
//            .Key = CO_KEY(0x1600, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 3, 8),
//        },
//        { // Max Cell Voltage
//            .Key = CO_KEY(0x1600, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 4, 16),
//        },
//        { // Max Cell Voltage ID
//            .Key = CO_KEY(0x1600, 5, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 5, 8),
//        },
//
//        // RPDO1 mapping, determines the PDO messages to send when TPDO1 is triggered
//        // 0: The number of PDO messages associated with the TPDO
//        // 1: Link to the first PDO message
//        // n: Link to the nth PDO message
//        {
//            .Key = CO_KEY(0x1601, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = (uintptr_t) 7,
//        },
//        { // Current - Unused
//            .Key = CO_KEY(0x1601, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x5000, 0, 8),
//        },
//        { // Pack Min Temp
//            .Key = CO_KEY(0x1601, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 7, 8),
//        },
//        { // Pack Max Temp
//            .Key = CO_KEY(0x1601, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 8, 8),
//        },
//        { // SOC - Unused
//            .Key = CO_KEY(0x1601, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x5000, 0, 8),
//        },
//        { // State
//            .Key = CO_KEY(0x1601, 5, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x2100, 10, 8),
//        },
//        { //RecapActualAllowed - Unused
//            .Key = CO_KEY(0x1601, 6, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x5000, 0, 8),
//        },
//        { //DischargeActualAllowed - Unused
//            .Key = CO_KEY(0x1601, 7, CO_UNSIGNED32 | CO_OBJ_D__R_),
//            .Type = nullptr,
//            .Data = CO_LINK(0x5000, 0, 8),
//        },

        //PRDO0-1 map for Pack 1
        GEN_PACK_RPDO_MAP(0)

        // User defined data, this will be where we put elements that can be
        // accessed via SDO and depending on configuration PDO

        //Pack 1 Data
//        {
//            .Key = CO_KEY(0x2100, 1, CO_UNSIGNED16 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.batteryVoltage,
//        },
//        {
//            .Key = CO_KEY(0x2100, 2, CO_UNSIGNED16 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.minCellVoltage,
//        },
//        {
//            .Key = CO_KEY(0x2100, 3, CO_UNSIGNED8 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.minCellVoltageID,
//        },
//        {
//            .Key = CO_KEY(0x2100, 4, CO_UNSIGNED16 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.maxCellVoltage,
//        },
//        {
//            .Key = CO_KEY(0x2100, 5, CO_UNSIGNED8 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.maxCellVoltageID,
//        },
//        {
//            .Key = CO_KEY(0x2100, 7, CO_SIGNED8 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.batteryPackMinTemp,
//        },
//        {
//            .Key = CO_KEY(0x2100, 8, CO_SIGNED8 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.batteryPackMaxTemp,
//        },
//        {
//            .Key = CO_KEY(0x2100, 10, CO_UNSIGNED8 | CO_OBJ___PR_),
//            .Type = nullptr,
//            .Data = (uintptr_t) &packs[0].data.status,
//        },

        //Pack 1 Data
        GEN_PACK_DATA(0,packs[0].data)

        {
            .Key = CO_KEY(0x5000, 0, CO_UNSIGNED8 | CO_OBJ___PR_),
            .Type = nullptr,
            .Data = (uintptr_t) &dummy,
        },

        // End of dictionary marker
        CO_OBJ_DIR_ENDMARK,
    };
};

#endif// EXAMPLE_BMSMANAGER_HPP
