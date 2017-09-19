#include "stdafx.h"
#include "../Micros.h"
#include "ResColor.h"

ResColor::ResColor()
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
bool ResColor::Init(const XMLElement* pElement)
{

	return true;
}
void ResColor::InitAttrMap()
{
	ADD_ATTR("value", "")
}
void ResColor::InitAttrValuePatternMap()
{

}
void ResColor::InitAttrValueParserMap()
{

}