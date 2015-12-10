/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /**
  * @file smartcard_doc.h
  * @brief This file contains high level documentation the SMARTCARD API.
  */

 /**
  * @defgroup CAPI_NETWORK_SMARTCARD_MODULE Smartcard
  * @brief The SMARTCARD API provides application communication to SE applet method.
  * @ingroup CAPI_NETWORK_FRAMEWORK
  *
  * @section CAPI_NETWORK_SMARTCARD_MODULE_HEADER Required Header
  * \#include <smartcard.h>

  * @section CAPI_NETWORK_SMARTCARD_MODULE_OVERVIEW Overview
  * The SMARTCARD API provides function to communication to SE.
  *
  * - set up se service\n
  * - create reader from service\n
  * - open session from reader\n
  * - open channel from session\n
  * - transmit apdu data to channel and get response\n
  *
  * @subsection CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_DESCRIPTION SE Service
  * The SMARTCARD API provides functions to service framework\n
  * - get specification version. \n
  * - get readers \n
  *
  * @subsection CAPI_NETWORK_SMARTCARD_READER_MODULE_DESCRIPTION Reader
  *  The Reader api provides functions to about reader\n
  * - get reader name\n
  * - open / close session\n
  *
  * @subsection CAPI_NETWORK_SMARTCARD_SESSION_MODULE_DESCRIPTION Session
  * The Session api provide functions to about session \n
  * - open basic/logical channel\n
  * - close channels
  * - get atr (answer to reset)
  *
  * @subsection CAPI_NETWORK_SMARTCARD_CHANNEL_MODULE_DESCRIPTION Channel
  * The Channel api provide functions to about channel.
  * - close channel\n
  * - transmit apdu\n
  *
  */

 /**
  * @defgroup CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE SE Service
  * @brief The SE Service API provides functions to service framework.
  * @ingroup CAPI_NETWORK_SMARTCARD_MODULE
  *
  * @section CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_HEADER Required Header
  * \#include <smartcard.h>
  * @section CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_FEATURE Related Features
  * This API is related with the following features:\n
  * - http://tizen.org/feature/network.secure_element
  * - http://tizen.org/feature/network.secure_element.ese
  * - http://tizen.org/feature/network.secure_element.uicc
  * It is recommended to design feature related codes in your application for reliability.\n
  *
  * You can check if a devrice supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
  *
  * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
  *
  * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
  *
  */

 /**
  * @defgroup CAPI_NETWORK_SMARTCARD_READER_MODULE Reader
  * @brief The Reader API provides functions to reader.
  * @ingroup CAPI_NETWORK_SMARTCARD_MODULE
  *
  * @section CAPI_NETWORK_SMARTCARD_READER_MODULE_HEADER Required Header
  * \#include <smartcard.h>
  * @section CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_FEATURE Related Features
  * This API is related with the following features:\n
  * - http://tizen.org/feature/network.secure_element
  * - http://tizen.org/feature/network.secure_element.ese
  * - http://tizen.org/feature/network.secure_element.uicc
  * It is recommended to design feature related codes in your application for reliability.\n
  *
  * You can check if a devrice supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
  *
  * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
  *
  * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
  *
  */

 /**
  * @defgroup CAPI_NETWORK_SMARTCARD_SESSION_MODULE Session
  * @brief The Session API provides functions to session.
  * @ingroup CAPI_NETWORK_SMARTCARD_MODULE
  *
  * @section CAPI_NETWORK_SMARTCARD_SESSION_MODULE_HEADER Required Header
  * \#include <smartcard.h>
  * @section CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_FEATURE Related Features
  * This API is related with the following features:\n
  * - http://tizen.org/feature/network.secure_element
  * - http://tizen.org/feature/network.secure_element.ese
  * - http://tizen.org/feature/network.secure_element.uicc
  * It is recommended to design feature related codes in your application for reliability.\n
  *
  * You can check if a devrice supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
  *
  * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
  *
  * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
  *
  */

 /**
  * @defgroup CAPI_NETWORK_SMARTCARD_CHANNEL_MODULE Channel
  * @brief The Channel API provides functions to channel.
  * @ingroup CAPI_NETWORK_SMARTCARD_MODULE
  *
  * @section CAPI_NETWORK_SMARTCARD_CHANNEL_MODULE_HEADER Required Header
  * \#include <smartcard.h>
  * @section CAPI_NETWORK_SMARTCARD_SE_SERVICE_MODULE_FEATURE Related Features
  * This API is related with the following features:\n
  * - http://tizen.org/feature/network.secure_element
  * - http://tizen.org/feature/network.secure_element.ese
  * - http://tizen.org/feature/network.secure_element.uicc
  * It is recommended to design feature related codes in your application for reliability.\n
  *
  * You can check if a devrice supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
  *
  * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
  *
  * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
  *
  */

