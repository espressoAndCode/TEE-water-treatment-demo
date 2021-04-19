/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <water_treatment_ta.h>

/*Water Treatment Sensor State Variables*/
int temp_val = 50;
int ph_val = 60;
int disinf_val = 70;
int path_val = 80;
// int sod_hydrox_flow_is_on = 0;
// int disinf_flow_is_on = 0;


//Getters 
int get_temp_val()
{
	return temp_val;
};

int get_ph_val()
{
	return ph_val;
};

int get_disinf_val()
{
	return disinf_val;
};

int get_path_val()
{
	return path_val;
};

// int get_sod_hydrox_flow_is_on()
// {
// 	return sod_hydrox_flow_is_on;
// };

// int get_disinf_flow_is_on()
// {
// 	return disinf_flow_is_on;
// };

// Setters
void set_temp_val(int val)
{
	temp_val = val;
};

void set_ph_val(int val)
{
	ph_val = val;
};

void set_disinf_val(int val)
{
	disinf_val = val;
};

void set_path_val(int val)
{
	path_val = val;
};

// void set_sod_hydrox_flow_is_on(val)
// {
// 	sod_hydrox_flow_is_on = val;
// };

// void set_disinf_flow_is_on(val)
// {
// 	disinf_flow_is_on = val;
// };


/* TEE resources */
struct test_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

void prepare_tee_session(struct test_ctx *ctx)
{
	TEEC_UUID uuid = TA_WATER_TREATMENT_UUID;
	uint32_t origin;
	TEEC_Result res;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
}

void terminate_tee_session(struct test_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}




/////////////////////////////////////
// WATER TREATMENT USERLAND FUNCTIONS

TEEC_Result turn_sodiumhydroxide_on(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_disinf_val();
	op.params[3].value.a = get_path_val();


	/*
	* TA_WATER_TREATMENT_CMD_SOD_HYDROX_ON is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn sodium hydroxide pump on.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_SOD_HYDROX_ON, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[2].value.a == 0 && op.params[3].value.a == 0){
		printf("Sodium hydroxide pump value is now %d\n", op.params[1].value.a);
	}else{
		printf("*** FAILURE ***");
	}
}

TEEC_Result turn_sodiumhydroxide_off(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_disinf_val();
	op.params[3].value.a = get_path_val();


	/*
	* TA_WATER_TREATMENT_CMD_SOD_HYDROX_ON is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn sodium hydroxide pump off.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_SOD_HYDROX_OFF, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[2].value.a == 0 && op.params[3].value.a == 0){
		printf("Sodium hydroxide pump value is now %d\n", op.params[1].value.a);
	}else{
		printf("*** FAILURE ***");
	}
}

TEEC_Result turn_disinfectant_on(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_disinf_val();
	op.params[3].value.a = get_path_val();


	/*
	* TA_WATER_TREATMENT_CMD_DISINFECTANT_ON is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn disinfectant pump on.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_DISINFECTANT_ON, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[3].value.a == 0){
		printf("Disinfectant pump value is now %d\n", op.params[2].value.a);
	}else{
		printf("*** FAILURE ***");
	}
}

TEEC_Result turn_disinfectant_off(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_disinf_val();
	op.params[3].value.a = get_path_val();


	/*
	* TA_WATER_TREATMENT_CMD_DISINFECTANT_OFF is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn disinfectant pump on.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_DISINFECTANT_OFF, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[3].value.a == 0){
		printf("Disinfectant pump value is now %d\n", op.params[2].value.a);
	}else{
		printf("*** FAILURE ***");
	}
}


/* Lambda function to pass in different functions */

int invoke_ta(TEEC_Result (*func)(struct test_ctx *ctx))
{
	struct test_ctx ctx;

	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);

	// increment_secure_counter(&ctx);
	(*func)(&ctx);

	printf("We're done, close and release TEE resources\n\n");
	terminate_tee_session(&ctx);

	return 0;
}

int main (void)
{

	printf("\nStarting water treatment TAI demo\n");
	invoke_ta(turn_sodiumhydroxide_on);
	invoke_ta(turn_sodiumhydroxide_off);
	invoke_ta(turn_disinfectant_on);
	invoke_ta(turn_disinfectant_off);
	printf("\nFinished water treatment TAI demo\n");


}