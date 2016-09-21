//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _SEPARATOR_H_
#define _SEPARATOR_H_

#include <QRect>
#include <QSet>

namespace bricolage{

class BentoBlock;
	
//#####################################################################
// Class Separator
//#####################################################################
class Separator : public QRect
{
public:
	int mWeight;
	QSet<BentoBlock*> mBlocks;
	
public:
	Separator(int x=0, int y=0, int width=0, int height=0, int weight=0) 
	:QRect(x,y,width,height), mWeight(weight)
	{}
	
	~Separator() {}
	
	QSet<BentoBlock*> blocks() { return mBlocks; }

	static bool lessThanWeight(const Separator& lhs, const Separator& rhs) 
	{ return lhs.mWeight < rhs.mWeight; } 
	
	static bool greaterThanWeight(const Separator& lhs, const Separator& rhs) 
	{ return lhs.mWeight > rhs.mWeight; }
	
	static bool lessThanTop(const Separator& lhs, const Separator& rhs) 
	{ return lhs.top() < rhs.top(); } 
	
	static bool greaterThanTop(const Separator& lhs, const Separator& rhs) 
	{ return lhs.top() > rhs.top(); } 

	static bool lessThanBottom(const Separator& lhs, const Separator& rhs) 
	{ return lhs.bottom() < rhs.bottom(); } 
	
	static bool greaterThanBottom(const Separator& lhs, const Separator& rhs) 
	{ return lhs.bottom() > rhs.bottom(); } 
	
	static bool lessThanLeft(const Separator& lhs, const Separator& rhs) 
	{ return lhs.left() < rhs.left(); } 

	static bool greaterThanLeft(const Separator& lhs, const Separator& rhs) 
	{ return lhs.left() > rhs.left(); }

	static bool lessThanRight(const Separator& lhs, const Separator& rhs) 
	{ return lhs.right() < rhs.right(); } 
	
	static bool greaterThanRight(const Separator& lhs, const Separator& rhs) 
	{ return lhs.right() > rhs.right(); } 
		
	
//#####################################################################
};

//#####################################################################
} // namespace bricolage

#endif // _SEPARATOR_H_
