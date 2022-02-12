#include "matrix.h"
#include "unity.h"

/* Data taken from
 * https://github.com/Nheko-Reborn/mtxclient/blob/master/tests/events.cpp */

/* TODO test invalid json. */

void
setUp(void) {
}

void
tearDown(void) {
}

static void
test_message(void) {
}

static void
test_redaction(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
	  "unsigned": {
            "age": 146
	  },
          "content": {
              "reason": "No reason"
          },
          "event_id": "$143273582443PhrSn:localhost",
          "origin_server_ts": 1432735824653,
          "room_id": "!jEsUZKDJdhlrceRyVU:localhost",
          "redacts": "$1521361675759563UDexf:matrix.org",
          "sender": "@example:localhost",
          "type": "m.room.redaction"
        })",
	  0);

	struct matrix_timeline_event event;
	TEST_ASSERT_EQUAL(0, matrix_event_timeline_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_REDACTION);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$143273582443PhrSn:localhost");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@example:localhost");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653L);
	TEST_ASSERT_EQUAL_STRING(
	  event.redaction.redacts, "$1521361675759563UDexf:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.redaction.reason, "No reason");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
	  "unsigned": {
			"age": 146
	  },
		  "content": {
		  },
		  "event_id": "$143273582443PhrSn:localhost",
		  "origin_server_ts": 1432735824653,
		  "room_id": "!jEsUZKDJdhlrceRyVU:localhost",
		  "redacts": "$1521361675759563UDexf:matrix.org",
		  "sender": "@example:localhost",
		  "type": "m.room.redaction"
		})",
	  0);

	struct matrix_sync_event sevent;

	TEST_ASSERT_EQUAL(0, matrix_event_sync_parse(&sevent, jevent));

	TEST_ASSERT_EQUAL(sevent.type, MATRIX_EVENT_TIMELINE);
	TEST_ASSERT_EQUAL(sevent.timeline.type, MATRIX_ROOM_REDACTION);

	matrix_json_delete(jevent);
}

static void
test_attachment(void) {
}

static void
test_member(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1510473132947,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104731322646264oUPqj:matrix.org",
		  "unsigned": {
			"age": 72
		  },
		  "state_key": "@nheko_test:matrix.org",
		  "content": {
			"membership": "join",
			"avatar_url": "mxc://matrix.org/JKiSOBDDxCHxmaLAgoQwSAHa",
			"displayname": "NhekoTest"
		  },
		  "membership": "join",
		  "type": "m.room.member",
		  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
	})",
	  0);

	struct matrix_sync_event sevent;

	TEST_ASSERT_EQUAL(0, matrix_event_sync_parse(&sevent, jevent));

	TEST_ASSERT_EQUAL(sevent.state.type, MATRIX_ROOM_MEMBER);
	TEST_ASSERT_FALSE(sevent.state.is_in_timeline);

	TEST_ASSERT_EQUAL_STRING(
	  sevent.state.base.event_id, "$15104731322646264oUPqj:matrix.org");
	TEST_ASSERT_EQUAL_STRING(
	  sevent.state.base.sender, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL(sevent.state.base.origin_server_ts, 1510473132947L);
	TEST_ASSERT_EQUAL_STRING(
	  sevent.state.base.state_key, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL_STRING(sevent.state.content.member.membership, "join");
	TEST_ASSERT_EQUAL_STRING(
	  sevent.state.content.member.displayname, "NhekoTest");
	TEST_ASSERT_EQUAL_STRING(sevent.state.content.member.avatar_url,
	  "mxc://matrix.org/JKiSOBDDxCHxmaLAgoQwSAHa");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "prev_content": {
			"membership": "join",
			"avatar_url": "mxc://matrix.org/IvqcnGakfvwwKeZxbJWhblFp",
			"displayname": "NhekoTest"
		  },
		  "origin_server_ts": 1509214100884,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15092141005099019aHvYG:matrix.org",
		  "age": 1259000688,
		  "unsigned": {
			"prev_content": {
			  "membership": "join",
			  "avatar_url": "mxc://matrix.org/IvqcnGakfvwwKeZxbJWhblFp",
			  "displayname": "NhekoTest"
			},
			"prev_sender": "@nheko_test:matrix.org",
			"replaces_state": "$15092124385075888YpYOh:matrix.org",
			"age": 1259000688
		  },
		  "state_key": "@nheko_test:matrix.org",
		  "content": {
			"membership": "join",
			"avatar_url": "mxc://matrix.org/JKiSOBDDxCHxmaLAgoQwSAHa",
			"displayname": "NhekoTest"
		  },
		  "membership": "join",
		  "room_id": "!VaMCVKSVcyPtXbcMXh:matrix.org",
		  "user_id": "@nheko_test:matrix.org",
		  "replaces_state": "$15092124385075888YpYOh:matrix.org",
		  "type": "m.room.member"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));
	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_MEMBER);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15092141005099019aHvYG:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1509214100884L);
	TEST_ASSERT_EQUAL_STRING(
	  event.replaces_state, "$15092124385075888YpYOh:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.content.member.membership, "join");
	TEST_ASSERT_EQUAL_STRING(event.content.member.displayname, "NhekoTest");
	TEST_ASSERT_EQUAL_STRING(event.content.member.avatar_url,
	  "mxc://matrix.org/JKiSOBDDxCHxmaLAgoQwSAHa");
	TEST_ASSERT_EQUAL_STRING(
	  event.prev_content.member.displayname, "NhekoTest");
	TEST_ASSERT_EQUAL_STRING(event.prev_content.member.avatar_url,
	  "mxc://matrix.org/IvqcnGakfvwwKeZxbJWhblFp");

	matrix_json_delete(jevent);
}

static void
test_power_levels(void) {
}

static void
test_canonical_alias(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
	"content": {
		"alias": "#somewhere:localhost",
		"alt_aliases": [
			"#somewhere:example.org",
			"#myroom:example.com"
		]
	},
	"event_id": "$143273582443PhrSn:example.org",
	"origin_server_ts": 1432735824653,
	"room_id": "!jEsUZKDJdhlrceRyVU:example.org",
	"sender": "@example:example.org",
	"state_key": "",
	"type": "m.room.canonical_alias",
	"unsigned": {
		"age": 1234,
		"prev_content": {
			"alias": "#old:localhost"
		}
	}
})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_CANONICAL_ALIAS);

	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$143273582443PhrSn:example.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@example:example.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.canonical_alias.alias, "#somewhere:localhost");
	TEST_ASSERT_EQUAL_STRING(
	  event.prev_content.canonical_alias.alias, "#old:localhost");
#ifdef TODO
	TEST_ASSERT_EQUAL(event.content.alt_aliases.size(), 2);
	TEST_ASSERT_EQUAL_STRING(
	  event.content.alt_aliases.at(0), "#somewhere:example.org");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.alt_aliases.at(1), "#myroom:example.com");
#endif

	matrix_json_delete(jevent);
}

static void
test_create(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761923948,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619231414398jhvQC:matrix.org",
		  "unsigned": {
			"age": 3715756343
		  },
		  "state_key": "",
		  "content": {
			"creator": "@mujx:matrix.org"
		  },
		  "type": "m.room.create"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_CREATE);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15067619231414398jhvQC:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1506761923948L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_TRUE(event.content.create.federate);
	TEST_ASSERT_EQUAL_STRING(event.content.create.creator, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.content.create.room_version, "1");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
			"content": {
				"creator": "@example:example.org",
				"m.federate": false,
				"predecessor": {
					"event_id": "$something:example.org",
					"room_id": "!oldroom:example.org"
				},
				"room_version": "5"
			},
			"event_id": "$143273582443PhrSn:example.org",
			"origin_server_ts": 1432735824653,
			"room_id": "!jEsUZKDJdhlrceRyVU:example.org",
			"sender": "@example:example.org",
			"state_key": "",
			"type": "m.room.create",
			"unsigned": {
				"age": 1234
			}
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_CREATE);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$143273582443PhrSn:example.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@example:example.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.create.creator, "@example:example.org");
	TEST_ASSERT_FALSE(event.content.create.federate);
	TEST_ASSERT_EQUAL_STRING(event.content.create.room_version, "5");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.create.predecessor.room_id, "!oldroom:example.org");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.create.predecessor.event_id, "$something:example.org");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761923948,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619231414398jhvQC:matrix.org",
		  "unsigned": {
			"age": 3715756343
		  },
		  "state_key": "",
		  "content": {
		    "m.federate": true,
			"creator": "@mujx:matrix.org",
			"type": "m.space"
		  },
		  "type": "m.room.create"
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_CREATE);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15067619231414398jhvQC:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1506761923948L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(event.content.create.creator, "@mujx:matrix.org");
	TEST_ASSERT_TRUE(event.content.create.federate);
	TEST_ASSERT_EQUAL_STRING(event.content.create.type, "m.space");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "content": {
			"creator": "@deepbluev7:neko.dev",
			"room_version": "6",
			"type": "m.space"
		  },
		  "origin_server_ts": 1623788764437,
		  "sender": "@deepbluev7:neko.dev",
		  "state_key": "",
		  "type": "m.room.create",
		  "unsigned": {
			"age": 2241800
		  },
		  "event_id": "$VXf-Ze1j8D3KQ95b66sB7cNp1LzCqOHOJ8nk2iHtZvE",
		  "room_id": "!KLPDfgYGHhdZWLbUwD:neko.dev"
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_TRUE(event.content.create.federate);
	TEST_ASSERT_EQUAL_STRING(event.content.create.room_version, "6");
	TEST_ASSERT_EQUAL_STRING(event.content.create.type, "m.space");

	matrix_json_delete(jevent);
}

static void
test_join_rules(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761924018,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619241414401ASocy:matrix.org",
		  "unsigned": {
			"age": 3715756273
	  },
		  "state_key": "",
		  "content": {
			"join_rule": "invite"
	  },
		  "type": "m.room.join_rules"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_JOIN_RULES);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15067619241414401ASocy:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1506761924018L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(event.content.join_rules.join_rule, "invite");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761924018,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619241414401ASocy:matrix.org",
		  "unsigned": {
			"age": 3715756273
	  },
		  "state_key": "",
		  "content": {
			"join_rule": "public"
	  },
		  "type": "m.room.join_rules"
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));
	TEST_ASSERT_EQUAL_STRING(event.content.join_rules.join_rule, "public");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761924018,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619241414401ASocy:matrix.org",
		  "unsigned": {
			"age": 3715756273
	  },
		  "state_key": "",
		  "content": {
			"join_rule": "knock"
	  },
		  "type": "m.room.join_rules"
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));
	TEST_ASSERT_EQUAL_STRING(event.content.join_rules.join_rule, "knock");

	matrix_json_delete(jevent);

	jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506761924018,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619241414401ASocy:matrix.org",
		  "unsigned": {
			"age": 3715756273
	  },
		  "state_key": "",
		  "content": {
			"join_rule": "private"
	  },
		  "type": "m.room.join_rules"
		})",
	  0);

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));
	TEST_ASSERT_EQUAL_STRING(event.content.join_rules.join_rule, "private");

	matrix_json_delete(jevent);

#ifdef TODO
	data = R"({
		  "type": "m.room.join_rules",
		  "state_key": "",
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619241414401ASocy:matrix.org",
		  "origin_server_ts": 1506761924018,
		  "content": {
			  "join_rule": "restricted",
			  "allow": [
				  {
					  "type": "m.room_membership",
					  "room_id": "!mods:example.org"
				  },
				  {
					  "type": "m.room_membership",
					  "room_id": "!users:example.org"
				  }
			  ]
		  }
	  })"_json;

	ns::StateEvent<ns::state::JoinRules> event2 = data;
	ASSERT_EQ(event2.content.allow.size(), 2);
	ASSERT_EQ(event2.content.allow[0].type,
	  mtx::events::state::JoinAllowanceType::RoomMembership);
	ASSERT_EQ(event2.content.allow[0].room_id, "!mods:example.org");
	ASSERT_EQ(event2.content.allow[1].type,
	  mtx::events::state::JoinAllowanceType::RoomMembership);
	ASSERT_EQ(event2.content.allow[1].room_id, "!users:example.org");
#endif
}

static void
test_name(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1510473133142,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104731332646270uaKBS:matrix.org",
		  "unsigned": {
			"age": 70
		  },
		  "state_key": "",
		  "content": {
			"name": "Random name"
		  },
		  "type": "m.room.name",
		  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_NAME);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15104731332646270uaKBS:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1510473133142L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(event.content.name.name, "Random name");

	matrix_json_delete(jevent);
}

static void
test_topic(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "unsigned": {
			"age": 37
		  },
		  "state_key": "",
		  "content": {
			"topic": "Test topic"
		  },
		  "type": "m.room.topic",
		  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_TOPIC);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15104760642668662QICBu:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1510476064445);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(event.content.topic.topic, "Test topic");

	matrix_json_delete(jevent);
}

static void
test_avatar(void) {
	matrix_json_t *jevent = matrix_json_parse(R"({
		  "origin_server_ts": 1506762071625,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067620711415511reUFC:matrix.org",
		  "age": 3717700323,
		  "unsigned": {
			"age": 3717700323
		  },
		  "state_key": "",
		  "content": {
			"url": "mxc://matrix.org/EPKcIsAPzREMrvZIjrIuJnEl"
		  },
		  "room_id": "!VaMCVKSVcyPtXbcMXh:matrix.org",
		  "user_id": "@mujx:matrix.org",
		  "type": "m.room.avatar"
		})",
	  0);

	struct matrix_state_event event;

	TEST_ASSERT_EQUAL(0, matrix_event_state_parse(&event, jevent));

	TEST_ASSERT_EQUAL(event.type, MATRIX_ROOM_AVATAR);
	TEST_ASSERT_EQUAL_STRING(
	  event.base.event_id, "$15067620711415511reUFC:matrix.org");
	TEST_ASSERT_EQUAL_STRING(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1506762071625L);
	TEST_ASSERT_EQUAL_STRING(event.base.state_key, "");
	TEST_ASSERT_EQUAL_STRING(
	  event.content.avatar.url, "mxc://matrix.org/EPKcIsAPzREMrvZIjrIuJnEl");

	matrix_json_delete(jevent);
}

static void
test_space_child(void) {
}

static void
test_space_parent(void) {
}

static void
test_unknown(void) {
}

int
main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_message);
	RUN_TEST(test_redaction);
	RUN_TEST(test_attachment);
	RUN_TEST(test_member);
	RUN_TEST(test_power_levels);
	RUN_TEST(test_canonical_alias);
	RUN_TEST(test_create);
	RUN_TEST(test_join_rules);
	RUN_TEST(test_name);
	RUN_TEST(test_topic);
	RUN_TEST(test_avatar);
	RUN_TEST(test_space_child);
	RUN_TEST(test_space_parent);
	RUN_TEST(test_unknown);
	return UNITY_END();
}

#if 0
TEST(StateEvents, GuestAccess)
{
	json data = R"({
		  "origin_server_ts": 1506761923948,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619231414398jhvQC:matrix.org",
		  "unsigned": {
			"age": 3715756343
		  },
		  "state_key": "",
		  "content": {
			"guest_access": "can_join"
		  },
		  "type": "m.room.guest_access"
		})"_json;

	ns::StateEvent<ns::state::GuestAccess> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomGuestAccess);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15067619231414398jhvQC:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@mujx:matrix.org"); TEST_ASSERT_EQUAL(event.unsigned_data.age, 3715756343L);
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1506761923948L);
	TEST_ASSERT_EQUAL(event.base.state_key, "");
	TEST_ASSERT_EQUAL(event.content.guest_access,
mtx::events::state::AccessState::CanJoin);
}

TEST(StateEvents, HistoryVisibility)
{
	json data = R"({
	  "origin_server_ts": 1510473133072,
	  "sender": "@nheko_test:matrix.org",
	  "event_id": "$15104731332646268uOFJp:matrix.org",
	  "unsigned": {
		"age": 140
	  },
	  "state_key": "",
	  "content": {
		"history_visibility": "shared"
	  },
	  "type": "m.room.history_visibility",
	  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
	})"_json;

	ns::StateEvent<ns::state::HistoryVisibility> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomHistoryVisibility);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104731332646268uOFJp:matrix.org"); TEST_ASSERT_EQUAL(event.room_id,
"!lfoDRlNFWlvOnvkBwQ:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510473133072L); TEST_ASSERT_EQUAL(event.unsigned_data.age, 140);
	TEST_ASSERT_EQUAL(event.base.state_key, "");
	TEST_ASSERT_EQUAL(event.content.history_visibility,
ns::state::Visibility::Shared);

	json data2 = R"({
		  "origin_server_ts": 1510476778190,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104767782674661tXoeB:matrix.org",
		  "unsigned": {
			"prev_content": {
			  "history_visibility": "shared"
			},
			"prev_sender": "@nheko_test:matrix.org",
			"replaces_state": "$15104731332646268uOFJp:matrix.org",
			"age": 48
		  },
		  "state_key": "",
		  "content": {
			"history_visibility": "invited"
		  },
		  "type": "m.room.history_visibility",
		  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
		})"_json;

	ns::StateEvent<ns::state::HistoryVisibility> event2 = data2;

	TEST_ASSERT_EQUAL(event2.type, RoomHistoryVisibility);
	TEST_ASSERT_EQUAL(event2.event_id, "$15104767782674661tXoeB:matrix.org");
	TEST_ASSERT_EQUAL(event2.room_id, "!lfoDRlNFWlvOnvkBwQ:matrix.org");
	TEST_ASSERT_EQUAL(event2.sender, "@nheko_test:matrix.org");
	TEST_ASSERT_EQUAL(event2.origin_server_ts, 1510476778190L);
	TEST_ASSERT_EQUAL(event2.unsigned_data.age, 48);
	TEST_ASSERT_EQUAL(event2.unsigned_data.replaces_state,
"$15104731332646268uOFJp:matrix.org"); TEST_ASSERT_EQUAL(event2.state_key, "");
	TEST_ASSERT_EQUAL(event2.content.history_visibility,
ns::state::Visibility::Invited);
}

TEST(StateEvents, PinnedEvents)
{
	json data = R"({
	  "unsigned": {
			"age": 242352
	  },
		  "content": {
			"pinned": [
			  "$one:localhost",
			  "$two:localhost",
			  "$three:localhost"
			]
		  },
		  "event_id": "$WLGTSEFSEF:localhost",
		  "origin_server_ts": 1431961217939,
		  "sender": "@example:localhost",
		  "state_key": "",
		  "type": "m.room.pinned_events"
		})"_json;

	ns::StateEvent<ns::state::PinnedEvents> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomPinnedEvents);
	TEST_ASSERT_EQUAL(event.base.event_id, "$WLGTSEFSEF:localhost");
	TEST_ASSERT_EQUAL(event.base.sender, "@example:localhost");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1431961217939L);
	TEST_ASSERT_EQUAL(event.unsigned_data.age, 242352);
	TEST_ASSERT_EQUAL(event.base.state_key, "");

	TEST_ASSERT_EQUAL(event.content.pinned.size(), 3);
	TEST_ASSERT_EQUAL(event.content.pinned[0], "$one:localhost");
	TEST_ASSERT_EQUAL(event.content.pinned[1], "$two:localhost");
	TEST_ASSERT_EQUAL(event.content.pinned[2], "$three:localhost");
}

TEST(StateEvents, PowerLevels)
{
	json data = R"({
		  "origin_server_ts": 1506761923995,
		  "sender": "@mujx:matrix.org",
		  "event_id": "$15067619231414400iQDgf:matrix.org",
		  "unsigned": {
			"age": 3715756296
	  },
		  "state_key": "",
		  "content": {
			"events_default": 0,
			"invite": 0,
			"state_default": 50,
			"redact": 50,
			"ban": 50,
			"users_default": 0,
			"kick": 50,
			"events": {
			  "m.room.avatar": 29,
			  "m.room.name": 50,
			  "m.room.canonical_alias":	50,
			  "m.room.history_visibility": 120,
			  "m.room.power_levels": 109
		},
			"users": {
			  "@mujx:matrix.org": 30
		}
	  },
		  "type": "m.room.power_levels"
	})"_json;

	ns::StateEvent<ns::state::PowerLevels> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomPowerLevels);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15067619231414400iQDgf:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@mujx:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1506761923995); TEST_ASSERT_EQUAL(event.unsigned_data.age, 3715756296);
	TEST_ASSERT_EQUAL(event.base.state_key, "");

	TEST_ASSERT_EQUAL(event.content.kick, 50);
	TEST_ASSERT_EQUAL(event.content.ban, 50);
	TEST_ASSERT_EQUAL(event.content.invite, 0);
	TEST_ASSERT_EQUAL(event.content.redact, 50);
	TEST_ASSERT_EQUAL(event.content.events_default, 0);
	TEST_ASSERT_EQUAL(event.content.users_default, 0);
	TEST_ASSERT_EQUAL(event.content.state_default, 50);

	TEST_ASSERT_EQUAL(event.content.events.size(), 5);
	TEST_ASSERT_EQUAL(event.content.events["m.room.name"], 50);
	TEST_ASSERT_EQUAL(event.content.events["m.room.avatar"], 29);
	TEST_ASSERT_EQUAL(event.content.events["m.room.canonical_alias"], 50);
	TEST_ASSERT_EQUAL(event.content.events["m.room.history_visibility"], 120);
	TEST_ASSERT_EQUAL(event.content.events["m.room.power_levels"], 109);

	TEST_ASSERT_EQUAL(event.content.users.size(), 1);
	TEST_ASSERT_EQUAL(event.content.users["@mujx:matrix.org"], 30);

	TEST_ASSERT_EQUAL(event.content.event_level("m.room.name"), 50);
	TEST_ASSERT_EQUAL(event.content.event_level("m.room.avatar"), 29);
	TEST_ASSERT_EQUAL(event.content.event_level("m.room.canonical_alias"), 50);
	TEST_ASSERT_EQUAL(event.content.event_level("m.room.history_visibility"),
120); TEST_ASSERT_EQUAL(event.content.event_level("m.room.power_levels"), 109);
	TEST_ASSERT_EQUAL(event.content.event_level("m.custom.event"),
event.content.events_default);

	TEST_ASSERT_EQUAL(event.content.user_level("@mujx:matrix.org"), 30);
	TEST_ASSERT_EQUAL(event.content.user_level("@not:matrix.org"),
event.content.users_default);
}

TEST(StateEvents, Tombstone)
{
	json data = R"({
			"content": {
				"body": "This room has been replaced",
				"replacement_room": "!newroom:example.org"
			},
			"event_id": "$143273582443PhrSn:example.org",
			"origin_server_ts": 1432735824653,
			"room_id": "!jEsUZKDJdhlrceRyVU:example.org",
			"sender": "@example:example.org",
			"state_key": "",
			"type": "m.room.tombstone",
			"unsigned": {
				"age": 1234
			}
		})"_json;

	ns::StateEvent<ns::state::Tombstone> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomTombstone);
	TEST_ASSERT_EQUAL(event.base.event_id, "$143273582443PhrSn:example.org");
	TEST_ASSERT_EQUAL(event.room_id, "!jEsUZKDJdhlrceRyVU:example.org");
	TEST_ASSERT_EQUAL(event.base.sender, "@example:example.org");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653);
	TEST_ASSERT_EQUAL(event.unsigned_data.age, 1234);
	TEST_ASSERT_EQUAL(event.base.state_key, "");
	TEST_ASSERT_EQUAL(event.content.body, "This room has been replaced");
	TEST_ASSERT_EQUAL(event.content.replacement_room, "!newroom:example.org");
}

TEST(StateEvents, SpaceChild)
{
	json data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
	  "type": "m.space.child",
	  "state_key": "!abcd:example.com",
	  "content": {
		  "via": ["example.com", "test.org"]
	  }
}
		)"_json;

	ns::StateEvent<ns::state::space::Child> event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceChild);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!abcd:example.com");
	ASSERT_TRUE(event.content.via.has_value());
	std::vector<std::string> via{"example.com", "test.org"};
	TEST_ASSERT_EQUAL(event.content.via, via);
	EXPECT_FALSE(event.content.order.has_value());

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"via": ["example.com"],
		"order": "abcd"
	}
}
		)"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceChild);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!efgh:example.com");
	ASSERT_TRUE(event.content.via.has_value());
	std::vector<std::string> via2{"example.com"};
	TEST_ASSERT_EQUAL(event.content.via, via2);
	ASSERT_TRUE(event.content.order.has_value());
	ASSERT_EQ(event.content.order, "abcd");

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!jklm:example.com",
		"content": {}
}
		)"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceChild);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!jklm:example.com");
	ASSERT_FALSE(event.content.via.has_value());
	ASSERT_FALSE(event.content.order.has_value());

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"via": ["example.com"],
		"order": "01234567890123456789012345678901234567890123456789_"
	}
}
		)"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceChild);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!efgh:example.com");
	EXPECT_TRUE(event.content.via.has_value());
	ASSERT_FALSE(event.content.order.has_value());

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"via": [],
		"order": "01234567890123456789012345678901234567890123456789_"
	}
}
		)"_json;

	event = data;

	EXPECT_FALSE(event.content.via.has_value());

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"via": 5,
		"order": "01234567890123456789012345678901234567890123456789_"
	}
}
		)"_json;

	event = data;

	EXPECT_FALSE(event.content.via.has_value());
	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"via": null,
		"order": "01234567890123456789012345678901234567890123456789_"
	}
}
		)"_json;

	event = data;

	EXPECT_FALSE(event.content.via.has_value());

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		"type": "m.space.child",
		"state_key": "!efgh:example.com",
		"content": {
		"order": "01234567890123456789012345678901234567890123456789_"
	}
}
		)"_json;

	event = data;

	EXPECT_FALSE(event.content.via.has_value());
}
TEST(StateEvents, SpaceParent)
{
	json data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": ["example.com"],
			"canonical": true
		  }
		})"_json;

	ns::StateEvent<ns::state::space::Parent> event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	ASSERT_TRUE(event.content.via.has_value());
	std::vector<std::string> via{"example.com"};
	TEST_ASSERT_EQUAL(event.content.via, via);
	EXPECT_TRUE(event.content.canonical);

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": ["example.org"]
		  }
		})"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	EXPECT_TRUE(event.content.via.has_value());
	EXPECT_FALSE(event.content.canonical);

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": [],
			"canonical": true
		  }
		})"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	EXPECT_FALSE(event.content.via.has_value());
	EXPECT_TRUE(event.content.canonical);

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": null,
			"canonical": true
		  }
		})"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	EXPECT_FALSE(event.content.via.has_value());
	EXPECT_TRUE(event.content.canonical);

	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": 5,
			"canonical": true
		  }
		})"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	EXPECT_FALSE(event.content.via.has_value());
	EXPECT_TRUE(event.content.canonical);
	data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "type": "m.space.parent",
		  "state_key": "!space:example.com",
		  "content": {
			"via": "adjsa",
			"canonical": true
		  }
		})"_json;

	event = data;

	TEST_ASSERT_EQUAL(event.type, SpaceParent);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.base.state_key, "!space:example.com");
	EXPECT_FALSE(event.content.via.has_value());
	EXPECT_TRUE(event.content.canonical);
}

TEST(StateEvents, ImagePack)
{
	json data = R"({
		  "origin_server_ts": 1510476064445,
		  "sender": "@nheko_test:matrix.org",
		  "event_id": "$15104760642668662QICBu:matrix.org",
		  "unsigned": {
			"age": 37
		  },
		  "state_key": "my-pack",
		  "content": {
  "images": {
	"emote": {
	  "url": "mxc://example.org/blah"
	},
	"sticker": {
	  "url": "mxc://example.org/sticker",
	  "body": "stcikerly",
	  "usage": ["sticker"]
	}
  },
  "pack": {
	"display_name": "Awesome Pack",
	"avatar_url": "mxc://example.org/asdjfasd",
	"usage": ["emoticon"],
	"attribution": "huh"
  }
},
		  "type": "im.ponies.room_emotes",
		  "room_id": "!lfoDRlNFWlvOnvkBwQ:matrix.org"
		})"_json;

	ns::StateEvent<ns::msc2545::ImagePack> event = data;

	TEST_ASSERT_EQUAL(event.type, ImagePackInRoom);
	TEST_ASSERT_EQUAL(event.base.event_id,
"$15104760642668662QICBu:matrix.org"); TEST_ASSERT_EQUAL(event.room_id,
"!lfoDRlNFWlvOnvkBwQ:matrix.org"); TEST_ASSERT_EQUAL(event.base.sender,
"@nheko_test:matrix.org"); TEST_ASSERT_EQUAL(event.base.origin_server_ts,
1510476064445); TEST_ASSERT_EQUAL(event.unsigned_data.age, 37);
	TEST_ASSERT_EQUAL(event.base.state_key, "my-pack");
	TEST_ASSERT_EQUAL(event.content.pack.has_value(), true);
	TEST_ASSERT_EQUAL(event.content.pack->display_name, "Awesome Pack");
	TEST_ASSERT_EQUAL(event.content.pack->attribution, "huh");
	TEST_ASSERT_EQUAL(event.content.pack->avatar_url,
"mxc://example.org/asdjfasd"); TEST_ASSERT_EQUAL(event.content.pack->is_emoji(),
true); TEST_ASSERT_EQUAL(event.content.pack->is_sticker(), false);
	ASSERT_EQ(event.content.images.size(), 2);
	TEST_ASSERT_EQUAL(event.content.images["emote"].url,
"mxc://example.org/blah");
	TEST_ASSERT_EQUAL(event.content.images["emote"].overrides_usage(), false);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].url,
"mxc://example.org/sticker");
	TEST_ASSERT_EQUAL(event.content.images["sticker"].body, "stcikerly");
	TEST_ASSERT_EQUAL(event.content.images["sticker"].is_sticker(), true);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].is_emoji(), false);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].overrides_usage(), true);
	TEST_ASSERT_EQUAL(json(event)["content"]["images"].size(), 2);
}

TEST(RoomEvents, OlmEncrypted)
{
	json data = R"({
		  "content": {
			"algorithm": "m.olm.v1.curve25519-aes-sha2",
			"ciphertext": {
			  "1OaiUJ7OfIEGAtnMQyTPFi9Ou6LD5UjSZ4eMk6WzI3E": {
				"body":
"AwogkcAq9+r4YNvCwvBXmipeM30ZVhVDYBWPZ.......69/rEhCK38SIILvCA5NvEH", "type": 0
			  }
			},
			"sender_key": "IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA"
		  },
		  "event_id": "$143273582443PhrSn:localhost",
		  "origin_server_ts": 1432735824653,
		  "room_id": "!jEsUZKDJdhlrceRyVU:localhost",
		  "sender": "@example:localhost",
		  "type": "m.room.encrypted",
		  "unsigned": {
			"age": 146,
			"transaction_id": "m1476648745605.19"
		  }
		})"_json;

	ns::EncryptedEvent<ns::msg::OlmEncrypted> event = data;
	const auto key                                  =
event.content.ciphertext.begin()->first;

	TEST_ASSERT_EQUAL(event.type, RoomEncrypted);
	TEST_ASSERT_EQUAL(event.base.event_id, "$143273582443PhrSn:localhost");
	TEST_ASSERT_EQUAL(event.room_id, "!jEsUZKDJdhlrceRyVU:localhost");
	TEST_ASSERT_EQUAL(event.base.sender, "@example:localhost");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653L);
	TEST_ASSERT_EQUAL(event.unsigned_data.age, 146);
	TEST_ASSERT_EQUAL(event.content.algorithm, "m.olm.v1.curve25519-aes-sha2");
	TEST_ASSERT_EQUAL(event.content.ciphertext.at(key).type, 0);
	TEST_ASSERT_EQUAL(event.content.ciphertext.at(key).body,
			  "AwogkcAq9+r4YNvCwvBXmipeM30ZVhVDYBWPZ.......69/rEhCK38SIILvCA5NvEH");
	TEST_ASSERT_EQUAL(event.content.sender_key,
"IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA");

	ns::msg::OlmEncrypted e1;
	e1.algorithm  = "m.olm.v1.curve25519-aes-sha2";
	e1.ciphertext = {{"1OaiUJ7OfIEGAtnMQyTPFi9Ou6LD5UjSZ4eMk6WzI3E",
					  {"AwogkcAq9+r4YNvCwvBXmipeM30ZVhVDYBWPZ.......69/rEhCK38SIILvCA5NvEH",
0}}}; e1.sender_key = "IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA";

	json j = e1;
	ASSERT_EQ(
	  j.dump(),
	  "{\"algorithm\":\"m.olm.v1.curve25519-aes-sha2\","
	  "\"ciphertext\":{\"1OaiUJ7OfIEGAtnMQyTPFi9Ou6LD5UjSZ4eMk6WzI3E\":{\"body\":\"AwogkcAq9+"
	  "r4YNvCwvBXmipeM30ZVhVDYBWPZ.......69/rEhCK38SIILvCA5NvEH\",\"type\":0}},"
	  "\"sender_key\":\"IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA\"}");
}

TEST(RoomEvents, Encrypted)
{
	json data = R"({
		  "content": {
			"algorithm": "m.megolm.v1.aes-sha2",
			"ciphertext":
"AwgAEnACgAkLmt6qF84IK++J7UDH2Za1YVchHyprqTqsg2yyOwAtHaZTwyNg37afzg8f3r9IsN9r4RNFg7MaZencUJe4qvELiDiopUjy5wYVDAtqdBzer5bWRD9ldxp1FLgbQvBcjkkywYjCsmsq6+hArLd9oAQZnGKn/qLsK+5uNX3PaWzDRC9wZPQvWYYPCTov3jCwXKTPsLKIiTrcCXDqMvnn8m+T3zF/I2zqxg158tnUwWWIw51UO",
			"device_id": "RJYKSTBOIE",
			"sender_key": "IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA",
			"session_id": "X3lUlvLELLYxeTx4yOVu6UDpasGEVO0Jbu+QFnm0cKQ"
		  },
		  "event_id": "$143273582443PhrSn:localhost",
		  "origin_server_ts": 1432735824653,
		  "room_id": "!jEsUZKDJdhlrceRyVU:localhost",
		  "sender": "@example:localhost",
		  "type": "m.room.encrypted",
		  "unsigned": {
			"age": 146,
			"transaction_id": "m1476648745605.19"
		  }
		})"_json;

	ns::EncryptedEvent<ns::msg::Encrypted> event = data;

	TEST_ASSERT_EQUAL(event.type, RoomEncrypted);
	TEST_ASSERT_EQUAL(event.base.event_id, "$143273582443PhrSn:localhost");
	TEST_ASSERT_EQUAL(event.room_id, "!jEsUZKDJdhlrceRyVU:localhost");
	TEST_ASSERT_EQUAL(event.base.sender, "@example:localhost");
	TEST_ASSERT_EQUAL(event.base.origin_server_ts, 1432735824653L);
	TEST_ASSERT_EQUAL(event.unsigned_data.age, 146);
	TEST_ASSERT_EQUAL(event.content.algorithm, "m.megolm.v1.aes-sha2");
	TEST_ASSERT_EQUAL(
	  event.content.ciphertext,
	  "AwgAEnACgAkLmt6qF84IK++"
	  "J7UDH2Za1YVchHyprqTqsg2yyOwAtHaZTwyNg37afzg8f3r9IsN9r4RNFg7MaZencUJe4qvELiDiopUjy5wYVDAt"
	  "qdBzer5bWRD9ldxp1FLgbQvBcjkkywYjCsmsq6+hArLd9oAQZnGKn/"
	  "qLsK+5uNX3PaWzDRC9wZPQvWYYPCTov3jCwXKTPsLKIiTrcCXDqMvnn8m+T3zF/I2zqxg158tnUwWWIw51UO");
	TEST_ASSERT_EQUAL(event.content.device_id, "RJYKSTBOIE");
	TEST_ASSERT_EQUAL(event.content.sender_key,
"IlRMeOPX2e0MurIyfWEucYBRVOEEUMrOHqn/8mLqMjA");
	TEST_ASSERT_EQUAL(event.content.session_id,
"X3lUlvLELLYxeTx4yOVu6UDpasGEVO0Jbu+QFnm0cKQ");

	ns::msg::Encrypted e1;
	e1.algorithm  = "m.megolm.v1.aes-sha2";
	e1.ciphertext =
"AwgAEoABgw1DG6mgKwvrAJU+V7jPu3poEaujNWPnMtIO6+1kFHzEcK6vbYpbg/WlPq/"
					"B23wqKWJ3DIaBsV305VdpisGK7dMN5WgnnTp9JhtztxpCuXnX92rWFBUFM9+"
					"PC5xVJExVBm1qwv8xgWjD5NFqfcVsZ3jLGbGiftPHairq8bxPxTsjrblMHLpXyXLhK6A7YGTey"
					"okcrdXS+IQ4Apq1RLP+kw5RF6M8a/aK3UhUlSAf7OLjaj/03qEwE3TGNaBbLBdOxzoGpxNfQ8";
	e1.device_id  = "YEGDJGLQTZ";
	e1.sender_key = "FyYq6RrnjvsIw0XLGF1jHYlorPgDmJQd15lMJw3D7QI";
	e1.session_id = "/bHcdWPHsJLFd8dkyvG0n7q/RTDmfBIc+gC4laHJCQQ";

	json j = e1;
	ASSERT_EQ(j.dump(),
			  "{\"algorithm\":\"m.megolm.v1.aes-sha2\","
			  "\"ciphertext\":\"AwgAEoABgw1DG6mgKwvrAJU+V7jPu3poEaujNWPnMtIO6+1kFHzEcK6vbYpbg/"
			  "WlPq/B23wqKWJ3DIaBsV305VdpisGK7dMN5WgnnTp9JhtztxpCuXnX92rWFBUFM9"
			  "+PC5xVJExVBm1qwv8xgWjD5NFqfcVsZ3jLGbGiftPHairq8bxPxTsjrblMHLpXyXLhK6A7YGTeyokcrdXS"
			  "+IQ4Apq1RLP+kw5RF6M8a/aK3UhUlSAf7OLjaj/03qEwE3TGNaBbLBdOxzoGpxNfQ8\","
			  "\"device_id\":\"YEGDJGLQTZ\","
			  "\"sender_key\":\"FyYq6RrnjvsIw0XLGF1jHYlorPgDmJQd15lMJw3D7QI\","
			  "\"session_id\":\"/bHcdWPHsJLFd8dkyvG0n7q/RTDmfBIc+gC4laHJCQQ\"}");
}

TEST(Ephemeral, Typing)
{
	json j = R"( {
	"content": {
		"user_ids": [
			"@alice:matrix.org",
			"@bob:example.com"
		]
	},
	"room_id": "!jEsUZKDJdhlrceRyVU:example.org",
	"type": "m.typing"
})"_json;

	ns::EphemeralEvent<ns::ephemeral::Typing> event = j;

	TEST_ASSERT_EQUAL(event.room_id, "!jEsUZKDJdhlrceRyVU:example.org");
	TEST_ASSERT_EQUAL(event.type, Typing);
	TEST_ASSERT_EQUAL(event.content.user_ids.at(0), "@alice:matrix.org");
	TEST_ASSERT_EQUAL(event.content.user_ids.at(1), "@bob:example.com");
	TEST_ASSERT_EQUAL(j.dump(), json(event).dump());
}

TEST(Ephemeral, Receipt)
{
	json j = R"({
	"content": {
		"$1435641916114394fHBLK:matrix.org": {
			"m.read": {
				"@rikj:jki.re": {
					"ts": 1436451550453
				}
			}
		}
	},
	"room_id": "!jEsUZKDJdhlrceRyVU:example.org",
	"type": "m.receipt"
})"_json;

	ns::EphemeralEvent<ns::ephemeral::Receipt> event = j;

	TEST_ASSERT_EQUAL(event.room_id, "!jEsUZKDJdhlrceRyVU:example.org");
	TEST_ASSERT_EQUAL(event.type, Receipt);
	TEST_ASSERT_EQUAL(
	  event.content.receipts.at("$1435641916114394fHBLK:matrix.org").users.at("@rikj:jki.re").ts,
	  1436451550453);
	TEST_ASSERT_EQUAL(j.dump(), json(event).dump());
}

TEST(AccountData, Direct)
{
	json j = R"({
	  "content": {
		"@bob:example.com": [
		   "!abcdefgh:example.com",
		   "!hgfedcba:example.com"
		 ]
	   },
	   "type": "m.direct"
	 })"_json;

	ns::AccountDataEvent<ns::account_data::Direct> event = j;

	ASSERT_EQ(event.content.user_to_rooms.size(), 1);
	ASSERT_EQ(event.content.user_to_rooms.count("@bob:example.com"), 1);
	ASSERT_EQ(event.content.user_to_rooms["@bob:example.com"].size(), 2);
	ASSERT_EQ(event.content.user_to_rooms["@bob:example.com"][0],
"!abcdefgh:example.com");
	ASSERT_EQ(event.content.user_to_rooms["@bob:example.com"][1],
"!hgfedcba:example.com"); TEST_ASSERT_EQUAL(j.dump(), json(event).dump());
}

TEST(AccountData, FullyRead)
{
	json j = R"({
			"content": {
			"event_id": "$someplace:example.org"
		  },
		  "room_id": "!somewhere:example.org",
		  "type": "m.fully_read"
		})"_json;

	ns::AccountDataEvent<ns::account_data::FullyRead> event = j;

	TEST_ASSERT_EQUAL(event.room_id, "!somewhere:example.org");
	TEST_ASSERT_EQUAL(event.type, FullyRead);
	TEST_ASSERT_EQUAL(event.content.event_id, "$someplace:example.org");
	TEST_ASSERT_EQUAL(j.dump(), json(event).dump());
}

TEST(ToDevice, KeyVerificationRequest)
{
	json request_data = R"({
	"content": {
		"from_device": "AliceDevice2",
		"methods": [
			"m.sas.v1"
		],
		"timestamp": 1559598944869,
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.request"
})"_json;

	ns::Event<ns::msg::KeyVerificationRequest> event = request_data;
	auto keyEvent                                    = event.content;
	TEST_ASSERT_EQUAL(keyEvent.from_device, "AliceDevice2");
	TEST_ASSERT_EQUAL(event.type,
mtx::events::EventType::KeyVerificationRequest);
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
	TEST_ASSERT_EQUAL(keyEvent.methods[0], ns::msg::VerificationMethods::SASv1);
	TEST_ASSERT_EQUAL(keyEvent.timestamp, 1559598944869);
	TEST_ASSERT_EQUAL(request_data.dump(), json(event).dump());
}

TEST(ToDevice, KeyVerificationStart)
{
	json request_data = R"({
	"content": {
		"from_device": "BobDevice1",
		"hashes": [
			"sha256"
		],
		"key_agreement_protocols": [
			"curve25519"
		],
		"message_authentication_codes": [
			"hkdf-hmac-sha256"
		],
		"method": "m.sas.v1",
		"short_authentication_string": [
			"decimal",
			"emoji",
			"some-random-invalid-method"
		],
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.start"
})"_json;

	ns::Event<ns::msg::KeyVerificationStart> event = request_data;
	auto keyEvent                                  = event.content;
	TEST_ASSERT_EQUAL(keyEvent.from_device, "BobDevice1");
	TEST_ASSERT_EQUAL(keyEvent.hashes[0], "sha256");
	TEST_ASSERT_EQUAL(keyEvent.key_agreement_protocols[0], "curve25519");
	TEST_ASSERT_EQUAL(keyEvent.message_authentication_codes[0],
"hkdf-hmac-sha256"); TEST_ASSERT_EQUAL(keyEvent.short_authentication_string[0],
ns::msg::SASMethods::Decimal);
	TEST_ASSERT_EQUAL(keyEvent.short_authentication_string[1],
ns::msg::SASMethods::Emoji);
	TEST_ASSERT_EQUAL(keyEvent.short_authentication_string[2],
ns::msg::SASMethods::Unsupported); TEST_ASSERT_EQUAL(event.type,
mtx::events::EventType::KeyVerificationStart);
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
	TEST_ASSERT_EQUAL(keyEvent.method, ns::msg::VerificationMethods::SASv1);
	// The incoming and outgoing JSON will not match due to the Unsupported
SASMethod in the
	// request_data, so no point in comparing the dump of both for equality.
}

TEST(ToDevice, KeyVerificationAccept)
{
	json request_data = R"({
	"content": {
		"commitment":
"fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg",
		"hash": "sha256",
		"key_agreement_protocol": "curve25519",
		"message_authentication_code": "hkdf-hmac-sha256",
		"method": "m.sas.v1",
		"short_authentication_string": [
			"decimal",
			"emoji"
		],
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.accept"
})"_json;

	ns::Event<ns::msg::KeyVerificationAccept> event = request_data;
	auto keyEvent                                   = event.content;
	TEST_ASSERT_EQUAL(event.type,
mtx::events::EventType::KeyVerificationAccept); TEST_ASSERT_EQUAL(
	  keyEvent.commitment,
	  "fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg");
	TEST_ASSERT_EQUAL(keyEvent.hash, "sha256");
	TEST_ASSERT_EQUAL(keyEvent.key_agreement_protocol, "curve25519");
	TEST_ASSERT_EQUAL(keyEvent.message_authentication_code, "hkdf-hmac-sha256");
	TEST_ASSERT_EQUAL(keyEvent.short_authentication_string[0],
ns::msg::SASMethods::Decimal);
	TEST_ASSERT_EQUAL(keyEvent.short_authentication_string[1],
ns::msg::SASMethods::Emoji); TEST_ASSERT_EQUAL(keyEvent.transaction_id,
"S0meUniqueAndOpaqueString"); TEST_ASSERT_EQUAL(request_data.dump(),
json(event).dump());
}

TEST(ToDevice, KeyVerificationReady)
{
	json request_data = R"({
	"content": {
	  "from_device":"@alice:localhost",
	  "methods":["m.sas.v1"],
	  "transaction_id":"S0meUniqueAndOpaqueString"
	},
	"sender": "test_user",
	"type": "m.key.verification.ready"
})"_json;

	ns::Event<ns::msg::KeyVerificationReady> event = request_data;
	auto keyEvent                                  = event.content;
	TEST_ASSERT_EQUAL(event.base.sender, "test_user");
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::KeyVerificationReady);
	TEST_ASSERT_EQUAL(keyEvent.from_device, "@alice:localhost");
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
	TEST_ASSERT_EQUAL(keyEvent.methods[0], ns::msg::VerificationMethods::SASv1);
}

TEST(ToDevice, KeyVerificationDone)
{
	json request_data = R"({
	"content": {
	  "transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "test_user",
	"type": "m.key.verification.done"
})"_json;

	ns::Event<ns::msg::KeyVerificationDone> event = request_data;
	auto keyEvent                                 = event.content;
	TEST_ASSERT_EQUAL(event.base.sender, "test_user");
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::KeyVerificationDone);
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
}

TEST(ToDevice, KeyVerificationCancel)
{
	json request_data = R"({
	"content": {
		"code": "m.user",
		"reason": "User rejected the key verification request",
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.cancel"
})"_json;

	ns::Event<ns::msg::KeyVerificationCancel> event = request_data;
	auto keyEvent                                   = event.content;
	TEST_ASSERT_EQUAL(event.type,
mtx::events::EventType::KeyVerificationCancel); TEST_ASSERT_EQUAL(keyEvent.code,
"m.user"); TEST_ASSERT_EQUAL(keyEvent.reason, "User rejected the key
verification request"); TEST_ASSERT_EQUAL(keyEvent.transaction_id,
"S0meUniqueAndOpaqueString"); TEST_ASSERT_EQUAL(request_data.dump(),
json(event).dump());
}

TEST(ToDevice, KeyVerificationKey)
{
	json request_data = R"({
	"content": {
		"key":
"fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg",
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.key"
})"_json;

	ns::Event<ns::msg::KeyVerificationKey> event = request_data;
	auto keyEvent                                = event.content;
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::KeyVerificationKey);
	TEST_ASSERT_EQUAL(
	  keyEvent.key,
	  "fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg");
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
	TEST_ASSERT_EQUAL(request_data.dump(), json(event).dump());
}

TEST(ToDevice, KeyVerificationMac)
{
	json request_data = R"({
	"content": {
		"keys":
"2Wptgo4CwmLo/Y8B8qinxApKaCkBG2fjTWB7AbP5Uy+aIbygsSdLOFzvdDjww8zUVKCmI02eP9xtyJxc/cLiBA",
		"mac": {
			"ed25519:ABCDEF":
"fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg"
		},
		"transaction_id": "S0meUniqueAndOpaqueString"
	},
	"sender": "",
	"type": "m.key.verification.mac"
})"_json;

	ns::Event<ns::msg::KeyVerificationMac> event = request_data;
	auto keyEvent                                = event.content;
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::KeyVerificationMac);
	TEST_ASSERT_EQUAL(
	  keyEvent.keys,
	  "2Wptgo4CwmLo/Y8B8qinxApKaCkBG2fjTWB7AbP5Uy+aIbygsSdLOFzvdDjww8zUVKCmI02eP9xtyJxc/cLiBA");
	TEST_ASSERT_EQUAL(keyEvent.mac.count("ed25519:ABCDEF"), 1);
	TEST_ASSERT_EQUAL(
	  keyEvent.mac.at("ed25519:ABCDEF"),
	  "fQpGIW1Snz+pwLZu6sTy2aHy/DYWWTspTJRPyNp0PKkymfIsNffysMl6ObMMFdIJhk6g6pwlIqZ54rxo8SLmAg");
	TEST_ASSERT_EQUAL(keyEvent.transaction_id, "S0meUniqueAndOpaqueString");
	TEST_ASSERT_EQUAL(request_data.dump(), json(event).dump());
}

TEST(ToDevice, KeyRequest)
{
	json request_data = R"({
	"content": {
	  "action": "request",
	  "body": {
		"algorithm": "m.megolm.v1.aes-sha2",
		"room_id": "!iapLxlpZgOzqGnWkXR:matrix.org",
		"sender_key": "9im1n0bSYQpnF700sXJqAAYiqGgkyRqMZRdobj0kymY",
		"session_id": "oGj6sEDraRDf+NdmvZTI7urDJk/Z+i7TX2KFLbfMGlE"
	  },
	  "request_id": "m1529936829480.0",
	  "requesting_device_id": "GGUBYESVPI"
	},
		"sender": "@mujx:matrix.org",
		"type": "m.room_key_request"
	})"_json;
	mtx::events::DeviceEvent<ns::msg::KeyRequest> event(request_data);
	TEST_ASSERT_EQUAL(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::RoomKeyRequest);
	TEST_ASSERT_EQUAL(event.content.action, ns::msg::RequestAction::Request);
	TEST_ASSERT_EQUAL(event.content.algorithm, "m.megolm.v1.aes-sha2");
	TEST_ASSERT_EQUAL(event.content.room_id, "!iapLxlpZgOzqGnWkXR:matrix.org");
	TEST_ASSERT_EQUAL(event.content.sender_key,
"9im1n0bSYQpnF700sXJqAAYiqGgkyRqMZRdobj0kymY");
	TEST_ASSERT_EQUAL(event.content.session_id,
"oGj6sEDraRDf+NdmvZTI7urDJk/Z+i7TX2KFLbfMGlE");
	TEST_ASSERT_EQUAL(event.content.request_id, "m1529936829480.0");
	TEST_ASSERT_EQUAL(event.content.requesting_device_id, "GGUBYESVPI");
	TEST_ASSERT_EQUAL(request_data.dump(), json(event).dump());
}

TEST(ToDevice, KeyCancellation)
{
	json cancellation_data = R"({
	  "content": {
			"action": "request_cancellation",
			"request_id": "m1529936829480.0",
			"requesting_device_id": "GGUBYESVPI"
		  },
		  "sender": "@mujx:matrix.org",
		  "type": "m.room_key_request"
	})"_json;

	mtx::events::DeviceEvent<ns::msg::KeyRequest> event(cancellation_data);
	TEST_ASSERT_EQUAL(event.base.sender, "@mujx:matrix.org");
	TEST_ASSERT_EQUAL(event.type, mtx::events::EventType::RoomKeyRequest);
	TEST_ASSERT_EQUAL(event.content.action,
ns::msg::RequestAction::Cancellation);
	TEST_ASSERT_EQUAL(event.content.request_id, "m1529936829480.0");
	TEST_ASSERT_EQUAL(event.content.requesting_device_id, "GGUBYESVPI");

	TEST_ASSERT_EQUAL(cancellation_data.dump(), json(event).dump());
}

TEST(Collection, Events)
{
	json data = R"({
	  "unsigned": {
		"age": 242352,
		"transaction_id": "txnid"
	  },
	  "content": {
		"aliases": [
		  "#somewhere:localhost",
		  "#another:localhost"
		]
	  },
	  "event_id": "$WLGTSEFSEF:localhost",
	  "origin_server_ts": 1431961217939,
		  "room_id": "!Cuyf34gef24t:localhost",
	  "sender": "@example:localhost",
	  "state_key": "localhost",
	  "type": "m.room.aliases"
	})"_json;

	mtx::events::collections::TimelineEvent event = data;

	ASSERT_TRUE(std::get_if<ns::StateEvent<ns::state::Aliases>>(&event.data) !=
nullptr);
}

TEST(RoomAccountData, Tags)
{
	json data = R"({
		  "content": {
			  "tags": {
				"m.favourite": {
				  "order": 1
				},
				"u.Project1": {
				  "order": 0
				},
				"com.example.nheko.text": {
				  "associated_data": ["some", "json", "list"]
				}
			  }
		  },
		  "type": "m.tag"
		})"_json;

	ns::AccountDataEvent<ns::account_data::Tags> event = data;

	TEST_ASSERT_EQUAL(event.type, Tag);
	TEST_ASSERT_EQUAL(event.content.tags.size(), 3);
	TEST_ASSERT_EQUAL(event.content.tags.at("m.favourite").order, 1);
	TEST_ASSERT_EQUAL(event.content.tags.at("u.Project1").order, 0);
	// NOTE(Nico): We are currently not parsing arbitrary attached json data
(anymore).
	TEST_ASSERT_EQUAL(event.content.tags.at("com.example.nheko.text").order,
std::nullopt);
}

TEST(RoomAccountData, NhekoHiddenEvents)
{
	json data = R"({
		  "content": {
			  "hidden_event_types": [
			  "m.reaction",
		  "m.room.member"
		  ]
		  },
		  "type": "im.nheko.hidden_events"
		})"_json;

	ns::AccountDataEvent<ns::account_data::nheko_extensions::HiddenEvents> event
= data;

	TEST_ASSERT_EQUAL(event.type, NhekoHiddenEvents);
	TEST_ASSERT_EQUAL(event.content.hidden_event_types.size(), 2);
	TEST_ASSERT_EQUAL(event.content.hidden_event_types[0], Reaction);
	TEST_ASSERT_EQUAL(event.content.hidden_event_types[1], RoomMember);
}

TEST(RoomAccountData, ImagePack)
{
	json data = R"({
		  "content": {
  "images": {
	"emote": {
	  "url": "mxc://example.org/blah"
	},
	"sticker": {
	  "url": "mxc://example.org/sticker",
	  "body": "stcikerly",
	  "usage": ["sticker"]
	}
  },
  "pack": {
	"display_name": "Awesome Pack",
	"avatar_url": "mxc://example.org/asdjfasd",
	"usage": ["emoticon"],
	"attribution": "huh"
  }
},
		  "type": "im.ponies.user_emotes"
		})"_json;

	ns::AccountDataEvent<ns::msc2545::ImagePack> event = data;

	TEST_ASSERT_EQUAL(event.type, ImagePackInAccountData);
	TEST_ASSERT_EQUAL(event.content.pack.has_value(), true);
	TEST_ASSERT_EQUAL(event.content.pack->display_name, "Awesome Pack");
	TEST_ASSERT_EQUAL(event.content.pack->attribution, "huh");
	TEST_ASSERT_EQUAL(event.content.pack->avatar_url,
"mxc://example.org/asdjfasd"); TEST_ASSERT_EQUAL(event.content.pack->is_emoji(),
true); TEST_ASSERT_EQUAL(event.content.pack->is_sticker(), false);
	ASSERT_EQ(event.content.images.size(), 2);
	TEST_ASSERT_EQUAL(event.content.images["emote"].url,
"mxc://example.org/blah");
	TEST_ASSERT_EQUAL(event.content.images["emote"].overrides_usage(), false);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].url,
"mxc://example.org/sticker");
	TEST_ASSERT_EQUAL(event.content.images["sticker"].body, "stcikerly");
	TEST_ASSERT_EQUAL(event.content.images["sticker"].is_sticker(), true);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].is_emoji(), false);
	TEST_ASSERT_EQUAL(event.content.images["sticker"].overrides_usage(), true);
	TEST_ASSERT_EQUAL(json(event)["content"]["images"].size(), 2);
}

TEST(RoomAccountData, ImagePackRooms)
{
	json data = R"({
		  "content": {
  "rooms": {
	"!someroom:example.org": {
	  "": {},
	  "de.sorunome.mx-puppet-bridge.discord": {}
	},
	"!someotherroom:example.org": {
	  "": {}
	}
  }
},
		  "type": "im.ponies.emote_rooms"
		})"_json;

	ns::AccountDataEvent<ns::msc2545::ImagePackRooms> event = data;

	TEST_ASSERT_EQUAL(event.type, ImagePackRooms);
	TEST_ASSERT_EQUAL(event.content.rooms.size(), 2);
	TEST_ASSERT_EQUAL(event.content.rooms["!someroom:example.org"].size(), 2);
	TEST_ASSERT_EQUAL(event.content.rooms["!someroom:example.org"].count(""),
1); TEST_ASSERT_EQUAL(
	  event.content.rooms["!someroom:example.org"].count("de.sorunome.mx-puppet-bridge.discord"),
	  1);
	TEST_ASSERT_EQUAL(json(event)["content"]["rooms"].size(), 2);

	ns::msc2545::ImagePackRooms empty = {};
	TEST_ASSERT_EQUAL(json(empty).dump(), "{\"rooms\":{}}");

	ns::msc2545::ImagePackRooms empty2 = json::object();
	EXPECT_TRUE(empty2.rooms.empty());
}

TEST(Presence, Presence)
{
	json data = R"({
		"content": {
		"avatar_url": "mxc://localhost:wefuiwegh8742w",
		"currently_active": true,
		"last_active_ago": 2478593,
		"presence": "online",
		"status_msg": "Making cupcakes"
		},
		"sender": "@example:localhost",
		"type": "m.presence"
	})"_json;

	ns::Event<ns::presence::Presence> event = data;

	TEST_ASSERT_EQUAL(event.type, Presence);
	TEST_ASSERT_EQUAL(event.base.sender, "@example:localhost");
	TEST_ASSERT_EQUAL(event.content.avatar_url,
"mxc://localhost:wefuiwegh8742w");
	TEST_ASSERT_EQUAL(event.content.currently_active, true);
	TEST_ASSERT_EQUAL(event.content.last_active_ago, 2478593);
	TEST_ASSERT_EQUAL(event.content.presence, mtx::presence::online);
	TEST_ASSERT_EQUAL(event.content.status_msg, "Making cupcakes");
	TEST_ASSERT_EQUAL(data, json(event));
}
#endif
