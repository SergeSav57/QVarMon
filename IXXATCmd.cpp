#include <QLibrary>
#include <QtWidgets/QMessageBox>
#include "IXXATCmd.h"

//QLibrary *lib = Q_NULLPTR;

IXXATCmd::IXXATCmd()
{
//    LoadDLL();
}

IXXATCmd::~IXXATCmd()
{
//    lib.unload();
//    delete lib;
}

bool IXXATCmd::LoadDLL()
{
    //lib = new QLibrary("vcinpl.dll");	// должен быть установлен драйвер VCI4-USB-to-CAN compact
    QLibrary lib(QLatin1String("vcinpl"));
//    lib.load();
//    if (!lib.isLoaded()) {
    if(!lib.load()) {
        //sError = "Error: Can't Load IXXAT driver (vcinpl.dll)";
        return false;
    }

	//***********************************
	// general VCI functions
	//***********************************
    m_vciInitialize = reinterpret_cast<p_vciInitialize>(lib.resolve("vciInitialize"));
	if (m_vciInitialize == NULL) {
        //sError = "Error: Can't Find 'vciInitialize'";
        return false;
	}
	//----------------------------
    m_vciFormatError = reinterpret_cast<p_vciFormatError>(lib.resolve("vciFormatError"));
	if (m_vciFormatError == NULL) {
        //sError = "Error: Can't Find 'vciFormatError'";
        return false;
	}
	//----------------------------
    m_vciFindDeviceByHwid = reinterpret_cast<p_vciFindDeviceByHwid>(lib.resolve("vciFindDeviceByHwid"));
	if (m_vciFindDeviceByHwid == NULL) {
        //sError = "Error: Can't Find 'vciFindDeviceByHwid'";
        return false;
	}
	//----------------------------
    m_vciLuidToChar = reinterpret_cast<p_vciLuidToChar>(lib.resolve("vciLuidToChar"));
	if (m_vciLuidToChar == NULL) {
        //sError = "Error: Can't Find 'vciGetVersion'";
        return false;
	}
	//----------------------------
    m_vciGetVersion = reinterpret_cast<p_vciGetVersion>(lib.resolve("vciGetVersion"));
	if (m_vciGetVersion == NULL) {
        //sError = "Error: Can't Find 'vciGetVersion'";
        return false;
	}

	//***********************************
	// device manager specific functions
	//***********************************
    m_vciEnumDeviceOpen = reinterpret_cast<p_vciEnumDeviceOpen>(lib.resolve("vciEnumDeviceOpen"));
	if (m_vciEnumDeviceOpen == NULL) {
        //sError = "Error: Can't Find 'vciEnumDeviceOpen'";
        return false;
	}
	//----------------------------
    m_vciEnumDeviceNext = reinterpret_cast<p_vciEnumDeviceNext>(lib.resolve("vciEnumDeviceNext"));
	if (m_vciEnumDeviceNext == NULL) {
        //sError = "Error: Can't Find 'vciEnumDeviceNext'";
        return false;
	}

	//***********************************
	// device specific functions
	//***********************************
    m_vciDeviceOpen = reinterpret_cast<p_vciDeviceOpen>(lib.resolve("vciDeviceOpen"));
	if (m_vciDeviceOpen == NULL) {
        //sError = "Error: Can't Find 'vciDeviceOpen'";
        return false;
	}
	//----------------------------
    m_vciDeviceClose = reinterpret_cast<p_vciDeviceClose>(lib.resolve("vciDeviceClose"));
	if (m_vciDeviceClose == NULL) {
        //sError = "Error: Can't Find 'vciDeviceClose'";
        return false;
	}

	//***********************************
	// CAN message channel specific functions
	//***********************************
    m_canChannelOpen = reinterpret_cast<p_canChannelOpen>(lib.resolve("canChannelOpen"));
	if (m_canChannelOpen == NULL) {
        //sError = "Error: Can't Find 'canChannelOpen'";
        return false;
	}
	//----------------------------
    m_canChannelGetStatus = reinterpret_cast<p_canChannelGetStatus>(lib.resolve("canChannelGetStatus"));
	if (m_canChannelGetStatus == NULL) {
        //sError = "Error: Can't Find 'canChannelGetStatus'";
        return false;
	}
	//----------------------------
    m_canChannelClose = reinterpret_cast<p_canChannelClose>(lib.resolve("canChannelClose"));
	if (m_canChannelClose == NULL) {
        //sError = "Error: Can't Find 'canChannelClose'";
        return false;
	}
	//----------------------------
    m_canChannelInitialize = reinterpret_cast<p_canChannelInitialize>(lib.resolve("canChannelInitialize"));
	if (m_canChannelInitialize == NULL) {
        //sError = "Error: Can't Find 'canChannelInitialize'";
        return false;
	}
	//----------------------------
    m_canChannelActivate = reinterpret_cast<p_canChannelActivate>(lib.resolve("canChannelActivate"));
	if (m_canChannelActivate == NULL) {
        //sError = "Error: Can't Find 'canChannelActivate'";
        return false;
	}
	//----------------------------
    m_canChannelSendMessage = reinterpret_cast<p_canChannelSendMessage>(lib.resolve("canChannelSendMessage"));
	if (m_canChannelSendMessage == NULL) {
        //sError = "Error: Can't Find 'canChannelSendMessage'";
        return false;
	}
	//----------------------------
    m_canChannelWaitRxEvent = reinterpret_cast<p_canChannelWaitRxEvent>(lib.resolve("canChannelWaitRxEvent"));
	if (m_canChannelWaitRxEvent == NULL) {
        //sError = "Error: Can't Find 'canChannelWaitRxEvent'";
        return false;
	}
	//----------------------------
    m_canChannelReadMessage = reinterpret_cast<p_canChannelReadMessage>(lib.resolve("canChannelReadMessage"));
	if (m_canChannelReadMessage == NULL) {
        //sError = "Error: Can't Find 'canChannelReadMessage'";
        return false;
	}
	//----------------------------
    m_canChannelPeekMessage = reinterpret_cast<p_canChannelPeekMessage>(lib.resolve("canChannelPeekMessage"));
	if (m_canChannelPeekMessage == NULL) {
        //sError = "Error: Can't Find 'canChannelPeekMessage'";
        return false;
	}

	//***********************************
	// CAN controller specific functions
	//***********************************
    m_canControlOpen = reinterpret_cast<p_canControlOpen>(lib.resolve("canControlOpen"));
	if (m_canControlOpen == NULL) {
        //sError = "Error: Can't Find 'canControlOpen'";
        return false;
	}
	//----------------------------
    m_canControlClose = reinterpret_cast<p_canControlClose>(lib.resolve("canControlClose"));
	if (m_canControlClose == NULL) {
        //sError = "Error: Can't Find 'canControlClose'";
        return false;
	}
	//----------------------------
    m_canControlInitialize = reinterpret_cast<p_canControlInitialize>(lib.resolve("canControlInitialize"));
	if (m_canControlInitialize == NULL) {
        //sError = "Error: Can't Find 'canControlInitialize'";
        return false;
	}
	//----------------------------
    m_canControlStart = reinterpret_cast<p_canControlStart>(lib.resolve("canControlStart"));
	if (m_canControlStart == NULL) {
        //sError = "Error: Can't Find 'canControlStart'";
        return false;
	}
	//----------------------------
    m_canControlSetAccFilter = reinterpret_cast<p_canControlSetAccFilter>(lib.resolve("canControlSetAccFilter"));
	if (m_canControlSetAccFilter == NULL) {
        //sError = "Error: Can't Find 'canControlSetAccFilter'";
        return false;
	}
	//----------------------------

	return TRUE;
}


//***********************************
// general VCI functions
//***********************************

HRESULT IXXATCmd::vciInitialize()
{
	if (!m_vciInitialize) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciInitialize is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciInitialize)();
}
//----------------------------
void IXXATCmd::vciFormatError(HRESULT hrError, PCHAR pszText, uint32_t dwSize)
{
	if (!m_vciFormatError) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciFormatError is not valid!", QMessageBox::Ok);
        return;
	}
    return (*m_vciFormatError)(hrError, pszText, dwSize);
}
//----------------------------
HRESULT IXXATCmd::vciFindDeviceByHwid(REFGUID hwid, PVCIID vciid)
{
	if (!m_vciFindDeviceByHwid) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciFindDeviceByHwid is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciFindDeviceByHwid)(hwid, vciid);
}
//----------------------------
HRESULT IXXATCmd::vciLuidToChar(VCIID* vciid, PCHAR luid, LONG* size)
{
	if (!m_vciLuidToChar) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciLuidToChar is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciLuidToChar)(vciid, luid, size);
}
//----------------------------
HRESULT IXXATCmd::vciGetVersion(uint32_t majorVer, uint32_t minorVer)
{
	if (!m_vciGetVersion) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciDeviceOpen is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciGetVersion)(majorVer, minorVer);
}

//***********************************
// device manager specific functions
//***********************************
HRESULT IXXATCmd::vciEnumDeviceOpen(PHANDLE hEnum)
{
	if (!m_vciEnumDeviceOpen) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciEnumDeviceOpen is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciEnumDeviceOpen)(hEnum);
}
//----------------------------
HRESULT IXXATCmd::vciEnumDeviceNext(HANDLE hEnum, PVCIDEVICEINFO pInfo)
{
	if (!m_vciEnumDeviceNext) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciEnumDeviceNext is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciEnumDeviceNext)(hEnum, pInfo);
}

//***********************************
// device specific functions
//***********************************
HRESULT IXXATCmd::vciDeviceOpen(REFVCIID vciid, PHANDLE hDevice)
{
	if (!m_vciDeviceOpen) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciDeviceOpen is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciDeviceOpen)(vciid, hDevice);
}
//----------------------------
HRESULT IXXATCmd::vciDeviceClose(HANDLE hDevice)
{
	if (!m_vciDeviceClose) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "vciDeviceClose is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_vciDeviceClose)(hDevice);
}

//***********************************
// CAN message channel specific functions
//***********************************
HRESULT IXXATCmd::canChannelOpen(HANDLE hDevice, uint32_t dwCanNo, BOOL fExclusive, PHANDLE phCanChn)
{
	if (!m_canChannelOpen) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelOpen is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelOpen)(hDevice, dwCanNo, fExclusive, phCanChn);
}
//----------------------------
HRESULT IXXATCmd::canChannelClose(HANDLE hCanChn)
{
	if (!m_canChannelClose) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelClose is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelClose)(hCanChn);
}
//----------------------------
HRESULT IXXATCmd::canChannelGetStatus(HANDLE hCanChn, PCANCHANSTATUS pStatus)
{
	if (!m_canChannelGetStatus) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelGetStatus is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelGetStatus)(hCanChn, pStatus);
}
//----------------------------
HRESULT IXXATCmd::canChannelInitialize(HANDLE hCanChn, uint16_t wRxFifoSize, uint16_t wRxThreshold, uint16_t wTxFifoSize, uint16_t wTxThreshold)
{
	if (!m_canChannelInitialize) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelInitialize is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelInitialize)(hCanChn, wRxFifoSize, wRxThreshold, wTxFifoSize, wTxThreshold);
}
//----------------------------
HRESULT IXXATCmd::canChannelActivate(HANDLE hCanChn, BOOL fEnable)
{
	if (!m_canChannelActivate) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelActivate is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelActivate)(hCanChn, fEnable);
}
//----------------------------
HRESULT IXXATCmd::canChannelSendMessage(HANDLE hCanChn, uint32_t dwMsTimeout, PCANMSG pCanMsg)
{
	if (!m_canChannelSendMessage) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelSendMessage is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelSendMessage)(hCanChn, dwMsTimeout, pCanMsg);
}
//----------------------------
HRESULT IXXATCmd::canChannelWaitRxEvent(HANDLE hCanChn, uint32_t dwMsTimeout)
{
	if (!m_canChannelWaitRxEvent) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelWaitRxEvent is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelWaitRxEvent)(hCanChn, dwMsTimeout);
}
//----------------------------
HRESULT IXXATCmd::canChannelReadMessage(HANDLE hCanChn, uint32_t dwMsTimeout, PCANMSG pCanMsg)
{
	if (!m_canChannelReadMessage) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelReadMessage is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelReadMessage)(hCanChn, dwMsTimeout, pCanMsg);
}
//----------------------------
HRESULT IXXATCmd::canChannelPeekMessage(HANDLE hCanChn, PCANMSG pCanMsg)
{
	if (!m_canChannelPeekMessage) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canChannelPeekMessage is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canChannelPeekMessage)(hCanChn, pCanMsg);
}

//***********************************
// CAN controller specific functions
//***********************************
HRESULT IXXATCmd::canControlOpen(HANDLE hDevice, uint32_t dwCanNo, PHANDLE phCanCtl)
{
	if (!m_canControlOpen) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canControlOpen is not valid!", QMessageBox::Ok);
		return 0;
	}
	return (*m_canControlOpen)(hDevice, dwCanNo, phCanCtl);
}
//----------------------------
HRESULT IXXATCmd::canControlClose(HANDLE hCanCtl)
{
	if (!m_canControlClose) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canControlClose is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canControlClose)(hCanCtl);
}
//----------------------------
HRESULT IXXATCmd::canControlInitialize(HANDLE hCanCtl, uint8_t bMode, uint8_t bBtr0, uint8_t bBtr1)
{
	if (!m_canControlInitialize) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canControlInitialize is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canControlInitialize)(hCanCtl, bMode, bBtr0, bBtr1);
}
//----------------------------
HRESULT IXXATCmd::canControlStart(HANDLE hCanCtl, BOOL fStart)
{
	if (!m_canControlStart) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canControlStart is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canControlStart)(hCanCtl, fStart);
}
//----------------------------
HRESULT IXXATCmd::canControlSetAccFilter(HANDLE hCanCtl, BOOL fExtend, uint32_t dwCode, uint32_t dwMask)
{
	if (!m_canControlSetAccFilter) {
        QMessageBox::critical(Q_NULLPTR, "Ошибка!", "canControlSetAccFilter is not valid!", QMessageBox::Ok);
        return 0;
	}
	return (*m_canControlSetAccFilter)(hCanCtl, fExtend, dwCode, dwMask);
}
//=========================================================================================================
