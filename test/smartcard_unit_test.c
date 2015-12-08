#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#include <smartcard.h>

#define BUFFER_LEN 10
#define PRT(format, args...) printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args)
#define TC_PRT(format, args...) PRT(format"\n", ##args)


GMainLoop *main_loop = NULL;

typedef struct {
	const char *tc_name;
	int tc_code;
} tc_table_t;

tc_table_t tc_table[] = {
	/* manage api*/
	{"smartcard_initialize", 1},
	{"smartcard_deinitialize", 2},

	/* service api */
	{"smartcard_get_readers", 3},
	{"smartcard_get_version", 4},

	/* reader api */
	{"smartcard_reader_get_name", 5},
	{"smartcard_reader_is_secure_element_present", 6},
	{"smartcard_reader_open_session", 7},
	{"smartcard_reader_close_sessions", 8},

	/* session api */
	{"smartcard_session_get_reader", 9},
	{"smartcard_session_get_atr", 10},
	{"smartcard_session_close", 11},
	{"smartcard_session_is_closed", 12},
	{"smartcard_session_close_channels", 13},
	{"smartcard_session_open_basic_channel", 14},
	{"smartcard_session_open_logical_channel", 15},

	/* channel api */
	{"smartcard_channel_close", 16},
	{"smartcard_channel_is_basic_channel", 17},
	{"smartcard_channel_is_closed", 18},
	{"smartcard_channel_get_select_response", 19},
	{"smartcard_channel_get_session", 20},
	{"smartcard_channel_transmit", 21},
	{"smartcard_channel_transmit_get_response", 22},
	{"smartcard_channel_select_next", 23},

	/* -----------*/
	{"Finish"               , 0x00ff},
	{NULL                   , 0x0000},
};

void tc_usage_print(void)
{
	int i = 0;

	while (tc_table[i].tc_name) {
		if (tc_table[i].tc_code != 0x00ff)
			TC_PRT("Key %d : usage %s", tc_table[i].tc_code, tc_table[i].tc_name);
		else
			TC_PRT("Key %d : usage %s\n\n", 0x00ff, tc_table[i].tc_name);

		i++;
	}
}

int test_input_callback(void *data)
{
	int ret = 0;
	long test_id = (long)data;

	switch (test_id) {
	case 0x00ff:
		TC_PRT("Finished");
		g_main_loop_quit(main_loop);
		break;
	case 1:
		{
			int ret;
			ret = smartcard_initialize();

			if (ret == SMARTCARD_ERROR_NONE)
				TC_PRT("smartcard initialize success");
			else
				TC_PRT("smartcard initialize failed");
		}
		break;
	case 2:
		{
			int ret;
			ret = smartcard_deinitialize();

			if (ret == SMARTCARD_ERROR_NONE)
				TC_PRT("smartcard deinitialize success");
			else
				TC_PRT("smartcard deinitialize failed");
		}
		break;
	case 3:
		{
			int pLength;
			int *phReaders = NULL;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE) {
				TC_PRT("smartcard_get_readers is success");
				TC_PRT("reader handle : %d", phReaders[0]);
				TC_PRT("readers length : %d", pLength);
			} else {
				TC_PRT("smartcard_get_readers is failed : %d", ret);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 5:
		{
			int pLength;
			int *phReaders = NULL;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				char * pReader = NULL;

				ret = smartcard_reader_get_name(phReaders[0], &pReader);
				if (ret == SMARTCARD_ERROR_NONE) {
					TC_PRT("smartcard_reader_get_name success");
					TC_PRT("reader name : %s", pReader);
				} else {
					TC_PRT("smartcard_reader_get_name failed : %d", ret);
				}

				if (pReader != NULL)
					free(pReader);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 6:
		{
			int pLength;
			int *phReaders = NULL;
			bool is_present = false;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_is_secure_element_present(phReaders[0], &is_present);
				if (ret == SMARTCARD_ERROR_NONE) {
					TC_PRT("smartcard_reader_is_secure_element_present is success");
					TC_PRT("reader secure element present : %d", is_present);
				} else {
					TC_PRT("smartcard_reader_is_secure_element_present is failed : %d", ret);
				}
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 7:
		{
			int pLength;
			int *phReaders = NULL;
			int session;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE) {
					TC_PRT("smartcard_reader_open_session is success");
					TC_PRT("session : %d", (int)session);
				} else {
					TC_PRT("smartcard_reader_open_session is failed : %d", ret);
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 8:
		{
			int pLength;
			int *phReaders = NULL;
			int session;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE) {
					ret = smartcard_reader_close_sessions(phReaders[0]);
					if (ret == SMARTCARD_ERROR_NONE)
						TC_PRT("smartcard_reader_close_sessions success");
					else
						TC_PRT("smartcard_reader_close_sessions failed : %d", ret);
				} else {
					TC_PRT("open session failed : %d", ret);
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 9:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int reader;
			char * pReader = NULL;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_get_reader(session, &reader);
					ret = smartcard_reader_get_name(reader, &pReader);
					if (ret == SMARTCARD_ERROR_NONE) {
						TC_PRT("smartcard_session_get_reader success");
						TC_PRT("reader name : %s", pReader);
					} else {
						TC_PRT("smartcard_session_get_reader failed");
					}

					if (pReader != NULL)
						free(pReader);
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 10:
		{
			int i = 0;
			int pLength;
			int *phReaders = NULL;
			int session;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					pLength = 0;
					unsigned char * pAtr = NULL;

					ret = smartcard_session_get_atr(session, &pAtr, &pLength);

					if (ret == SMARTCARD_ERROR_NONE) {
						TC_PRT("smartcard_serssion_get_atr success : %d", pLength);
						g_print("response is ");
						for (i = 0; i < pLength; i++)
							g_print("%x ", (int)pAtr[i]);
						g_print("\n");
					} else {
						TC_PRT("smartcard_serssion_get_atr is failed");
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 11:
		{
			int pLength;
			int *phReaders = NULL;
			int session;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_close(session);
					if (ret == SMARTCARD_ERROR_NONE)
						TC_PRT("smartcard_session_close is success");
					else
						TC_PRT("smartcard_session_close is failed");
				}
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 12:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			bool is_closed = false;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_close(session);
					ret = smartcard_session_is_closed(session, &is_closed);
					if (ret == SMARTCARD_ERROR_NONE && is_closed == true)
						TC_PRT("smartcard_session_is_closed is success");
					else
						TC_PRT("smartcard_session_is_closed is failed");
				}
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 13:
		{
			int pLength;
			int *phReaders = NULL;
			int session;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_close_channels(session);
					if (ret == SMARTCARD_ERROR_NONE)
						TC_PRT("smartcard_session_close_channels is success");
					else
						TC_PRT("smartcard_session_close_channels is failed");
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 14:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_open_basic_channel(session, aid, 12, 0x00, &channel);
					if (ret == SMARTCARD_ERROR_NONE)
						TC_PRT("smartcard_session_open_basic_channel is success : %d", (int)channel);
					else
						TC_PRT("smartcard_session_open_basic_channel is failed");
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 15:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x04, &channel);
					if (ret == SMARTCARD_ERROR_NONE)
						TC_PRT("smartcard_session_open_basic_channel is success : %d", (int)channel);
					else
						TC_PRT("smartcard_session_open_basic_channel is failed : %d", ret);
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 16:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);

					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_close(channel);
						if (ret == SMARTCARD_ERROR_NONE)
							TC_PRT("smartcard_channel_close is success");
						else
							TC_PRT("smartcard_channel_close is failed : %d", ret);
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 17:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					bool is_basic = true;
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);

					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_is_basic_channel(channel, &is_basic);
						if (ret == SMARTCARD_ERROR_NONE && is_basic == false)
							TC_PRT("smartcard_channel_is_basic_channel is success");
						else
							TC_PRT("smartcard_channel_is_basic_channel is failed");
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 18:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					bool is_close = false;
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);
					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_close(channel);

						ret = smartcard_channel_is_closed(channel, &is_close);
						if (ret == SMARTCARD_ERROR_NONE && is_close == true)
							TC_PRT("smartcard_channel_is_closed is success");
						else
							TC_PRT("smartcard_channel_is_closed is failed");
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 19:
		{
			int pLength = 5;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					int i;
					unsigned char * pSelectResponse;

					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);

					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_get_select_response(channel, &pSelectResponse, &pLength);
						if (ret == SMARTCARD_ERROR_NONE) {
							TC_PRT("smartcard_channel_get_select_response is success");
							for (i = 0; i < pLength; i++)
								g_print("%x ", (int)pSelectResponse[i]);
						} else {
							TC_PRT("smartcard_channel_get_select_response is failed");
						}
					}

					if (pSelectResponse != NULL)
						free(pSelectResponse);
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 20:
		{
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					int session_handle;

					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);
					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_get_session(channel, &session_handle);

						if (ret == SMARTCARD_ERROR_NONE)
							TC_PRT("smartcard_channel_get_session is success: %d", session_handle);
						else
							TC_PRT("smartcard_channel_get_session is failed");
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 21:
		{
			int i = 0;
			int pLength;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};
			unsigned char command[] = { 0x00, 0x28, 0x00, 0x00 };
			unsigned char *response = NULL;
			int resp_len = 50;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);

					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_transmit(channel, command, 4, &response, &resp_len);
						if (ret == SMARTCARD_ERROR_NONE) {
							TC_PRT("smartcard_channel_transmit is success");

							g_print("response is ");
							for (i = 0; i < resp_len; i++)
								g_print("%x ", (int)response[i]);
							g_print("\n");
						} else {
							TC_PRT("smartcard_channel_transmit is failed");
						}
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 22:
		{
			int pLength = 5;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};
			unsigned char command[] = { 0x00, 0x28, 0x00, 0x00 };
			unsigned char *response = NULL;
			int resp_len = 50;

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					int i;
					unsigned char * ptransmitResponse;

					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);
					if (ret == SMARTCARD_ERROR_NONE) {
						ret = smartcard_channel_transmit(channel, command, 4, &response, &resp_len);
						ret = smartcard_channel_transmit_retrieve_response(channel, &ptransmitResponse, &pLength);

						if (ret == SMARTCARD_ERROR_NONE) {
							TC_PRT("smartcard_channel_transmit_get_response is success");

							g_print("response is ");
							for (i = 0; i < pLength; i++)
								g_print("%x ", (int)ptransmitResponse[i]);
							g_print("\n");
						} else {
							TC_PRT("smartcard_channel_transmit_get_response is failed");
						}
					}
				}

				ret = smartcard_session_close(session);
			}

			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	case 23:
		{
			int pLength = 5;
			int *phReaders = NULL;
			int session;
			int channel;
			unsigned char aid[] = {0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

			ret = smartcard_get_readers(&phReaders, &pLength);

			if (ret == SMARTCARD_ERROR_NONE && pLength != 0) {
				ret = smartcard_reader_open_session(phReaders[0], &session);

				if (ret == SMARTCARD_ERROR_NONE && session != 0) {
					ret = smartcard_session_open_logical_channel(session, aid, 12, 0x00, &channel);
					if (ret == SMARTCARD_ERROR_NONE) {
						bool is_next = true;
						ret = smartcard_channel_select_next(channel, &is_next);
						if (ret == SMARTCARD_ERROR_NONE && is_next == false)
							TC_PRT("smartcard_channel_select_next is success");
						else
							TC_PRT("smartcard_channel_select_next is failed");
					}
				}

				ret = smartcard_session_close(session);
			}
			if (phReaders != NULL)
				free(phReaders);
		}
		break;
	default:
		break;
	}

	return 0;
}

static gboolean key_event_cb(GIOChannel *chan,
		GIOCondition cond,
		gpointer data)
{
	char buf[BUFFER_LEN] = { 0 };

	gsize len = 0;
	long test_id;

	memset(buf, 0, sizeof(buf));

	if (g_io_channel_read_chars(chan, buf, sizeof(buf), &len, NULL) == G_IO_STATUS_ERROR)
		return FALSE;

	tc_usage_print();
	test_id = atoi(buf);

	if (test_id)
		g_idle_add(test_input_callback, (void *)test_id);

	return TRUE;
}


int main(int argc, char ** argv)
{
	GIOChannel *key_io;

	key_io = g_io_channel_unix_new(fileno(stdin));

	g_io_channel_set_encoding(key_io, NULL, NULL);
	g_io_channel_set_flags(key_io, G_IO_FLAG_NONBLOCK, NULL);

	g_io_add_watch(key_io, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
		key_event_cb, NULL);

	g_io_channel_unref(key_io);

	main_loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(main_loop);

	return 0;
}
