/*
 * Copyright (c) 2010-2022 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone 
 * (see https://gitlab.linphone.org/BC/public/liblinphone).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "c-wrapper/internal/c-tools.h"
#include "call-session-params-p.h"
#include "media-session-params-p.h"

#include "media-session-params.h"

#include "core/core.h"
#include "logger/logger.h"

#include "private.h"

using namespace std;

LINPHONE_BEGIN_NAMESPACE

// =============================================================================

void MediaSessionParamsPrivate::clone (const MediaSessionParamsPrivate *src) {
	clean();
	CallSessionParamsPrivate::clone(src);
	audioEnabled = src->audioEnabled;
	audioBandwidthLimit = src->audioBandwidthLimit;
	audioDirection = src->audioDirection;
	audioMulticastEnabled = src->audioMulticastEnabled;
	usedAudioCodec = src->usedAudioCodec;
	videoEnabled = src->videoEnabled;
	videoDirection = src->videoDirection;
	videoMulticastEnabled = src->videoMulticastEnabled;
	usedVideoCodec = src->usedVideoCodec;
	receivedFps = src->receivedFps;
	receivedVideoDefinition = src->receivedVideoDefinition ? linphone_video_definition_ref(src->receivedVideoDefinition) : nullptr;
	sentFps = src->sentFps;
	sentVideoDefinition = src->sentVideoDefinition ? linphone_video_definition_ref(src->sentVideoDefinition) : nullptr;
	realtimeTextEnabled = src->realtimeTextEnabled;
	realtimeTextKeepaliveInterval = src->realtimeTextKeepaliveInterval;
	usedRealtimeTextCodec = src->usedRealtimeTextCodec;
	avpfEnabled = src->avpfEnabled;
	hasAvpfEnabledBeenSet = src->hasAvpfEnabledBeenSet;
	avpfRrInterval = src->avpfRrInterval;
	lowBandwidthEnabled = src->lowBandwidthEnabled;
	recordFilePath = src->recordFilePath;
	earlyMediaSendingEnabled = src->earlyMediaSendingEnabled;
	encryption = src->encryption;
	mandatoryMediaEncryptionEnabled = src->mandatoryMediaEncryptionEnabled;
	_implicitRtcpFbEnabled = src->_implicitRtcpFbEnabled;
	downBandwidth = src->downBandwidth;
	upBandwidth = src->upBandwidth;
	downPtime = src->downPtime;
	upPtime = src->upPtime;
	updateCallWhenIceCompleted = src->updateCallWhenIceCompleted;
	updateCallWhenIceCompletedWithDTLS = src->updateCallWhenIceCompletedWithDTLS;
	if (src->customSdpAttributes)
		customSdpAttributes = sal_custom_sdp_attribute_clone(src->customSdpAttributes);
	for (unsigned int i = 0; i < (unsigned int)LinphoneStreamTypeUnknown; i++) {
		if (src->customSdpMediaAttributes[i])
			customSdpMediaAttributes[i] = sal_custom_sdp_attribute_clone(src->customSdpMediaAttributes[i]);
	}
	rtpBundle = src->rtpBundle;
	recordAware = src->recordAware;
	recordState = src->recordState;
	videoDownloadBandwidth = src->videoDownloadBandwidth;

	micEnabled = src->micEnabled;
	if (src->inputAudioDevice) {
		src->inputAudioDevice->ref();
		inputAudioDevice = src->inputAudioDevice;
	}
	if (src->outputAudioDevice) {
		src->outputAudioDevice->ref();
		outputAudioDevice = src->outputAudioDevice;
	}
}

void MediaSessionParamsPrivate::clean () {
	if (receivedVideoDefinition)
		linphone_video_definition_unref(receivedVideoDefinition);
	if (sentVideoDefinition)
		linphone_video_definition_unref(sentVideoDefinition);
	if (customSdpAttributes)
		sal_custom_sdp_attribute_free(customSdpAttributes);
	for (unsigned int i = 0; i < (unsigned int)LinphoneStreamTypeUnknown; i++) {
		if (customSdpMediaAttributes[i])
			sal_custom_sdp_attribute_free(customSdpMediaAttributes[i]);
	}
	memset(customSdpMediaAttributes, 0, sizeof(customSdpMediaAttributes));

	if (inputAudioDevice) {
		inputAudioDevice->unref();
		inputAudioDevice = nullptr;
	}
	if (outputAudioDevice) {
		outputAudioDevice->unref();
		outputAudioDevice = nullptr;
	}
}

// -----------------------------------------------------------------------------

SalStreamDir MediaSessionParamsPrivate::mediaDirectionToSalStreamDir (LinphoneMediaDirection direction) {
	switch (direction) {
		case LinphoneMediaDirectionInactive:
			return SalStreamInactive;
		case LinphoneMediaDirectionSendOnly:
			return SalStreamSendOnly;
		case LinphoneMediaDirectionRecvOnly:
			return SalStreamRecvOnly;
		case LinphoneMediaDirectionSendRecv:
			return SalStreamSendRecv;
		case LinphoneMediaDirectionInvalid:
			lError() << "LinphoneMediaDirectionInvalid shall not be used";
			return SalStreamInactive;
	}
	return SalStreamSendRecv;
}

LinphoneMediaDirection MediaSessionParamsPrivate::salStreamDirToMediaDirection (SalStreamDir dir) {
	switch (dir) {
		case SalStreamInactive:
			return LinphoneMediaDirectionInactive;
		case SalStreamSendOnly:
			return LinphoneMediaDirectionSendOnly;
		case SalStreamRecvOnly:
			return LinphoneMediaDirectionRecvOnly;
		case SalStreamSendRecv:
			return LinphoneMediaDirectionSendRecv;
	}
	return LinphoneMediaDirectionSendRecv;
}

// -----------------------------------------------------------------------------

void MediaSessionParamsPrivate::adaptToNetwork (LinphoneCore *core, int pingTimeMs) {
	L_Q();
	if ((pingTimeMs > 0) && linphone_config_get_int(linphone_core_get_config(core), "net", "activate_edge_workarounds", 0)) {
		lInfo() << "STUN server ping time is " << pingTimeMs << " ms";
		int threshold = linphone_config_get_int(linphone_core_get_config(core), "net", "edge_ping_time", 500);
		if (pingTimeMs > threshold) {
			/* We might be in a 2G network */
			q->enableLowBandwidth(true);
		} /* else use default settings */
	}
	if (q->lowBandwidthEnabled()) {
		setUpBandwidth(linphone_core_get_edge_bw(core));
		setDownBandwidth(linphone_core_get_edge_bw(core));
		setUpPtime(linphone_core_get_edge_ptime(core));
		setDownPtime(linphone_core_get_edge_ptime(core));
		q->enableVideo(false);
	}
}

// -----------------------------------------------------------------------------

SalStreamDir MediaSessionParamsPrivate::getSalAudioDirection () const {
	L_Q();
	return MediaSessionParamsPrivate::mediaDirectionToSalStreamDir(q->getAudioDirection());
}

SalStreamDir MediaSessionParamsPrivate::getSalVideoDirection () const {
	L_Q();
	return MediaSessionParamsPrivate::mediaDirectionToSalStreamDir(q->getVideoDirection());
}

// -----------------------------------------------------------------------------

void MediaSessionParamsPrivate::setReceivedVideoDefinition (LinphoneVideoDefinition *value) {
	if (receivedVideoDefinition)
		linphone_video_definition_unref(receivedVideoDefinition);
	receivedVideoDefinition = linphone_video_definition_ref(value);
}

void MediaSessionParamsPrivate::setSentVideoDefinition (LinphoneVideoDefinition *value) {
	if (sentVideoDefinition)
		linphone_video_definition_unref(sentVideoDefinition);
	sentVideoDefinition = linphone_video_definition_ref(value);
}

// -----------------------------------------------------------------------------

SalCustomSdpAttribute * MediaSessionParamsPrivate::getCustomSdpAttributes () const {
	return customSdpAttributes;
}

void MediaSessionParamsPrivate::setCustomSdpAttributes (const SalCustomSdpAttribute *csa) {
	if (customSdpAttributes) {
		sal_custom_sdp_attribute_free(customSdpAttributes);
		customSdpAttributes = nullptr;
	}
	if (csa)
		customSdpAttributes = sal_custom_sdp_attribute_clone(csa);
}

// -----------------------------------------------------------------------------

SalCustomSdpAttribute * MediaSessionParamsPrivate::getCustomSdpMediaAttributes (LinphoneStreamType lst) const {
	return customSdpMediaAttributes[lst];
}

void MediaSessionParamsPrivate::setCustomSdpMediaAttributes (LinphoneStreamType lst, const SalCustomSdpAttribute *csa) {
	if (customSdpMediaAttributes[lst]) {
		sal_custom_sdp_attribute_free(customSdpMediaAttributes[lst]);
		customSdpMediaAttributes[lst] = nullptr;
	}
	if (csa)
		customSdpMediaAttributes[lst] = sal_custom_sdp_attribute_clone(csa);
}

bool MediaSessionParamsPrivate::getUpdateCallWhenIceCompleted() const{
	if (encryption == LinphoneMediaEncryptionDTLS){
		lInfo() << "DTLS used, reINVITE requested: " << updateCallWhenIceCompletedWithDTLS;
		return updateCallWhenIceCompletedWithDTLS;
	}
	return updateCallWhenIceCompleted;
}

// =============================================================================

MediaSessionParams::MediaSessionParams () : CallSessionParams(*new MediaSessionParamsPrivate) {
	L_D();
	memset(d->customSdpMediaAttributes, 0, sizeof(d->customSdpMediaAttributes));
}

MediaSessionParams::MediaSessionParams (const MediaSessionParams &other)
	: CallSessionParams(*new MediaSessionParamsPrivate) {
	L_D();
	memset(d->customSdpMediaAttributes, 0, sizeof(d->customSdpMediaAttributes));
	d->clone(other.getPrivate());
}

MediaSessionParams::~MediaSessionParams () {
	L_D();
	d->clean();
}

MediaSessionParams &MediaSessionParams::operator= (const MediaSessionParams &other) {
	L_D();
	if (this != &other)
		d->clone(other.getPrivate());
	return *this;
}

// -----------------------------------------------------------------------------

void MediaSessionParams::initDefault (const std::shared_ptr<Core> &core, LinphoneCallDir dir) {
	L_D();
	CallSessionParams::initDefault(core, dir);
	LinphoneCore *cCore = core->getCCore();
	d->audioEnabled = true;

	LinphoneConference *conference = linphone_core_get_conference(cCore);

	if (conference) {
		// Default videoEnable to conference capabilities if the core is in a conference
		const LinphoneConferenceParams * params = linphone_conference_get_current_params(conference);
		d->videoEnabled = !!linphone_conference_params_video_enabled(params);
	} else {
		if (dir == LinphoneCallOutgoing){
			d->videoEnabled = cCore->video_policy.automatically_initiate;
		}else{
			d->videoEnabled = cCore->video_policy.automatically_accept;
		}
	}
	if (!linphone_core_video_enabled(cCore) && d->videoEnabled) {
		lError() << "LinphoneCore " << linphone_core_get_identity(cCore) << " has video disabled for both capture and display, but video policy is to start the call with video. "
			"This is a possible mis-use of the API. In this case, video is disabled in default LinphoneCallParams";
		d->videoEnabled = false;
	}
	d->realtimeTextEnabled = !!linphone_core_realtime_text_enabled(cCore);
	d->realtimeTextKeepaliveInterval = linphone_core_realtime_text_get_keepalive_interval(cCore);
	d->encryption = linphone_core_get_media_encryption(cCore);
	d->avpfEnabled = (linphone_core_get_avpf_mode(cCore) == LinphoneAVPFEnabled);
	d->hasAvpfEnabledBeenSet = false;
	d->_implicitRtcpFbEnabled = !!linphone_config_get_int(linphone_core_get_config(cCore), "rtp", "rtcp_fb_implicit_rtcp_fb", true);
	d->avpfRrInterval = static_cast<uint16_t>(linphone_core_get_avpf_rr_interval(cCore) * 1000);
	d->audioDirection = LinphoneMediaDirectionSendRecv;
	d->videoDirection = LinphoneMediaDirectionSendRecv;
	d->earlyMediaSendingEnabled = !!linphone_config_get_int(linphone_core_get_config(cCore), "misc", "real_early_media", false);
	d->audioMulticastEnabled = !!linphone_core_audio_multicast_enabled(cCore);
	d->videoMulticastEnabled = !!linphone_core_video_multicast_enabled(cCore);
	d->updateCallWhenIceCompleted = !!linphone_config_get_int(linphone_core_get_config(cCore), "sip", "update_call_when_ice_completed", true);
	/*
	 * At the time of WebRTC/JSSIP interoperability tests, it was found that the ICE re-INVITE was breaking communication.
	 * The update_call_when_ice_completed_with_dtls property is hence set to false.
	 * If this is no longer the case it should be changed to true.
	 * Otherwise an application may decide to set to true as ICE reINVITE is mandatory per ICE RFC and unless from this WebRTC interoperability standpoint
	 * there is no problem in having the ICE re-INVITE to be done when SRTP-DTLS is used.
	 */
	d->updateCallWhenIceCompletedWithDTLS = linphone_config_get_bool(linphone_core_get_config(cCore), "sip", "update_call_when_ice_completed_with_dtls", false);
	d->mandatoryMediaEncryptionEnabled = !!linphone_core_is_media_encryption_mandatory(cCore);
	d->rtpBundle = linphone_core_rtp_bundle_enabled(cCore);
	enableRecordAware(linphone_core_is_record_aware_enabled(cCore));

	d->micEnabled = linphone_core_mic_enabled(cCore);
	setInputAudioDevice(core->getDefaultInputAudioDevice());
	setOutputAudioDevice(core->getDefaultOutputAudioDevice());
}

// -----------------------------------------------------------------------------

bool MediaSessionParams::audioEnabled () const {
	L_D();
	return d->audioEnabled;
}

bool MediaSessionParams::audioMulticastEnabled () const {
	L_D();
	return d->audioMulticastEnabled;
}

void MediaSessionParams::enableAudio (bool value) {
	L_D();
	d->audioEnabled = value;
	//if (d->audioEnabled && (getAudioDirection() == LinphoneMediaDirectionInactive))
	//	setAudioDirection(LinphoneMediaDirectionSendRecv);
}

void MediaSessionParams::enableAudioMulticast (bool value) {
	L_D();
	d->audioMulticastEnabled = value;
}

int MediaSessionParams::getAudioBandwidthLimit () const {
	L_D();
	return d->audioBandwidthLimit;
}

LinphoneMediaDirection MediaSessionParams::getAudioDirection () const {
	L_D();
	return d->audioDirection;
}

const OrtpPayloadType * MediaSessionParams::getUsedAudioCodec () const {
	L_D();
	return d->usedAudioCodec;
}

LinphonePayloadType * MediaSessionParams::getUsedAudioPayloadType () const {
	L_D();
	return d->usedAudioCodec ? linphone_payload_type_new(nullptr, d->usedAudioCodec) : nullptr;
}

void MediaSessionParams::setAudioBandwidthLimit (int value) {
	L_D();
	d->audioBandwidthLimit = value;
}

void MediaSessionParams::setAudioDirection (SalStreamDir direction) {
	L_D();
	d->audioDirection = MediaSessionParamsPrivate::salStreamDirToMediaDirection(direction);
}

void MediaSessionParams::setAudioDirection (LinphoneMediaDirection direction) {
	L_D();
	d->audioDirection = direction;
}

// -----------------------------------------------------------------------------

void MediaSessionParams::enableVideo (bool value) {
	L_D();
	d->videoEnabled = value;
	//if (d->videoEnabled && (getVideoDirection() == LinphoneMediaDirectionInactive))
	//	setVideoDirection(LinphoneMediaDirectionSendRecv);
}

void MediaSessionParams::enableVideoMulticast (bool value) {
	L_D();
	d->videoMulticastEnabled = value;
}

float MediaSessionParams::getReceivedFps () const {
	L_D();
	return d->receivedFps;
}

LinphoneVideoDefinition * MediaSessionParams::getReceivedVideoDefinition () const {
	L_D();
	return d->receivedVideoDefinition;
}

float MediaSessionParams::getSentFps () const {
	L_D();
	return d->sentFps;
}

LinphoneVideoDefinition * MediaSessionParams::getSentVideoDefinition () const {
	L_D();
	return d->sentVideoDefinition;
}

const OrtpPayloadType * MediaSessionParams::getUsedVideoCodec () const {
	L_D();
	return d->usedVideoCodec;
}

LinphonePayloadType * MediaSessionParams::getUsedVideoPayloadType () const {
	L_D();
	return d->usedVideoCodec ? linphone_payload_type_new(nullptr, d->usedVideoCodec) : nullptr;
}

LinphoneMediaDirection MediaSessionParams::getVideoDirection () const {
	L_D();
	return d->videoDirection;
}

void MediaSessionParams::setVideoDirection (SalStreamDir direction) {
	L_D();
	d->videoDirection = MediaSessionParamsPrivate::salStreamDirToMediaDirection(direction);
}

void MediaSessionParams::setVideoDirection (LinphoneMediaDirection direction) {
	L_D();
	d->videoDirection = direction;
}

bool MediaSessionParams::videoEnabled () const {
	L_D();
	return d->videoEnabled;
}

bool MediaSessionParams::videoMulticastEnabled () const {
	L_D();
	return d->videoMulticastEnabled;
}

// -----------------------------------------------------------------------------

void MediaSessionParams::enableFec(bool value) {
	L_D();
	d->fecEnabled = value;
}

bool MediaSessionParams::fecEnabled() const {
	L_D();
	return d->fecEnabled;
}
const OrtpPayloadType * MediaSessionParams::getUsedFecCodec() const {
	L_D();
	return d->usedFecCodec;
}

// -----------------------------------------------------------------------------
void MediaSessionParams::enableRealtimeText (bool value) {
	L_D();
	d->realtimeTextEnabled = value;
}

void MediaSessionParams::setRealtimeTextKeepaliveInterval (unsigned int interval) {
	L_D();
	d->realtimeTextKeepaliveInterval = interval;
}

const OrtpPayloadType * MediaSessionParams::getUsedRealtimeTextCodec () const {
	L_D();
	return d->usedRealtimeTextCodec;
}

LinphonePayloadType * MediaSessionParams::getUsedRealtimeTextPayloadType () const {
	L_D();
	return d->usedRealtimeTextCodec ? linphone_payload_type_new(nullptr, d->usedRealtimeTextCodec) : nullptr;
}

bool MediaSessionParams::realtimeTextEnabled () const {
	L_D();
	return d->realtimeTextEnabled;
}

unsigned int MediaSessionParams::realtimeTextKeepaliveInterval() const {
	L_D();
	return d->realtimeTextKeepaliveInterval;
}

// -----------------------------------------------------------------------------

bool MediaSessionParams::avpfEnabled () const {
	L_D();
	return d->avpfEnabled;
}

bool MediaSessionParams::hasAvpfEnabledBeenSet () const {
	L_D();
	return d->hasAvpfEnabledBeenSet;
}

void MediaSessionParams::enableAvpf (bool value) {
	L_D();
	d->hasAvpfEnabledBeenSet = true;
	d->avpfEnabled = value;
}


uint16_t MediaSessionParams::getAvpfRrInterval () const {
	L_D();
	return d->avpfRrInterval;
}

void MediaSessionParams::setAvpfRrInterval (uint16_t value) {
	L_D();
	d->avpfRrInterval = value;
}

// -----------------------------------------------------------------------------

bool MediaSessionParams::lowBandwidthEnabled () const {
	L_D();
	return d->lowBandwidthEnabled;
}

void MediaSessionParams::enableLowBandwidth (bool value) {
	L_D();
	d->lowBandwidthEnabled = value;
}

// -----------------------------------------------------------------------------

const string& MediaSessionParams::getRecordFilePath () const {
	L_D();
	return d->recordFilePath;
}

void MediaSessionParams::setRecordFilePath (const string &path) {
	L_D();
	d->recordFilePath = path;
}

// -----------------------------------------------------------------------------

bool MediaSessionParams::earlyMediaSendingEnabled () const {
	L_D();
	return d->earlyMediaSendingEnabled;
}

void MediaSessionParams::enableEarlyMediaSending (bool value) {
	L_D();
	d->earlyMediaSendingEnabled = value;
}

// -----------------------------------------------------------------------------

void MediaSessionParams::enableMandatoryMediaEncryption (bool value) {
	L_D();
	d->mandatoryMediaEncryptionEnabled = value;
}

LinphoneMediaEncryption MediaSessionParams::getMediaEncryption () const {
	L_D();
	return d->encryption;
}

bool MediaSessionParams::mandatoryMediaEncryptionEnabled () const {
	L_D();
	return d->mandatoryMediaEncryptionEnabled;
}

void MediaSessionParams::setMediaEncryption (LinphoneMediaEncryption encryption) {
	L_D();
	d->encryption = encryption;
}

// -----------------------------------------------------------------------------

SalMediaProto MediaSessionParams::getMediaProto () const {
	return linphone_media_encryption_to_sal_media_proto(getMediaEncryption(), (avpfEnabled() ? TRUE : FALSE));
}

const char * MediaSessionParams::getRtpProfile () const {
	return sal_media_proto_to_string(getMediaProto());
}

// -----------------------------------------------------------------------------

void MediaSessionParams::addCustomSdpAttribute (const string &attributeName, const string &attributeValue) {
	L_D();
	d->customSdpAttributes = sal_custom_sdp_attribute_append(d->customSdpAttributes, attributeName.c_str(), L_STRING_TO_C(attributeValue));
}

void MediaSessionParams::clearCustomSdpAttributes () {
	L_D();
	d->setCustomSdpAttributes(nullptr);
}

const char * MediaSessionParams::getCustomSdpAttribute (const string &attributeName) const {
	L_D();
	return sal_custom_sdp_attribute_find(d->customSdpAttributes, attributeName.c_str());
}

// -----------------------------------------------------------------------------

void MediaSessionParams::addCustomSdpMediaAttribute (LinphoneStreamType lst, const string &attributeName, const string &attributeValue) {
	L_D();
	d->customSdpMediaAttributes[lst] = sal_custom_sdp_attribute_append(d->customSdpMediaAttributes[lst], attributeName.c_str(), L_STRING_TO_C(attributeValue));
}

void MediaSessionParams::clearCustomSdpMediaAttributes (LinphoneStreamType lst) {
	L_D();
	d->setCustomSdpMediaAttributes(lst, nullptr);
}

const char * MediaSessionParams::getCustomSdpMediaAttribute (LinphoneStreamType lst, const string &attributeName) const {
	L_D();
	return sal_custom_sdp_attribute_find(d->customSdpMediaAttributes[lst], attributeName.c_str());
}

void MediaSessionParams::enableRtpBundle(bool value){
	L_D();
	d->rtpBundle = value;
}

bool MediaSessionParams::rtpBundleEnabled()const{
	L_D();
	return d->rtpBundle;
}

bool MediaSessionParams::isRecording() const {
	L_D();
	return d->recordState == SalMediaRecordOn;
}

void MediaSessionParams::enableRecordAware(bool value) {
	L_D();
	d->recordAware = value;

	if (d->recordAware && d->recordState == SalMediaRecordNone) {
		// If activated set to off to offer it in sdp
		d->recordState = SalMediaRecordOff;
	}
}

bool MediaSessionParams::recordAwareEnabled() const {
	L_D();
	return d->recordAware;
}

void MediaSessionParams::setRecordingState (SalMediaRecord recordState) {
	L_D();
	d->recordState = recordState;
}

SalMediaRecord MediaSessionParams::getRecordingState () const {
	L_D();
	return d->recordState;
}

void MediaSessionParams::enableMic (bool value) {
	L_D();
	d->micEnabled = value;
}

bool MediaSessionParams::isMicEnabled () const {
	L_D();
	return d->micEnabled;
}

void MediaSessionParams::setInputAudioDevice(AudioDevice* device) {
	L_D();
	if (device) {
		device->ref();
	}
	if (d->inputAudioDevice) {
		d->inputAudioDevice->unref();
	}
	d->inputAudioDevice = device;
}

void MediaSessionParams::setOutputAudioDevice(AudioDevice* device) {
	L_D();
	if (device) {
		device->ref();
	}
	if (d->outputAudioDevice) {
		d->outputAudioDevice->unref();
	}
	d->outputAudioDevice = device;
}

AudioDevice* MediaSessionParams::getInputAudioDevice() const {
	L_D();
	return d->inputAudioDevice;
}

AudioDevice* MediaSessionParams::getOutputAudioDevice() const {
	L_D();
	return d->outputAudioDevice;
}

LINPHONE_END_NAMESPACE
