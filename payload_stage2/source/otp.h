#include <stdbool.h>

#define OTP_OFFSET        0x24000000

extern bool OTPChecked;
extern bool validOTPFound;

typedef enum {
    OTP_LOCATION_DISK,
    OTP_LOCATION_MEMORY
} OTPLocation;

bool otpIsValid(char * path, OTPLocation location);
void sa9lhi(bool allowExit);