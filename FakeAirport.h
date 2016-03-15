
#ifndef _FAKEAIRPORT_H
#define _FAKEAIRPORT_H

#include <IOKit/IOLib.h>
#include "IO80211Controller.h"
#include "IO80211Interface.h"

#define IOCTL_GET_VAL 3223349705UL // value of SIOCGA80211

class net_mercurysquad_driver_FakeAirport : public IO80211Controller {
	OSDeclareDefaultStructors(net_mercurysquad_driver_FakeAirport)
public:
	IO80211Interface* fFakeIF;
	IOService* fProvider;
	bool init(OSDictionary *dict);
	bool start(IOService *provider);
	void free(void);
	void stop(IOService *provider);
	virtual SInt32 apple80211Request(UInt32 req, int type, IO80211Interface * intf, void * data);
	virtual IOReturn getHardwareAddress(IOEthernetAddress *addr);
	virtual IOReturn setPromiscuousMode( IOEnetPromiscuousMode mode );
	virtual IOReturn setMulticastMode( IOEnetMulticastMode mode );
	virtual IOReturn setMulticastList( IOEthernetAddress *addrs, UInt32 count );
	virtual const OSString * newVendorString() const;
	virtual const OSString * newModelString() const;
	virtual const OSString * newRevisionString() const;
	virtual IOReturn registerWithPolicyMaker(IOService * policyMaker);
	virtual IOReturn enable(IONetworkInterface *aNetif);
	virtual IOReturn disable(IONetworkInterface *aNetif);
};

#endif//_FAKEAIRPORT_H