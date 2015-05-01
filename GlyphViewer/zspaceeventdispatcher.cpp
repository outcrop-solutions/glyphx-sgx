#include "zspaceeventdispatcher.h"

ZSpaceEventDispatcher::ZSpaceEventDispatcher(QObject* parent) :
	QObject(parent) {

}

ZSpaceEventDispatcher::~ZSpaceEventDispatcher() {

}

void ZSpaceEventDispatcher::Dispatch(ZSHandle targetHandle, const ZSTrackerEventData* eventData, const void* userData) {

	if (userData != nullptr) {

		const ZSpaceEventDispatcher* eventDispatcher = static_cast<const ZSpaceEventDispatcher*>(userData);
		if (eventData->type == ZS_TRACKER_EVENT_BUTTON_PRESS) {

			eventDispatcher->EmitZSpaceButtonPressed(targetHandle, eventData);
		}
		else if (eventData->type == ZS_TRACKER_EVENT_BUTTON_RELEASE) {

			eventDispatcher->EmitZSpaceButtonReleased(targetHandle, eventData);
		}
		else if (eventData->type == ZS_TRACKER_EVENT_MOVE) {

			eventDispatcher->EmitZSpaceStylusMoved(targetHandle, eventData);
		}
		//else if (eventData->type == ZS_TRACKER_EVENT_TAP_SINGLE) {
		//
		//	eventDispatcher->EmitZSpaceStylusTappedOnce(targetHandle, eventData);
		//}
	}
}

void ZSpaceEventDispatcher::EmitZSpaceButtonPressed(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const {

	if ((eventData->buttonId == 1) || (eventData->buttonId == 2)) {

		emit ZSpaceButtonPressed(targetHandle, eventData);
	}
}

void ZSpaceEventDispatcher::EmitZSpaceButtonReleased(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const {

	if (eventData->buttonId == 0) {

		emit ZSpaceButtonReleased(targetHandle, eventData);
	}
}

void ZSpaceEventDispatcher::EmitZSpaceStylusMoved(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const {

	ZSBool isButton2ButtonPressed = false;
	ZSBool isButton1ButtonPressed = false;
	ZSError error = zsIsTargetButtonPressed(targetHandle, 2, &isButton2ButtonPressed);
	error = zsIsTargetButtonPressed(targetHandle, 1, &isButton1ButtonPressed);
	if (isButton2ButtonPressed || isButton1ButtonPressed) {

		emit ZSpaceStylusMoved(targetHandle, eventData);
	}
}

void ZSpaceEventDispatcher::EmitZSpaceStylusTappedOnce(ZSHandle targetHandle, const ZSTrackerEventData* eventData) const {

	emit ZSpaceStylusTappedOnce(targetHandle, eventData);
}