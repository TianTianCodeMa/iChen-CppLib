#pragma once

#include <vector>

namespace IChenNativeLibrary
{
    union UiChenValueType
    {
        UINT32 UI32;
        FLOAT F;
        BOOL B;

        UiChenValueType(UINT32 ui) : UI32(ui) {}
        UiChenValueType(FLOAT f) : F(f) {}
        UiChenValueType(BOOL b) : B(b) {}
        UiChenValueType() : UI32(0) {}
    };

    struct SiChenDataType
    {
        UINT32 iChenDataIndex;
        UiChenValueType iChenDataValue;
        bool isFloatingPoint;
        bool isAbsoluteValue;

        // Default constructor
        SiChenDataType() :
            iChenDataIndex(0),
            iChenDataValue(0),
            isFloatingPoint(false),
            isAbsoluteValue(false)
        {
        }
    };

    enum IChenActivityType
    {
        NO_SERVER_DATA,
        HOST_STATE_REQUEST,
        MOLD_DATA_SAVED,
        MOLD_SUMMARY_REQUEST,
        SERVER_MESSAGE,
        PASSWORD_LEVEL_REPLY,
        USER_FORCED_LOGOUT,
        MOLD_LIST_REPLY,
        MOLD_DATA_REPLY,
        JOBCARD_LIST_REPLY,
        JOBCARD_DATA_REPLY,
        JOBMODE_LIST_REPLY,
        TIME_SYNC
    };

    struct SMoldItem
    {
        UINT32 moldItemID;
        std::wstring moldItemName;
        UINT32 serialID;
        std::wstring moldItemCreateDate;
        std::wstring moldItemVersionMajor;
        std::wstring moldItemVersionMinor;
    };

    struct SJobCardItem
    {
        UINT32 jobCardItemID;
        UINT32 jobCardItemNumber;
        std::wstring jobCardItemMoldName;
        std::wstring jobCardItemName;
        INT32 jobCardItemCurrentYield;
        INT32 jobCardItemMaxYield;
    };

    struct SiChenBaseMessageType
    {
        bool isConnected;
        IChenActivityType iChenActivityType;
    };

    struct SiChenServerMessageType : SiChenBaseMessageType
    {
        std::wstring serverMessage;
    };

    struct SiChenPwdLvRepType : SiChenBaseMessageType
    {
        UINT16 userPasswordLevel;
        bool isAllowAuto;
        std::wstring userName;
    };

    struct SiChenUserForcedLogoutType : SiChenBaseMessageType
    {
        UINT16 userPasswordLevel;
        bool isAllowAuto;
    };

    struct SiChenMoldListRepType : SiChenBaseMessageType
    {
        std::vector<SMoldItem> returnedMoldList;
    };

    struct SiChenMoldDataRepType : SiChenBaseMessageType
    {
        std::wstring moldName;
        std::vector<SiChenDataType> returnedMoldData;
    };

    struct SiChenJobCardListRepType : SiChenBaseMessageType
    {
        std::vector<SJobCardItem> returnedJobCardList;
    };

    struct SiChenJobModeListRepType : SiChenBaseMessageType
    {
        std::vector<std::wstring> returnedJobModeList;
    };

    struct SiChenMoldSummaryReqType : SiChenBaseMessageType
    {
        UINT32 terminalIp;
        UINT16 terminalPort;
    };

    struct SiChenTimeSyncType : SiChenBaseMessageType
    {
        SYSTEMTIME serverTime;
    };
}
