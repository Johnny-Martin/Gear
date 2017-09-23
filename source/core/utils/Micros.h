
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
*以下 5 个宏用于粗略检查pos表达式的合法性
*是否是4段
*是否有错误的字符
*pos表达式中，宽度、高度不支持#mid命令
*******************************************************************************/
#define R_ATTR_POS_LEFTEXP    "([0-9heigtwdm\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_TOPEXP     R_ATTR_POS_LEFTEXP
#define R_ATTR_POS_WIDTHEXP   "([0-9heigtwd\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_HEIGHTEXP  R_ATTR_POS_WIDTHEXP

#define R_CHECK_POS           "(" R_ATTR_POS_LEFTEXP "," R_ATTR_POS_TOPEXP "," R_ATTR_POS_WIDTHEXP "," R_ATTR_POS_HEIGHTEXP ")*"

/*******************************************************************************
*以下 7 个宏精用于确检查leftexp\topexp\widthexp\heightexp表达式的合法性
*命令是否拼写错误、是否有空格/回车
*pos表达式中，宽度、高度不支持#mid命令
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
*检查ID、布尔型、整形类的属性值
*******************************************************************************/
#define R_CHECK_ID			  "[1-9A-Za-z_\\.]+"
#define R_CHECK_BOOL		  "[01]"
#define R_CHECK_INT			  "[0-9]+"

/*******************************************************************************
*检查窗口的shadow、border、resize等四段类的属性
*******************************************************************************/
#define R_CHECK_WINDOW_ATTR	  "(([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*))"

/*******************************************************************************
*检查颜色的色值设置是否合法
*******************************************************************************/
#define R_CHECK_COLOR_VALUE	  "((([0-9a-fA-F]{6}){0,1})(([0-9a-fA-F]{8}){0,1}))"
#define HEX_CHANNEL "([0-9a-fA-F]{2})"
#define R_CHECK_COLOR_HEX_6 "(" HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL ")"
#define R_CHECK_COLOR_HEX_8 "(" HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL HEX_CHANNEL")"

#define TEN "([0-9\\s*]*)"
#define R_CHECK_COLOR_RGB	  "(" TEN "," TEN "," TEN ")"
#define R_CHECK_COLOR_RGBA	  "(" TEN "," TEN "," TEN "," TEN ")"