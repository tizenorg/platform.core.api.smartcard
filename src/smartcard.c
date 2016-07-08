/*
* Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <unistd.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

#include <dlog.h>
#include <system_info.h>

#include "smartcard.h"
#include "smartcard_debug.h"

#ifdef TIZEN_SMARTCARD_SUPPORT
#include "smartcard-service.h"

#define  SE_FEATURE "http://tizen.org/feature/network.secure_element"
#define  SE_UICC_FEATURE "http://tizen.org/feature/network.secure_element.uicc"
#define  SE_ESE_FEATURE "http://tizen.org/feature/network.secure_element.ese"

#define  SMARTCARD_LOCK \
do { \
	pthread_mutex_lock(&mutex); \
} while (0);

#define  SMARTCARD_UNLOCK \
do { \
	pthread_mutex_unlock(&mutex); \
} while (0);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int ref_count;
se_service_h se_service;
bool _is_initialize = false;

GList *reader_list;
GList *session_list;
GList *channel_list;

#define CHECK_SUPPORTED() \
do { \
	{ \
		if (_is_smartcard_supported() == false) { \
			return SMARTCARD_ERROR_NOT_SUPPORTED; \
		} \
	} \
} while (0);


#define CHECK_INIT() \
do { \
	SMARTCARD_LOCK; \
	{ \
		if (_is_initialize == false) { \
			SMARTCARD_UNLOCK; \
			return SMARTCARD_ERROR_NOT_INITIALIZED; \
		} \
	} \
	SMARTCARD_UNLOCK; \
} while (0);

static bool _is_smartcard_supported()
{
	bool ret = false;
	bool is_supported_se = false;
	bool is_supported_se_uicc = false;
	bool is_supported_se_ese = false;

	system_info_get_platform_bool(SE_FEATURE, &is_supported_se);
	system_info_get_platform_bool(SE_UICC_FEATURE, &is_supported_se_uicc);
	system_info_get_platform_bool(SE_ESE_FEATURE, &is_supported_se_ese);

	if (is_supported_se && (is_supported_se_uicc || is_supported_se_ese))
		ret = true;

	return ret;
}

static int _check_precondition_reader(int handle)
{
	CHECK_INIT();

	SMARTCARD_LOCK;
	if (g_list_find(g_list_first(reader_list), GINT_TO_POINTER(handle)) == NULL) {
		SMARTCARD_UNLOCK;
		return SMARTCARD_ERROR_INVALID_PARAMETER;
	}
	SMARTCARD_UNLOCK;

	return SMARTCARD_ERROR_NONE;
}

static int _check_precondition_session(int handle)
{
	CHECK_INIT();

	SMARTCARD_LOCK;
	if (g_list_find(g_list_first(session_list), GINT_TO_POINTER(handle)) == NULL) {
		SMARTCARD_UNLOCK;
		return SMARTCARD_ERROR_INVALID_PARAMETER;
	}
	SMARTCARD_UNLOCK;

	return SMARTCARD_ERROR_NONE;
}

static int _check_precondition_channel(int handle)
{
	CHECK_INIT();

	SMARTCARD_LOCK;
	if (g_list_find(g_list_first(channel_list), GINT_TO_POINTER(handle)) == NULL) {
		SMARTCARD_UNLOCK;
		return SMARTCARD_ERROR_INVALID_PARAMETER;
	}
	/* LCOV_EXCL_START */
	SMARTCARD_UNLOCK;

	return SMARTCARD_ERROR_NONE;
	/* LCOV_EXCL_STOP */
}

static smartcard_error_e _convert_error_code(const char *func, int native_error_code)
{
	smartcard_error_e error_code = SMARTCARD_ERROR_NONE;
	char *errorstr = NULL;

	switch (native_error_code) {
	case SCARD_ERROR_OK:
		error_code = SMARTCARD_ERROR_NONE;
		errorstr = "SMARTCARD_ERROR_NONE";
		break;
	case SCARD_ERROR_NOT_SUPPORTED:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_OPERATION_NOT_SUPPORTED;
		errorstr = "SMARTCARD_ERROR_OPERATION_NOT_SUPPORTED";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_UNAVAILABLE:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_CHANNEL_NOT_AVAILABLE;
		errorstr = "SMARTCARD_ERROR_CHANNEL_NOT_AVAILABLE";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_IPC_FAILED:
	case SCARD_ERROR_IO_FAILED:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_IO_ERROR;
		errorstr = "SMARTCARD_ERROR_IO_ERROR";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_SECURITY_NOT_ALLOWED:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_PERMISSION_DENIED;
		errorstr = "SMARTCARD_ERROR_PERMISSION_DENIED";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_ILLEGAL_STATE:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_ILLEGAL_STATE;
		errorstr = "SMARTCARD_ERROR_ILLEGAL_STATE";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_ILLEGAL_PARAM:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_INVALID_PARAMETER;
		errorstr = "SMARTCARD_ERROR_INVALID_PARAMETER";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_ILLEGAL_REFERENCE:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_ILLEGAL_REFERENCE;
		errorstr = "SMARTCARD_ERROR_ILLEGAL_REFERENCE";
		break;
		/* LCOV_EXCL_STOP */
	case SCARD_ERROR_NOT_INITIALIZED:
	case SCARD_ERROR_SE_NOT_INITIALIZED:
	case SCARD_ERROR_OPERATION_NOT_SUPPORTED:
	case SCARD_ERROR_NEED_MORE_BUFFER:
	case SCARD_ERROR_OPERATION_TIMEOUT:
	case SCARD_ERROR_NOT_ENOUGH_RESOURCE:
	case SCARD_ERROR_OUT_OF_MEMORY:
	case SCARD_ERROR_UNKNOWN:
	default:
		/* LCOV_EXCL_START */
		error_code = SMARTCARD_ERROR_GENERAL;
		errorstr = "SMARTCARD_ERROR_GENERAL";
		/* LCOV_EXCL_STOP */
	}

	_ERR("smartcard func : %s, %s(0x%08x)\n", func, errorstr, error_code);

	return error_code;
}
#endif

/* managing apis : initaialize, deinitialize */

int smartcard_initialize(void)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();
	CHECK_SUPPORTED();

	SMARTCARD_LOCK;

		if (se_service == NULL || ref_count > 0) {
			/* create se service instance. */
			se_service = se_service_create_instance_sync(NULL, &ret);
			if (se_service == NULL || ret != SCARD_ERROR_OK) {
				SMARTCARD_UNLOCK; /* LCOV_EXCL_LINE */
				if (ret == SCARD_ERROR_SECURITY_NOT_ALLOWED) /* LCOV_EXCL_LINE */
					return SMARTCARD_ERROR_PERMISSION_DENIED;

				return SMARTCARD_ERROR_GENERAL; /* LCOV_EXCL_LINE */
			}

			_is_initialize = true;
		}
		ref_count++;

	SMARTCARD_UNLOCK;

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_deinitialize(void)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();
	CHECK_SUPPORTED();

	SMARTCARD_LOCK;

		if (ref_count > 0) {
			ref_count--;
		} else {
			SMARTCARD_UNLOCK;
			return SMARTCARD_ERROR_NOT_INITIALIZED;
		}

		if (se_service != NULL && ref_count == 0) {
			se_service_shutdown(se_service);

			/* destroy se service instance. */
			ret = se_service_destroy_instance(se_service);
			if (ret != SCARD_ERROR_OK) {
				/* LCOV_EXCL_START */
				SMARTCARD_UNLOCK;
				return SMARTCARD_ERROR_GENERAL;
				/* LCOV_EXCL_STOP */
			}

			se_service = NULL;
			g_list_free(reader_list);
			g_list_free(session_list);
			g_list_free(channel_list);
			reader_list = NULL;
			session_list = NULL;
			channel_list = NULL;
			_is_initialize = false;
		}

	SMARTCARD_UNLOCK;

	_END();

	return _convert_error_code(__func__, ret);
#endif
}


/*----------------<SE Service mapping api>------------------*/

int smartcard_get_readers(int **readers, int *length)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int i;
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == readers, SMARTCARD_ERROR_INVALID_PARAMETER);
	cond_expr_ret(NULL == length, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* precondition check end */

	SMARTCARD_LOCK;

	ret = se_service_get_readers(se_service, readers, length);

	SMARTCARD_UNLOCK;

	for (i = 0; i < *length; i++) {
		SMARTCARD_LOCK;

			if (g_list_find(g_list_first(reader_list), GINT_TO_POINTER(*(readers[i]))) == NULL)
				reader_list = g_list_append(reader_list, GINT_TO_POINTER(*(readers[i])));

		SMARTCARD_UNLOCK;
	}

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

/*----------------<reader mapping api>------------------*/

int smartcard_reader_set_event_cb(smartcard_reader_event_cb cb, void *user_data)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */

	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == cb, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* precondition check end */

	se_service_set_event_handler(se_service, (se_service_event_cb)cb, user_data);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_reader_unset_event_cb(void)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */

	CHECK_SUPPORTED();
	CHECK_INIT();

	/* precondition check end */

	se_service_unset_event_handler(se_service);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_reader_get_name(int reader, char **reader_name)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == reader_name, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_reader(reader);
	cond_ret(ret);

	/* precondition check end */

	/* process */
	ret = reader_get_name((reader_h)reader, reader_name);

	if (ret != SCARD_ERROR_OK || strlen(*reader_name) == 0)
		return SMARTCARD_ERROR_GENERAL;

	_END();

	return SMARTCARD_ERROR_NONE;
#endif
}

int smartcard_reader_is_secure_element_present(int reader, bool *is_present)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == is_present, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_reader(reader);
	cond_ret(ret);

	/* precondition check end */

	ret = reader_is_secure_element_present((reader_h)reader, is_present);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_reader_open_session(int reader, int *session)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	ret = _check_precondition_reader(reader);
	cond_ret(ret);

	cond_expr_ret(NULL == session, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* precondition check end */

	ret = reader_open_session_sync((reader_h)reader, session);

	if (ret == SCARD_ERROR_OK) {
		SMARTCARD_LOCK;
			if (g_list_find(g_list_first(session_list), GINT_TO_POINTER(*session)) == NULL)
				session_list = g_list_append(session_list, GINT_TO_POINTER(*session));
		SMARTCARD_UNLOCK;
	}

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_reader_close_sessions(int reader)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	ret = _check_precondition_reader(reader);
	cond_ret(ret);

	/* precondition check end */

	ret = reader_close_sessions((reader_h)reader);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

/*----------------<session mapping api>------------------*/

int smartcard_session_get_reader(int session, int *reader)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == reader, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret = session_get_reader((session_h)session, reader);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_session_get_atr(int session, unsigned char **atr, int *length)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == atr, SMARTCARD_ERROR_INVALID_PARAMETER);
	cond_expr_ret(NULL == length, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_session(session);
	cond_ret(ret);

	*length = 0;
	*atr = NULL;

	/* precondition check end */
	ret = session_get_atr_sync((session_h)session, atr, (unsigned int *)length);

	if (ret == SCARD_ERROR_OK) {
		if (((*atr == NULL) && (*length != 0)) ||
			((*atr != NULL) && (*length == 0))) {
			return SMARTCARD_ERROR_GENERAL;
		}
	}

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_session_close(int session)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret =	session_close_sync((session_h)session);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_session_is_closed(int session, bool *is_closed)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == is_closed, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret = session_is_closed((session_h)session, is_closed);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_session_close_channels(int session)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret = session_close_channels((session_h)session);

	_END();

	return _convert_error_code(__func__, ret);
#endif
}

int smartcard_session_open_basic_channel(int session, unsigned char *aid, int aid_len,
	unsigned char P2, int *channel)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == channel, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret = session_open_basic_channel_sync((session_h)session, aid, aid_len, P2, channel);

	if (ret == SCARD_ERROR_OK) {
		SMARTCARD_LOCK;
			if (g_list_find(g_list_first(channel_list), GINT_TO_POINTER(*channel)) == NULL)
				channel_list = g_list_append(channel_list, GINT_TO_POINTER(*channel));
		SMARTCARD_UNLOCK;
	}

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_session_open_logical_channel(int session, unsigned char *aid, int aid_len,
		unsigned char P2, int *channel)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == channel, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_session(session);
	cond_ret(ret);

	/* precondition check end */

	ret = session_open_logical_channel_sync((session_h)session, aid, aid_len, P2, channel);

	if (ret == SCARD_ERROR_OK) {
		SMARTCARD_LOCK;
			if (g_list_find(g_list_first(channel_list), GINT_TO_POINTER(*channel)) == NULL)
				channel_list = g_list_append(channel_list, GINT_TO_POINTER(*channel));
		SMARTCARD_UNLOCK;
	}

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

/*----------------<channel mapping api>------------------*/

int smartcard_channel_close(int channel)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */
	/* LCOV_EXCL_START */
	ret = channel_close_sync((channel_h)channel);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_is_basic_channel(int channel, bool *is_basic_channel)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == is_basic_channel, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_is_basic_channel((channel_h)channel, is_basic_channel);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_is_closed(int channel, bool *is_closed)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == is_closed, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_is_closed((channel_h)channel, is_closed);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_get_select_response(int channel, unsigned char **select_resp,
	int *length)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	size_t s_length;
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == select_resp, SMARTCARD_ERROR_INVALID_PARAMETER);
	/* LCOV_EXCL_START */
	cond_expr_ret(NULL == length, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_get_select_response((channel_h)channel, select_resp, &s_length);
	*length = (int)s_length;

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_get_session(int channel, int *session)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == session, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_get_session((channel_h)channel, session);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_transmit(int channel, unsigned char *cmd, int cmd_len,
	unsigned char **resp, int *length)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == resp, SMARTCARD_ERROR_INVALID_PARAMETER);
	/* LCOV_EXCL_START */
	cond_expr_ret(NULL == length, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_transmit_sync((channel_h)channel, cmd, cmd_len, resp, (unsigned int *)length);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_transmit_retrieve_response(int channel, unsigned char **resp, int *length)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	size_t s_length;
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == resp, SMARTCARD_ERROR_INVALID_PARAMETER);
	/* LCOV_EXCL_START */
	cond_expr_ret(NULL == length, SMARTCARD_ERROR_INVALID_PARAMETER);

	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_get_transmit_response((channel_h)channel, resp, &s_length);
	*length = (int)s_length;

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}

int smartcard_channel_select_next(int channel, bool *is_success)
{
#ifndef TIZEN_SMARTCARD_SUPPORT
	return SMARTCARD_ERROR_NOT_SUPPORTED;
#else
	int ret = SMARTCARD_ERROR_NONE;

	_BEGIN();

	/* precondition check start */
	CHECK_SUPPORTED();
	CHECK_INIT();

	cond_expr_ret(NULL == is_success, SMARTCARD_ERROR_INVALID_PARAMETER);

	/* LCOV_EXCL_START */
	ret = _check_precondition_channel(channel);
	cond_ret(ret);

	/* precondition check end */

	ret = channel_select_next((channel_h)channel, is_success);

	_END();

	return _convert_error_code(__func__, ret);
	/* LCOV_EXCL_STOP */
#endif
}
