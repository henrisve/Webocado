//#####################################################################
// Copyright 2012, Arvind Satyanarayan, Ranjitha Kumar.
// This software is governed by the license contained in LICENSE.
//#####################################################################
#ifndef _DOM_ELEMENT_H_
#define _DOM_ELEMENT_H_

#include <QString>
#include <QList>
#include <QMap>
#include <QRect>

#define TYPE_ELEMENT 1
#define TYPE_ATTRIBUTE 2
#define TYPE_TEXT 3
#define TYPE_COMMENT 8
#define TYPE_DOCUMENT 9

namespace bricolage {
    
//#####################################################################
// Class DOMElement
//#####################################################################
class DOMElement
{
    
public: 
    int mDOMID;
    QString mTagName;     
    DOMElement* mParent; 
    QList<DOMElement*> mChildren;
    QList<DOMElement*> mElementChildren;
    QRect mGeometry;
    QMap<QString, QString> mAttributes; // TODO: change to hash?
    QMap<QString, QString> mComputedStyles; // TODO: change to hash?
    int mNodeType; // 1:element; 2:attribute; 3:text; 8:comment; 9:document 
    QString mNodeValue; // element: undefined;

	DOMElement()
	:mParent(NULL)
	{}
	
	DOMElement(int domID, QString tagName, QMap<QString, QString> attrs, QMap<QString, QString> styles)
	:mDOMID(domID), mTagName(tagName), mParent(NULL), mGeometry(0,0,0,0), mAttributes(attrs), mComputedStyles(styles), mNodeType(1), mNodeValue("")
	{}
	
	~DOMElement()
	{}
    
    void setElementChildren(){
        mElementChildren.clear();
        foreach(DOMElement* child, mChildren)
            if(mNodeType==1) mElementChildren.append(child);
    }
    
//#####################################################################
};


//#####################################################################
} // namespace bricolage

#endif // _DOM_ELEMENT_H_
