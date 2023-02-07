#include <napi.h>
#include <stdlib.h>
#include <windows.h>
#include <Lmcons.h>
#include <lmaccess.h>
#include <string>
#include <LM.h>
#include <algorithm>
#pragma comment(lib, "netapi32.lib")

Napi::Boolean CheckUser(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  
  bool userExists = false;
  
  std::string s1 = info[0].As<Napi::String>();
  const char* s = s1.c_str();
  std::size_t s_len = strlen(s);
  
  
   LPUSER_INFO_1 pBuf = NULL;
    LPUSER_INFO_1 pTmpBuf;
    DWORD dwLevel = 1;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
 
    do {
        nStatus = NetUserEnum((LPCWSTR) pszServerName,
            dwLevel,
            FILTER_NORMAL_ACCOUNT, // global users
            (LPBYTE*)&pBuf,
            dwPrefMaxLen,
            &dwEntriesRead,
            &dwTotalEntries,
            &dwResumeHandle);
 
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
            if ((pTmpBuf = pBuf) != NULL) {
                for (DWORD i = 0; i < dwEntriesRead; i++) {
                    if (pTmpBuf == NULL) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }
					std::wstring ws = pTmpBuf->usri1_name;
					userExists = userExists || 
					( ws.length() == s_len &&
					std::equal(ws.begin(), ws.end(), s));
					
                    pTmpBuf++;
                }
            }
        }
        else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
 
        if (pBuf != NULL) {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    while (nStatus == ERROR_MORE_DATA); // end do
 
    // Check again for allocated memory.
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
	
	return Napi::Boolean::New(env, userExists);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "checkUser"),
              Napi::Function::New(env, CheckUser));
  return exports;
}

NODE_API_MODULE(hello, Init)