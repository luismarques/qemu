/* Generated from otp_ctrl_mmap.hjson with otptool.py */

/* this prevents linters from checking this file without its parent file */
#ifdef OT_OTP_EG_PARTS

/* clang-format off */
/* NOLINTBEGIN */
static const OtOTPPartDesc OtOTPPartDescs[] = {
    [OTP_PART_VENDOR_TEST] = {
        .size = 64u,
        .offset = 0u,
        .digest_offset = 56u,
        .hw_digest = false,
        .sw_digest = true,
        .secret = false,
        .buffered = false,
        .write_lock = false,
        .read_lock_csr = true,
        .read_lock = true,
    },
    [OTP_PART_CREATOR_SW_CFG] = {
        .size = 800u,
        .offset = 64u,
        .digest_offset = 856u,
        .hw_digest = false,
        .sw_digest = true,
        .secret = false,
        .buffered = false,
        .write_lock = false,
        .read_lock_csr = true,
        .read_lock = true,
    },
    [OTP_PART_OWNER_SW_CFG] = {
        .size = 792u,
        .offset = 864u,
        .digest_offset = 1648u,
        .hw_digest = false,
        .sw_digest = true,
        .secret = false,
        .buffered = false,
        .write_lock = false,
        .read_lock_csr = true,
        .read_lock = true,
    },
    [OTP_PART_HW_CFG] = {
        .size = 88u,
        .offset = 1656u,
        .digest_offset = 1736u,
        .hw_digest = true,
        .sw_digest = false,
        .secret = false,
        .buffered = true,
        .write_lock = false,
        .read_lock = false,
    },
    [OTP_PART_SECRET0] = {
        .size = 40u,
        .offset = 1744u,
        .digest_offset = 1776u,
        .hw_digest = true,
        .sw_digest = false,
        .secret = true,
        .buffered = true,
        .write_lock = false,
        .read_lock = true,
    },
    [OTP_PART_SECRET1] = {
        .size = 88u,
        .offset = 1784u,
        .digest_offset = 1864u,
        .hw_digest = true,
        .sw_digest = false,
        .secret = true,
        .buffered = true,
        .write_lock = false,
        .read_lock = true,
    },
    [OTP_PART_SECRET2] = {
        .size = 88u,
        .offset = 1872u,
        .digest_offset = 1952u,
        .hw_digest = true,
        .sw_digest = false,
        .secret = true,
        .buffered = true,
        .write_lock = false,
        .read_lock = true,
    },
    [OTP_PART_LIFE_CYCLE] = {
        .size = 88u,
        .offset = 1960u,
        .digest_offset = UINT16_MAX,
        .hw_digest = false,
        .sw_digest = false,
        .secret = false,
        .buffered = true,
        .write_lock = false,
        .read_lock = false,
    },
};

#define OTP_PART_COUNT ARRAY_SIZE(OtOTPPartDescs)

/* NOLINTEND */
/* clang-format on */

#endif /* OT_OTP_EG_PARTS */
