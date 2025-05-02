//#pragma once

//#include "IXXAT/vciapi.h"
#include <IXXAT/vciapi.h>

class IXXATCmd
{
public:
    IXXATCmd();
    ~IXXATCmd();

    bool LoadDLL();

	//***********************************
	// general VCI functions
	//***********************************
	typedef HRESULT(WINAPI* p_vciInitialize)();
	p_vciInitialize m_vciInitialize;
	HRESULT vciInitialize();

    typedef void (WINAPI* p_vciFormatError)(HRESULT hrError, PCHAR pszText, UINT32 dwSize);
	p_vciFormatError m_vciFormatError;
    void vciFormatError(HRESULT hrError, PCHAR pszText, UINT32 dwSize);

	typedef HRESULT(WINAPI* p_vciFindDeviceByHwid)(REFGUID, PVCIID);
	p_vciFindDeviceByHwid m_vciFindDeviceByHwid;
	HRESULT vciFindDeviceByHwid(REFGUID hwid, PVCIID vciid);

    typedef HRESULT(WINAPI* p_vciGetVersion)(UINT32, UINT32);
	p_vciGetVersion m_vciGetVersion;
    HRESULT vciGetVersion(uint32_t majorVer, UINT32 minorVer);

	typedef HRESULT(WINAPI* p_vciLuidToChar)(VCIID*, PCHAR, LONG*);
	p_vciLuidToChar m_vciLuidToChar;
	HRESULT vciLuidToChar(VCIID* vciid, PCHAR luid, LONG* size);

	//***********************************
	// device manager specific functions
	//***********************************
	typedef HRESULT(WINAPI* p_vciEnumDeviceOpen)(PHANDLE hEnum);
	p_vciEnumDeviceOpen m_vciEnumDeviceOpen;
	HRESULT vciEnumDeviceOpen(PHANDLE hEnum);

	typedef HRESULT(WINAPI* p_vciEnumDeviceNext)(HANDLE hEnum, PVCIDEVICEINFO pInfo);
	p_vciEnumDeviceNext m_vciEnumDeviceNext;
	HRESULT vciEnumDeviceNext(HANDLE hEnum, PVCIDEVICEINFO pInfo);

	//***********************************
	// device specific functions
	//***********************************
	typedef HRESULT(WINAPI* p_vciDeviceOpen)(REFVCIID vciid, PHANDLE hDevice);
	p_vciDeviceOpen m_vciDeviceOpen;
	HRESULT vciDeviceOpen(REFVCIID vciid, PHANDLE hDevice);

	typedef HRESULT(WINAPI* p_vciDeviceClose)(HANDLE hDevice);
	p_vciDeviceClose m_vciDeviceClose;
	HRESULT vciDeviceClose(HANDLE hDevice);

	//***********************************
	// CAN message channel specific functions
	//***********************************
    typedef HRESULT(WINAPI* p_canChannelOpen)(HANDLE hDevice, UINT32 dwCanNo, BOOL fExclusive, PHANDLE phCanChn);
	p_canChannelOpen m_canChannelOpen;
    HRESULT canChannelOpen(HANDLE hDevice, UINT32 dwCanNo, BOOL fExclusive, PHANDLE phCanChn);

	typedef HRESULT(WINAPI* p_canChannelClose)(HANDLE hCanChn);
	p_canChannelClose m_canChannelClose;
	HRESULT canChannelClose(HANDLE hCanChn);

	typedef HRESULT(WINAPI* p_canChannelGetStatus)(HANDLE hCanChn, PCANCHANSTATUS pStatus);
	p_canChannelGetStatus m_canChannelGetStatus;
	HRESULT canChannelGetStatus(HANDLE hCanChn, PCANCHANSTATUS pStatus);

    typedef HRESULT(WINAPI* p_canChannelInitialize)(HANDLE hCanChn, UINT16 wRxFifoSize, UINT16 wRxThreshold, UINT16 wTxFifoSize, UINT16 wTxThreshold);
	p_canChannelInitialize m_canChannelInitialize;
    HRESULT canChannelInitialize(HANDLE hCanChn, UINT16 wRxFifoSize, UINT16 wRxThreshold, UINT16 wTxFifoSize, UINT16 wTxThreshold);

	typedef HRESULT(WINAPI* p_canChannelActivate)(HANDLE hCanChn, BOOL fEnable);
	p_canChannelActivate m_canChannelActivate;
	HRESULT canChannelActivate(HANDLE hCanChn, BOOL fEnable);

    typedef HRESULT(WINAPI* p_canChannelSendMessage)(HANDLE hCanChn, UINT32  dwMsTimeout, PCANMSG pCanMsg);
	p_canChannelSendMessage m_canChannelSendMessage;
    HRESULT canChannelSendMessage(HANDLE hCanChn, UINT32  dwMsTimeout, PCANMSG pCanMsg);

    typedef HRESULT(WINAPI* p_canChannelWaitRxEvent)(HANDLE hCanChn, UINT32 dwMsTimeout);
	p_canChannelWaitRxEvent m_canChannelWaitRxEvent;
    HRESULT canChannelWaitRxEvent(HANDLE hCanChn, UINT32 dwMsTimeout);

    typedef HRESULT(WINAPI* p_canChannelReadMessage)(HANDLE hCanChn, UINT32 dwMsTimeout, PCANMSG pCanMsg);
	p_canChannelReadMessage m_canChannelReadMessage;
    HRESULT canChannelReadMessage(HANDLE hCanChn, UINT32 dwMsTimeout, PCANMSG pCanMsg);

	typedef HRESULT(WINAPI* p_canChannelPeekMessage)(HANDLE hCanChn, PCANMSG pCanMsg);
	p_canChannelPeekMessage m_canChannelPeekMessage;
	HRESULT canChannelPeekMessage(HANDLE hCanChn, PCANMSG pCanMsg);

	//***********************************
	// CAN controller specific functions
	//***********************************
    typedef HRESULT(WINAPI* p_canControlOpen)(HANDLE  hDevice, UINT32 dwCanNo, PHANDLE phCanCtl);
	p_canControlOpen m_canControlOpen;
    HRESULT canControlOpen(HANDLE hDevice, UINT32 dwCanNo, PHANDLE phCanCtl);

	typedef HRESULT(WINAPI* p_canControlClose)(HANDLE hCanCtl);
	p_canControlClose m_canControlClose;
	HRESULT canControlClose(HANDLE hCanCtl);

    typedef HRESULT(WINAPI* p_canControlInitialize)(HANDLE hCanCtl, UINT8 bMode, UINT8 bBtr0, UINT8 bBtr1);
	p_canControlInitialize m_canControlInitialize;
    HRESULT canControlInitialize(HANDLE hCanCtl, UINT8 bMode, UINT8 bBtr0, UINT8 bBtr1);

	typedef HRESULT(WINAPI* p_canControlStart)(HANDLE hCanCtl, BOOL fStart);
	p_canControlStart m_canControlStart;
	HRESULT canControlStart(HANDLE hCanCtl, BOOL fStart);

    typedef HRESULT(WINAPI* p_canControlSetAccFilter)(HANDLE hCanCtl, BOOL fExtend, UINT32 dwCode, UINT32 dwMask);
	p_canControlSetAccFilter m_canControlSetAccFilter;
    HRESULT canControlSetAccFilter(HANDLE hCanCtl, BOOL fExtend, UINT32 dwCode, UINT32 dwMask);
	//=============================================================================================================
};

