// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

using namespace xbox::httpclient;
using namespace xbox::httpclient::log;


STDAPI 
HCMockCallCreate(
    _Out_ hc_mock_call_handle* call
    ) HC_NOEXCEPT
{
    return HCHttpCallCreate(call);
}

STDAPI 
HCMockAddMock(
    _In_ hc_mock_call_handle call,
    _In_opt_z_ const char* method,
    _In_opt_z_ const char* url,
    _In_reads_bytes_opt_(requestBodySize) const uint8_t* requestBodyBytes,
    _In_ uint32_t requestBodySize
    ) HC_NOEXCEPT
try 
{
    if (call == nullptr)
    {
        return E_INVALIDARG;
    }

    auto httpSingleton = get_http_singleton(true);
    if (nullptr == httpSingleton)
        return E_HC_NOT_INITIALISED;

    if (method != nullptr && url != nullptr)
    {
        HRESULT hr = HCHttpCallRequestSetUrl(call, method, url);
        if (hr != S_OK)
        {
            return hr;
        }
    }

    if (requestBodyBytes)
    {
        HRESULT hr = HCHttpCallRequestSetRequestBodyBytes(call, requestBodyBytes, requestBodySize);
        if (hr != S_OK)
        {
            return hr;
        }
    }

    std::lock_guard<std::mutex> guard(httpSingleton->m_mocksLock);
    httpSingleton->m_mocks.push_back(call);
    httpSingleton->m_mocksEnabled = true;
    return S_OK;
}
CATCH_RETURN()

STDAPI 
HCMockClearMocks() HC_NOEXCEPT
try 
{
    auto httpSingleton = get_http_singleton(true);
    if (nullptr == httpSingleton)
        return E_HC_NOT_INITIALISED;

    std::lock_guard<std::mutex> guard(httpSingleton->m_mocksLock);

    for (auto& mockCall : httpSingleton->m_mocks)
    {
        HCHttpCallCloseHandle(mockCall);
    }

    httpSingleton->m_mocks.clear();
    httpSingleton->m_mocksEnabled = false;
    return S_OK;
}
CATCH_RETURN()

STDAPI 
HCMockResponseSetResponseBodyBytes(
    _In_ hc_mock_call_handle call,
    _In_reads_bytes_(bodySize) const uint8_t* bodyBytes,
    _In_ uint32_t bodySize
    ) HC_NOEXCEPT
{
    return HCHttpCallResponseSetResponseBodyBytes(call, bodyBytes, bodySize);
}

STDAPI 
HCMockResponseSetStatusCode(
    _In_ hc_mock_call_handle call,
    _In_ uint32_t statusCode
    ) HC_NOEXCEPT
{
    return HCHttpCallResponseSetStatusCode(call, statusCode);
}

STDAPI 
HCMockResponseSetNetworkErrorCode(
    _In_ hc_mock_call_handle call,
    _In_ HRESULT networkErrorCode,
    _In_ uint32_t platformNetworkErrorCode
    ) HC_NOEXCEPT
{
    return HCHttpCallResponseSetNetworkErrorCode(call, networkErrorCode, platformNetworkErrorCode);
}

STDAPI 
HCMockResponseSetHeader(
    _In_ hc_mock_call_handle call,
    _In_z_ const char* headerName,
    _In_z_ const char* headerValue
    ) HC_NOEXCEPT
{
    return HCHttpCallResponseSetHeader(call, headerName, headerValue);
}

