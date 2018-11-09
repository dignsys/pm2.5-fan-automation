/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <service_app.h>
#include <stdio.h>

#include "st_things.h"
#include "log.h"

#define JSON_PATH "device_def.json"

static const char* RES_CAPABILITY_SWITCH_MAIN_0 = "/capability/switch/main/0";

/* get and set request handlers */
extern bool handle_get_request_on_resource_capability_switch_main_0(st_things_get_request_message_s *req_msg, st_things_representation_s *resp_rep);
extern bool handle_set_request_on_resource_capability_switch_main_0(st_things_set_request_message_s *req_msg, st_things_representation_s *resp_rep);

extern bool release_gpio(void);

/* callback functions */
bool handle_reset_request(void)
{
    DBG("Received a request for RESET.");

    return true;
}

void handle_reset_result(bool result)
{
    DBG("Reset %s.\n", result ? "succeeded" : "failed");
}

bool handle_ownership_transfer_request(void)
{
    DBG("Received a request for Ownership-transfer.");

    return true;
}

void handle_things_status_change(st_things_status_e things_status)
{
    DBG("Things status is changed: %d\n", things_status);
}

bool init_user() {
	FN_CALL;

	return true;
}


/* handle : for getting request on resources */
bool handle_get_request(st_things_get_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
	//DBG("resource_uri [%s]", req_msg->resource_uri);

    if (0 == strcmp(req_msg->resource_uri, RES_CAPABILITY_SWITCH_MAIN_0)) {
        return handle_get_request_on_resource_capability_switch_main_0(req_msg, resp_rep);
    }

    ERR("not supported uri");
    return false;
}

/* handle : for setting request on resources */
bool handle_set_request(st_things_set_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
	//DBG("resource_uri [%s]", req_msg->resource_uri);

    if (0 == strcmp(req_msg->resource_uri, RES_CAPABILITY_SWITCH_MAIN_0)) {
        return handle_set_request_on_resource_capability_switch_main_0(req_msg, resp_rep);
    }

    ERR("not supported uri");
    return false;
}

/* initialize */
void init_thing()
{
	FN_CALL;
	static bool binitialized = false;
	if (binitialized) {
		DBG("Already initialized!!");
		return;
	}

	bool easysetup_complete = false;

	char app_json_path[128] = {0,};
	char *app_res_path = NULL;
	char *app_data_path = NULL;

	app_res_path = app_get_resource_path();
	if (!app_res_path) {
		ERR("app_res_path is NULL!!");
		return;
	}

	app_data_path = app_get_data_path();
	if (!app_data_path) {
		ERR("app_data_path is NULL!!");
		free(app_res_path);
		return;
	}

	snprintf(app_json_path, sizeof(app_json_path), "%s/%s", app_res_path, JSON_PATH);

	if (0 != st_things_set_configuration_prefix_path((const char *)app_res_path, (const char *)app_data_path)) {
		ERR("st_things_set_configuration_prefix_path() failed!!");
		free(app_res_path);
		free(app_data_path);
		return;
	}

	free(app_res_path);
	free(app_data_path);

	if (0 != st_things_initialize(app_json_path, &easysetup_complete)) {
		ERR("st_things_initialize() failed!!");
		return;
	}

	binitialized = true;
	init_user();

	DBG("easysetup_complete:[%d] ", easysetup_complete);

	st_things_register_request_cb(handle_get_request, handle_set_request);
	st_things_register_reset_cb(handle_reset_request, handle_reset_result);
	st_things_register_user_confirm_cb(handle_ownership_transfer_request);
	st_things_register_things_status_change_cb(handle_things_status_change);

	st_things_start();

	FN_END;
}

static bool service_app_create(void *user_data)
{
	FN_CALL;

	return true;
}

static void service_app_terminate(void *user_data)
{
	FN_CALL;
	release_gpio();
}

static void service_app_control(app_control_h app_control, void *user_data)
{
	FN_CALL;
	if (app_control == NULL) {
		ERR("app_control is NULL");
		return;
	}

	init_thing();
}

int main(int argc, char *argv[])
{
	FN_CALL;

	service_app_lifecycle_callback_s event_callback;

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	return service_app_main(argc, argv, &event_callback, NULL);
}
