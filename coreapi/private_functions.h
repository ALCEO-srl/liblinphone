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

#ifndef _PRIVATE_FUNCTIONS_H_
#define _PRIVATE_FUNCTIONS_H_

#ifdef HAVE_XML2
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif

#include <mediastreamer2/msconference.h>

#include "private_types.h"
#include "tester_utils.h"
#include "conference/participant-imdn-state.h"
#include "sal/op.h"
#include "sal/event-op.h"
#include "sal/register-op.h"

#include "linphone/core_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

LinphoneCallCbs *_linphone_call_cbs_new(void);
LinphoneConferenceCbs *_linphone_conference_cbs_new(void);

void linphone_call_notify_state_changed(LinphoneCall *call, LinphoneCallState cstate, const char *message);
void linphone_call_notify_dtmf_received(LinphoneCall *call, int dtmf);
void linphone_call_notify_goclear_ack_sent (LinphoneCall *call);
void linphone_call_notify_encryption_changed(LinphoneCall *call, bool_t on, const char *authentication_token);
void linphone_call_notify_transfer_state_changed(LinphoneCall *call, LinphoneCallState cstate);
void linphone_call_notify_stats_updated(LinphoneCall *call, const LinphoneCallStats *stats);
void linphone_call_notify_info_message_received(LinphoneCall *call, const LinphoneInfoMessage *msg);
void linphone_call_notify_ack_processing(LinphoneCall *call, LinphoneHeaders *msg, bool_t is_received);
void linphone_call_notify_tmmbr_received(LinphoneCall *call, int stream_index, int tmmbr);
void linphone_call_notify_snapshot_taken(LinphoneCall *call, const char *file_path);
void linphone_call_notify_next_video_frame_decoded(LinphoneCall *call);
void linphone_call_notify_camera_not_working(LinphoneCall *call, const char *camera_name);
void linphone_call_notify_audio_device_changed(LinphoneCall *call, LinphoneAudioDevice *audioDevice);
void linphone_call_notify_remote_recording(LinphoneCall *call, bool_t recording);

LinphoneCall * linphone_call_new_outgoing(struct _LinphoneCore *lc, const LinphoneAddress *from, const LinphoneAddress *to, const LinphoneCallParams *params, LinphoneProxyConfig *cfg);
LinphoneCall * linphone_call_new_incoming(struct _LinphoneCore *lc, const LinphoneAddress *from, const LinphoneAddress *to, LinphonePrivate::SalCallOp *op);

LINPHONE_PUBLIC LinphoneCallLog *linphone_call_log_new(LinphoneCore *core, LinphoneCallDir dir, LinphoneAddress *from, LinphoneAddress * to);
void linphone_call_log_set_call_id(LinphoneCallLog *cl, const char *call_id);
LinphonePlayer *linphone_call_build_player(LinphoneCall*call);

LinphonePrivate::SalCallOp *linphone_call_get_op(const LinphoneCall *call);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC LinphoneProxyConfig *linphone_call_get_dest_proxy(const LinphoneCall *call);

LINPHONE_PUBLIC MediaStream * linphone_call_get_stream(LinphoneCall *call, LinphoneStreamType type);
LINPHONE_PUBLIC VideoStream * linphone_core_get_preview_stream(LinphoneCore *call);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC IceSession *linphone_call_get_ice_session(const LinphoneCall *call);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC bool_t linphone_call_get_all_muted(const LinphoneCall *call);

LINPHONE_PUBLIC void linphone_core_set_keep_stream_direction_for_rejected_stream(LinphoneCore *lc, bool_t yesno);
LINPHONE_PUBLIC bool_t linphone_core_get_keep_stream_direction_for_rejected_stream(LinphoneCore *lc);

void _linphone_call_set_conf_ref (LinphoneCall *call, LinphoneConference *ref);
MSAudioEndpoint *_linphone_call_get_endpoint (const LinphoneCall *call);
void _linphone_call_set_endpoint (LinphoneCall *call, MSAudioEndpoint *endpoint);

LinphoneCallParams * linphone_call_params_new(LinphoneCore *core);
SalMediaProto get_proto_from_call_params(const LinphoneCallParams *params);
SalStreamDir get_audio_dir_from_call_params(const LinphoneCallParams *params);
SalStreamDir get_video_dir_from_call_params(const LinphoneCallParams *params);
void linphone_call_params_set_custom_headers(LinphoneCallParams *params, const SalCustomHeader *ch);
void linphone_call_params_set_custom_sdp_attributes(LinphoneCallParams *params, const SalCustomSdpAttribute *csa);
void linphone_call_params_set_custom_sdp_media_attributes(LinphoneCallParams *params, LinphoneStreamType type, const SalCustomSdpAttribute *csa);
bool_t linphone_call_params_get_in_conference(const LinphoneCallParams *params);
void linphone_call_params_set_in_conference(LinphoneCallParams *params, bool_t value);
const char * linphone_call_params_get_conference_id (const LinphoneCallParams *params);
void linphone_call_params_set_conference_id (LinphoneCallParams *params, const char * id);
bool_t linphone_call_params_get_internal_call_update(const LinphoneCallParams *params);
void linphone_call_params_set_internal_call_update(LinphoneCallParams *params, bool_t value);
bool_t linphone_call_params_implicit_rtcp_fb_enabled(const LinphoneCallParams *params);
void linphone_call_params_enable_implicit_rtcp_fb(LinphoneCallParams *params, bool_t value);
int linphone_call_params_get_down_bandwidth(const LinphoneCallParams *params);
void linphone_call_params_set_down_bandwidth(LinphoneCallParams *params, int value);
int linphone_call_params_get_up_bandwidth(const LinphoneCallParams *params);
void linphone_call_params_set_up_bandwidth(LinphoneCallParams *params, int value);
int linphone_call_params_get_down_ptime(const LinphoneCallParams *params);
void linphone_call_params_set_down_ptime(LinphoneCallParams *params, int value);
int linphone_call_params_get_up_ptime(const LinphoneCallParams *params);
void linphone_call_params_set_up_ptime(LinphoneCallParams *params, int value);
SalCustomHeader * linphone_call_params_get_custom_headers(const LinphoneCallParams *params);
SalCustomSdpAttribute * linphone_call_params_get_custom_sdp_attributes(const LinphoneCallParams *params);
SalCustomSdpAttribute * linphone_call_params_get_custom_sdp_media_attributes(const LinphoneCallParams *params, LinphoneStreamType type);
LinphoneCall * linphone_call_params_get_referer(const LinphoneCallParams *params);
void linphone_call_params_set_referer(LinphoneCallParams *params, LinphoneCall *referer);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC bool_t linphone_call_params_get_update_call_when_ice_completed(const LinphoneCallParams *params);

void linphone_call_params_set_update_call_when_ice_completed(LinphoneCallParams *params, bool_t value);
void linphone_call_params_set_sent_vsize(LinphoneCallParams *params, MSVideoSize vsize);
void linphone_call_params_set_recv_vsize(LinphoneCallParams *params, MSVideoSize vsize);
void linphone_call_params_set_sent_video_definition(LinphoneCallParams *params, LinphoneVideoDefinition *vdef);
void linphone_call_params_set_received_video_definition(LinphoneCallParams *params, LinphoneVideoDefinition *vdef);
void linphone_call_params_set_sent_fps(LinphoneCallParams *params, float value);
void linphone_call_params_set_received_fps(LinphoneCallParams *params, float value);
void linphone_call_params_set_used_audio_codec(LinphoneCallParams *params, OrtpPayloadType *codec);
void linphone_call_params_set_used_video_codec(LinphoneCallParams *params, OrtpPayloadType *codec);
void linphone_call_params_set_used_fec_codec (LinphoneCallParams *params, OrtpPayloadType *codec);
void linphone_call_params_set_used_text_codec(LinphoneCallParams *params, OrtpPayloadType *codec);
bool_t linphone_call_params_get_no_user_consent(const LinphoneCallParams *params);
time_t linphone_call_params_get_start_time (const LinphoneCallParams *params);
time_t linphone_call_params_get_end_time (const LinphoneCallParams *params);
const char * linphone_call_params_get_description (const LinphoneCallParams *params);


void linphone_call_params_set_end_time (LinphoneCallParams *params, time_t time);
void linphone_call_params_set_start_time (LinphoneCallParams *params, time_t time);
void linphone_call_params_set_description (LinphoneCallParams *params, const char * desc);
void linphone_call_params_set_conference_creation (LinphoneCallParams *params, bool_t conference_creation);
// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC void linphone_call_params_set_no_user_consent(LinphoneCallParams *params, bool_t value);
LINPHONE_PUBLIC void linphone_call_start_basic_incoming_notification(LinphoneCall *call);
LINPHONE_PUBLIC void linphone_call_start_push_incoming_notification(LinphoneCall *call);
LINPHONE_PUBLIC LinphoneCall *linphone_call_new_incoming_with_callid(LinphoneCore *lc, const char *callid);
LINPHONE_PUBLIC bool_t linphone_call_is_op_configured (const LinphoneCall *call);

void _linphone_core_stop_async_end(LinphoneCore *lc);
void _linphone_core_uninit(LinphoneCore *lc);
void linphone_core_write_auth_info(LinphoneCore *lc, LinphoneAuthInfo *ai);
void linphone_core_stop_tone_manager(LinphoneCore *lc);
const LinphoneAuthInfo *_linphone_core_find_tls_auth_info(LinphoneCore *lc);
const LinphoneAuthInfo *_linphone_core_find_indexed_tls_auth_info(LinphoneCore *lc, const char *username, const char *domain);
const LinphoneAuthInfo *_linphone_core_find_auth_info(LinphoneCore *lc, const char *realm, const char *username, const char *domain, const char *algorithm, bool_t ignore_realm);
void linphone_auth_info_fill_belle_sip_event(const LinphoneAuthInfo *auth_info, belle_sip_auth_event *event);
void linphone_core_fill_belle_sip_auth_event(LinphoneCore *lc, belle_sip_auth_event *event, const char *username, const char *domain);

void linphone_core_update_proxy_register(LinphoneCore *lc);
const char *linphone_core_get_nat_address_resolved(LinphoneCore *lc);

int linphone_proxy_config_send_publish(LinphoneProxyConfig *cfg, LinphonePresenceModel *presence);
void linphone_proxy_config_set_state(LinphoneProxyConfig *cfg, LinphoneRegistrationState rstate, const char *message);
void linphone_proxy_config_stop_refreshing(LinphoneProxyConfig *obj);
void linphone_proxy_config_write_all_to_config_file(LinphoneCore *lc);
void _linphone_proxy_config_release(LinphoneProxyConfig *cfg);
void _linphone_proxy_config_unpublish(LinphoneProxyConfig *obj);
void linphone_proxy_config_notify_publish_state_changed(LinphoneProxyConfig *cfg, LinphonePublishState state);
LinphoneEvent *linphone_proxy_config_create_publish(LinphoneProxyConfig *cfg, const char *event, int expires);

/*
 * returns service route as defined in as defined by rfc3608, might be a list instead of just one.
 * Can be NULL
 * */
const LinphoneAddress* linphone_proxy_config_get_service_route(const LinphoneProxyConfig* cfg);
const LinphoneAddress *_linphone_proxy_config_get_contact_without_params (const LinphoneProxyConfig *cfg);
LinphonePrivate::SalRegisterOp *linphone_proxy_config_get_op(const LinphoneProxyConfig *cfg);
/* Returns as a LinphoneAddress the Contact header sent in a register, fixed thanks to nat helper.*/
LINPHONE_PUBLIC LinphoneAddress * linphone_proxy_config_get_transport_contact(LinphoneProxyConfig *cfg);

void linphone_friend_list_invalidate_subscriptions(LinphoneFriendList *list);
void linphone_friend_list_notify_presence_received(LinphoneFriendList *list, LinphoneEvent *lev, const LinphoneContent *body);
void linphone_friend_list_subscription_state_changed(LinphoneCore *lc, LinphoneEvent *lev, LinphoneSubscriptionState state);
void linphone_friend_list_invalidate_friends_maps(LinphoneFriendList *list);

/**
 * Removes all bodyless friend lists.
 * @param[in] lc #LinphoneCore object
 */
void linphone_core_clear_bodyless_friend_lists(LinphoneCore *lc);
void _linphone_friend_list_release(LinphoneFriendList *list);
/*get rls either from list or core if any*/
const LinphoneAddress * _linphone_friend_list_get_rls_address(const LinphoneFriendList *list);

LINPHONE_PUBLIC void linphone_friend_invalidate_subscription(LinphoneFriend *lf);
void linphone_friend_close_subscriptions(LinphoneFriend *lf);
void _linphone_friend_release(LinphoneFriend *lf);
LINPHONE_PUBLIC void linphone_friend_update_subscribes(LinphoneFriend *fr, bool_t only_when_registered);
void linphone_friend_notify(LinphoneFriend *lf, LinphonePresenceModel *presence);
void linphone_friend_apply(LinphoneFriend *fr, LinphoneCore *lc);
void linphone_friend_add_incoming_subscription(LinphoneFriend *lf, LinphonePrivate::SalOp *op);
void linphone_friend_remove_incoming_subscription(LinphoneFriend *lf, LinphonePrivate::SalOp *op);
const char * linphone_friend_phone_number_to_sip_uri(LinphoneFriend *lf, const char *phone_number);
const char * linphone_friend_sip_uri_to_phone_number(LinphoneFriend *lf, const char *uri);
void linphone_friend_clear_presence_models(LinphoneFriend *lf);
LinphoneFriend *linphone_friend_list_find_friend_by_inc_subscribe(const LinphoneFriendList *list, LinphonePrivate::SalOp *op);
LinphoneFriend *linphone_friend_list_find_friend_by_out_subscribe(const LinphoneFriendList *list, LinphonePrivate::SalOp *op);
LinphoneFriend *linphone_core_find_friend_by_out_subscribe(const LinphoneCore *lc, LinphonePrivate::SalOp *op);
LinphoneFriend *linphone_core_find_friend_by_inc_subscribe(const LinphoneCore *lc, LinphonePrivate::SalOp *op);
MSList *linphone_find_friend_by_address(MSList *fl, const LinphoneAddress *addr, LinphoneFriend **lf);
bool_t linphone_core_should_subscribe_friends_only_when_registered(const LinphoneCore *lc);
void linphone_core_update_friends_subscriptions(LinphoneCore *lc);
void _linphone_friend_list_update_subscriptions(LinphoneFriendList *list, LinphoneProxyConfig *cfg, bool_t only_when_registered);
void linphone_core_friends_storage_init(LinphoneCore *lc);
LINPHONE_PUBLIC int linphone_core_friends_storage_resync_friends_lists(LinphoneCore *lc);
void linphone_core_friends_storage_close(LinphoneCore *lc);
void linphone_core_store_friend_in_db(LinphoneCore *lc, LinphoneFriend *lf);
void linphone_core_remove_friend_from_db(LinphoneCore *lc, LinphoneFriend *lf);
void linphone_core_store_friends_list_in_db(LinphoneCore *lc, LinphoneFriendList *list);
void linphone_core_remove_friends_list_from_db(LinphoneCore *lc, LinphoneFriendList *list);
LINPHONE_PUBLIC MSList* linphone_core_fetch_friends_from_db(LinphoneCore *lc, LinphoneFriendList *list);
LINPHONE_PUBLIC MSList* linphone_core_fetch_friends_lists_from_db(LinphoneCore *lc);
LINPHONE_PUBLIC LinphoneFriendListStatus linphone_friend_list_import_friend(LinphoneFriendList *list, LinphoneFriend *lf, bool_t synchronize);
LinphoneFriendListCbs * linphone_friend_list_cbs_new(void);
void linphone_friend_list_set_current_callbacks(LinphoneFriendList *friend_list, LinphoneFriendListCbs *cbs);
void linphone_friend_add_addresses_and_numbers_into_maps(LinphoneFriend *lf, LinphoneFriendList *list);

int linphone_parse_host_port(const char *input, char *host, size_t hostlen, int *port);
int parse_hostname_to_addr(const char *server, struct sockaddr_storage *ss, socklen_t *socklen, int default_port);

bool_t host_has_ipv6_network(void);
bool_t lp_spawn_command_line_sync(const char *command, char **result,int *command_ret);

static MS2_INLINE void set_string(char **dest, const char *src, bool_t lowercase){
	if (*dest){
		ms_free(*dest);
		*dest=NULL;
	}
	if (src) {
		*dest=ms_strdup(src);
		if (lowercase) {
			char *cur = *dest;
			for (; *cur; cur++) *cur = (char)tolower(*cur);
		}
	}
}

void linphone_process_authentication(LinphoneCore* lc, LinphonePrivate::SalOp *op);
void linphone_authentication_ok(LinphoneCore *lc, LinphonePrivate::SalOp *op);
void linphone_subscription_new(LinphoneCore *lc, LinphonePrivate::SalSubscribeOp *op, const char *from);
void linphone_core_send_presence(LinphoneCore *lc, LinphonePresenceModel *presence);
void linphone_notify_parse_presence(const char *content_type, const char *content_subtype, const char *body, SalPresenceModel **result);
void linphone_notify_convert_presence_to_xml(LinphonePrivate::SalOp *op, SalPresenceModel *presence, const char *contact, char **content);
void linphone_notify_recv(LinphoneCore *lc, LinphonePrivate::SalOp *op, SalSubscribeStatus ss, SalPresenceModel *model);
void linphone_proxy_config_process_authentication_failure(LinphoneCore *lc, LinphonePrivate::SalOp *op);


void linphone_subscription_answered(LinphoneCore *lc, LinphonePrivate::SalOp *op);
void linphone_subscription_closed(LinphoneCore *lc, LinphonePrivate::SalOp *op);

void linphone_core_update_allocated_audio_bandwidth(LinphoneCore *lc);

LINPHONE_PUBLIC int linphone_run_stun_tests(LinphoneCore *lc, int audioPort, int videoPort, int textPort,
	char *audioCandidateAddr, int *audioCandidatePort, char *videoCandidateAddr, int *videoCandidatePort, char *textCandidateAddr, int *textCandidatePort);
void linphone_core_resolve_stun_server(LinphoneCore *lc);
LINPHONE_PUBLIC const struct addrinfo *linphone_core_get_stun_server_addrinfo(LinphoneCore *lc);
LINPHONE_PUBLIC void linphone_core_enable_short_turn_refresh(LinphoneCore *lc, bool_t enable);
LINPHONE_PUBLIC void linphone_call_stats_fill(LinphoneCallStats *stats, MediaStream *ms, OrtpEvent *ev);
void linphone_call_stats_update(LinphoneCallStats *stats, MediaStream *stream);
LinphoneCallStats *_linphone_call_stats_new(void);
void _linphone_call_stats_set_ice_state (LinphoneCallStats *stats, LinphoneIceState state);
void _linphone_call_stats_set_type (LinphoneCallStats *stats, LinphoneStreamType type);
void _linphone_call_stats_set_received_rtcp (LinphoneCallStats *stats, mblk_t *m);
mblk_t *_linphone_call_stats_get_sent_rtcp (const LinphoneCallStats *stats);
void _linphone_call_stats_set_sent_rtcp (LinphoneCallStats *stats, mblk_t *m);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC int _linphone_call_stats_get_updated (const LinphoneCallStats *stats);

void _linphone_call_stats_set_updated (LinphoneCallStats *stats, int updated);
void _linphone_call_stats_set_rtp_stats (LinphoneCallStats *stats, const rtp_stats_t *rtpStats);
void _linphone_call_stats_set_download_bandwidth (LinphoneCallStats *stats, float bandwidth);
void _linphone_call_stats_set_upload_bandwidth (LinphoneCallStats *stats, float bandwidth);
void _linphone_call_stats_set_rtcp_download_bandwidth (LinphoneCallStats *stats, float bandwidth);
void _linphone_call_stats_set_rtcp_upload_bandwidth (LinphoneCallStats *stats, float bandwidth);
void _linphone_call_stats_set_ip_family_of_remote (LinphoneCallStats *stats, LinphoneAddressFamily family);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC bool_t _linphone_call_stats_rtcp_received_via_mux (const LinphoneCallStats *stats);

bool_t linphone_core_media_description_contains_video_stream(const LinphonePrivate::SalMediaDescription *md);

void linphone_core_send_initial_subscribes(LinphoneCore *lc);

void linphone_proxy_config_update(LinphoneProxyConfig *cfg);
void linphone_account_update(LinphoneAccount *account);
LinphoneProxyConfig * linphone_core_lookup_known_proxy(LinphoneCore *lc, const LinphoneAddress *uri);
LinphoneProxyConfig * linphone_core_lookup_proxy_by_identity_strict(LinphoneCore *lc, const LinphoneAddress *uri);
LinphoneProxyConfig * linphone_core_lookup_proxy_by_identity(LinphoneCore *lc, const LinphoneAddress *uri);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC LinphoneAccount * linphone_core_lookup_known_account(LinphoneCore *lc, const LinphoneAddress *uri);
LinphoneAccount * linphone_core_lookup_account_by_identity_strict(LinphoneCore *lc, const LinphoneAddress *uri);
LinphoneAccount * linphone_core_lookup_account_by_identity(LinphoneCore *lc, const LinphoneAddress *uri);
const char *linphone_core_find_best_identity(LinphoneCore *lc, const LinphoneAddress *to);
LINPHONE_PUBLIC void linphone_core_get_local_ip(LinphoneCore *lc, int af, const char *dest, char *result);

LinphoneProxyConfig *linphone_proxy_config_new_from_config_file(LinphoneCore *lc, int index);
void linphone_proxy_config_write_to_config_file(LinphoneConfig* config,LinphoneProxyConfig *obj, int index);

LinphoneReason linphone_core_message_received(LinphoneCore *lc, LinphonePrivate::SalOp *op, const SalMessage *msg);

void linphone_call_init_media_streams(LinphoneCall *call);
void linphone_call_start_media_streams_for_ice_gathering(LinphoneCall *call);
void linphone_call_stop_media_streams(LinphoneCall *call);
int _linphone_core_apply_transports(LinphoneCore *lc);

void linphone_core_start_waiting(LinphoneCore *lc, const char *purpose);
void linphone_core_update_progress(LinphoneCore *lc, const char *purpose, float progresses);
void linphone_core_stop_waiting(LinphoneCore *lc);

extern LinphonePrivate::Sal::Callbacks linphone_sal_callbacks;
LINPHONE_PUBLIC bool_t linphone_core_rtcp_enabled(const LinphoneCore *lc);
LINPHONE_PUBLIC bool_t linphone_core_symmetric_rtp_enabled(LinphoneCore*lc);
bool_t _linphone_core_is_conference_creation (const LinphoneCore *lc, const LinphoneAddress *addr);
LinphoneChatRoom *_linphone_core_create_server_group_chat_room (LinphoneCore *lc, LinphonePrivate::SalCallOp *op);

void linphone_core_queue_task(LinphoneCore *lc, belle_sip_source_func_t task_fun, void *data, const char *task_description);


LINPHONE_PUBLIC LinphoneProxyConfigAddressComparisonResult linphone_proxy_config_address_equal(const LinphoneAddress *a, const LinphoneAddress *b);
LINPHONE_PUBLIC LinphoneProxyConfigAddressComparisonResult linphone_proxy_config_is_server_config_changed(const LinphoneProxyConfig* obj);
/**
 * unregister without moving the register_enable flag
 */
void _linphone_proxy_config_unregister(LinphoneProxyConfig *obj);

/* conference */
LINPHONE_PUBLIC bool_t linphone_participant_preserve_session(const LinphoneParticipant *participant);
void _linphone_conference_notify_participant_added(LinphoneConference *conference, const LinphoneParticipant *participant);
void _linphone_conference_notify_participant_removed(LinphoneConference *conference, const LinphoneParticipant *participant);
void _linphone_conference_notify_participant_device_added(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device);
void _linphone_conference_notify_participant_device_removed(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device);
void _linphone_conference_notify_participant_admin_status_changed(LinphoneConference *conference, const LinphoneParticipant *participant);
void _linphone_conference_notify_participant_device_media_capability_changed(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device);
void _linphone_conference_notify_participant_device_media_availability_changed(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device);
void _linphone_conference_notify_participant_device_state_changed(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device, const LinphoneParticipantDeviceState state);
void _linphone_conference_notify_state_changed(LinphoneConference *conference, LinphoneConferenceState newState);
void _linphone_conference_notify_available_media_changed(LinphoneConference *conference);
void _linphone_conference_notify_subject_changed(LinphoneConference *conference, const char *subject);
void _linphone_conference_notify_participant_device_is_speaking_changed(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device, bool_t is_speaking);
void _linphone_conference_notify_participant_device_is_muted(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device, bool_t is_muted);
void _linphone_conference_notify_active_speaker_participant_device(LinphoneConference *conference, const LinphoneParticipantDevice *participant_device);

void _linphone_participant_device_notify_is_speaking_changed(LinphoneParticipantDevice *participant_device, bool_t is_speaking);
void _linphone_participant_device_notify_is_muted(LinphoneParticipantDevice *participant_device, bool_t is_muted);
void _linphone_participant_device_notify_state_changed(LinphoneParticipantDevice *participant_device, const LinphoneParticipantDeviceState state);
void _linphone_participant_device_notify_stream_availability_changed(LinphoneParticipantDevice *participant_device, bool_t available, const LinphoneStreamType stream_type);
void _linphone_participant_device_notify_stream_capability_changed(LinphoneParticipantDevice *participant_device, LinphoneMediaDirection direction, const LinphoneStreamType stream_type);

void linphone_conference_scheduler_notify_state_changed(LinphoneConferenceScheduler *conference_scheduler, LinphoneConferenceSchedulerState state);
void linphone_conference_scheduler_notify_invitations_sent(LinphoneConferenceScheduler *conference_scheduler, bctbx_list_t *failed_invites);

LINPHONE_PUBLIC void linphone_participant_device_set_state (LinphoneParticipantDevice *participant_device, LinphoneParticipantDeviceState state);

LINPHONE_PUBLIC bool_t linphone_conference_params_is_static(const LinphoneConferenceParams *params);

/*account*/
void _linphone_account_notify_registration_state_changed(LinphoneAccount* account, LinphoneRegistrationState state, const char *message);

/*chat*/
LinphoneChatRoom *_linphone_server_group_chat_room_new (LinphoneCore *core, LinphonePrivate::SalCallOp *op);
void linphone_chat_room_set_call(LinphoneChatRoom *cr, LinphoneCall *call);
LinphoneChatRoomCbs * _linphone_chat_room_cbs_new (void);
void _linphone_chat_room_notify_is_composing_received(LinphoneChatRoom *cr, const LinphoneAddress *remoteAddr, bool_t isComposing);
void _linphone_chat_room_notify_message_received(LinphoneChatRoom *cr, LinphoneChatMessage *msg);
void _linphone_chat_room_notify_messages_received(LinphoneChatRoom *cr, const bctbx_list_t* chat_messages);
void _linphone_chat_room_notify_new_event(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_new_events(LinphoneChatRoom *cr, const bctbx_list_t *event_logs);
void _linphone_chat_room_notify_participant_added(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_participant_removed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_participant_device_added(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_participant_device_removed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_participant_device_state_changed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log, const LinphoneParticipantDeviceState state);
void _linphone_chat_room_notify_participant_device_media_availability_changed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_participant_admin_status_changed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_state_changed(LinphoneChatRoom *cr, LinphoneChatRoomState newState);
void _linphone_chat_room_notify_security_event(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_subject_changed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_conference_joined(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_conference_left(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_ephemeral_event(LinphoneChatRoom *cr, const LinphoneEventLog *eventLog);
void _linphone_chat_room_notify_ephemeral_message_timer_started(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_ephemeral_message_deleted(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_undecryptable_message_received(LinphoneChatRoom *cr, LinphoneChatMessage *msg);
void _linphone_chat_room_notify_chat_message_received(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_chat_messages_received(LinphoneChatRoom *cr, const bctbx_list_t *event_logs);
void _linphone_chat_room_notify_chat_message_sending(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_chat_message_sent(LinphoneChatRoom *cr, const LinphoneEventLog *event_log);
void _linphone_chat_room_notify_conference_address_generation(LinphoneChatRoom *cr);
void _linphone_chat_room_notify_participant_device_fetch_requested(LinphoneChatRoom *cr, const LinphoneAddress *participantAddr);
void _linphone_chat_room_notify_participants_capabilities_checked(LinphoneChatRoom *cr, const LinphoneAddress *deviceAddr, const bctbx_list_t *participantsAddr);
void _linphone_chat_room_notify_participant_registration_subscription_requested(LinphoneChatRoom *cr, const LinphoneAddress *participantAddr);
void _linphone_chat_room_notify_participant_registration_unsubscription_requested(LinphoneChatRoom *cr, const LinphoneAddress *participantAddr);
void _linphone_chat_room_notify_chat_message_should_be_stored(LinphoneChatRoom *cr, LinphoneChatMessage *msg);
void _linphone_chat_room_notify_chat_message_participant_imdn_state_changed(LinphoneChatRoom *cr, LinphoneChatMessage *msg, const LinphoneParticipantImdnState *state);
void _linphone_chat_room_clear_callbacks (LinphoneChatRoom *cr);

void _linphone_chat_message_notify_msg_state_changed(LinphoneChatMessage* msg, LinphoneChatMessageState state);
void _linphone_chat_message_notify_participant_imdn_state_changed(LinphoneChatMessage* msg, const LinphoneParticipantImdnState *state);
void _linphone_chat_message_notify_file_transfer_recv(LinphoneChatMessage *msg, LinphoneContent* content, const LinphoneBuffer *buffer);
void _linphone_chat_message_notify_file_transfer_send(LinphoneChatMessage *msg, LinphoneContent* content, size_t offset, size_t size);
void _linphone_chat_message_notify_file_transfer_send_chunk(LinphoneChatMessage *msg, LinphoneContent* content, size_t offset, size_t size, LinphoneBuffer *buffer);
void _linphone_chat_message_notify_file_transfer_progress_indication(LinphoneChatMessage *msg, LinphoneContent* content, size_t offset, size_t total);
void _linphone_chat_message_notify_ephemeral_message_timer_started(LinphoneChatMessage* msg);
void _linphone_chat_message_notify_ephemeral_message_deleted(LinphoneChatMessage* msg);
void _linphone_chat_message_clear_callbacks (LinphoneChatMessage *msg);

void _linphone_magic_search_notify_search_results_received(LinphoneMagicSearch* magic_search);
void _linphone_magic_search_notify_ldap_have_more_results(LinphoneMagicSearch* magic_search, LinphoneLdap* ldap);


const LinphoneParticipantImdnState *_linphone_participant_imdn_state_from_cpp_obj (const LinphonePrivate::ParticipantImdnState &state);

LinphoneToneDescription * linphone_tone_description_new(LinphoneReason reason, LinphoneToneID id, const char *audiofile);
void linphone_tone_description_destroy(LinphoneToneDescription *obj);

void linphone_task_list_init(LinphoneTaskList *t);
void linphone_task_list_add(LinphoneTaskList *t, LinphoneCoreIterateHook hook, void *hook_data);
void linphone_task_list_remove(LinphoneTaskList *t, LinphoneCoreIterateHook hook, void *hook_data);
void linphone_task_list_run(LinphoneTaskList *t);
void linphone_task_list_free(LinphoneTaskList *t);

LinphoneCoreCbs * _linphone_core_cbs_new(void);
void _linphone_core_cbs_set_v_table(LinphoneCoreCbs *cbs, LinphoneCoreVTable *vtable, bool_t autorelease);

LinphoneLoggingServiceCbs *linphone_logging_service_cbs_new(void);

LinphoneTunnel *linphone_core_tunnel_new(LinphoneCore *lc);
void linphone_tunnel_configure(LinphoneTunnel *tunnel);
void linphone_tunnel_enable_logs_with_handler(LinphoneTunnel *tunnel, bool_t enabled, OrtpLogFunc logHandler);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC int linphone_core_get_calls_nb(const LinphoneCore *lc);

void linphone_core_set_state(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message);
void linphone_call_update_biggest_desc(LinphoneCall *call, LinphonePrivate::SalMediaDescription *md);
void linphone_call_make_local_media_description_with_params(LinphoneCore *lc, LinphoneCall *call, LinphoneCallParams *params);

bool_t linphone_core_is_payload_type_usable_for_bandwidth(const LinphoneCore *lc, const PayloadType *pt, int bandwidth_limit);

#define linphone_core_ready(lc) ((lc)->state==LinphoneGlobalOn || (lc)->state==LinphoneGlobalShutdown)
void _linphone_core_configure_resolver(void);

void linphone_core_initialize_supported_content_types(LinphoneCore *lc);

LinphoneEcCalibratorStatus ec_calibrator_get_status(EcCalibrator *ecc);

void ec_calibrator_destroy(EcCalibrator *ecc);

/*conferencing subsystem*/
void _post_configure_audio_stream(AudioStream *st, LinphoneCore *lc, bool_t muted);
bool_t linphone_core_sound_resources_available(LinphoneCore *lc);
LINPHONE_PUBLIC unsigned int linphone_core_get_audio_features(LinphoneCore *lc);

void _linphone_core_codec_config_write(LinphoneCore *lc);

LINPHONE_PUBLIC bctbx_list_t * linphone_core_read_call_logs_from_config_file(LinphoneCore *lc);
void call_logs_write_to_config_file(LinphoneCore *lc);
void linphone_core_store_call_log(LinphoneCore *lc, LinphoneCallLog *log);
LINPHONE_PUBLIC const MSList *linphone_core_get_call_history(LinphoneCore *lc);
LINPHONE_PUBLIC void linphone_core_delete_call_history(LinphoneCore *lc);
LINPHONE_PUBLIC void linphone_core_delete_call_log(LinphoneCore *lc, LinphoneCallLog *log);
LINPHONE_PUBLIC int linphone_core_get_call_history_size(LinphoneCore *lc);

int linphone_core_get_edge_bw(LinphoneCore *lc);
int linphone_core_get_edge_ptime(LinphoneCore *lc);

LinphoneCore *_linphone_core_new_with_config(LinphoneCoreCbs *cbs, struct _LpConfig *config, void *userdata, void *system_context, bool_t automatically_start);
LinphoneCore *_linphone_core_new_shared_with_config(LinphoneCoreCbs *cbs, struct _LpConfig *config, void *userdata, void *system_context, bool_t automatically_start, const char *app_group_id, bool_t main_core);

int linphone_upnp_init(LinphoneCore *lc);
void linphone_upnp_destroy(LinphoneCore *lc);

#ifdef HAVE_SQLITE
int _linphone_sqlite3_open(const char *db_file, sqlite3 **db);
#endif

LinphoneChatMessageStateChangedCb linphone_chat_message_get_message_state_changed_cb(LinphoneChatMessage* msg);
void linphone_chat_message_set_message_state_changed_cb(LinphoneChatMessage* msg, LinphoneChatMessageStateChangedCb cb);
void linphone_chat_message_set_message_state_changed_cb_user_data(LinphoneChatMessage* msg, void *user_data);
void * linphone_chat_message_get_message_state_changed_cb_user_data(LinphoneChatMessage* msg);

bool_t linphone_core_tone_indications_enabled(LinphoneCore*lc);
const char *linphone_core_create_uuid(LinphoneCore *lc);
void linphone_configure_op(LinphoneCore *lc, LinphonePrivate::SalOp *op, const LinphoneAddress *dest, SalCustomHeader *headers, bool_t with_contact);
void linphone_configure_op_2(LinphoneCore *lc, LinphonePrivate::SalOp *op, const LinphoneAddress *local, const LinphoneAddress *dest, SalCustomHeader *headers, bool_t with_contact);
void linphone_configure_op_with_proxy(LinphoneCore *lc, LinphonePrivate::SalOp *op, const LinphoneAddress *dest, SalCustomHeader *headers, bool_t with_contact, LinphoneProxyConfig *proxy);
void linphone_configure_op_with_account(LinphoneCore *lc, LinphonePrivate::SalOp *op, const LinphoneAddress *dest, SalCustomHeader *headers, bool_t with_contact, LinphoneAccount *account);
LinphoneContent * linphone_content_new(void);
// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC LinphoneContent * linphone_content_copy(const LinphoneContent *ref);
SalBodyHandler *sal_body_handler_from_content(const LinphoneContent *content, bool parseMultipart = true);
SalReason linphone_reason_to_sal(LinphoneReason reason);
LinphoneReason linphone_reason_from_sal(SalReason reason);
void linphone_error_info_to_sal(const LinphoneErrorInfo* ei, SalErrorInfo* sei);

SalStreamType linphone_stream_type_to_sal(LinphoneStreamType type);
LinphoneEventCbs *linphone_event_cbs_new(void);
LinphoneEvent *linphone_event_new(LinphoneCore *lc, LinphoneSubscriptionDir dir, const char *name, int expires);

// FIXME: Remove this declaration, use LINPHONE_PUBLIC as ugly workaround, already defined in tester_utils.h
LINPHONE_PUBLIC LinphoneEvent *linphone_event_new_with_op(LinphoneCore *lc, LinphonePrivate::SalEventOp *op, LinphoneSubscriptionDir dir, const char *name);
LinphoneEvent *_linphone_core_create_publish(LinphoneCore *core, LinphoneAccount *account, const LinphoneAddress *resource, const char *event, int expires);
void linphone_event_unpublish(LinphoneEvent *lev);
void linphone_event_set_current_callbacks(LinphoneEvent *ev, LinphoneEventCbs *cbs);
/**
 * Useful for out of dialog notify
 * */
LinphoneEvent *linphone_event_new_with_out_of_dialog_op(LinphoneCore *lc, LinphonePrivate::SalEventOp *op, LinphoneSubscriptionDir dir, const char *name);
void linphone_event_set_internal(LinphoneEvent *lev, bool_t internal);
bool_t linphone_event_is_internal(LinphoneEvent *lev);
LINPHONE_PUBLIC void linphone_event_set_state(LinphoneEvent *lev, LinphoneSubscriptionState state);
void linphone_event_set_publish_state(LinphoneEvent *lev, LinphonePublishState state);
void _linphone_event_notify_notify_response(LinphoneEvent *lev);
LinphoneSubscriptionState linphone_subscription_state_from_sal(SalSubscribeStatus ss);
LinphoneContent *linphone_content_from_sal_body_handler(const SalBodyHandler *ref, bool parseMultipart = true);
void linphone_core_invalidate_friend_subscriptions(LinphoneCore *lc);
void linphone_core_register_offer_answer_providers(LinphoneCore *lc);

void linphone_core_create_im_notif_policy(LinphoneCore *lc);

LINPHONE_PUBLIC LinphoneFriend * linphone_friend_new_from_config_file(LinphoneCore *lc, int index);
LINPHONE_PUBLIC int linphone_friend_get_rc_index(const LinphoneFriend *lf);

/*****************************************************************************
 * REMOTE PROVISIONING FUNCTIONS                                             *
 ****************************************************************************/

void linphone_configuring_terminated(LinphoneCore *lc, LinphoneConfiguringState state, const char *message);
int linphone_remote_provisioning_download_and_apply(LinphoneCore *lc, const char *remote_provisioning_uri);
LINPHONE_PUBLIC int linphone_remote_provisioning_load_file( LinphoneCore* lc, const char* file_path);

/*****************************************************************************
 * SESSION TIMERS FUNCTIONS                                                  *
 ****************************************************************************/
/**
 * Enable or disable the UPDATE method support
 * @param[in] lc #LinphoneCore object
 * @param[in] value Enable or disable it
 * @ingroup media_parameters
**/
LINPHONE_PUBLIC void linphone_core_set_enable_sip_update(const LinphoneCore *lc, int value);

/*****************************************************************************
 * Player interface                                                          *
 ****************************************************************************/

LinphonePlayerCbs *linphone_player_cbs_new(void);
LinphonePlayer * linphone_player_new(LinphoneCore *core);
void _linphone_player_destroy(LinphonePlayer *player);
void linphone_player_set_current_callbacks(LinphonePlayer *player, LinphonePlayerCbs *cbs);

/*****************************************************************************
 * XML UTILITY FUNCTIONS                                                     *
 ****************************************************************************/

#ifdef HAVE_XML2
xmlparsing_context_t * linphone_xmlparsing_context_new(void);
void linphone_xmlparsing_context_destroy(xmlparsing_context_t *ctx);
void linphone_xmlparsing_genericxml_error(void *ctx, const char *fmt, ...);
int linphone_create_xml_xpath_context(xmlparsing_context_t *xml_ctx);
void linphone_xml_xpath_context_set_node(xmlparsing_context_t *xml_ctx, xmlNodePtr node);
char * linphone_get_xml_text_content(xmlparsing_context_t *xml_ctx, const char *xpath_expression);
char * linphone_get_xml_attribute_text_content(xmlparsing_context_t *xml_ctx, const char *xpath_expression, const char *attribute_name);
void linphone_free_xml_text_content(char *text);
xmlXPathObjectPtr linphone_get_xml_xpath_object_for_node_list(xmlparsing_context_t *xml_ctx, const char *xpath_expression);
void linphone_xml_xpath_context_init_carddav_ns(xmlparsing_context_t *xml_ctx);
#endif
/*****************************************************************************
 * OTHER UTILITY FUNCTIONS                                                     *
 ****************************************************************************/

char * linphone_timestamp_to_rfc3339_string(time_t timestamp);

void linphone_error_info_from_sal_op(LinphoneErrorInfo *ei, const LinphonePrivate::SalOp *op);

void payload_type_set_enable(OrtpPayloadType *pt, bool_t value);
bool_t payload_type_enabled(const OrtpPayloadType *pt);

LinphonePayloadType *linphone_payload_type_new(LinphoneCore *lc, OrtpPayloadType *ortp_pt);
bool_t _linphone_core_check_payload_type_usability(const LinphoneCore *lc, const OrtpPayloadType *pt);
OrtpPayloadType *linphone_payload_type_get_ortp_pt(const LinphonePayloadType *pt);

LINPHONE_PUBLIC void linphone_core_update_push_notification_information(LinphoneCore *core, const char *param, const char *prid);
LINPHONE_PUBLIC LinphonePushNotificationConfig* linphone_core_get_push_notification_config(LinphoneCore *core);

LINPHONE_PUBLIC const MSCryptoSuite * linphone_core_get_srtp_crypto_suites_array(LinphoneCore *lc);
const MSCryptoSuite * linphone_core_get_all_supported_srtp_crypto_suites(LinphoneCore *lc);
MsZrtpCryptoTypesCount linphone_core_get_zrtp_key_agreement_suites(LinphoneCore *lc, MSZrtpKeyAgreement keyAgreements[MS_MAX_ZRTP_CRYPTO_TYPES]);
MsZrtpCryptoTypesCount linphone_core_get_zrtp_cipher_suites(LinphoneCore *lc, MSZrtpCipher ciphers[MS_MAX_ZRTP_CRYPTO_TYPES]);
MsZrtpCryptoTypesCount linphone_core_get_zrtp_hash_suites(LinphoneCore *lc, MSZrtpHash hashes[MS_MAX_ZRTP_CRYPTO_TYPES]);
MsZrtpCryptoTypesCount linphone_core_get_zrtp_auth_suites(LinphoneCore *lc, MSZrtpAuthTag authTags[MS_MAX_ZRTP_CRYPTO_TYPES]);
MsZrtpCryptoTypesCount linphone_core_get_zrtp_sas_suites(LinphoneCore *lc, MSZrtpSasType sasTypes[MS_MAX_ZRTP_CRYPTO_TYPES]);

LinphoneImEncryptionEngineCbs * linphone_im_encryption_engine_cbs_new(void);

LinphoneRange *linphone_range_new(void);

LINPHONE_PUBLIC LinphoneTransports *linphone_transports_new(void);

LINPHONE_PUBLIC LinphoneVideoActivationPolicy *linphone_video_activation_policy_new(void);

void linphone_core_notify_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message);
void linphone_core_notify_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message);
void linphone_core_notify_call_goclear_ack_sent(LinphoneCore *lc, LinphoneCall *call);
void linphone_core_notify_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token);
void linphone_core_notify_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
void linphone_core_notify_account_registration_state_changed(LinphoneCore *core, LinphoneAccount *account, LinphoneRegistrationState state, const char *message);
void linphone_core_notify_new_subscription_requested(LinphoneCore *lc, LinphoneFriend *lf, const char *url);
void linphone_core_notify_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username, const char *domain);
void linphone_core_notify_authentication_requested(LinphoneCore *lc, LinphoneAuthInfo *auth_info, LinphoneAuthMethod method);
void linphone_core_notify_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl);
void linphone_core_notify_call_id_updated(LinphoneCore *lc, const char*previous, const char *current);
void linphone_core_notify_text_message_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message);
void linphone_core_notify_message_received(LinphoneCore *lc, LinphoneChatRoom *room, LinphoneChatMessage *message);
void linphone_core_notify_messages_received(LinphoneCore *lc, LinphoneChatRoom *room, const bctbx_list_t *messages);
void linphone_core_notify_message_sent(LinphoneCore *lc, LinphoneChatRoom *room, LinphoneChatMessage *message);
void linphone_core_notify_message_received_unable_decrypt(LinphoneCore *lc, LinphoneChatRoom *room, LinphoneChatMessage *message);
void linphone_core_notify_chat_room_read(LinphoneCore *lc, LinphoneChatRoom *room);
void linphone_core_notify_file_transfer_recv(LinphoneCore *lc, LinphoneChatMessage *message, LinphoneContent* content, const char* buff, size_t size);
void linphone_core_notify_file_transfer_send(LinphoneCore *lc, LinphoneChatMessage *message, LinphoneContent* content, char* buff, size_t* size);
void linphone_core_notify_file_transfer_progress_indication(LinphoneCore *lc, LinphoneChatMessage *message, LinphoneContent* content, size_t offset, size_t total);
void linphone_core_notify_is_composing_received(LinphoneCore *lc, LinphoneChatRoom *room);
void linphone_core_notify_dtmf_received(LinphoneCore* lc, LinphoneCall *call, int dtmf);
void linphone_core_notify_first_call_started(LinphoneCore *lc);
void linphone_core_notify_last_call_ended(LinphoneCore *lc);
void linphone_core_notify_audio_device_changed(LinphoneCore *lc, LinphoneAudioDevice *audioDevice);
void linphone_core_notify_audio_devices_list_updated(LinphoneCore *lc);
void linphone_core_notify_conference_info_received(LinphoneCore *lc, const LinphoneConferenceInfo *conference_info);
void linphone_core_notify_push_notification_received(LinphoneCore *lc, const char *payload);
/*
 * return true if at least a registered vtable has a cb for dtmf received*/
bool_t linphone_core_dtmf_received_has_listener(const LinphoneCore* lc);
void linphone_core_notify_refer_received(LinphoneCore *lc, const char *refer_to);
void linphone_core_notify_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf);
void linphone_core_notify_transfer_state_changed(LinphoneCore *lc, LinphoneCall *transfered, LinphoneCallState new_call_state);
void linphone_core_notify_call_stats_updated(LinphoneCore *lc, LinphoneCall *call, const LinphoneCallStats *stats);
void linphone_core_notify_info_received(LinphoneCore *lc, LinphoneCall *call, const LinphoneInfoMessage *msg);
void linphone_core_notify_configuring_status(LinphoneCore *lc, LinphoneConfiguringState status, const char *message);
void linphone_core_notify_network_reachable(LinphoneCore *lc, bool_t reachable);

void linphone_core_notify_notify_sent(LinphoneCore *lc, LinphoneEvent *lev, const LinphoneContent *body);
void linphone_core_notify_notify_received(LinphoneCore *lc, LinphoneEvent *lev, const char *notified_event, const LinphoneContent *body);
void linphone_core_notify_subscribe_received(LinphoneCore *lc, LinphoneEvent *lev, const char *subscribe_event, const LinphoneContent *body);
void linphone_core_notify_subscription_state_changed(LinphoneCore *lc, LinphoneEvent *lev, LinphoneSubscriptionState state);
void linphone_core_notify_publish_state_changed(LinphoneCore *lc, LinphoneEvent *lev, LinphonePublishState state);
void linphone_core_notify_log_collection_upload_state_changed(LinphoneCore *lc, LinphoneCoreLogCollectionUploadState state, const char *info);
void linphone_core_notify_log_collection_upload_progress_indication(LinphoneCore *lc, size_t offset, size_t total);
void linphone_core_notify_friend_list_created(LinphoneCore *lc, LinphoneFriendList *list);
void linphone_core_notify_friend_list_removed(LinphoneCore *lc, LinphoneFriendList *list);
void linphone_core_notify_call_created(LinphoneCore *lc, LinphoneCall *call);
void linphone_core_notify_conference_state_changed (LinphoneCore *lc, LinphoneConference *conference, LinphoneConferenceState state);
void linphone_core_notify_version_update_check_result_received(LinphoneCore *lc, LinphoneVersionUpdateCheckResult result, const char *version, const char *url);
void linphone_core_notify_chat_room_state_changed (LinphoneCore *lc, LinphoneChatRoom *cr, LinphoneChatRoomState state);
void linphone_core_notify_chat_room_subject_changed (LinphoneCore *lc, LinphoneChatRoom *cr);
void linphone_core_notify_chat_room_ephemeral_message_deleted (LinphoneCore *lc, LinphoneChatRoom *cr);
void linphone_core_notify_imee_user_registration (LinphoneCore *lc, bool_t status, const char *userId, const char *info);
void linphone_core_notify_qrcode_found(LinphoneCore *lc, const char *result);
void linphone_core_notify_ec_calibration_result(LinphoneCore *lc, LinphoneEcCalibratorStatus status, int delay_ms);
void linphone_core_notify_ec_calibration_audio_init(LinphoneCore *lc);
void linphone_core_notify_ec_calibration_audio_uninit(LinphoneCore *lc);
void linphone_core_notify_chat_room_exhumed(LinphoneCore *lc, LinphoneChatRoom *chat_room);

void set_playback_gain_db(AudioStream *st, float gain);

LinphoneMediaDirection media_direction_from_sal_stream_dir(SalStreamDir dir);
LINPHONE_PUBLIC SalStreamDir sal_dir_from_call_params_dir(LinphoneMediaDirection cpdir);

/*****************************************************************************
 * LINPHONE CONTENT PRIVATE ACCESSORS                                        *
 ****************************************************************************/

/**
 * Get the address of the crypto context associated with a RCS file transfer message if encrypted
 * @param[in] content LinphoneContent object.
 * @return The address of the pointer to the crypto context. Crypto context is managed(alloc/free)
 *         by the encryption/decryption functions, so we give the address to store/retrieve the pointer
 */
void ** linphone_content_get_cryptoContext_address(LinphoneContent *content);

void v_table_reference_destroy(VTableReference *ref);

LINPHONE_PUBLIC void _linphone_core_add_callbacks(LinphoneCore *lc, LinphoneCoreCbs *vtable, bool_t internal);

MSWebCam *get_nowebcam_device(MSFactory *f);

LinphoneLimeState linphone_core_lime_for_file_sharing_enabled(const LinphoneCore *lc);

LINPHONE_PUBLIC void linphone_core_set_default_proxy_index(LinphoneCore *core, int index);
int linphone_core_get_default_proxy_config_index(LinphoneCore *lc);
LINPHONE_PUBLIC void linphone_core_set_default_account_index(LinphoneCore *core, int index);
int linphone_core_get_default_account_index(LinphoneCore *lc);

char *linphone_presence_model_to_xml(LinphonePresenceModel *model) ;

LinphoneVideoDefinition * linphone_video_definition_new(unsigned int width, unsigned int height, const char *name);

LinphoneVideoDefinition * linphone_factory_find_supported_video_definition(const LinphoneFactory *factory, unsigned int width, unsigned int height);
LinphoneVideoDefinition * linphone_factory_find_supported_video_definition_by_name(const LinphoneFactory *factory, const char *name);

const char* _linphone_config_load_from_xml_string(LpConfig *lpc, const char *buffer);
void _linphone_config_apply_factory_config (LpConfig *config);

SalCustomHeader *linphone_info_message_get_headers (const LinphoneInfoMessage *im);
void linphone_info_message_set_headers (LinphoneInfoMessage *im, const SalCustomHeader *headers);

void _linphone_core_set_log_handler(OrtpLogFunc logfunc);

void _linphone_core_set_native_preview_window_id(LinphoneCore *lc, void *id);
void _linphone_core_set_native_video_window_id(LinphoneCore *lc, void *id);
void linphone_core_resize_video_preview(LinphoneCore *lc, int width, int height);

LinphoneAccountCreatorCbs * linphone_account_creator_cbs_new(void);
void linphone_account_creator_set_current_callbacks(LinphoneAccountCreator *creator, LinphoneAccountCreatorCbs *cbs);
LinphoneXmlRpcRequestCbs * linphone_xml_rpc_request_cbs_new(void);
void linphone_xml_rpc_request_set_current_callbacks(LinphoneXmlRpcRequest *request, LinphoneXmlRpcRequestCbs *cbs);

void linphone_core_invalidate_friends_maps(LinphoneCore *lc);
bctbx_list_t * linphone_core_get_supported_media_encryptions_at_compile_time();

// The following methods are private and they allow a conversion from conference state enum to chat room state enum and viceversa.
// This allows to easily go from one type to another one ensuring that they are synchronized and ease debugging in case they are not
// Note that these methods must be updated if either the value of conference state or chat room state change.
// The compiler will throw an error if not doing it:
// <pathToLiblinphone>/src/c-wrapper/api/c-chat-room.cpp: In function ‘LinphoneChatRoomState linphone_conference_state_to_chat_room_state(LinphoneConferenceState)’:
// <pathToLiblinphone>/src/c-wrapper/api/c-chat-room.cpp:676:9: error: enumeration value ‘<name>’ not handled in switch [-Werror=switch]
LinphoneConferenceState linphone_chat_room_state_to_conference_state(LinphoneChatRoomState state);
LinphoneChatRoomState linphone_conference_state_to_chat_room_state(LinphoneConferenceState state);

LINPHONE_PUBLIC unsigned int linphone_conference_info_get_ics_sequence(const LinphoneConferenceInfo *conference_info);
LINPHONE_PUBLIC const char *linphone_conference_info_get_utf8_ics_uid(const LinphoneConferenceInfo *conference_info);
LINPHONE_PUBLIC const char *linphone_conference_info_get_ics_uid(const LinphoneConferenceInfo *conference_info);

LinphoneDigestAuthenticationPolicy *linphone_digest_authentication_policy_new(void);
LinphoneDigestAuthenticationPolicy * linphone_digest_authentication_policy_new_from_config(LinphoneConfig *config);
void linphone_digest_authentication_policy_save(const LinphoneDigestAuthenticationPolicy* policy, LinphoneConfig *config);

typedef struct _ZrtpAlgo ZrtpAlgo;
/**
 * Get the zrtp info
 * @param stats #LinphoneCallStats object @notnil
 * @return The zrtp info
 */
LINPHONE_PUBLIC const ZrtpAlgo *linphone_call_stats_get_zrtp_algo (const LinphoneCallStats *stats);

typedef struct _SrtpInfo SrtpInfo;
/**
 * Get the srtp info
 * @param stats #LinphoneCallStats object @notnil
 * @return The srtp info
 */
LINPHONE_PUBLIC const SrtpInfo *linphone_call_stats_get_srtp_info (const LinphoneCallStats *stats);
/**
 * Create a new #LinphoneNatPolicy by reading the config of a #LinphoneCore according to the passed ref.
 * @param core #LinphoneCore object @notnil
 * @param ref The reference of a NAT policy in the config of the #LinphoneCore @notnil
 * @return A new #LinphoneNatPolicy object. @maybenil
 */
LINPHONE_PUBLIC LinphoneNatPolicy * linphone_core_create_nat_policy_from_ref(LinphoneCore *core, const char *ref);

/**
 * Create a new #LinphoneNatPolicy by reading the config of a #LinphoneCore according to the passed section.
 * @param core #LinphoneCore object @notnil
 * @param section The section name of a NAT policy in the config of the #LinphoneCore @notnil
 * @return A new #LinphoneNatPolicy object. @maybenil
 */
LINPHONE_PUBLIC LinphoneNatPolicy * linphone_core_create_nat_policy_from_config(LinphoneCore *core, const char *section);


#ifdef __cplusplus
}
#endif

#endif /* _PRIVATE_FUNCTIONS_H_ */
