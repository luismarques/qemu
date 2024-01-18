/*
 * QEMU OpenTitan One Time Programmable (OTP) memory controller
 *
 * Copyright (c) 2023-2024 Rivos, Inc.
 *
 * Author(s):
 *  Emmanuel Blot <eblot@rivosinc.com>
 *  Loïc Lefort <loic@rivosinc.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HW_OPENTITAN_OT_OTP_H
#define HW_OPENTITAN_OT_OTP_H

#include "qom/object.h"
#include "hw/sysbus.h"

#define TYPE_OT_OTP "ot-otp"
OBJECT_DECLARE_TYPE(OtOTPState, OtOTPStateClass, OT_OTP)

/*
 * Hardware configuration (for HW_CFG partition)
 */
typedef struct {
    uint32_t device_id[8u];
    uint32_t manuf_state[8u];
    uint32_t soc_dbg_state; /* meaningless for Earlgrey platforms */
    /* the following value is stored as OT_MULTIBITBOOL8 */
    uint8_t en_sram_ifetch;
} OtOTPHWCfg;

typedef struct {
    /* the following values are stored as OT_MULTIBITBOOL8 */
    uint8_t en_csrng_sw_app_read;
    uint8_t en_entropy_src_fw_read;
    uint8_t en_entropy_src_fw_over;
} OtOTPEntropyCfg;

typedef enum {
    OTP_TOKEN_TEST_UNLOCK,
    OTP_TOKEN_TEST_EXIT,
    OTP_TOKEN_RMA,
    OTP_TOKEN_COUNT
} OtOTPToken;

typedef struct {
    uint64_t lo;
    uint64_t hi;
} OtOTPTokenValue;

typedef struct {
    OtOTPTokenValue values[OTP_TOKEN_COUNT];
    uint32_t valid_bm; /* OtLcCtrlToken-indexed valid bit flags */
} OtOTPTokens;

struct OtOTPState {
    SysBusDevice parent_obj;
};

typedef void (*ot_otp_program_ack_fn)(void *opaque, bool ack);

struct OtOTPStateClass {
    SysBusDeviceClass parent_class;

    /*
     * Provide OTP lifecycle information.
     *
     * @s the OTP device
     * @lc_state if not NULL, updated with the 5-bit encoded LifeCycle state
     * @tcount if not NULL, updated with the LifeCycle transition count
     * @lc_valid if not NULL, update with the LC valid state
     * @secret_valid if not NULL, update with the LC secret_valid info
     *
     * @note: lc_valid and secret_valid use OT_MULTIBITBOOL_LC4 encoding
     */
    void (*get_lc_info)(const OtOTPState *s, uint32_t *lc_state,
                        unsigned *tcount, uint8_t *lc_valid,
                        uint8_t *secret_valid, const OtOTPTokens **tokens);

    /*
     * Retrieve HW configuration.
     *
     * @s the OTP device
     * @return the HW config data (never NULL)
     */
    const OtOTPHWCfg *(*get_hw_cfg)(const OtOTPState *s);

    /*
     * Retrieve entropy configuration.
     *
     * @s the OTP device
     * @return the entropy config data (may be NULL if not present in OTP)
     */
    const OtOTPEntropyCfg *(*get_entropy_cfg)(const OtOTPState *s);

    /**
     * Request the OTP to program the state, transition count pair.
     * OTP only accepts one request at a time. If another program request is
     * on going, this function returns immediately and never invoke the
     * callback. Conversely, it should always invoke the callback if the request
     * is accepted.
     *
     * @s the OTP device
     * @lc_state the LifeCycle 5-bit state
     * @tcount the LifeCycle transition count
     * @ack the callback to asynchronously invoke on OTP completion/error
     * @opaque opaque data to forward to the ot_otp_program_ack_fn function
     * @return @c true if request is accepted, @c false is rejected.
     */
    bool (*program_req)(OtOTPState *s, uint32_t lc_state, unsigned tcount,
                        ot_otp_program_ack_fn ack, void *opaque);
};

#endif /* HW_OPENTITAN_OT_OTP_H */
