
#pragma once

#define ADD_ATTR(attrName, defaultValue)	\
		m_attrMap.insert(pair<string, string>(attrName, defaultValue));

#define ADD_EVENT(attrName, defaultValue)	\
		m_eventMap.insert(pair<string, UIEvent*>(attrName, defaultValue));

#define ADD_ATTR_PATTERN(attrName, sPattern)\
		m_attrValuePatternMap.insert(pair<string, string>(attrName, sPattern));

#define ADD_ATTR_PARSER(attrName, sParser)	\
		m_attrValueParserMap.insert(pair<string, function<bool(const string&)> >(attrName, sParser));

/*******************************************************************************
*���� 5 �������ڴ��Լ��pos���ʽ�ĺϷ���
*�Ƿ���4��
*�Ƿ��д�����ַ�
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_ATTR_POS_LEFTEXP    "([0-9heigtwdm\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_TOPEXP     R_ATTR_POS_LEFTEXP
#define R_ATTR_POS_WIDTHEXP   "([0-9heigtwd\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_HEIGHTEXP  R_ATTR_POS_WIDTHEXP

#define R_CHECK_POS           "(" R_ATTR_POS_LEFTEXP "," R_ATTR_POS_TOPEXP "," R_ATTR_POS_WIDTHEXP "," R_ATTR_POS_HEIGHTEXP ")*"

/*******************************************************************************
*���� 7 ���꾫����ȷ���leftexp\topexp\widthexp\heightexp���ʽ�ĺϷ���
*�����Ƿ�ƴд�����Ƿ��пո�/�س�
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_MATH_EXP            "[0-9\\+\\-\\*\\(\\)/]*" 
#define R_ATTR_LEFTEXP_CMD    "((#mid)*(#height)*(#width)*)*"
#define R_ATTR_WIDTHEXP_CMD   "((#height)*(#width)*)*"

#define R_CHECK_LEFTEXP		  "(" R_MATH_EXP R_ATTR_LEFTEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_TOPEXP		  R_CHECK_LEFTEXP
#define R_CHECK_WIDTHEXP	  "(" R_MATH_EXP R_ATTR_WIDTHEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_HEIGHTEXP     R_CHECK_WIDTHEXP
#define R_CHECK_EVENT_FUNC    "([^=>]*)(=>)?([^=>]*)"

/*******************************************************************************
*���ID�������͡������������ֵ
*******************************************************************************/
#define R_CHECK_ID			  "[1-9A-Za-z_\\.]+"
#define R_CHECK_BOOL		  "[01]"
#define R_CHECK_INT			  "[0-9]+"

/*******************************************************************************
*��鴰�ڵ�shadow��border��resize���Ķ��������
*******************************************************************************/
#define R_CHECK_WINDOW_ATTR	  "(([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*))"

/*******************************************************************************
*�����ɫ��ɫֵ�����Ƿ�Ϸ�
*******************************************************************************/
#define R_CHECK_COLOR_VALUE	  "((([0-9a-fA-F]{6}){0,1})(([0-9a-fA-F]{8}){0,1}))"
#define HEX_CHANNEL "([0-9a-fA-F]{2})"
#define R_CHECK_COLOR_HEX_6 "(" HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL ")"
#define R_CHECK_COLOR_HEX_8 "(" HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL")"

#define TEN "([0-9\\s*]*)"
#define R_CHECK_COLOR_RGB	  "(" TEN "," TEN "," TEN ")"
#define R_CHECK_COLOR_RGBA	  "(" TEN "," TEN "," TEN "," TEN ")"