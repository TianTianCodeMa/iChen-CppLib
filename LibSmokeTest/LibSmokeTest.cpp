// LibSmokeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CiChenNativeLibrary.h"

#include <iostream>
#include <io.h>
#include <fcntl.h>

using namespace std;
using namespace IChenNativeLibrary;

vector<SiChenDataType> GenerateMoldData()
{
    const int NumOfDataPerType = 10;
    vector<SiChenDataType> dataList;

    // Generate integer.
    for (int i = 0; i < NumOfDataPerType; i++)
    {
        SiChenDataType myType;
        myType.iChenDataIndex = i;
        myType.iChenDataValue.UI32 = i;
        myType.isAbsoluteValue = true;
        //myType.iChenDataValue.UI32 = rand();
        //myType.isAbsoluteValue = (i % 2 == 0) ? true : false;
        myType.isFloatingPoint = false;

        dataList.push_back(myType);
    }

    // Generate float.
    for (int i = 0; i < NumOfDataPerType; i++)
    {
        SiChenDataType myType;
        myType.iChenDataIndex = NumOfDataPerType + i;
        myType.iChenDataValue.F = NumOfDataPerType + i;
        myType.isAbsoluteValue = true;
        //myType.iChenDataValue.F = (float)rand() / (RAND_MAX + 1) * 10000.0f;
        //myType.isAbsoluteValue = (i % 2 == 0 && myType.iChenDataValue.F >= 100.0) ? true : false;
        myType.isFloatingPoint = true;

        dataList.push_back(myType);
    }

    // Generate boolean.
    for (int i = 0; i < NumOfDataPerType; i++)
    {
        SiChenDataType myType;
        myType.iChenDataIndex = 2 * NumOfDataPerType + i;
        myType.iChenDataValue.B = TRUE;
        //myType.iChenDataValue.B = ((float)rand() / (RAND_MAX + 1)) > 0.5f ? TRUE : FALSE;
        myType.isAbsoluteValue = true;
        myType.isFloatingPoint = false;

        dataList.push_back(myType);
    }

    // Uncomment this if uploaded mold data from this smoke test application is in correct.
    //int counter = 0;
    //for each (SiChenDataType item in dataList)
    //{
    //    wprintf(L"Generated mold data [%02d]: index - %u, isFloat - %d, UT32 - %u, F - %f, B - %d\n", counter++, item.iChenDataIndex, item.isFloatingPoint, item.iChenDataValue.UI32, item.iChenDataValue.F, item.iChenDataValue.B);
    //}

    return dataList;
}

void PrintTimestamp()
{
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    wprintf(L"\nLocal timestamp: %02d:%02d:%02d.%03d\n", localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
}

int wmain(int argc, wchar_t* argv[])
{
    _setmode(_fileno(stdout), _O_WTEXT);

    CiChenLibraryInterface iChenObj;

    wcout << L"震雄測試" << endl;

    // EnableLogging()
    PrintTimestamp();
    iChenObj.EnableLogging(L".\\iChenLog.txt", 1024 * 1024 * 100, true);

    // GetLibraryVersion().
    PrintTimestamp();
    wcout << L"Lib version: " << iChenObj.GetLibraryVersion().c_str() << endl;

    // SetControllerType() 
    PrintTimestamp();
    wcout << L"SetControllerType result: " << iChenObj.SetControllerType(0) << endl;

    // SetLanguage() 
    PrintTimestamp();
    wcout << L"SetLanguage result: " << iChenObj.SetLanguage(1) << endl;

    // SetMachineInfo()
    PrintTimestamp();
    wprintf(L"Set controller version to %s.%s\n", argv[6], argv[7]);
    wcout << L"SetMachineInfo result: " << iChenObj.SetMachineInfo(_wtoi(argv[6]), _wtoi(argv[7]), 61308, "JM138-AI") << endl;

    // SetServerIP()
    PrintTimestamp();
    wprintf(L"Set server to %s.%s.%s.%s:%s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
    wcout << L"SetServerIP result: " << iChenObj.SetServerIP(_wtoi(argv[1]), _wtoi(argv[2]), _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5])) << endl;

    // SetIChenConnection(true)
    PrintTimestamp();
    bool connected = iChenObj.SetIChenConnection(true);
    wcout << L"SetIChenConnection(true) result: " << connected << endl;

    if (connected)
    {
        UINT32 curOpModeId = 1, curJobModeId = 1, curJobCardId = 0, curMoldId = 0, curYield = 0, maxYield = 0;

        iChenObj.SendJobMode(curJobModeId, 0);
        iChenObj.SendOperationMode(curOpModeId, 0);

        // GetNextServerMessage
        SiChenBaseMessageType *pMyData = NULL;
        bool dataReady = false;
        wstring currentMoldName = L"NIL";
        vector<SiChenDataType> moldData;

        // To avoid printing NO_SERVER_DATA at the first cycle.
        int disconnectCounter = 1;

        // Only allow no new message for 60s.
        while (!dataReady && disconnectCounter < 60)
        {
            if (!iChenObj.GetNextServerMessage(&pMyData))
            {
                break;
            }

            connected = pMyData->isConnected;

            switch (pMyData->iChenActivityType)
            {
                case IChenActivityType::NO_SERVER_DATA:
                {
                    // Print this message every 5s.
                    if (disconnectCounter % 5 == 0)
                    {
                        PrintTimestamp();
                        wcout << L"GetNextServerMessage - NO_SERVER_DATA" << endl;
                    }

                    Sleep(1000);
                    disconnectCounter++;
                    break;
                }

                case IChenActivityType::TIME_SYNC:
                {
                    SiChenTimeSyncType *pData = static_cast<SiChenTimeSyncType*>(pMyData);

                    PrintTimestamp();

                    FILETIME ft; // for data correctiveness checking.
                    if (TRUE != SystemTimeToFileTime(&pData->serverTime, &ft))
                    {
                        return -1;
                    }

                    wprintf(L"GetNextServerMessage - TIME_SYNC: %04d.%02d.%02d-%02d:%02d:%02d.%03d\n", pData->serverTime.wYear, pData->serverTime.wMonth, pData->serverTime.wDay, pData->serverTime.wHour, pData->serverTime.wMinute, pData->serverTime.wSecond, pData->serverTime.wMilliseconds);
                    break;
                }

                case IChenActivityType::SERVER_MESSAGE:
                {
                    SiChenServerMessageType *pData = static_cast<SiChenServerMessageType*>(pMyData);

                    PrintTimestamp();

                    wprintf(L"GetNextServerMessage - SERVER_MESSAGE: %s\n", pData->serverMessage.c_str());
                    break;
                }

                case IChenActivityType::JOBMODE_LIST_REPLY:
                {
                    SiChenJobModeListRepType *pData = static_cast<SiChenJobModeListRepType*>(pMyData);

                    PrintTimestamp();
                    for each (wstring wstrMode in pData->returnedJobModeList)
                    {
                        wprintf(L"GetNextServerMessage - JOBMODE_LIST_REPLY: %s\n", wstrMode.c_str());
                    }

                    wcout << "SendLoginRequest result :" << iChenObj.SendLoginRequest("12345678") << endl;
                    break;
                }

                case IChenActivityType::HOST_STATE_REQUEST:
                {
                    PrintTimestamp();

                    wcout << L"GetNextServerMessage - HOST_STATE_REQUEST" << endl;
                    wcout << "SendStatusReply result :" << iChenObj.SendStatusReply(curOpModeId, curJobModeId, curJobCardId, curMoldId, currentMoldName) << endl;
                    break;
                }

                case IChenActivityType::PASSWORD_LEVEL_REPLY:
                {
                    SiChenPwdLvRepType *pData = static_cast<SiChenPwdLvRepType*>(pMyData);

                    PrintTimestamp();

                    wprintf(L"GetNextServerMessage - PASSWORD_LEVEL_REPLY: User name: %s\n", pData->userName.c_str());
                    wcout << L"GetNextServerMessage - PASSWORD_LEVEL_REPLY: Pwd Lvl: " << pData->userPasswordLevel << endl;
                    wcout << L"GetNextServerMessage - PASSWORD_LEVEL_REPLY: Allow auto: " << pData->isAllowAuto << endl;

                    if (pData->userPasswordLevel != 0)
                    {
                        wcout << "SendJobCardListRequest result :" << iChenObj.SendJobCardListRequest() << endl;
                    }

                    break;
                }

                case IChenActivityType::JOBCARD_LIST_REPLY:
                {
                    SiChenJobCardListRepType *pData = static_cast<SiChenJobCardListRepType*>(pMyData);

                    PrintTimestamp();
                    for each (SJobCardItem item in pData->returnedJobCardList)
                    {
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Job card ID: %u\n", item.jobCardItemID);
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Job card name: %s\n", item.jobCardItemName.c_str());
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Job card number: %u\n", item.jobCardItemNumber);
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Mold Name: %s\n", item.jobCardItemMoldName.c_str());
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Cur yield: %d\n", item.jobCardItemCurrentYield);
                        wprintf(L"GetNextServerMessage - JOBCARD_LIST_REPLY: Max yield: %d\n", item.jobCardItemMaxYield);
                    }

                    curJobCardId = pData->returnedJobCardList[0].jobCardItemID;
                    curYield = pData->returnedJobCardList[0].jobCardItemCurrentYield;
                    maxYield = pData->returnedJobCardList[0].jobCardItemMaxYield;
                    currentMoldName = pData->returnedJobCardList[0].jobCardItemMoldName;

                    moldData = GenerateMoldData();

                    wcout << "SendMoldDataUploadRequest result :" << iChenObj.SendMoldDataUploadRequest(currentMoldName, moldData) << endl;
                    break;
                }

                case IChenActivityType::MOLD_LIST_REPLY:
                {
                    SiChenMoldListRepType *pData = static_cast<SiChenMoldListRepType*>(pMyData);

                    PrintTimestamp();
                    for each (SMoldItem item in pData->returnedMoldList)
                    {
                        wprintf(L"GetNextServerMessage - MOLD_LIST_REPLY: Mold ID: %u\n", item.moldItemID);
                        wprintf(L"GetNextServerMessage - MOLD_LIST_REPLY: Mold name: %s\n", item.moldItemName.c_str());
                        wprintf(L"GetNextServerMessage - MOLD_LIST_REPLY: Create date: %s\n", item.moldItemCreateDate.c_str());
                        wprintf(L"GetNextServerMessage - MOLD_LIST_REPLY: Serial ID: %u\n", item.serialID);
                        wprintf(L"GetNextServerMessage - MOLD_LIST_REPLY: Version: %s.%s\n", item.moldItemVersionMajor.c_str(), item.moldItemVersionMinor.c_str());
                    }

                    wcout << "SendJobCardRequest result :" << iChenObj.SendJobCardRequest(curJobCardId) << endl;
                    break;
                }

                case IChenActivityType::MOLD_DATA_SAVED:
                {
                    PrintTimestamp();

                    wcout << L"GetNextServerMessage - MOLD_DATA_SAVED!!!" << endl;
                    wcout << "SendMoldDataListRequest result :" << iChenObj.SendMoldDataListRequest(L"") << endl;
                    break;
                }

                case IChenActivityType::JOBCARD_DATA_REPLY:
                {
                    SiChenMoldListRepType *pData = static_cast<SiChenMoldListRepType*>(pMyData);

                    PrintTimestamp();
                    for each (SMoldItem item in pData->returnedMoldList)
                    {
                        wprintf(L"GetNextServerMessage - JOBCARD_DATA_REPLY: Mold ID: %u\n", item.moldItemID);
                        wprintf(L"GetNextServerMessage - JOBCARD_DATA_REPLY: Mold name: %s\n", item.moldItemName.c_str());
                        wprintf(L"GetNextServerMessage - JOBCARD_DATA_REPLY: Create date: %s\n", item.moldItemCreateDate.c_str());
                        wprintf(L"GetNextServerMessage - JOBCARD_DATA_REPLY: Serial ID: %u\n", item.serialID);
                        wprintf(L"GetNextServerMessage - JOBCARD_DATA_REPLY: Version: %s.%s\n", item.moldItemVersionMajor.c_str(), item.moldItemVersionMinor.c_str());
                    }

                    curMoldId = pData->returnedMoldList[0].moldItemID;
                    wcout << "SendMoldDataRequest result :" << iChenObj.SendMoldDataRequest(curMoldId, curJobCardId, curYield, maxYield) << endl;
                    break;
                }

                case IChenActivityType::MOLD_DATA_REPLY:
                {
                    SiChenMoldDataRepType *pData = static_cast<SiChenMoldDataRepType*>(pMyData);

                    PrintTimestamp();
                    for each (SiChenDataType item in pData->returnedMoldData)
                    {
                        wprintf(L"GetNextServerMessage - MOLD_DATA_REPLY: %u - %f%c\n", item.iChenDataIndex, item.isFloatingPoint ? item.iChenDataValue.F : item.iChenDataValue.UI32, item.isAbsoluteValue ? L' ' : L'%');
                    }

                    dataReady = true;
                    break;
                }

                case IChenActivityType::USER_FORCED_LOGOUT:
                {
                    SiChenUserForcedLogoutType *pData = static_cast<SiChenUserForcedLogoutType*>(pMyData);

                    PrintTimestamp();
                    wprintf(L"GetNextServerMessage - USER_FORCED_LOGOUT: New level - %d\n", pData->userPasswordLevel);
                    wprintf(L"GetNextServerMessage - USER_FORCED_LOGOUT: Allow auto - %d\n,", pData->isAllowAuto);

                    wcout << "SendForcedLogoutReply result :" << iChenObj.SendForcedLogoutReply() << endl;
                    break;
                }

                case IChenActivityType::MOLD_SUMMARY_REQUEST:
                {
                    SiChenMoldSummaryReqType *pData = static_cast<SiChenMoldSummaryReqType*>(pMyData);

                    PrintTimestamp();
                    wprintf(L"GetNextServerMessage - MOLD_SUMMARY_REQUEST: Terminal IP - %u\n", pData->terminalIp);
                    wprintf(L"GetNextServerMessage - MOLD_SUMMARY_REQUEST: Terminal Port - %u\n", pData->terminalPort);

                    moldData = GenerateMoldData();

                    wcout << "SendMoldSummaryReply result :" << iChenObj.SendMoldSummaryReply(pData->terminalIp, pData->terminalPort, currentMoldName, moldData) << endl;
                    break;
                }

                default:
                    break;
            }

            if (pMyData->iChenActivityType != IChenActivityType::NO_SERVER_DATA)
            {
                // To avoid printing NO_SERVER_DATA at the first cycle.
                disconnectCounter = 1;
            }
        }

        if (dataReady)
        {
            // SendAuditTrail
            PrintTimestamp();
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        SiChenDataType dataNew;
                        rand_s(&dataNew.iChenDataIndex);
                        rand_s(&dataNew.iChenDataValue.UI32);

                        UiChenValueType valueOld;
                        rand_s(&valueOld.UI32);

                        wcout << L"SendAuditTrail result: " << iChenObj.SendAuditTrail(dataNew, valueOld) << endl;
                    }
                }
            }

            // SendAlarmTriggered and SendAlarmReset
            PrintTimestamp();
            {
                UINT32 auiAlmIdx[5];
                for (int i = 0; i < 5; i++)
                {
                    rand_s(&auiAlmIdx[i]);
                }

                for (int i = 0; i < 5; i++)
                {
                    wcout << L"SendAlarmTriggered " << auiAlmIdx[i] << " result: " << iChenObj.SendAlarmTriggered(auiAlmIdx[i]) << endl;
                    Sleep(500);

                    if (i > 0)
                    {
                        wcout << L"SendAlarmReset " << auiAlmIdx[i - 1] << " result: " << iChenObj.SendAlarmReset(auiAlmIdx[i - 1]) << endl;
                        Sleep(500);
                    }
                }

                wcout << L"SendAlarmReset " << auiAlmIdx[4] << " result: " << iChenObj.SendAlarmReset(auiAlmIdx[9]) << endl;
            }

            // SendAction
            PrintTimestamp();
            {
                UINT32 randValue, actIdx1, actIdx2, actIdx3, actIdx4;

                for (int i = 0; i < 5; i++)
                {
                    rand_s(&randValue);
                    actIdx1 = (UINT32)((randValue * 1.0f / MAXUINT32) * 256);

                    rand_s(&randValue);
                    actIdx2 = (UINT32)((randValue * 1.0f / MAXUINT32) * 256);

                    rand_s(&randValue);
                    actIdx3 = (UINT32)((randValue * 1.0f / MAXUINT32) * 256);

                    rand_s(&randValue);
                    actIdx4 = (UINT32)((randValue * 1.0f / MAXUINT32) * 256);

                    wprintf(L"SendAction action index: %u, %u, %u, %d\n", actIdx1, actIdx2, actIdx3, actIdx4);
                    wcout << L"SendAction result: " << iChenObj.SendAction(actIdx1, actIdx2, actIdx3, actIdx4) << endl;

                    Sleep(500);
                }
            }

            // SendCycleData
            PrintTimestamp();
            for (int y = 0; y < 10; y++)
            {
                vector<SiChenDataType> myVector;
                unsigned int dummy;

                for (int i = 0; i < 10; i++)
                {
                    SiChenDataType data;

                    rand_s(&dummy);
                    data.iChenDataIndex = (UINT32)((dummy * 1.0f / MAXUINT32) * 0xFFFFFF);

                    rand_s(&dummy);
                    data.iChenDataValue.F = (dummy > 100000) ? (dummy * 1.0f / 100000) : dummy;

                    data.isAbsoluteValue = true;
                    data.isFloatingPoint = true;

                    wprintf(L"Generated cycle data [%d]: index - %u, isFloat - %d, F - %f\n", i, data.iChenDataIndex, data.isFloatingPoint, data.iChenDataValue.F);

                    myVector.push_back(data);
                }

                wcout << L"SendCycleData result: " << iChenObj.SendCycleData(curJobCardId, curMoldId, curJobModeId, myVector) << endl;
            }
        }
    }

    if (connected)
    {
#ifndef WINCE
        wcout << L"Press ENTER to disconnect from iChen" << endl;
        system("PAUSE");
#endif

        // SetIChenConnection(false)
        wcout << L"SetIChenConnection(false) result: " << iChenObj.SetIChenConnection(false) << endl;
    }

    // DisableLogging()
    iChenObj.DisableLogging();

    return 0;
}