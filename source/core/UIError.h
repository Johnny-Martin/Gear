/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		¥ÌŒÛ∂®“Â
*****************************************************/

#pragma once

namespace Gear {
	namespace Res {
		enum RESERROR {
			RES_SUCCESS = 0,
			RES_ERROR_ILLEGAL_ID,
			RES_ERROR_FILE_NOT_FOUND,
			RES_ERROR_ILLEGAL_FILE_TYPE,
			RES_ERROR_PARSE_FILE_FALIED,
			RES_ERROR_ILLEGAL_PNG_FILE,
			RES_ERROR_CREATE_D2D1BITMAP_FAILED,
			RES_ERROR_UNKNOWN,
			RES_ERROR_COUNT
		};

	}//namespace Res

	namespace Xml {
		enum XMLERROR {
			//tinyxml2 XMLError
			XML_SUCCESS = 0,
			XML_TINYXML2_ERROR,

			//XmlAgent error
			XML_ERROR_ILLEGAL_ATTRIBUTE_NAME,
			XML_ERROR_ILLEGAL_FILE_ENCODE,
			XML_ERROR_UNKNOWN
		};
	}//namespace Xml
}//namespace Gear