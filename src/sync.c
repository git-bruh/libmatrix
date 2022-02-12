#include "matrix-priv.h"

/* For boolean evaluation */
#define STREQ(s1, s2) ((strcmp(s1, s2)) == 0)

/* Safely get an int from a cJSON object without overflows. */
static int
get_int(const cJSON *json, const char name[], int int_default) {
	double tmp = cJSON_GetNumberValue(cJSON_GetObjectItem(json, name));

	if (!(isnan(tmp))) {
		if (tmp > INT_MAX) {
			return INT_MAX;
		}

		if (tmp < INT_MIN) {
			return INT_MIN;
		}

		return (int) tmp;
	}

	return int_default;
}

static uint64_t
get_uint64(const cJSON *json, const char name[], uint64_t uint64_default) {
	double tmp = cJSON_GetNumberValue(cJSON_GetObjectItem(json, name));

	if (!(isnan(tmp)) && tmp >= 0) {
		/* Overflow. */
		if (((double) ((uint64_t) tmp)) < tmp) {
			return UINT64_MAX;
		}

		return (uint64_t) tmp;
	}

	return uint64_default;
}

static cJSON *
get_array(const cJSON *const object, const char *const string) {
	cJSON *tmp = cJSON_GetObjectItem(object, string);

	/* Elements are located in the child member. */
	return tmp ? tmp->child : NULL;
}

static int
parse_summary(struct matrix_room_summary *summary, const cJSON *data) {
	if (!data) {
		return -1;
	}

	*summary = (struct matrix_room_summary) {
	  .joined_member_count = get_int(data, "m.joined_member_count", 0),
	  .invited_member_count = get_int(data, "m.invited_member_count", 0),
	  .heroes = cJSON_GetObjectItem(data, "m.heroes"),
	};

	return 0;
}

static int
parse_timeline(struct matrix_room_timeline *timeline, const cJSON *data) {
	if (!data) {
		return -1;
	}

	*timeline = (struct matrix_room_timeline) {
	  .prev_batch = GETSTR(data, "prev_batch"),
	  .limited = cJSON_IsTrue(cJSON_GetObjectItem(data, "limited")),
	};

	return 0;
}

int
matrix_sync_room_next(
  struct matrix_sync_response *response, struct matrix_room *room) {
	for (enum matrix_room_type type = 0; type < MATRIX_ROOM_MAX; type++) {
		cJSON *room_json = response->rooms[type];

		while (room_json) {
			*room = (struct matrix_room) {
			  .id = room_json->string,
			  .events
			  = {[MATRIX_EVENT_STATE]
= type != MATRIX_ROOM_INVITE
? get_array(
cJSON_GetObjectItem(room_json, "state"), "events")
 : get_array(
 cJSON_GetObjectItem(room_json, "invite_state"), "events"),
					[MATRIX_EVENT_TIMELINE] = get_array(
				  cJSON_GetObjectItem(room_json, "timeline"), "events"),
					[MATRIX_EVENT_EPHEMERAL] = get_array(
				  cJSON_GetObjectItem(room_json, "ephemeral"), "events")},
			  .type = type,
			};

			switch (type) {
			case MATRIX_ROOM_LEAVE:
			case MATRIX_ROOM_JOIN:
				parse_summary(
				  &room->summary, cJSON_GetObjectItem(room_json, "summary"));
				parse_timeline(
				  &room->timeline, cJSON_GetObjectItem(room_json, "timeline"));
				break;
			case MATRIX_ROOM_INVITE:
				parse_summary(
				  &room->summary, cJSON_GetObjectItem(room_json, "summary"));
				break;
			default:
				assert(0);
			};

			room_json = response->rooms[type] = room_json->next;

			if (room->id) {
				return 0;
			}
		}
	}

	return -1;
}

static bool
parse_state_member(struct matrix_room_member *revent, const cJSON *content,
  const char *state_key) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_member) {
	  .is_direct = cJSON_IsTrue(cJSON_GetObjectItem(content, "is_direct")),
	  .membership = GETSTR(content, "membership"),
	  .avatar_url = GETSTR(content, "avatar_url"),
	  .displayname = GETSTR(content, "displayname"),
	};

	return !!state_key && !!revent->membership && ((strnlen(state_key, 1)) > 0);
}

static bool
parse_state_power_levels(
  struct matrix_room_power_levels *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	const int default_power = 50;

	*revent = (struct matrix_room_power_levels) {
	  .ban = get_int(content, "ban", default_power),
	  .events_default = get_int(content, "events_default", 0), /* Exception. */
	  .invite = get_int(content, "invite", default_power),
	  .kick = get_int(content, "kick", default_power),
	  .redact = get_int(content, "redact", default_power),
	  .state_default = get_int(content, "state_default", default_power),
	  .users_default = get_int(content, "users_default", 0), /* Exception. */
	  .events = cJSON_GetObjectItem(content, "events"),
	  .notifications = cJSON_GetObjectItem(content, "notifications"),
	  .users = cJSON_GetObjectItem(content, "users"),
	};

	return true;
}

static bool
parse_state_canonical_alias(
  struct matrix_room_canonical_alias *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_canonical_alias) {
	  .alias = GETSTR(content, "alias"),
	};

	return true;
}

static bool
parse_state_create(struct matrix_room_create *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	cJSON *federate = cJSON_GetObjectItem(content, "m.federate");
	cJSON *predecessor = cJSON_GetObjectItem(content, "predecessor");

	const char *version = GETSTR(content, "room_version");

	if (!version) {
		version = "1";
	}

	*revent = (struct matrix_room_create) {
		  .federate = federate ? cJSON_IsTrue(federate)
							   : true, /* Federation is enabled if the key
										  doesn't exist. */
		  .creator = GETSTR(content, "creator"),
		  .predecessor = {
			.event_id = GETSTR(predecessor, "event_id"),
			.room_id = GETSTR(predecessor, "room_id"),
		  },
		  .room_version = version,
		  .type = GETSTR(content, "type"),
	};

	return true;
}

static bool
parse_state_join_rules(
  struct matrix_room_join_rules *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_join_rules) {
	  .join_rule = GETSTR(content, "join_rule"),
	};

	return !!revent->join_rule;
}

static bool
parse_state_name(struct matrix_room_name *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_name) {
	  .name = GETSTR(content, "name"),
	};

	return true;
}

static bool
parse_state_topic(struct matrix_room_topic *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_topic) {
	  .topic = GETSTR(content, "topic"),
	};

	return true;
}

static bool
parse_state_avatar(struct matrix_room_avatar *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	cJSON *info = cJSON_GetObjectItem(content, "info");

	*revent = (struct matrix_room_avatar){
		  .url = GETSTR(content, "url"),
		  .info = {
		  .size = get_int(info, "size", 0),
		  .mimetype = GETSTR(info, "mimetype"),
		},
	};

	return true;
}

static bool
parse_state_space_child(struct matrix_room_space_child *revent,
  const cJSON *content, const char *state_key) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_space_child) {
	  .suggested = cJSON_IsTrue(cJSON_GetObjectItem(content, "suggested")),
	  .order = GETSTR(content, "order"),
	  .via = cJSON_GetObjectItem(content, "via"),
	};

	if (!(cJSON_IsArray(revent->via))
		|| (cJSON_GetArraySize(revent->via)) < 1) {
		revent->via = NULL;
	}

	return !!state_key && state_key[0] == '!';
}

static bool
parse_state_space_parent(struct matrix_room_space_parent *revent,
  const cJSON *content, const char *state_key) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_room_space_parent) {
	  .canonical = cJSON_IsTrue(cJSON_GetObjectItem(content, "canonical")),
	  .via = cJSON_GetObjectItem(content, "via"),
	};

	if (!(cJSON_IsArray(revent->via))
		|| (cJSON_GetArraySize(revent->via)) < 1) {
		revent->via = NULL;
	}

	return !!state_key && state_key[0] == '!';
}

static bool
parse_state_unknown(struct matrix_unknown_state *revent, const cJSON *content) {
	assert(revent);
	assert(content);

	*revent = (struct matrix_unknown_state) {
	  .content = content,
	};

	return true;
}

/* Assign the event type and compare in the same statement to reduce chance of
 * typos. */
#define TYPE(enumeration, string)                                              \
	(revent->type = (enumeration), (STREQ(revent->base.type, string)))

int
matrix_event_state_parse(
  struct matrix_state_event *revent, const matrix_json_t *event) {
	if (!revent || !event) {
		return -1;
	}

	memset(revent, 0, sizeof(*revent));

	bool is_valid = true;

	revent->base = (struct matrix_state_base) {
	  .state_key = GETSTR(event, "state_key"),
	  .origin_server_ts = get_uint64(event, "origin_server_ts", 0),
	  .event_id = GETSTR(event, "event_id"),
	  .sender = GETSTR(event, "sender"),
	  .type = GETSTR(event, "type"),
	};

	cJSON *content = cJSON_GetObjectItem(event, "content");
	cJSON *unsigned_obj = cJSON_GetObjectItem(event, "unsigned");
	cJSON *prev_content = cJSON_GetObjectItem(unsigned_obj, "prev_content");

	if (!prev_content) {
		prev_content = cJSON_GetObjectItem(event, "prev_content");
	}

	if (!content || !revent->base.state_key || !revent->base.origin_server_ts
		|| !revent->base.event_id || !revent->base.sender
		|| !revent->base.type) {
		return -1;
	}

	revent->replaces_state = GETSTR(unsigned_obj, "replaces_state");

	/* Set the .content and .prev_content for a given state type. */
#define SET_STATE(state_member)                                                \
	do {                                                                       \
		is_valid = parse_state_##state_member(                                 \
		  &revent->content.state_member, content);                             \
		revent->prev_content_is_valid                                          \
		  = (prev_content                                                      \
			 && parse_state_##state_member(                                    \
			   &revent->prev_content.state_member, prev_content));             \
	} while (0)

	/* The same, but for state types that check state keys for validity. */
#define SET_STATE_WITH_KEY(state_member)                                       \
	do {                                                                       \
		is_valid = parse_state_##state_member(                                 \
		  &revent->content.state_member, content, revent->base.state_key);     \
		revent->prev_content_is_valid                                          \
		  = (prev_content                                                      \
			 && parse_state_##state_member(&revent->prev_content.state_member, \
			   prev_content, revent->base.state_key));                         \
	} while (0)

	if (TYPE(MATRIX_ROOM_MEMBER, "m.room.member")) {
		SET_STATE_WITH_KEY(member);
	} else if (TYPE(MATRIX_ROOM_POWER_LEVELS, "m.room.power_levels")) {
		SET_STATE(power_levels);
	} else if (TYPE(MATRIX_ROOM_CANONICAL_ALIAS, "m.room.canonical_alias")) {
		SET_STATE(canonical_alias);
	} else if (TYPE(MATRIX_ROOM_CREATE, "m.room.create")) {
		SET_STATE(create);
	} else if (TYPE(MATRIX_ROOM_JOIN_RULES, "m.room.join_rules")) {
		SET_STATE(join_rules);
	} else if (TYPE(MATRIX_ROOM_NAME, "m.room.name")) {
		SET_STATE(name);
	} else if (TYPE(MATRIX_ROOM_TOPIC, "m.room.topic")) {
		SET_STATE(topic);
	} else if (TYPE(MATRIX_ROOM_AVATAR, "m.room.avatar")) {
		SET_STATE(avatar);
	} else if (TYPE(MATRIX_ROOM_SPACE_CHILD, "m.space.child")) {
		SET_STATE_WITH_KEY(space_child);
	} else if (TYPE(MATRIX_ROOM_SPACE_PARENT, "m.space.parent")) {
		SET_STATE_WITH_KEY(space_parent);
	} else {
		revent->type = MATRIX_ROOM_UNKNOWN_STATE;
		SET_STATE(unknown);
	}

#undef SET_STATE
#undef SET_STATE_WITH_KEY

	if (!is_valid && !matrix_json_has_content(event)) {
		revent->base.content_was_empty = true;
		is_valid = true;
	}

	return is_valid ? 0 : -1;
}

int
matrix_event_timeline_parse(
  struct matrix_timeline_event *revent, const matrix_json_t *event) {
	/* Timeline event must not have a state_key. */
	if (!revent || !event || (GETSTR(event, "state_key"))) {
		return -1;
	}

	memset(revent, 0, sizeof(*revent));

	bool is_valid = false;

	revent->base = (struct matrix_room_base) {
	  .origin_server_ts = get_uint64(event, "origin_server_ts", 0),
	  .event_id = GETSTR(event, "event_id"),
	  .sender = GETSTR(event, "sender"),
	  .type = GETSTR(event, "type"),
	};

	cJSON *content = NULL;

	if (!revent->base.origin_server_ts || !revent->base.event_id
		|| !revent->base.sender || !revent->base.type
		|| !(content = cJSON_GetObjectItem(event, "content"))) {
		return -1;
	}

	{
		cJSON *relates_to = cJSON_GetObjectItem(content, "m.relates_to");
		cJSON *in_reply_to = cJSON_GetObjectItem(relates_to, "m.in_reply_to");
		char *event_id = GETSTR(relates_to, "event_id");

		enum matrix_rel_type rel_type = MATRIX_RELATION_UNKNOWN;
		char *rel_type_str = GETSTR(relates_to, "rel_type");

		/* "m.in_reply_to" is a nested json object. */
		if (in_reply_to) {
			rel_type = MATRIX_RELATION_IN_REPLY_TO;
			event_id = GETSTR(in_reply_to, "event_id");
		} else if (rel_type_str) {
			if ((STREQ(rel_type_str, "m.annotation"))) {
				rel_type = MATRIX_RELATION_ANNOTATION;
			} else if ((STREQ(rel_type_str, "m.replace"))) {
				rel_type = MATRIX_RELATION_REPLACE;
			}
		}

		revent->relation = (struct matrix_event_relation) {.rel_type = rel_type,
		  .rel_type_str = rel_type_str,
		  .event_id = event_id,
		  .key = GETSTR(relates_to, "key"),
		  .new_content = cJSON_GetObjectItem(content, "new_content")};
	}

	if (TYPE(MATRIX_ROOM_MESSAGE, "m.room.message")) {
		revent->message = (struct matrix_room_message) {
		  .body = GETSTR(content, "body"),
		  .msgtype = GETSTR(content, "msgtype"),
		  .format = GETSTR(content, "format"),
		  .formatted_body = GETSTR(content, "formatted_body"),
		};

		is_valid = !!revent->message.body && !!revent->message.msgtype;

		cJSON *info = cJSON_GetObjectItem(content, "info");

		/* Check if the message is an attachment. */
		if (is_valid && info
			&& (STREQ(revent->message.msgtype, "m.image")
				|| STREQ(revent->message.msgtype, "m.file")
				|| STREQ(revent->message.msgtype, "m.audio")
				|| STREQ(revent->message.msgtype, "m.video"))) {
			revent->type = MATRIX_ROOM_ATTACHMENT;
			revent->attachment = (struct matrix_room_attachment) {
			  .body = GETSTR(content, "body"),
			  .msgtype = GETSTR(content, "msgtype"),
			  .url = GETSTR(content, "url"),
			  .filename = GETSTR(content, "filename"),
			  .info = {.size = get_int(info, "size", 0),
						.mimetype = GETSTR(info, "mimetype")},
			};

			is_valid = !!revent->attachment.body && !!revent->attachment.msgtype
					&& !!revent->attachment.url;
		}
	} else if (TYPE(MATRIX_ROOM_REDACTION, "m.room.redaction")) {
		revent->redaction = (struct matrix_room_redaction) {
		  .redacts = GETSTR(event, "redacts"),
		  .reason = GETSTR(content, "reason"),
		};

		is_valid = !!revent->redaction.redacts;
	}

	return is_valid ? 0 : -1;
}

int
matrix_event_ephemeral_parse(
  struct matrix_ephemeral_event *revent, const matrix_json_t *event) {
	if (!revent || !event) {
		return -1;
	}

	memset(revent, 0, sizeof(*revent));

	bool is_valid = false;

	revent->base = (struct matrix_ephemeral_base) {
	  .type = GETSTR(event, "type"),
	  .room_id = GETSTR(event, "room_id"),
	};

	cJSON *content = cJSON_GetObjectItem(event, "content");

	if (!content) {
		return -1;
	}

	if (TYPE(MATRIX_ROOM_TYPING, "m.typing")) {
		revent->typing = (struct matrix_room_typing) {
		  .user_ids = cJSON_GetObjectItem(content, "user_ids"),
		};

		is_valid = !!revent->typing.user_ids;
	}

	return is_valid ? 0 : -1;
}

#undef TYPE

int
matrix_event_sync_parse(
  struct matrix_sync_event *revent, const matrix_json_t *event) {
	if (!revent || !event) {
		return -1;
	}

	/* State events must have a state_key */
	if ((GETSTR(event, "state_key"))) {
		revent->type = MATRIX_EVENT_STATE;
		return matrix_event_state_parse(&revent->state, event);
	}

	revent->type = MATRIX_EVENT_TIMELINE;
	return matrix_event_timeline_parse(&revent->timeline, event);
}

int
matrix_sync_event_next(
  struct matrix_room *room, struct matrix_sync_event *revent) {
	if (!room || !revent) {
		return -1;
	}

	for (revent->type = 0; revent->type < MATRIX_EVENT_MAX; revent->type++) {
		bool done = false;

		matrix_json_t **json = &room->events[revent->type];

		while (!done && *json) {
			revent->json = *json;

			switch (revent->type) {
			case MATRIX_EVENT_STATE:
				done = ((matrix_event_state_parse(&revent->state, *json)) == 0);
				break;
			case MATRIX_EVENT_TIMELINE:
				done = ((matrix_event_timeline_parse(&revent->timeline, *json))
						== 0);

				/* Sometimes state events might pop up in the timeline. */
				if (!done
					&& (matrix_event_state_parse(&revent->state, *json)) == 0) {
					revent->type = MATRIX_EVENT_STATE;
					revent->state.is_in_timeline = true;
					done = true;
				}
				break;
			case MATRIX_EVENT_EPHEMERAL:
				done
				  = ((matrix_event_ephemeral_parse(&revent->ephemeral, *json))
					 == 0);
				break;
			default:
				assert(0);
			}

			*json = (*json)->next;
		}

		if (done) {
			return 0;
		}
	}

	return -1;
}

const char *
matrix_sync_event_id(struct matrix_sync_event *event) {
	if (!event) {
		return NULL;
	}

	switch (event->type) {
	case MATRIX_EVENT_STATE:
		return event->state.base.event_id;
	case MATRIX_EVENT_TIMELINE:
		return event->timeline.base.event_id;
	case MATRIX_EVENT_EPHEMERAL:
	default:
		return NULL;
	}
}

int
matrix_dispatch_sync(struct matrix *matrix,
  const struct matrix_sync_callbacks *callbacks, const cJSON *sync) {
	if (!matrix || !callbacks || !sync) {
		return -1;
	}

	cJSON *rooms = cJSON_GetObjectItem(sync, "rooms");

	struct matrix_sync_response response = {
	  .next_batch = GETSTR(sync, "next_batch"),
	  .rooms =
		{
		  [MATRIX_ROOM_LEAVE] = get_array(rooms, "leave"),
		  [MATRIX_ROOM_JOIN] = get_array(rooms, "join"),
		  [MATRIX_ROOM_INVITE] = get_array(rooms, "invite"),
		},
	};

	callbacks->sync_cb(matrix, &response);

	return 0;
}
