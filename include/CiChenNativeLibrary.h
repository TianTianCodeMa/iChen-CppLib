#pragma once

#include "SiChenDataStruct.h"

namespace IChenNativeLibrary
{
    // ------------------------------------------------------------------
    // API interface.
    // ------------------------------------------------------------------
    class CiChenLibraryInterface
    {
    public:
        CiChenLibraryInterface();
        CiChenLibraryInterface(UINT8 controllerType, UINT32 languageIndex, UINT32 clientVersionMajor, UINT32 clientVersionMinor, UINT32 machineSerialNumber, const std::string& machineModel, UINT8 serverIp1, UINT8 serverIp2, UINT8 serverIp3, UINT8 serverIp4, UINT16 serverPort);
        ~CiChenLibraryInterface();

        void EnableLogging(const std::wstring& filename, int maxFileSize = 1048576, bool IsLogRawMessage = false);
        void DisableLogging();

        bool GetNextServerMessage(SiChenBaseMessageType **data);

        bool SendAction(UINT32 primaryActionIndex, UINT32 auxiliaryAction1Index, UINT32 auxiliaryAction2Index, UINT32 auxiliaryAction3Index);
        bool SendAlarmReset(UINT32 alarmIndex);
        bool SendAlarmTriggered(UINT32 alarmIndex);
        bool SendAuditTrail(const SiChenDataType& dataWithNewValue, const UiChenValueType& oldValue);
        bool SendCycleData(UINT32 currentJobCardId, UINT32 currentMoldId, UINT32 currentJobMode, const std::vector<SiChenDataType>& cycleDataNameValuePair);
        bool SendJobMode(UINT32 newJobMode, UINT32 oldJobMode);
        bool SendOperationMode(UINT32 newOpMode, UINT32 oldOpMode);
        bool SendStatusReply(UINT32 curOpModeId, UINT32 curJobModeId, UINT32 curJobCardId, UINT32 curMoldId, const std::wstring& currentMoldName);
        bool SendLoginRequest(const std::string& password);
        bool SendForcedLogoutReply();
        bool SendMoldDataUploadRequest(const std::wstring& moldName, const std::vector<SiChenDataType>& moldData);
        bool SendMoldDataListRequest(const std::wstring& searchPattern);
        bool SendMoldDataRequest(UINT32 moldId, UINT32 jobCardId, UINT32 currentYield, UINT32 maxYield);
        bool SendMoldSummaryReply(UINT32 terminalIp, UINT16 terminalPort, const std::wstring& moldName, const std::vector<SiChenDataType>& moldData);
        bool SendJobCardListRequest();
        bool SendJobCardRequest(UINT32 jobCardId);
        bool SendJobCardChanged(UINT32 newJobCardId);

        bool SetControllerType(UINT8 controllerType);
        bool SetIChenConnection(bool isConnect);
        bool SetLanguage(UINT32 languageIndex);
        bool SetMachineInfo(UINT32 clientVersionMajor, UINT32 clientVersionMinor, UINT32 machineSerialNumber, const std::string& machineModel);
        bool SetServerIP(UINT8 ip1, UINT8 ip2, UINT8 ip3, UINT8 ip4, UINT16 port);
        bool SetServerDisconnectTimeout(float timeoutInSec);
        bool SetControllerHeartBeatPeriod(float timeoutInSec);

        std::string GetLibraryVersion();
    };
}
