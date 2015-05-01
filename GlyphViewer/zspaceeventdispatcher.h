///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#ifndef ZSPACEEVENTDISPATCHER_H
#define ZSPACEEVENTDISPATCHER_H

#include <zSpace.h>
#include <QtCore/QObject>

//This class exists because zSpace events are generated from a different thread than the GUI.  This makes sure that the
//signals emitted cross thread boundaries successfully and wait until the slots connecte to the signals are finished

class ZSpaceEventDispatcher : public QObject
{
	Q_OBJECT

public:
	ZSpaceEventDispatcher(QObject* parent = nullptr);
	~ZSpaceEventDispatcher();

	static void Dispatch(ZSHandle targetHandle, const ZSTrackerEventData* eventData, const void* userData);

signals:
	void ZSpaceButtonPressed(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void ZSpaceButtonReleased(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void ZSpaceStylusMoved(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void ZSpaceStylusTappedOnce(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;

private:
	void EmitZSpaceButtonPressed(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void EmitZSpaceButtonReleased(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void EmitZSpaceStylusMoved(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
	void EmitZSpaceStylusTappedOnce(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const;
};


#endif //ZSPACEEVENTDISPATCHER_H