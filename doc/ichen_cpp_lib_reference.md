iChen® 4.0 C++ Integration Library API Reference
================================================

Copyright © Chen Hsong Holdings Ltd.  All rights reserved.  
Document Version: 4.0  
Last Edited: 2017-05-22


Usage Instructions
------------------

### Setup

|Library file|`iChenNativeLibrary.lib`|
|------------|------------------------|
|Header file|`iChenNativeLibrary.h`|
|Dependencies|`SiChenDataStruct.h` (iChen® 4.0 data structures)|
|Namespace|`IChenNativeLibrary`|
|Compiler Toolset|Visual C++ 2008, WIN32 or WINCE7|
|Libraries|Dynamic with ATL|

### Step 1

Construct a `CiChenLibraryInterface` class and set the necessary info.

~~~~~~~~~~~~~cpp
#include "iChenNativeLibrary.h"

using namespace IChenNativeLibrary;

// Create API object
CiChenLibraryInterface *pIChen = new CiChenLibraryInterface();
// Set controller type (10 = CBmold300)
pIChen->SetControllerType(10);
// Set interface language (0 = English)
pIChen->SetLanguage(0);
// Set information about the machine:
//   Serial number = 123456
//   Version = 1.0
//   Model = JM138Ai
pIChen->SetMachineInfo(1,0,123456,L"JM138Ai");
// Set IP address and port of the iChen server (192.168.123.234:34954)
pIChen->SetServerIP(192,168,123,234,34954);
~~~~~~~~~~~~~

or 

~~~~~~~~~~~~~cpp
#include "iChenNativeLibrary.h"

using namespace IChenNativeLibrary;

// This call is equivalent to the one above
CiChenLibraryInterface *pIChen = 
	new CiChenLibraryInterface(10,0,1,0,123456,L"JM138Ai",192,168,123,234,34954);
~~~~~~~~~~~~~

### Step 2

Connect to the iChen® 4.0 Server.

~~~~~~~~~~~~~cpp
pIChen->SetIChenConnection(true);
~~~~~~~~~~~~~

### Step 3

Create a message loop, calling [GetNextServerMessage](#cichenlibraryinterfacegetnextservermessage),
to retrieve [messages](#sichenbasemessagetype) sent from the server.
Typically, this message loop is run on a dedicated background thread.

For each message, inspect its [iChenActivityType](#ichenactivitytype) field to determine the message
type and cast to the appropriate type where necessary.

Handle each message individually.

~~~~~~~~~~~~~cpp
SiChenBaseMessageType *pMessage = NULL;
long reconnect_count = -1;

while (true)
{
	// Notice that the last value for pMessage is passed into the next call
	if (!pIChen->GetNextServerMessage(&pMessage)) {
		// This shouldn't happen!  Something has gone wrong with the connection!
		// ... do something drastic (e.g. reconnect)...
		break;
	}

	// Check if the server connection is broken
	if (!pMessage->isConnected) {
		// Attempt to reconnect once every 100 cycles
		if (reconnect_count < 0) reconnect_count = 100;
		if (reconnect_count > 0) reconnect_count--;
		if (reconnect_count == 0) {
			// Warning: CiChenLibraryInterface::SetIChenConnection blocks
			pIChen->SetIChenConnection(true);
			reconnect_count = -1;
		}
	} else {
		reconnect_count = -1;
	}

	// Check the message type
	switch (pMessage->iChenActivityType)
	{
		case IChenActivityType::NO_SERVER_DATA: break;	// No message

		case IChenActivityType::SERVER_MESSAGE:
		{
			// Cast to SiChenServerMessageType
			SiChenServerMessageType *pSvrMsg =
				static_cast<SiChenServerMessageType *>(pMessage);

			// Handle message
			PopupMessageOnScreen(pMessage->ServerMessage);
			break;
		}

			:
		// Handle other messages
			:
	}

	Sleep(100);	// Throttle the message handling rate based on CPU resources
}
~~~~~~~~~~~~~

### Step 4

Send necessary messages to the server.
All method calls on the `CiChenLibraryInterface` class are thread-safe.

~~~~~~~~~~~~~cpp
// Send an alarm notification
pIChen->SendAlarmTriggered(123);
~~~~~~~~~~~~~



CiChenLibraryInterface
----------------------

### Usage

This class contains all the facilities to integrate with an iChen® 4.0 network.

### Constructors

~~~~~~~~~~~~~cpp
class CiChenLibraryInterface()

class CiChenLibraryInterface
(
	UINT8 controllerType,
	UINT32 languageIndex,
	UINT32 clientVersionMajor,
	UINT32 clientVersionMinor,
	UINT32 machineSerialNumber,
	std::string machineModel,
	UINT8 serverIp1,
	UINT8 serverIp2,
	UINT8 serverIp3,
	UINT8 serverIp4,
	UINT16 serverPort
)
~~~~~~~~~~~~~

### Constructor Parameters

|Parameters                |Description                     |
|--------------------------|--------------------------------|
|`controllerType`          |Type of controller (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#ControllerTypes) for a full list)|
|`languageIndex`           |Interface language (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#Languages) for a full list)|
|`clientVersionMajor`,<br />`clientVersionMinor`|Version of the communications protocol (default 1.0)|
|`machineSerialNumber`     |Unique serial number of the machine|
|`machineModel`            |Model of the machine            |
|`serverIp1`,<br />`serverIp2`,<br />`serverIp3`,<br />`serverIp4`|Four bytes representing the IP address of the iChen® 4.0 Server.|
|`serverPort`              |Port number of the iChen® 4.0 Server (default 34954)|

### Thread Safety

All method calls on this class are thread-safe.

### Remarks

For a controller not specified in the official list (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#ControllerTypes)),
use any number starting from 100 as the `controllerType`.  Controller types 0-99 are reserved.

### Example

~~~~~~~~~~~~~cpp
CiChenLibraryInterface *pIChen = 
	new CiChenLibraryInterface(10,0,1,0,123456,L"JM138Ai",192,168,123,234,34954);
~~~~~~~~~~~~~


CiChenLibraryInterface::GetLibraryVersion
-----------------------------------------

### Usage

Retrieves the version of this library.

### Syntax

~~~~~~~~~~~~~cpp
std::string GetLibraryVersion();
~~~~~~~~~~~~~

### Parameters

None.

### Return Value

A `std::string` containing the version of this library.

### Example

~~~~~~~~~~~~~cpp
std::string version = piChen->GetLibraryVersion();
~~~~~~~~~~~~~


CiChenLibraryInterface::EnableLogging
-------------------------------------

### Usage

Turns on logging to an external file.

### Syntax

~~~~~~~~~~~~~cpp
void EnableLogging(
	std::wstring filename,
	int maxFileSize = 1048576,
	bool isLogRawMessage = false);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`filename`                |Path of the file to log to.<br />If this path is relative, the log file will be created relative to the current directory.|
|`maxFileSize`             |Maximum size of the log file, in bytes (default 1MB)|
|`isLogRawMessage`         |Logs raw communications data to and from the server? (default FALSE)|

### Return Value

None.

### Warning

Logging is for debugging purposes and should always be turned off in a production system.
Turning on logging consumes a large amount of resources on the host controller and may
adversely affect its primary task in controlling the machine.

Turn on logging on a controller in production environment only when trouble-shooting
difficult-to-reproduce bugs; even so, try turning it on just before the occurrence of
the bug and turn it off immediately afterwards. Beware that, since logging consumes large
amounts of resources, turning on logging itself may actually prevent the bug from being
reproduced.

### Example

~~~~~~~~~~~~~cpp
// Enable logging to D:\iChenLog.txt, maximum size 1MB
piChen->EnableLogging(L"D:\\iChenLog.txt", 1024 * 1024, false);
~~~~~~~~~~~~~


CiChenLibraryInterface::DisableLogging
--------------------------------------

### Usage

Turns off logging.

### Syntax

~~~~~~~~~~~~~cpp
void DisableLogging();
~~~~~~~~~~~~~

### Parameters

None.

### Return Value

None.

### Example

~~~~~~~~~~~~~cpp
// Disable logging
piChen->DisableLogging();
~~~~~~~~~~~~~


CiChenLibraryInterface::SetControllerType
-----------------------------------------

### Usage

Sets the type of controller
(see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#ControllerTypes) for a full list).

### Syntax

~~~~~~~~~~~~~cpp
bool SetControllerType(UINT8 controllerType);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`controllerType`          |Type of controller (see (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#ControllerTypes) for a full list)|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

For a controller not specified in the official list (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#ControllerTypes)),
use any number starting from 100.  Controller types 0-99 are reserved.

This method cannot be called when already connected to an iChen® 4.0 Server.
Call [SetIChenConnection(false)](#cichenlibraryinterfacesetichenconnection) first to
disconnect from the Server before calling this method.

### Example

~~~~~~~~~~~~~cpp
// Set the type of the controller to CBmold300 (#10)
if (!piChen->SetControllerType(10))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetLanguage
-----------------------------------

### Usage

Sets the language to connect with the iChen® 4.0 Server
(see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#Languages) for a full list).

Typically this should be the language currently showing on the UI of the controller,
as the server will send any localized texts (e.g. job mode names) in the specified language.

### Syntax

~~~~~~~~~~~~~cpp
bool SetLanguage(UINT32 languageIndex);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`languageIndex`           |Language code (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#Languages) for a full list)|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

This method cannot be called when already connected to an iChen® 4.0 Server.
Call [SetIChenConnection(false)](#cichenlibraryinterfacesetichenconnection) first to
disconnect from the Server before calling this method.

### Example

~~~~~~~~~~~~~cpp
// Set the current UI language to French (#4)
if (!piChen->SetLanguage(4))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetMachineInfo
--------------------------------------

### Usage

Sets basic information about the machine.

### Syntax

~~~~~~~~~~~~~cpp
bool SetMachineInfo(
	UINT32 clientVersionMajor,
	UINT32 clientVersionMinor,
	UINT32 machineSerialNumber,
	std::string machineModel);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`clientVersionMajor`,<br />`clientVersionMinor`|Version of the communications protocol|
|`machineSerialNumber`     |Unique serial number of the machine|
|`machineModel`            |Model of the machine            |

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

This method cannot be called when already connected to an iChen® 4.0 Server.
Call [SetIChenConnection(false)](#cichenlibraryinterfacesetichenconnection) first to
disconnect from the Server before calling this method.

### Example

~~~~~~~~~~~~~cpp
// Set the following machine info:
//   Protocol version: 3.1
//   Machine serial number: 123456
//   Machine model: JM138-Ai
if (!piChen->SetMachineInfo(3, 1, 123456, "JM138-Ai"))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetServerIP
-----------------------------------

### Usage

Sets the IP address and port of the iChen® 4.0 Server.
The default port is 34954.

### Syntax

~~~~~~~~~~~~~cpp
bool SetServerIP(UINT8 ip1, UINT8 ip2, UINT8 ip3, UINT8 ip4, UINT16 port);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`ip1`,`ip2`,`ip3`,`ip4`   |Four bytes representing the IP address of the iChen® 4.0 Server.|
|`port`                    |Port number of the iChen® 4.0 Server (default 34954)|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

This method cannot be called when already connected to an iChen® 4.0 Server.
Call [SetIChenConnection(false)](#cichenlibraryinterfacesetichenconnection) first to
disconnect from the Server before calling this method.

### Example

~~~~~~~~~~~~~cpp
// Set the server's IP address to 192.168.123.234 and port 34954
if (!piChen->SetServerIP(192, 168, 123, 234, 34954))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetServerDisconnectTimeout
--------------------------------------------------

### Usage

Sets the time interval, in seconds, to time-out a connection to an iChen® 4.0 Server
when no *heartbeat* from the server is received within the time interval.

### Syntax

~~~~~~~~~~~~~cpp
bool SetServerDisconnectTimeout(float timeoutInSec);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`timeoutInSec`            |Interval to time-out a server connection|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

An iChen® 4.0 Server sends a *heartbeat* to all conencted clients on a periodic basis.
The *heartbeat* interval is configurable, and defaults to 10 seconds. In other words, by default,
an iChen® 4.0 Server sends a *heartbeat* to connected clients once every 10 seconds.

If a *heartbeat* is not received from the server after a long interval of time (typically
multiple times of the server's *heartbeat* interval), then the server can be assumed
dysfunctional or terminated.  This library automatically disconnects from the server when
this time-out event happens. The default value is 60 seconds.

Call this method only when the default *heartbeat* interval of the iChen® 4.0 Server
has been changed. When that happens, set the time-out interval to at least 4 or 5 times
of the server's *heartbeat* interval.

### Example

~~~~~~~~~~~~~cpp
// Set the server's heartbeat to 15 seconds
if (!piChen->SetServerDisconnectTimeout(15.0f))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetControllerHeartBeatPeriod
----------------------------------------------------

### Usage

Sets the time interval, in seconds, to send a *heartbeat* to the connected
iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SetControllerHeartBeatPeriod(float timeoutInSec);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`timeoutInSec`            |Interval to send a *heartbeat* to the server|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

Each controller connected to an iChen® 4.0 Server must send a *heartbeat* at regular
intervals. This *heartbeat* interval is configurable, and defaults to 10 seconds.
In other words, by default, this library sends a *heartbeat* to the iChen® 4.0 Server
once every 10 seconds.

If a *heartbeat* is not received from the server after a (configurable) period of time,
defaulted to 20 seconds, the server *may* then assume that the client is dysfunctional or
terminated and *may* disconnect the client.

Call this method only when the default required *heartbeat* interval of the iChen® 4.0 Server
has been changed. When that happens, set the *heartbeat* interval to at least 1/2 or 1/3
of the server's required *heartbeat* interval.

### Example

~~~~~~~~~~~~~cpp
// Set the heartbeat to 15 seconds
if (!piChen->SetControllerHeartBeatPeriod(15.0f))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SetIChenConnection
------------------------------------------

### Usage

Sets basic information about the machine.

### Syntax

~~~~~~~~~~~~~cpp
bool SetIChenConnection(bool isConnect);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`isConnect`               |TRUE = Connect to the server, FALSE = Disconnect from the server|

### Return Value

TRUE if successful, otherwise FALSE.

### Warning

This method call **blocks** until a connection is successfully made, is explicitly denied,
or the time-out period is passed.
Do not call this method in a time-critical thread (e.g. the main UI thread).

The time-out period is the same as the time-out for server *heartbeats* (default 60 seconds),
which can be modified with [SetServerDisconnectTimeout](#cichenlibraryinterface::setserverdisconnecttimeout).

### Example

~~~~~~~~~~~~~cpp
// Connects to the iChen 4.0 Server
pIChen->SendIChenConnection(true);
	:
	// Do some work
	:
// Disconnects from the iChen 4.0 Server
pIChen->SendIChenConnection(false);
~~~~~~~~~~~~~


CiChenLibraryInterface::SendLoginRequest
----------------------------------------

### Usage

Authenticates an operator on the machine with a unique password
to that operator's authorization level.

### Syntax

~~~~~~~~~~~~~cpp
bool SendLoginRequest(std::string password);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`password`                |Password of the operator attempting to log into the machine|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The server should respond with an [SiChenPwdLvRepType](#sichenpwdlvreptype) message.
However, this response message is not guaranteed to arrive.
If the response message is not received after a period of time, the controller
should assume that the login request has been denied and that no authority is
granted to the operator with this password.

### Example

~~~~~~~~~~~~~cpp
// Log into the server with password "hello"
if (!piChen->SendLoginRequest("hello"))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendForcedLogoutReply
---------------------------------------------

### Usage

Sends an acknowledgement in response to a [`SiChenUserForcedLogoutType`](#sichenuserforcedlogouttype)
message sent by an iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendForcedLogoutReply();
~~~~~~~~~~~~~

### Parameters

None.

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Acknowledge to the server that the force-logout command is obeyed
pIChen->SendForcedLogoutReply();
~~~~~~~~~~~~~


CiChenLibraryInterface::SendOperationMode
-----------------------------------------

### Usage

Updates the current *operation mode* of the machine to the connected iChen® 4.0 Server.
(See [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#OpModes) for the full list of operation modes.

### Syntax

~~~~~~~~~~~~~cpp
bool SendOperationMode(UINT32 newOpMode, UINT32 oldOpMode);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`newOpMode`               |The current [operation mode](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#OpModes) of the machine|
|`oldOpMode`               |The previous [operation mode](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#OpModes) of the machine|

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Operation mode is changed from Manual (1) to Automatic (3)
if (!piChen->SendOperationMode(3, 1))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendJobMode
-----------------------------------

### Usage

Updates the current *job mode* of the machine to the connected iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendJobMode(UINT32 newJobMode, UINT32 oldJobMode);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`newJobMode`              |The current [job mode](#job-modes) of the machine|
|`oldJobMode`              |The previous [job mode](#job-modes) of the machine|

### Job Modes

Job mode numbers for this library are *different* from those in OpenProtocol™ (see [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#JobModes)).
**DO NOT USE** the job mode numbers in OpenProtocol™, use the job modes numbers below instead:

|Job Mode         |Number       |
|-----------------|:-----------:|
|*Unknown*        |-1           |
|`ID01`           |0            |
|`ID02`           |1            |
|`ID03`           |2            |
|`ID04`           |3            |
|`ID05`           |4            |
|`ID06`           |5            |
|`ID07`           |6            |
|`ID08`           |7            |
|`ID09`           |8            |
|`ID10`           |9            |
|`ID11`           |10           |
|`ID12`           |11           |
|`ID13`           |12           |
|`ID14`           |13           |
|`ID15`           |14           |

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

There are fifteen (see [here](#job-modes) for details)
user-definable job modes that can be used by an organization.

Job modes are typically used to track utilization of the machine according to
different usages. Usually they do not affect any other operation of the machine.

However, some specific job modes may have significance in an operational context;
for example, a controller may only start counting production cycles only if
the current job mode is set to "Production" or some equivalent. This way, certain
non-production activities, such as trouble-shooting, mold trials and machine tuning,
do not pollute the final production count. Generally speaking, it is considered
prudent for a controller to take the current job mode into account when performing
its duties.

### Example

~~~~~~~~~~~~~cpp
// Job mode is changed from #4 to #7
if (!piChen->SendJobMode(7, 4))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendCycleData
-------------------------------------

### Usage

Sends a set of data relating to the last cycle completed by the machine to an
iChen® 4.0 Server.

Each set of cycle data contains a collection of variables and their corresponding
values, typically statistics on the production process or sensor measurements.
(See [this document](https://github.com/chenhsong/OpenProtocol/blob/master/doc/cycledata.md) for examples of cycle data variables.)

### Syntax

~~~~~~~~~~~~~cpp
bool SendCycleData(
	UINT32 currentJobCardId,
	UINT32 currentMoldId,
	UINT32 currentJobMode,
	std::vector<SiChenDataType> cycleDataNameValuePair);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`currentJobCardId`        |Unique numeric ID of the currently-loaded *job card*|
|`currentMoldId`           |Unique numeric ID of the currently-loaded mold|
|`currentJobModeId`        |The current [job mode](#job-modes) of the machine|
|`cycleDataNameValuePair`  |A list of [`SiChenDataType`](#sichendatatype)'s containing variables and corresponding `float` values for this set of cycle data<br/><br/>IMPORTANT: Only `float` values are supported.|

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Allocate list
std::vector<SiChenDataType> listOfDataValues;

// Fill list with values
for (int i = 0; i < 10; i++)
{
	// Allocate data value
	SiChenDataType data;

	// Set test data value to loop index, absolute integer value
	// Remember: Only float values are supported!
	data.iChenDataValue.F = i * 42;
	data.isFloatingPoint = true;
	data.isAbsoluteValue = true;

	// Copy variable index
	data.iChenDataIndex = i;

	// ... or use this concise format
	// Remember: Only float values are supported, so use cast the value to float
	// SiChenDataType data = { i, float(i * 42), true, true };

	// Add to the list
	listOfDataValues.push_back(data);
}

if (!piChen->SendCycleData(33, 88, 2, listOfDataValues))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendAlarmTriggered
------------------------------------------

### Usage

Sends an alarm raised by the machine to an iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendAlarmTriggered(UINT32 alarmIndex);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`alarmIndex`              |Unique numeric ID of the alarm  |

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Send a notification for alarm #123
pIChen->SendAlarmTriggered(123);
~~~~~~~~~~~~~


CiChenLibraryInterface::SendAlarmReset
--------------------------------------

### Usage

Resets an alarm raised by the machine to an iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendAlarmReset(UINT32 alarmIndex);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`alarmIndex`              |Unique numeric ID of the alarm  |

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Resets alarm #123
pIChen->SendAlarmReset(123);
~~~~~~~~~~~~~


CiChenLibraryInterface::SendAuditTrail
--------------------------------------

### Usage

Sends an *audit trail* to an iChen® 4.0 Server.
An *audit trail* tracks the changing of the value of a settings variable on the controller.

### Syntax

~~~~~~~~~~~~~cpp
bool SendAuditTrail(SiChenDataType dataWithNewValue, UiChenValueType oldValue);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`dataWithNewValue`        |Variable name with new value    |
|`oldValue`                |Old value of the same variable  |

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Allocate data value
SiChenDataType data;

// Set data value to 999, absolute integer value
data.iChenDataValue.UI32 = 999;
data.isFloatingPoint = false;
data.isAbsoluteValue = true;

// Copy variable index
data.iChenDataIndex = 123;

// ... or use this concise format
// SiChenDataType data = { 123, 999, false, true };

// Allocate old value
UiChenValueType oldval(888);

if (!piChen->SendAuditTrail(data, oldval))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendAction
----------------------------------

### Usage

Sends *action codes* representing the machine's current action
to an iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendAction(
	UINT32 primaryActionIndex,
	UINT32 auxiliaryAction1Index,
	UINT32 auxiliaryAction2Index,
	UINT32 auxiliaryAction3Index);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`primaryActionIndex`      |An [action code](https://github.com/chenhsong/OpenProtocol/blob/master/doc/actions.md) representing the primary action that the machine is performing at this instance (set to zero or `NOOP` for none)|
|`auxiliaryAction1Index`<br />`auxiliaryAction2Index`<br />`auxiliaryAction3Index`|[Action codes](https://github.com/chenhsong/OpenProtocol/blob/master/doc/actions.md) representing any other action(s), if any, that the machine is performing concurrently (set to zero or `NOOP` for none)|

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Primary action = 1003, Aux action = 1138
if (!piChen->SendAction(1003, 1138, 0 0))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendStatusReply
----------------------------------------

### Usage

Updates the current status of the controller to an iChen® 4.0 Server
in response to a [`HOST_STATE_REQUEST`](#ichenactivitytype) message sent by the server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendStatusReply(
	UINT32 curOpMode,
	UINT32 curJobMode,
	UINT32 curJobCardId,
	UINT32 curMoldId,
	std::wstring curMoldName);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`curOpMode`               |The current [operation mode](https://github.com/chenhsong/OpenProtocol/blob/master/doc/enums.md#OpModes) of the machine|
|`curJobMode`              |The current [job mode](#job-modes) of the machine|
|`curJobCardId`            |Unique numeric ID of the currently-loaded *job card*|
|`curMoldId`               |Unique numeric ID of the currently-loaded mold|
|`curMoldName`             |Unique name of the currently-loaded mold|

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Status of the machine:
//   Operation mode = Semi-Automatic
//   Job mode = #1 (Idle)
//   Job card ID = 33
//   Mold ID = 88
//   Mold name = ABC-123
if (!piChen->SendStatusReply(2, 1, 33, 88, L"ABC-123"))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendMoldDataUploadRequest
-------------------------------------------------

### Usage

Uploads the current set of settings values of the machine to an iChen® 4.0 Server
to persist it under a mold name.

### Syntax

~~~~~~~~~~~~~cpp
bool SendMoldDataUploadRequest(
	std::wstring moldName,
	std::vector<SiChenDataType> moldData);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`moldName`                |Unique name of the mold         |
|`moldData`                |A list of [`SiChenDataType`](#sichendatatype)'s|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The current version of the iChen® 4.0 Server only accepts the numeric index of
each setting variable, starting from zero.

If variable names are used, a mapping table must be employed
to convert between the numeric index and textual name of each variable.

The server should respond with a [`MOLD_DATA_SAVED`](#ichenactivitytype) message
if this set of mold settings data has successfully been saved.
However, this response message is not guaranteed to arrive.

### Example

~~~~~~~~~~~~~cpp
// Allocate list
std::vector<SiChenDataType> listOfDataValues;

// Fill list with values
for (int i = 0; i < 10; i++)
{
	// Allocate data value
	SiChenDataType data;

	// Set test data value to loop index, absolute integer value
	data.iChenDataValue.UI32 = i * 42;
	data.isFloatingPoint = false;
	data.isAbsoluteValue = true;

	// Copy variable index
	data.iChenDataName = i;

	// ... or use this concise format
	// SiChenDataType data = { i, i * 42, false, true };

	// Add to the list
	listOfDataValues.push_back(data);
}

// Upload as mold name "ABC-123"
if (!piChen->SendMoldDataUploadRequest(L"ABC-123", listOfDataValues))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendMoldSummaryReply
--------------------------------------------

### Usage

Sends the current set of settings values of the machine to an iChen® 4.0 Server,
most likely in response to a [SiChenMoldSummaryReqType](#sichenmoldsummaryreqtype)
sent by the server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendMoldSummaryReply(
	UINT32 terminalIp,
	UINT16 terminalPort,
	std::wstring moldName,
	std::vector<SiChenDataType> moldData);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`terminalIp`              |IP address of the host requesting this information|
|`terminalPort`            |IP port of the host requesting this information|
|`moldName`                |Unique name of the mold         |
|`moldData`                |A list of [`SiChenDataType`](#sichendatatype)'s|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The current version of the iChen® 4.0 Server only accepts the numeric index of
each setting variable.
If variable names are used, a mapping table must be employed
to convert between the numeric index and textual name of each variable.

### Example

~~~~~~~~~~~~~cpp
// Allocate list
std::vector<SiChenDataType> listOfDataValues;

// Fill list with values
for (int i = 0; i < 10; i++)
{
	// Allocate data value
	SiChenDataType data;

	// Set test data value to loop index, absolute integer value
	data.iChenDataValue.UI32 = i * 42;
	data.isFloatingPoint = false;
	data.isAbsoluteValue = true;

	// Copy variable index
	data.iChenDataIndex = i;

	// ... or use this concise format
	// SiChenDataType data = { i, i * 42, false, true };

	// Add to the list
	listOfDataValues.push_back(data);
}

if (!piChen->SendMoldSummaryReply(0x12345678, 5678, L"ABC-123", listOfDataValues))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendMoldDataListRequest
-----------------------------------------------

### Usage

Sends a message to an iChen® 4.0 Server asking for a list of mold settings data
available for download.

### Syntax

~~~~~~~~~~~~~cpp
bool SendMoldDataListRequest(std::wstring searchPattern);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`searchPattern`           |Part of a mold's name to search for, or empty string to get all|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The server should respond with an [`SiChenMoldListRepType`](#sichenmoldlistreptype) message
containing the list of molds matching the search pattern criteria.
However, this response message is not guaranteed to arrive.

### Example

~~~~~~~~~~~~~cpp
// Search for molds containing the words "ABC"
if (!piChen->SendMoldDataListRequest(L"ABC"))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendMoldDataRequest
-------------------------------------------

### Usage

Sends a message to an iChen® 4.0 Server to download a particular
set of mold settings data.

### Syntax

~~~~~~~~~~~~~cpp
bool SendMoldDataRequest(
	UINT32 moldId,
	UINT32 jobCardId,
	UINT32 currentYield,
	UINT32 maxYield);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`moldId`                  |Unique numeric ID of a mold, which must match the mold ID of the *job card* specified under `jobCardId`|
|`jobCardId`               |Unique numeric ID of a *job card*, which much contain a mold ID matching the specified `moldId`|
|`currentYield`            |*Deprecated. This parameter is not used and should be passed zero.*|
|`maxYield`                |*Deprecated. This parameter is not used and should be passed zero.*|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The server should respond with an [`SiChenMoldDataRepType`](#sichenmolddatareptype) message
containing the mold settings data.
However, this response message is not guaranteed to arrive.

### Example

~~~~~~~~~~~~~cpp
// Load the mold with ID=88, job card ID=33
if (!piChen->SendMoldDataRequest(88, 33, 0, 0))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendJobCardListRequest
----------------------------------------------

### Usage

Sends a message to an iChen® 4.0 Server asking for a list of job cards
available for download.

### Syntax

~~~~~~~~~~~~~cpp
bool SendJobCardListRequest();
~~~~~~~~~~~~~

### Parameters

None.

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

The server should respond with an [`SiChenJobCardListRepType`](#sichenjobcardlistreptype) message
containing the list of job cards available for download.
However, this response message is not guaranteed to arrive.

### Example

~~~~~~~~~~~~~cpp
if (!piChen->SendJobCardListRequest())
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendJobCardRequest
------------------------------------------

### Usage

Sends a message to an iChen® 4.0 Server to download a particular job card.

### Syntax

~~~~~~~~~~~~~cpp
bool SendJobCardRequest(UINT32 jobCardId);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`jobCardId`               |Unique numeric ID of a job card|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

This method is deprecated and should not be used.

The server should respond with an [`SiChenMoldListRepType`](#sichenmoldlistreptype) message
containing the list of molds associated with the specified job card.
However, this response message is not guaranteed to arrive.

### Example

~~~~~~~~~~~~~cpp
// Load the job card with ID=33
if (!piChen->SendJobCardRequest(33))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::SendJobCardChanged
------------------------------------------

### Usage

Updates the ID of the currently-loaded job card to an iChen® 4.0 Server.

### Syntax

~~~~~~~~~~~~~cpp
bool SendJobCardChanged(UINT32 newJobCardId);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`newJobCardId`            |Unique numeric ID of the currently-loaded job card|

### Return Value

TRUE if successful, otherwise FALSE.

### Example

~~~~~~~~~~~~~cpp
// Notify the server that the current job card has changed to ID=33
if (!piChen->SendJobCardChanged(33))
{
	// Handle error
}
~~~~~~~~~~~~~


CiChenLibraryInterface::GetNextServerMessage
--------------------------------------------

### Usage

Gets the next message (which must be derived from type [SiChenBaseMessageType](#sichenbasemessagetype))
sent by the server (if any).

### Syntax

~~~~~~~~~~~~~cpp
bool GetNextServerMessage(SiChenBaseMessageType **data);
~~~~~~~~~~~~~

### Parameters

|Parameter                 |Description                     |
|--------------------------|--------------------------------|
|`data`                    |Pointer to a pointer to an [`SiChenBaseMessageType`](#sichenbasemessagetype)-derived structure containing the next message sent by the server|

### Return Value

TRUE if successful, otherwise FALSE.

### Remarks

Notice that this method should *always* return TRUE.
When there are no more messages sent by the server,
a message indicating "No Message" is returned in `data`.

This nethod returns FALSE only when there is a problem
communicating with the server, in which case the client
may disconnect from the server and retry the connection.

You should check the `DataType` field of the returned
message to see if equals `IChenActivityType::NO_SERVER_DATA`
which indicates that there is no outstanding message
sent from the server.

### Important

The pointer returned in `data` should be passed into the
*next* call, otherwise it is an error.

### Memory Management

The memory of the [`SiChenBaseMessageType`](#sichenbasemessagetype)-derived structure
returned by this method is managed internally and should be treated as read-only.

The user should **NOT** free this memory, not should this structure be changed in any way.
The library is responsible for allocating and freeing this memory as necessary.


UiChenValueType
---------------

### Usage

This union is used to hold a single 32-bit integer, floating-point number or a boolean.

### Structure

~~~~~~~~~~~~~cpp
union UiChenValueType
{
	UINT32 UI32;
	float F;
	bool B;
};
~~~~~~~~~~~~~


SiChenDataType
--------------

### Usage

This `struct` is used to hold a single, named numeric variable that can be either boolean,
integral or float-point.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenDataType
{
	UINT32 iChenDataIndex;
	UiChenValueType iChenDataValue;
	bool isFloatingPoint;
	bool isAbsoluteValue;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenDataIndex`          |Numeric index of the variable (starting from zero)|
|`iChenDataValue`          |Value of the variable (type = [`UiChenValueType`](#uichenvaluetype))|
|`isFloatingPoint`         |Does `iChenDataValue` hold a floating-point number?|
|`isAbsoluteValue`         |Does the value in `iChenDataValue` represent an absolute value (e.g. pressure, speed)?<br />If FALSE, it represents a *percentage* value from 0-100.|

### Remarks

Before sending to the server, all data values are converted to floating-point numbers according
to the following rules:

* Booleans are converted into zero and one.

* Integers are converted into floating-point.

* Absolute floating-point values are sent without conversion.

* Relative floating-point values represent percentages and are converted into floating-point
  numbers by dividing by 1000 (i.e. 0 becomes 0.0, 100 becomes 0.1).
  Typically, a server assumes that any data less than 0.1 is a percentage.


SMoldItem
---------

### Usage

This `struct` is used to hold metadata regarding a set of settings data
for a particular mold.

### Structure

~~~~~~~~~~~~~cpp
struct SMoldItem
{
	UINT32 moldItemID;
	std::wstring moldItemName;
	UINT32 SerialID;
	std::wstring moldItemCreateDate;
	std::wstring moldItemCreateTime;
	std::wstring moldItemVersion;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`moldItemID`              |Unique numeric ID of the mold   |
|`moldItemName`            |Unique name of the mold         |
|`SerialID`                |Unique serial number of the machine for which this mold data set was originally created|
|`moldItemCreateDate`      |Date/time that the mold data set was saved, in the format `YYYY/MM/DD HH:MM:SS`|
|`moldItemVersionMajor`<br />`moldItemVersionMinor`|Version of the mold data set|


SJobCardItem
------------

### Usage

This `struct` is used to hold metadata on a single job card.

### Structure

~~~~~~~~~~~~~cpp
struct SJobCardItem
{
	UINT32 jobCardItemId;
	UINT32 jobCardItemNumber;
	std::wstring jobCardItemMoldName;
	std::wstring jobCardItemName;
	UINT32 jobCardItemCurrentYield;
	UINT32 jobCardItemMaxYield;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`jobCardItemId`           |Unique numeric ID of the job card|
|`jobCardItemNumber`       |Unique textual ID of the job card|
|`jobCardItemMoldName`     |Unique name of the mold that has been allocated to this job card|
|`jobCardItemName`         |Unique name of the job card|
|`jobCardItemCurrentYield` |How many items have already been produced under this job card|
|`jobCardItemMaxYield`     |The maximum number of items to produce (production quota) under this job card|

### Remarks

When the current *job mode* is *countable*
(see [here](#countable-and-non-countable-job-modes) for more details), and the *operation mode* is set to
`Automatic` or `SemiAutomatic`, each cycle completed counts towards active production.
The number in `jobCardItemCurrentYield` should be incremented after each cycle
under such circumstances, and the machine should stop production when
`jobCardItemMaxYield` is reached.


IChenActivityType
-----------------

### Usage

Messages can be sent from the iChen® 4.0 Server,
usually in response to a message sent from the controller
to the server.

Some messages (e.g. `SERVER_MESSAGE` and `USER_FORCED_LOGOUT`)
are sent unilaterally by the server, usually when informing the
controller about a change in status.

It is the controller's responsibility to handle these messages
appropriately.

Each message sent by an iChen® 4.0 Server is based on the
base class [`SiChenBaseMessageType`](#sichenbasemessagetype)
which contains a `DataType` field of type `IChenActivityType`
indicating the type of the message.

### Definition

~~~~~~~~~~~~~cpp
enum IChenActivityType
{
	NO_SERVER_DATA,
	JOBMODE_LIST_REPLY,
	SERVER_MESSAGE,
	HOST_STATE_REQUEST,
	PASSWORD_LEVEL_REPLY,
	USER_FORCED_LOGOUT,
	MOLD_DATA_SAVED,
	MOLD_SUMMARY_REQUEST,
	MOLD_LIST_REPLY,
	MOLD_DATA_REPLY,
	JOBCARD_LIST_REPLY,
	JOBCARD_DATA_REPLY
};
~~~~~~~~~~~~~

### Descriptions

|Enum Value|Type of Message|
|----------|-----------|
|`NO_SERVER_DATA`|N/A (There is no outstanding message from the server)|
|`JOBMODE_LIST_REPLY`|[`SiChenJobModeListRepType`](#sichenjobmodelistreptype)|
|`SERVER_MESSAGE`|[`SiChenServerMessageType`](#sichenservermessagetype)|
|`HOST_STATE_REQUEST`|Message sent by the server to request a status update.<br />The controller should call [`SendStatusReply`](#cichenlibraryinterfacesendstatusreply) with the current status of the controller.|
|`PASSWORD_LEVEL_REPLY`|[`SiChenPwdLvRepType`](#sichenpwdlvreptype)|
|`USER_FORCED_LOGOUT`|[`SiChenUserForcedLogoutType`](#sichenuserforcedlogouttype)|
|`MOLD_DATA_SAVED`|Message sent by the server in response to the [`SendMoldDataUploadRequest`](#cichenlibraryinterfacesendmolddatauploadrequest) method call), indicating that the set of mold data settings has been saved|
|`MOLD_SUMMARY_REQUEST`|[`SiChenMoldSummaryReqType`](#sichenmoldsummaryreqtype)|
|`MOLD_LIST_REPLY`|[`SiChenMoldListRepType`](#sichenmoldlistreptype)|
|`MOLD_DATA_REPLY`|[`SiChenMoldDataRepType`](#sichenmolddatareptype)|
|`JOBCARD_LIST_REPLY`|[`SiChenJobCardListRepType`](#sichenjobcardlistreptype)|
|`JOBCARD_DATA_REPLY`|[`SiChenMoldListRepType`](#sichenmoldlistreptype)|


SiChenBaseMessageType
---------------------

### Usage

This `struct` is the base of all message types sent from an iChen® 4.0 Server
to the controller.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenBaseMessageType
{
	bool isConnected;
	IChenActivityType iChenActivityType;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |An enum of [IChenActivityType](#ichenactivitytype) indicating the type of message|
|`isConnected`             |Is the server still connected?<br />If FALSE, the connection to the server has been broken and it is up to the controller to handle this situation -- likely attempt a re-connection on a periodic basis.|


SiChenJobModeListRepType
------------------------

### Usage

This type holds a message sent by an iChen® 4.0 Server when the controller
successfully connects to it.
The message contains a list of names/descriptions, in the specified language,
for all the *job modes* (see [here](#job-modes) for a full list).

### Structure

~~~~~~~~~~~~~cpp
struct SiChenJobModeListRepType : SiChenBaseMessageType
{
	std::vector<std::wstring> returnedJobModeList;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`returnedJobModeList`     |A list of Unicode strings, each being a name/description corresponding to a single *job mode*|

### Remarks

There are fifteen (see [here](#job-modes) for details)
user-definable *job modes* that can be used by an organization.
Since the controller may operate in many different languages (up to the operator
to choose), the text descriptions for each user-defined *job mode* are likely
different for different languages.

The texts contained in this message are to be used by the controller when displaying a
list of possible *job modes* for the operator to choose from.

Only *job modes* `ID01` to `ID15` are relevant.  The *job mode* `Unknown`
is used as a placeholder when a controller is not connected to the server and is
typically not sent.  Therefore, the list of *job mode* names only contain
*job modes* `ID01` to `ID15` in ascending order (i.e. `ID01` = offset 0, `ID02` = offset 1, etc.).

If a particular slot is an empty string, then it represents a *job mode* that should not
be used (and should not be shown on-screen).

When changing the user interface language, a controller should disconnect from
the server and re-initiate a new connection in order to get the updated *job modes*
list in the new language.

### Countable and Non-Countable Job Modes

Certain *job modes* are *countable*, while others are *non-countable*.
Typically, only the `Production` (by default set to `ID02`) *job mode* is *countable*.

When a controller is set to a *countable job mode*, and its *operation mode* is
either `Automatic` or `SemiAutomatic`, any cycle completed counts towards
the production quota.
The machine should stop production when the production quota has been reached.
(Aso see [here](#sjobcarditem).)

When interpreting the list of *job mode* names, any name starting with an asterisk (`*`)
indicates a *countable job mode*.  Names not starting with asterisks are *non-countable*.
When displaying *job mode* names on-screen, strip out the prefix asterisk where necessary.


SiChenServerMessageType
-----------------------

### Usage

This type holds a texual message sent from an iChen® 4.0 Server
to the controller for display on its screen.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenServerMessageType : SiChenBaseMessageType
{
	std::wstring serverMessage;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`serverMessage`           |A message to be displayed on the controller's screen|

### Remarks

Upon receiving this message, the controller should display the message on-screen
in a *modal* manner (i.e. the operator must physically dismiss the message).


SiChenPwdLvRepType
------------------

### Usage

This type holds a reply sent by an iChen® 4.0 Server in response to
the [`SendLoginRequest`](#cichenlibraryinterfacesendloginrequest) method call.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenPwdLvRepType : SiChenBaseMessageType
{
	short userPasswordLevel;
	bool isAllowAuto;
	std::wstring userName;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`userPasswordLevel`       |Authorization level of the password (0-10)|
|`isAllowAuto`             |Is this password authorized to operate the machine in automatic mode?|
|`userName`                |Unique name of the user with the password|

### Remarks

Upon receiving this message, the controller should remove the current operator info
on the machine and revoke all authorities granted to the previous operator.
It should then grant the appropriate authorities (or lack thereof) allowed
by the message's `UserPasswordLevel` field, as well as update the name of
the user on-screen.


SiChenUserForcedLogoutType
--------------------------

### Usage

This type holds a message sent by an iChen® 4.0 Server to the controller
indicating that the controller should revoke the current operator's granted
authorities.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenUserForcedLogoutType : SiChenBaseMessageType
{
	short userPasswordLevel;
	bool isAllowAuto;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`userPasswordLevel`       |Authorization level of the password (0-10), usually zero|
|`isAllowAuto`             |Is the operator authorized to continue to operate the machine in automatic mode? (usually FALSE)|

### Remarks

This message is sent by the server in order to unilaterally remove authorities
granted to the current operator of the machine.

Upon receiving this message, the controller should remove the current operator info
on the machine and revoke all authorities granted to the operator.
It should then grant the appropriate authorities (or lack thereof) allowed
by the message's `UserPasswordLevel` field.

The controller should then call
[SendForcedLogoutReply()](#cichenlibraryinterfacesendforcedlogoutreply)
to acknowledge this request.


SiChenMoldSummaryReqType
------------------------

### Usage

This type holds a message sent by an iChen® 4.0 Server to the controller
indicating that the controller should send a snapshot of the values of all variables
of the machine.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenMoldSummaryReqType : SiChenBaseMessageType
{
	UINT32 terminalIp;
	UINT16 terminalPort;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`terminalIp`              |IP address of the host requesting this information|
|`terminalPort`            |IP port of the host requesting this information|

### Remarks

Upon receiving this message, the controller should call
[SendMoldSummaryReply](#cichenlibraryinterfacesendmoldsummaryreply)
with a list of all the current values of the machine's variables.


SiChenMoldListRepType
---------------------

### Usage

This type holds a reply sent by an iChen® 4.0 Server in response to
the [`SendMoldDataListRequest`](#cichenlibraryinterfacesendmolddatalistrequest) or
the [`SendJobCardRequest`](#cichenlibraryinterfacesendjobcardrequest)
method calls.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenMoldListRepType : SiChenBaseMessageType
{
	std::vector<SMoldItem> returnedMoldList;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`returnedMoldList`        |A list of [`SMoldItem`](#smolditem)'s|

### Remarks

Upon receiving this message, the controller should display the list of molds
on-screen, allowing the operator to choose from the list and download the
chosen mold data set.


SiChenMoldDataRepType
---------------------

### Usage

This type holds a reply sent by an iChen® 4.0 Server in response to
the [`SendMoldDataRequest`](#cichenlibraryinterfacesendmolddatarequest) method call.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenMoldDataRepType : SiChenBaseMessageType
{
	std::wstring moldName;
	std::vector<SiChenDataType> returnedMoldData;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`moldName`                |Unique name of the mold         |
|`returnedMoldData`        |A list of [`SiChenDataType`](#sichendatatype)'s containing machine variable settings for the mold|

### Remarks

Upon receiving this message, the controller should set the variables of the machine
according to the values contained in this message.

The current version of the iChen® 4.0 Server only accepts the numeric index of
each setting variable.
If variable names are used, a mapping table must be employed
to convert between the numeric index and textual name of each variable.


SiChenJobCardListRepType
------------------------

### Usage

This type holds a reply sent by an iChen® 4.0 Server in response to
the [`SendJobCardListRequest`](#cichenlibraryinterfacesendjobcardlistrequest) method call.

### Structure

~~~~~~~~~~~~~cpp
struct SiChenJobCardListRepType : SiChenBaseMessageType
{
	std::vector<SJobCardItem> returnedJobCardList;
};
~~~~~~~~~~~~~

### Fields

|Field                     |Description                     |
|--------------------------|--------------------------------|
|`iChenActivityType`       |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`isConnected`             |*Inherited from [`SiChenBaseMessageType`](#sichenbasemessagetype)*|
|`returnedJobCardList`     |A list of [`SJobCardItem`](#sjobcarditem)'s|

### Remarks

Upon receiving this message, the controller should display the list of job cards
on-screen, allowing the operator to choose from the list.
