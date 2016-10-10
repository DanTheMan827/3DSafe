#include <stdbool.h>

#define OTP_OFFSET        0x24000000

#define SHA_PATH "0:/SHA.BIN"

extern bool OTPChecked;
extern bool validOTPFound;

typedef enum {
    OTP_LOCATION_DISK,
    OTP_LOCATION_MEMORY
} OTPLocation;

typedef enum {
	SHACheckResultNoFile,
	SHACheckResultValid,
	SHACheckResultInvalid
} SHACheckResult;

bool otpIsValid(char * path, OTPLocation location);
void sa9lhi(bool allowExit);
bool saveSHA();
SHACheckResult checkSHA();