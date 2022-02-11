/*
// Copyright (c) 2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#ifndef OC_SESSION_EVENTS_INTERNAL_H
#define OC_SESSION_EVENTS_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief handle sessions on an endpoint
 * 
 * @param endpoint the endpoint
 * @param state the session state
 */
void oc_handle_session(oc_endpoint_t *endpoint, oc_session_state_t state);

/**
 * @brief check if there is a session
 * 
 * @return true 
 * @return false 
 */
bool oc_session_events_is_ongoing(void);

#ifdef __cplusplus
}
#endif

#endif /* OC_SESSION_EVENTS_INTERNAL_H */
