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
#include <unistd.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <water_treatment_ta.h>

/*Water Treatment Sensor State Variables*/
/* Initial values */
int temp_val = 70;		//Fahrenheit
int ph_val = 3;			//0 - 14, 7 is normal
int sod_hydrox_flow = 0;	//0 - 10
int acid_flow = 0;		//0 - 10

//Getters 
int get_temp_val()
{
	return temp_val;
};

int get_ph_val()
{
	return ph_val;
};

int get_sod_hydrox_flow()
{
	return sod_hydrox_flow;
};

int get_acid_flow()
{
	return acid_flow;
};

// Setters
void set_temp_val(int val)
{
	temp_val = val;
};

void set_ph_val(int val)
{
	ph_val = val;
};

void set_sod_hydrox_flow(int val)
{
	sod_hydrox_flow = val;
};

void set_acid_flow(int val)
{
	acid_flow = val;
};

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
	op.params[2].value.a = get_sod_hydrox_flow();
	op.params[3].value.a = get_acid_flow();

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
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[2].value.a == 0){
		printf("Sodium hydroxide pump value is now %d\n", op.params[3].value.a);
	}else{
		printf("*** FAILURE ***\n");
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
	op.params[2].value.a = get_sod_hydrox_flow();
	op.params[3].value.a = get_acid_flow();

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
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[2].value.a == 0){
		printf("Sodium hydroxide pump value is now %d\n", op.params[3].value.a);
	}else{
		printf("*** FAILURE ***\n");
	}
}

TEEC_Result turn_acid_on(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_sod_hydrox_flow();
	op.params[3].value.a = get_acid_flow();

	/*
	* TA_WATER_TREATMENT_CMD_ACID_ON is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn acid pump on.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_ACID_ON, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[3].value.a == 0){
		printf("Acid pump value is now %d\n", op.params[2].value.a);
	}else{
		printf("*** FAILURE ***\n");
	}
}

TEEC_Result turn_acid_off(struct test_ctx *ctx)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
	op.params[0].value.a = get_temp_val();
	op.params[1].value.a = get_ph_val();
	op.params[2].value.a = get_sod_hydrox_flow();
	op.params[3].value.a = get_acid_flow();

	/*
	* TA_WATER_TREATMENT_CMD_ACID_OFF is the actual function in the TA to be
	* called.
	*/
	printf("Invoking TA to turn acid pump off.\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_WATER_TREATMENT_CMD_ACID_OFF, &op,
				&origin);
	if (res != TEEC_SUCCESS){
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, origin);
	}
	if (op.params[0].value.a == 0 && op.params[1].value.a == 0 && op.params[3].value.a == 0){
		printf("Acid pump value is now %d\n", op.params[2].value.a);
	}else{
		printf("*** FAILURE ***\n");
	}
}

void verify_safe_ph()
{
	if(get_ph_val() >= 6 && get_ph_val() <= 8){
		//Safe ph - do nothing
	}else{
		printf("***** TAI Alert - Backward Edge Failure *****\n");
		printf("***** TAI Alert - pH value unsafe. *****\n\n");
	}
	return;
}


/* Lambda function to pass in different requests to TEE */

int invoke_ta(TEEC_Result (*func)(struct test_ctx *ctx))
{
	struct test_ctx ctx;
	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);

	(*func)(&ctx);

	printf("We're done, close and release TEE resources\n\n\n\n");
	terminate_tee_session(&ctx);

	return 0;
}

const char* call_function(val){
	switch(val)
	{
		case 1:
			invoke_ta(turn_sodiumhydroxide_on);
			break;
		case 2:
			invoke_ta(turn_sodiumhydroxide_off);
			break;
		case 3:
			invoke_ta(turn_acid_on);
			break;
		case 4:
			invoke_ta(turn_acid_off);
			break;
		default:
			break;
	}
}

struct Test_vals{
int temp_val;
int ph_val;
int acid_flow;
int sod_hydrox_flow;
int func;
}; 

struct Test_vals2{
int temp_val;
int ph_val;
int acid_flow;
int sod_hydrox_flow;
int adj_ph_val;
int func;
}; 

struct Test_vals forward_test_vals[12] = {
	//Good tests
	{70,4,0,0,1},
	{70,7,1,0,2},
	{70,10,0,0,3},
	{70,7,0,1,4},
	//Device limits exceeded
	{-60,4,0,0,1},
	{70,25,1,0,2},
	{70,10,20,0,3},
	{70,7,0,20,4},
	//Function arguments OOB
	{20,4,0,0,1},
	{70,4,1,0,2},
	{70,10,1,0,3},
	{70,7,0,1,4},
};

struct Test_vals2 backward_test_vals[8] = {
	//Good tests
	{70,4,0,0,7,1},
	{70,7,1,0,7,2},
	{70,10,0,0,7,3},
	{70,7,0,1,7,4},
	//Backward edge failure
	{70,4,0,0,4,1},
	{70,7,1,0,10,2},
	{70,10,0,0,10,3},
	{70,7,0,1,4,4},
};

/******** MAIN FUNCTION *************************/
int main (void)
{

	printf("\nStarting water treatment TAI demo\n");

	printf("Forward edge tests\n\n");
	for (int i = 0; i < 12; i++){
		printf("Test case %d\n", i+1);
		set_temp_val(forward_test_vals[i].temp_val);
		set_ph_val(forward_test_vals[i].ph_val);
		set_acid_flow(forward_test_vals[i].acid_flow);
		set_sod_hydrox_flow(forward_test_vals[i].sod_hydrox_flow);
		call_function(forward_test_vals[i].func);
		sleep(3);
	}

	printf("Backward edge tests\n\n");
	for (int i = 0; i < 8; i++){
		printf("Test case %d\n", i+1);
		set_temp_val(backward_test_vals[i].temp_val);
		set_ph_val(backward_test_vals[i].ph_val);
		set_acid_flow(backward_test_vals[i].acid_flow);
		set_sod_hydrox_flow(backward_test_vals[i].sod_hydrox_flow);
		call_function(backward_test_vals[i].func);
		sleep(3);
		set_ph_val(backward_test_vals[i].adj_ph_val);
		verify_safe_ph();
		sleep(3);
	}

	printf("\nFinished water treatment TAI demo\n");
}

// optee_example_water_treatment