/*
 * server-group-chat-room.cpp
 * Copyright (C) 2010-2017 Belledonne Communications SARL
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <algorithm>

#include "address/address-p.h"
#include "address/address.h"
#include "c-wrapper/c-wrapper.h"
#include "chat/chat-message/chat-message-p.h"
#include "chat/modifier/cpim-chat-message-modifier.h"
#include "conference/local-conference-event-handler.h"
#include "conference/local-conference-p.h"
#include "conference/participant-p.h"
#include "conference/session/call-session-p.h"
#include "content/content-type.h"
#include "logger/logger.h"
#include "sal/refer-op.h"
#include "server-group-chat-room-p.h"
#include "core/core.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

ServerGroupChatRoomPrivate::ServerGroupChatRoomPrivate () {}

// -----------------------------------------------------------------------------

shared_ptr<Participant> ServerGroupChatRoomPrivate::addParticipant (const Address &addr) {
	L_Q_T(LocalConference, qConference);
	shared_ptr<Participant> participant = make_shared<Participant>(addr);
	qConference->getPrivate()->participants.push_back(participant);
	return participant;
}

void ServerGroupChatRoomPrivate::confirmCreation () {
	L_Q();
	L_Q_T(LocalConference, qConference);

	shared_ptr<Participant> me = q->getMe();
	shared_ptr<CallSession> session = me->getPrivate()->getSession();
	session->startIncomingNotification();

	peerAddress = qConference->getPrivate()->conferenceAddress = generateConferenceAddress(me);
	// Let the SIP stack set the domain and the port
	Address addr = me->getContactAddress();
	addr.setParam("isfocus");
	session->redirect(addr);
	setState(ChatRoom::State::Created);
}

void ServerGroupChatRoomPrivate::confirmJoining (SalCallOp *op) {
	L_Q();
	L_Q_T(LocalConference, qConference);
	shared_ptr<Participant> participant;
	if (q->getNbParticipants() == 0) {
		// First participant (creator of the chat room)
		participant = addParticipant(Address(op->get_from()));
		participant->getPrivate()->setContactAddress(Address(op->get_remote_contact()));
		participant->getPrivate()->setAdmin(true);
	} else {
		// INVITE coming from an invited participant
		participant = q->findParticipant(Address(op->get_from()));
		if (!participant) {
			op->decline(SalReasonDeclined, nullptr);
			return;
		}
	}
	shared_ptr<CallSession> session = participant->getPrivate()->getSession();
	if (!session) {
		session = participant->getPrivate()->createSession(*q, nullptr, false, q);
		session->configure(LinphoneCallIncoming, nullptr, op, participant->getAddress(), Address(op->get_to()));
		session->startIncomingNotification();
		Address addr = qConference->getPrivate()->conferenceAddress;
		addr.setParam("isfocus");
		session->getPrivate()->getOp()->set_contact_address(addr.getPrivate()->getInternalAddress());
	}
	session->accept();

	// Changes are only allowed from admin participants
	if (participant->isAdmin())
		update(op);
}

shared_ptr<Participant> ServerGroupChatRoomPrivate::findRemovedParticipant (const shared_ptr<const CallSession> &session) const {
	for (const auto &participant : removedParticipants) {
		if (participant->getPrivate()->getSession() == session)
			return participant;
	}
	return nullptr;
}

Address ServerGroupChatRoomPrivate::generateConferenceAddress (shared_ptr<Participant> me) {
	L_Q();
	char token[11];
	ostringstream os;
	Address conferenceAddress = me->getContactAddress();
	do {
		belle_sip_random_token(token, sizeof(token));
		os.str("");
		os << "chatroom-" << token;
		conferenceAddress.setUsername(os.str());
	} while (static_cast<const CoreAccessor*>(q)->getCore()->findChatRoom(conferenceAddress));
	me->getPrivate()->setAddress(conferenceAddress);
	me->getPrivate()->setContactAddress(conferenceAddress);
	return me->getContactAddress();
}

void ServerGroupChatRoomPrivate::removeParticipant (const shared_ptr<const Participant> &participant) {
	L_Q();
	L_Q_T(LocalConference, qConference);

	// Remove participant before notifying so that the removed participant is not notified of its own removal
	for (const auto &p : qConference->getPrivate()->participants) {
		if (participant->getAddress() == p->getAddress()) {
			// Keep the participant in the removedParticipants list so that to keep the CallSession alive and
			// be able to answer to the BYE request.
			removedParticipants.push_back(p);
			qConference->getPrivate()->participants.remove(p);
			break;
		}
	}

	qConference->getPrivate()->eventHandler->notifyParticipantRemoved(participant->getAddress());
	if (q->getNbParticipants() == 0) {
		Core::deleteChatRoom(q->getSharedFromThis());
	} else if (!isAdminLeft())
		designateAdmin();
}

void ServerGroupChatRoomPrivate::subscribeReceived (LinphoneEvent *event) {
	L_Q_T(LocalConference, qConference);
	qConference->getPrivate()->eventHandler->subscribeReceived(event);
}

void ServerGroupChatRoomPrivate::update (SalCallOp *op) {
	L_Q();
	// Handle subject change
	q->setSubject(L_C_TO_STRING(op->get_subject()));
	// Handle participants addition
	const Content &content = op->get_remote_body();
	if ((content.getContentType() == ContentType::ResourceLists)
		&& (content.getContentDisposition() == "recipient-list")) {
		list<Address> addresses = q->parseResourceLists(content.getBodyAsString());
		q->addParticipants(addresses, nullptr, false);
	}
}

// -----------------------------------------------------------------------------

void ServerGroupChatRoomPrivate::dispatchMessage (const Address &fromAddr, const Content &content) {
	L_Q();
	L_Q_T(LocalConference, qConference);
	for (const auto &p : qConference->getPrivate()->participants) {
		if (!fromAddr.weakEqual(p->getAddress())) {
			shared_ptr<ChatMessage> msg = q->createMessage();
			msg->setInternalContent(content);
			msg->setFromAddress(q->getConferenceAddress());
			msg->setToAddress(p->getContactAddress());
			msg->getPrivate()->setApplyModifiers(false);
			msg->send();
		}
	}
}

void ServerGroupChatRoomPrivate::storeOrUpdateMessage (const std::shared_ptr<ChatMessage> &msg) {
	// Do not store the messages in the server group chat room
}

LinphoneReason ServerGroupChatRoomPrivate::messageReceived (SalOp *op, const SalMessage *salMsg) {
	L_Q();
	// Check that the message is coming from a participant of the chat room
	Address fromAddr(op->get_from());
	if (!q->findParticipant(fromAddr)) {
		return LinphoneReasonNotAcceptable;
	}
	// Check that we received a CPIM message
	ContentType contentType(salMsg->content_type);
	if (contentType != ContentType::Cpim)
		return LinphoneReasonNotAcceptable;
	Content content;
	content.setContentType(salMsg->content_type);
	content.setBody(salMsg->text ? salMsg->text : "");
	dispatchMessage(fromAddr, content);
	return LinphoneReasonNone;
}

// -----------------------------------------------------------------------------

void ServerGroupChatRoomPrivate::designateAdmin () {
	L_Q();
	L_Q_T(LocalConference, qConference);
	q->setParticipantAdminStatus(qConference->getPrivate()->participants.front(), true);
}

bool ServerGroupChatRoomPrivate::isAdminLeft () const {
	L_Q_T(LocalConference, qConference);
	for (const auto &p : qConference->getPrivate()->participants) {
		if (p->isAdmin())
			return true;
	}
	return false;
}

// =============================================================================

ServerGroupChatRoom::ServerGroupChatRoom (const std::shared_ptr<Core> &core, SalCallOp *op)
: ChatRoom(*new ServerGroupChatRoomPrivate(), core, Address(op->get_to())),
LocalConference(core->getCCore(), Address(linphone_core_get_conference_factory_uri(core->getCCore())), nullptr) {
	LocalConference::setSubject(op->get_subject() ? op->get_subject() : "");
	getMe()->getPrivate()->createSession(*this, nullptr, false, this);
	getMe()->getPrivate()->getSession()->configure(LinphoneCallIncoming, nullptr, op, Address(op->get_from()), Address(op->get_to()));
}

int ServerGroupChatRoom::getCapabilities () const {
	return static_cast<int>(Capabilities::Conference);
}

// -----------------------------------------------------------------------------

void ServerGroupChatRoom::addParticipant (const Address &addr, const CallSessionParams *params, bool hasMedia) {
	L_D_T(LocalConference, dConference);
	SalReferOp *referOp = new SalReferOp(CoreAccessor::getCore()->getCCore()->sal);
	LinphoneAddress *lAddr = linphone_address_new(addr.asString().c_str());
	linphone_configure_op(CoreAccessor::getCore()->getCCore(), referOp, lAddr, nullptr, false);
	linphone_address_unref(lAddr);
	Address referToAddr = getConferenceAddress();
	referToAddr.setParam("text");
	referOp->send_refer(referToAddr.getPrivate()->getInternalAddress());
	referOp->unref();
	// TODO: Wait for the response to the REFER to really add the participant
	LocalConference::addParticipant(addr, params, hasMedia);
	dConference->eventHandler->notifyParticipantAdded(addr);
}

void ServerGroupChatRoom::addParticipants (const list<Address> &addresses, const CallSessionParams *params, bool hasMedia) {
	LocalConference::addParticipants(addresses, params, hasMedia);
}

bool ServerGroupChatRoom::canHandleParticipants () const {
	return LocalConference::canHandleParticipants();
}

shared_ptr<Participant> ServerGroupChatRoom::findParticipant (const Address &addr) const {
	return LocalConference::findParticipant(addr);
}

const Address &ServerGroupChatRoom::getConferenceAddress () const {
	return LocalConference::getConferenceAddress();
}

int ServerGroupChatRoom::getNbParticipants () const {
	return LocalConference::getNbParticipants();
}

list<shared_ptr<Participant>> ServerGroupChatRoom::getParticipants () const {
	return LocalConference::getParticipants();
}

const string &ServerGroupChatRoom::getSubject () const {
	return LocalConference::getSubject();
}

void ServerGroupChatRoom::join () {}

void ServerGroupChatRoom::leave () {}

void ServerGroupChatRoom::removeParticipant (const shared_ptr<const Participant> &participant) {
	L_D();
	SalReferOp *referOp = new SalReferOp(CoreAccessor::getCore()->getCCore()->sal);
	LinphoneAddress *lAddr = linphone_address_new(participant->getContactAddress().asString().c_str());
	linphone_configure_op(CoreAccessor::getCore()->getCCore(), referOp, lAddr, nullptr, false);
	linphone_address_unref(lAddr);
	Address referToAddr = getConferenceAddress();
	referToAddr.setParam("text");
	referToAddr.setUriParam("method", "BYE");
	referOp->send_refer(referToAddr.getPrivate()->getInternalAddress());
	referOp->unref();
	// TODO: Wait for the response to the REFER to really remove the participant
	d->removeParticipant(participant);
}

void ServerGroupChatRoom::removeParticipants (const list<shared_ptr<Participant>> &participants) {
	LocalConference::removeParticipants(participants);
}

void ServerGroupChatRoom::setParticipantAdminStatus (shared_ptr<Participant> &participant, bool isAdmin) {
	L_D_T(LocalConference, dConference);
	if (isAdmin != participant->isAdmin()) {
		participant->getPrivate()->setAdmin(isAdmin);
		dConference->eventHandler->notifyParticipantSetAdmin(participant->getAddress(), participant->isAdmin());
	}
}

void ServerGroupChatRoom::setSubject (const std::string &subject) {
	L_D_T(LocalConference, dConference);
	if (subject != getSubject()) {
		LocalConference::setSubject(subject);
		dConference->eventHandler->notifySubjectChanged();
	}
}

// -----------------------------------------------------------------------------

void ServerGroupChatRoom::onChatMessageReceived (const std::shared_ptr<ChatMessage> &msg) {}

void ServerGroupChatRoom::onCallSessionStateChanged (const std::shared_ptr<const CallSession> &session, LinphoneCallState state, const std::string &message) {
	L_D();
	if (state == LinphoneCallEnd) {
		shared_ptr<Participant> participant = LocalConference::findParticipant(session);
		if (participant)
			d->removeParticipant(participant);
		participant = d->findRemovedParticipant(session);
		if (participant)
			d->removedParticipants.remove(participant);
	} else if (state == LinphoneCallUpdatedByRemote) {
		shared_ptr<Participant> participant = LocalConference::findParticipant(session);
		if (participant && participant->isAdmin())
			d->update(session->getPrivate()->getOp());
	}
}

LINPHONE_END_NAMESPACE
