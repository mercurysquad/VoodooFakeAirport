
#include "FakeAirport.h"
#include <IOKit/IOLib.h>
#include <libkern/libkern.h>
#include "IO80211Interface.h"
#include "apple80211_var.h"
#include "apple80211_ioctl.h"

#define PREFIX		"FakeAirport: "
#define super		IO80211Controller

OSDefineMetaClassAndStructors(net_mercurysquad_driver_FakeAirport, IO80211Controller)

bool net_mercurysquad_driver_FakeAirport::init(OSDictionary *dict) {
	IOLog(PREFIX "init\n");
	return super::init(dict);
}

bool net_mercurysquad_driver_FakeAirport::start(IOService *provider) {
	IOLog(PREFIX "starting. attaching netif\n");
	if (!super::start(provider)) {
		IOLog(PREFIX "super start failed\n");
		return false;
	}
	fProvider = provider;
	if (!fProvider) {
		IOLog(PREFIX "pci device cast failed\n");
		return false;
	}
	fProvider->retain();
	if (attachInterface((IONetworkInterface**) &fFakeIF, false)) {
		IOLog(PREFIX "stage 1 success - ");
	} else {
		IOLog(PREFIX "stage 1 failed - ");
	}
	if (fFakeIF) {
		fFakeIF->registerService();
		IOLog(PREFIX "attached.\n");
		return true;
	} else {
		IOLog(PREFIX "cannot attach\n");
		return false;
	}
}

void net_mercurysquad_driver_FakeAirport::free(void) {
	IOLog(PREFIX "freeing\n");
	if (fFakeIF) fFakeIF->release();
	if (fProvider) fProvider->release();
	super::free();
}

void net_mercurysquad_driver_FakeAirport::stop(IOService *provider) {
	IOLog(PREFIX "stopping\n");
	detachInterface(fFakeIF, false);
	super::stop(provider);
}

IOReturn
net_mercurysquad_driver_FakeAirport::getHardwareAddress(IOEthernetAddress *addr) {
	IOLog(PREFIX "getting mac address\n");
	addr->bytes[0] = 0x13;
	addr->bytes[1] = 0x37;
	addr->bytes[2] = 0xDE;
	addr->bytes[3] = 0xAD;
	addr->bytes[4] = 0xFA;
	addr->bytes[5] = 0xCE;
	return kIOReturnSuccess;
}

IOReturn
net_mercurysquad_driver_FakeAirport::setPromiscuousMode( IOEnetPromiscuousMode mode ) {
	IOLog(PREFIX "Setting promisc mode\n");
	return kIOReturnSuccess;
}

IOReturn
net_mercurysquad_driver_FakeAirport::setMulticastMode( IOEnetMulticastMode mode ) {
	IOLog(PREFIX "Setting multicast mode\n");
	return kIOReturnSuccess;
}

IOReturn
net_mercurysquad_driver_FakeAirport::setMulticastList( IOEthernetAddress *addrs, UInt32 count ) {
	IOLog(PREFIX "setting multicast list\n");
	return kIOReturnSuccess;
}

SInt32
net_mercurysquad_driver_FakeAirport::apple80211Request(UInt32 req,
						       int type,
						       IO80211Interface * intf,
						       void * data )
{
	IOLog(PREFIX "Airport request %s info type %d ", ((req == IOCTL_GET_VAL) ? "to get": "to set"), type);
	switch (type) {
		case APPLE80211_IOC_CARD_CAPABILITIES:
			if (req == IOCTL_GET_VAL) {
				apple80211_capability_data* caps =
				(apple80211_capability_data*) data;
				
				caps->version = APPLE80211_VERSION;
				caps->capabilities[0] = APPLE80211_CAP_WEP;
				caps->capabilities[1] = APPLE80211_CAP_IBSS;
				caps->capabilities[2] = APPLE80211_CAP_MONITOR;
				caps->capabilities[3] = APPLE80211_CAP_SHPREAMBLE;
			}
			break;
		case APPLE80211_IOC_STATUS_DEV_NAME:
			if (req == IOCTL_GET_VAL) {
				apple80211_status_dev_data* ret = (apple80211_status_dev_data*) data;
				ret->version = APPLE80211_VERSION;
				strcpy((char*) ret->dev_name, "Voodoo Labs(R) FakeAirport(TM)\0");
			}
			break;
		case APPLE80211_IOC_DRIVER_VERSION:
			if (req == IOCTL_GET_VAL) {
				apple80211_version_data* ret = (apple80211_version_data*) data;
				ret->version = APPLE80211_VERSION;
				ret->string_len = 3;
				strcpy(ret->string, "1.0");
			}
			break;
		case APPLE80211_IOC_HARDWARE_VERSION:
			if (req == IOCTL_GET_VAL) {
				apple80211_version_data* ret = (apple80211_version_data*) data;
				ret->version = APPLE80211_VERSION;
				ret->string_len = 5;
				strcpy(ret->string, "1.0HW");
			}
			break;
		case APPLE80211_IOC_COUNTRY_CODE:
			if (req == IOCTL_GET_VAL) {
				apple80211_country_code_data* ret = (apple80211_country_code_data*) data;
				ret->version = APPLE80211_VERSION;
				strcpy((char*)ret->cc, "IN\0");
			}
			break;
		case APPLE80211_IOC_LOCALE:
			if (req == IOCTL_GET_VAL) {
				apple80211_locale_data* ret = (apple80211_locale_data*) data;
				ret->version = APPLE80211_VERSION;
				ret->locale = APPLE80211_LOCALE_APAC;
			}
			break;
		default:
			IOLog("(unimplemented)");
	};
	IOLog("\n");
	return 0;
}

IOReturn
net_mercurysquad_driver_FakeAirport::registerWithPolicyMaker(IOService * policyMaker) {
	IOReturn ioreturn;
	IOLog(PREFIX "registering with power management\n");
	static IOPMPowerState powerStateArray[ 2 ] = {
		{ 1,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,kIOPMDeviceUsable,kIOPMPowerOn,kIOPMPowerOn,0,0,0,0,0,0,0,0 }
	};
	ioreturn = policyMaker->registerPowerDriver( this, powerStateArray, 2 );
	return ioreturn;
}

IOReturn
net_mercurysquad_driver_FakeAirport::enable(IONetworkInterface *aNetif) {
	IOLog(PREFIX "turning ON\n");
	return kIOReturnSuccess;
};

IOReturn
net_mercurysquad_driver_FakeAirport::disable(IONetworkInterface *aNetif) {
	IOLog(PREFIX "turning OFF\n");
	return kIOReturnSuccess;	
};


const OSString*
net_mercurysquad_driver_FakeAirport::newVendorString() const {
	IOLog(PREFIX "getting vendor string\n");
	return OSString::withCString("Voodoo Labs(R)");
}

const OSString*
net_mercurysquad_driver_FakeAirport::newModelString() const {
	IOLog(PREFIX "getting model string\n");
	return OSString::withCString("FakeAirport(TM)");
}

const OSString*
net_mercurysquad_driver_FakeAirport::newRevisionString() const {
	IOLog(PREFIX "getting revision string\n");
	return OSString::withCString("1");
}