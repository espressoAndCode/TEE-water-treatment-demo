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

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <water_treatment_ta.h>

/* Static water treatment values */
/* Device physical boundaries (min / max) */
int temp_dev_min = -40;
int temp_dev_max = 160;
int ph_dev_min = 0;
int ph_dev_max = 14;
int acid_flow_dev_min = 0;
int acid_flow_dev_max = 10;
int sod_hydrox_flow_dev_min = 0;
int sod_hydrox_flow_dev_max = 10;

/* State variables for chemical solenoid valves */
int sod_hydrox_flow_is_on = 0;
int acid_flow_is_on = 0;

//Getters - there are no setters. Values set at compile time.
int get_temp_dev_min()
{
	return temp_dev_min;
};

int get_temp_dev_max()
{
	return temp_dev_max;
};

int get_ph_dev_min()
{
	return ph_dev_min;
};

int get_ph_dev_max()
{
	return ph_dev_max;
};

int get_acid_flow_dev_min()
{
	return acid_flow_dev_min;
};

int get_acid_flow_dev_max()
{
	return acid_flow_dev_max;
};

int get_sod_hydrox_flow_dev_min()
{
	return sod_hydrox_flow_dev_min;
};

int get_sod_hydrox_flow_dev_max()
{
	return sod_hydrox_flow_dev_max;
};

int get_sod_hydrox_flow()
{
	return sod_hydrox_flow_is_on;
};

int get_acid_flow()
{
	return acid_flow_is_on;
};

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called\n\n");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("\n\n***** Secure water treatment process started *****\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("\n***** Secure water treatment process ended *****\n\n");
}

int verify_safe_bounds(int temp, int ph, int acid_flow, int sh_flow)
{
	if(
		temp >= temp_dev_min && temp <= temp_dev_max && \
		ph >= ph_dev_min && ph <= ph_dev_max && \
		acid_flow >= acid_flow_dev_min && acid_flow <= acid_flow_dev_max && \
		sh_flow >= sod_hydrox_flow_dev_min && sh_flow <= sod_hydrox_flow_dev_max
	){
		return 1;
	}else{
		return 0;
	}
}

static TEE_Result sod_hydrox_on(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Temperature value:             %u from REE", params[0].value.a);
	IMSG("pH value:                      %u from REE", params[1].value.a);
	IMSG("Acid flow value:               %u from REE", params[2].value.a);
	IMSG("Sodium hydroxide flow value:   %u from REE", params[3].value.a);

	if (verify_safe_bounds(params[0].value.a, params[1].value.a, params[2].value.a, params[3].value.a)){
		if(params[0].value.a > 40 && params[1].value.a < 5 && params[2].value.a == 0 && params[3].value.a == 0){
			sod_hydrox_flow_is_on = 1;
			params[0].value.a = 0;	//temp
			params[1].value.a = 0;	//ph
			params[2].value.a = 0;	//acid flow
			params[3].value.a = get_sod_hydrox_flow();
			IMSG("\nSetting sodium hydroxide pump value to: %d\n", params[3].value.a);
		}else{
			IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
			IMSG("\n***** TAI Alert - Failed due to function arguments OOB *****\n\n");
			IMSG("Exit process with no action taken.\n");
		}
		
	}else{
		IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
		IMSG("\n***** TAI Alert - Failed due to device limits exceeded *****\n\n");
		IMSG("Exit process with no action taken.\n");
	}
	return TEE_SUCCESS;

}

static TEE_Result sod_hydrox_off(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Temperature value:             %u from REE", params[0].value.a);
	IMSG("pH value:                      %u from REE", params[1].value.a);
	IMSG("Acid flow value:               %u from REE", params[2].value.a);
	IMSG("Sodium hydroxide flow value:   %u from REE", params[3].value.a);

	if (verify_safe_bounds(params[0].value.a, params[1].value.a, params[2].value.a, params[3].value.a)){
		if(params[1].value.a >= 6 && params[3].value.a > 0){
			sod_hydrox_flow_is_on = 0;
			params[0].value.a = 0;
			params[1].value.a = 0;
			params[2].value.a = 0;
			params[3].value.a = get_sod_hydrox_flow();
			IMSG("\nSodium hydroxide pump value now: %d\n", params[1].value.a);
		}else{
			IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
			IMSG("\n***** TAI Alert - Failed due to function arguments OOB *****\n\n");
			IMSG("Exit process with no action taken.\n");
		}
		
	}else{
		IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
		IMSG("\n***** TAI Alert - Failed due to device limits exceeded *****\n\n");
		IMSG("Exit process with no action taken.\n");
	}

	return TEE_SUCCESS;
}

static TEE_Result acid_on(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Temperature value:             %u from REE", params[0].value.a);
	IMSG("pH value:                      %u from REE", params[1].value.a);
	IMSG("Acid flow value:               %u from REE", params[2].value.a);
	IMSG("Sodium hydroxide flow value:   %u from REE", params[3].value.a);

	if (verify_safe_bounds(params[0].value.a, params[1].value.a, params[2].value.a, params[3].value.a)){
		if(params[0].value.a < 90 && params[1].value.a > 9 && params[2].value.a == 0 && params[3].value.a == 0){
			acid_flow_is_on = 1;
			params[0].value.a = 0;
			params[1].value.a = 0;
			params[2].value.a = get_acid_flow();
			params[3].value.a = 0;
			IMSG("\nAcid pump value now: %d\n", params[2].value.a);
		}else{
			IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
			IMSG("\n***** TAI Alert - Failed due to function arguments OOB *****\n\n");
			IMSG("Exit process with no action taken.\n");
		}
		
	}else{
		IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
		IMSG("\n***** TAI Alert - Failed due to device limits exceeded *****\n\n");
		IMSG("Exit process with no action taken.\n");
	}

	return TEE_SUCCESS;
}

static TEE_Result acid_off(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Temperature value:             %u from REE", params[0].value.a);
	IMSG("pH value:                      %u from REE", params[1].value.a);
	IMSG("Acid flow value:               %u from REE", params[2].value.a);
	IMSG("Sodium hydroxide flow value:   %u from REE", params[3].value.a);

	if (verify_safe_bounds(params[0].value.a, params[1].value.a, params[2].value.a, params[3].value.a)){
		if(params[1].value.a <= 8 && params[2].value.a > 0){
			acid_flow_is_on = 0;
			params[0].value.a = 0;
			params[1].value.a = 0;
			params[2].value.a = get_acid_flow();
			params[3].value.a = 0;
			IMSG("\nAcid pump value now: %d\n", params[2].value.a);
		}else{
			IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
			IMSG("\n***** TAI Alert - Failed due to function arguments OOB *****\n\n");
			IMSG("Exit process with no action taken.\n");
		}
		
	}else{
		IMSG("\n***** TAI Alert - Forward Edge Failure *****\n");
		IMSG("\n***** TAI Alert - Failed due to device limits exceeded *****\n\n");
		IMSG("Exit process with no action taken.\n");
	}

	return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {

	case TA_WATER_TREATMENT_CMD_SOD_HYDROX_ON:
		return sod_hydrox_on(param_types, params);
	case TA_WATER_TREATMENT_CMD_SOD_HYDROX_OFF:
		return sod_hydrox_off(param_types, params);		
	case TA_WATER_TREATMENT_CMD_ACID_ON:
		return acid_on(param_types, params);
	case TA_WATER_TREATMENT_CMD_ACID_OFF:
		return acid_off(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
