//#####################################################################
// Copyright 2012, Ranjitha Kumar. All rights reserved.
// This software is governed by the BSD 2-Clause License.
//#####################################################################
#ifndef _DOM_UTILS_H_
#define _DOM_UTILS_H_

#include <QColor>
#include <QDebug>
#include <QRect>
#include <QRegExp>
#include <QSet>
#include <QStringList>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
//extern int g_test;

namespace bricolage{

//#####################################################################
// Class DOMUtils
//#####################################################################
class DOMUtils
{
	
public:
	static bool isInlineNode(const QWebElement& domNode)
	{ return !(isLineBreakNode(domNode)); }

	static bool isLineBreakNode(const QWebElement& domNode)
	{ return (domNode.styleProperty("display", QWebElement::ComputedStyle) == "block"); }

	static int numTextChildren(QWebElement& domNode)
	{ 
		int numTextNodes = 0;
		int numChildren = domNode.evaluateJavaScript("this.childNodes.length").toInt();
		for (int i=0; i<numChildren; i++) {
			QString jScript = "this.childNodes["+QString::number(i)+"].nodeType";
			if (domNode.evaluateJavaScript(jScript).toInt()==3) {
				jScript = "this.childNodes["+QString::number(i)+"].nodeValue";
				if (domNode.evaluateJavaScript(jScript).toString().trimmed()!="") numTextNodes++;
			}
		}
		return numTextNodes;
	}
	
	static bool hasOnlyTextChildren(QWebElement& domNode)
	{
		int numChildren = domNode.evaluateJavaScript("this.childNodes.length").toInt();
		if (numChildren == numTextChildren(domNode)) return true;
		else return false;
	}
	
	static QString textInNode(QWebElement& domNode)
	{
		QString nodeText = "";
		
		QVector<QWebElement> nodeStack; nodeStack.append(domNode);
		while(nodeStack.size()>0){
			QWebElement currNode = nodeStack.front(); nodeStack.pop_front();
			
			int numChildren = currNode.evaluateJavaScript("this.childNodes.length").toInt();
			for(int i=0; i<numChildren; i++) {
				QString jScript = "this.childNodes["+QString::number(i)+"].nodeType";
				if(currNode.evaluateJavaScript(jScript).toInt()==3) {
					jScript = "this.childNodes["+QString::number(i)+"].nodeValue";
					nodeText += currNode.evaluateJavaScript(jScript).toString().trimmed() + " ";
				}
			}
			
			QWebElement child = currNode.firstChild();
			while (!child.isNull()) { nodeStack.append(child); child = child.nextSibling(); }
		}
		
		return nodeText;
	}
	
	static bool isVirtualTextNode(QWebElement& domNode)
	{
		if (isInlineNode(domNode)) {
			if (hasOnlyTextChildren(domNode)) return true;
			QWebElement child = domNode.firstChild();
			while (!child.isNull()) { if (!isVirtualTextNode(child)) return false; child = child.nextSibling(); }
			return true;
		}
		return false;
	}
	
	static bool hasValidDimensions(QRect& bBox) 
	{
		return (bBox.width()>5 && bBox.height()>5 && bBox.width()*bBox.height()>100);
	}
	
	static bool isValidNode(QWebElement& domNode)
	{		
		if ((domNode.tagName() == "SCRIPT") || (domNode.tagName() == "STYLE"))
			return false;
		
		if (domNode.styleProperty("display", QWebElement::ComputedStyle) == "none" ||
			domNode.styleProperty("visibility", QWebElement::ComputedStyle) == "hidden" ||
			domNode.styleProperty("opacity", QWebElement::ComputedStyle).toFloat() == 0.0)
			return false;
		
		QRect bBox = getGeometry(domNode);
		if (bBox.left()+bBox.width()<0 || bBox.top()+bBox.height()<0 || !hasValidDimensions(bBox)) {
			QWebElement child = domNode.firstChild();
			while(!child.isNull()) { if(isValidNode(child)) return true; child = child.nextSibling(); }
			return false;
		}
		
		return true;
	}
	
	static bool isHiddenOverflow(QWebElement& domNode)
	{	
		QRect bBox = getGeometry(domNode);
		QWebElement parentNode = domNode.parent();
		while (!parentNode.isNull()) {
			while (!parentNode.isNull()) {
				if(parentNode.styleProperty("overflow", QWebElement::ComputedStyle) == "hidden") break;
				parentNode = parentNode.parent();
			}
			
			if (!parentNode.isNull()) {
				QRect parentBBox = getGeometry(parentNode);
				if(!parentBBox.intersects(bBox)) return true;
				parentNode = parentNode.parent();
			}
		}
		return false;
	}
	
	static QRect isPartiallyHidden(QWebElement& domNode, bool& result)
	{	
		QRect bBox = getGeometry(domNode);
		QWebElement parentNode = domNode.parent();
		while (!parentNode.isNull()) {
			while (!parentNode.isNull()) {
				if(parentNode.styleProperty("overflow", QWebElement::ComputedStyle) == "hidden") break;
				parentNode = parentNode.parent();
			}
			
			if (!parentNode.isNull()) {
				QRect parentBBox = getGeometry(parentNode);
				if(parentBBox.intersects(bBox)) {
					parentBBox.setRect(parentBBox.left()-1, parentBBox.top()-1, parentBBox.width()+2, parentBBox.height()+2);
					if (!parentBBox.contains(bBox)) { result=true; return parentBBox.intersected(bBox); }
				}
				parentNode = parentNode.parent();
			}
		}
		
		result = false; return QRect(0,0,0,0);
	}

	static int parsePixelFeature (const QString& pixelString)
	{
		int pixelFeature; bool okay;
		QString conversionString = pixelString;
		conversionString.remove(QRegExp("px"));

		pixelFeature = conversionString.toInt(&okay);
		if(!okay) return 0;
		return pixelFeature;
	}
	
	static QColor parseColorFeature(const QString& colorString){
            bool okay; QColor colorFeature;
            QString conversionString = colorString;
            conversionString.replace("transparent", "rgba(0,0,0,0)");
            conversionString.remove(QRegExp("[rgba]")).chop(1);
            conversionString=conversionString.mid(1);
            QStringList rgba = conversionString.split(",");

//		Q_ASSERT(rgba.size()==3 || rgba.size()==4);
            if(rgba.size()==3 || rgba.size()==4){
                int r = rgba[0].toInt(&okay); Q_ASSERT(okay);
                int g = rgba[1].toInt(&okay); Q_ASSERT(okay);
                int b = rgba[2].toInt(&okay); Q_ASSERT(okay);
                int a = 255;
                if(rgba.size()==4){
                    double te=rgba[3].toDouble();
                    if(te<1){
                        a=255*te;
                    }else{
                        a = rgba[3].toInt(&okay); Q_ASSERT(okay);
                    }
                    if(a<1){ //if totaly transparant
                        colorFeature.setRgb(255,0,0,0);
                        return colorFeature;
                    }
                }
                colorFeature.setRgb(r,g,b,a);
            }else{
                colorFeature.setRgb(0,0,0,0);
            }
            return colorFeature;

	}
        static QStringList parseColorList(const QString& colorString)
        {
                bool okay; QColor colorFeature;
                QString conversionString = colorString;
                conversionString.replace("transparent", "rgba(0,0,0,0)");
                conversionString.remove(QRegExp("[rgba]")).chop(1);
                conversionString=conversionString.mid(1);
                QStringList rgba = conversionString.split(",");


                if(rgba.size() < 3 || rgba[0]==0){
                    rgba << "N";
                    rgba[0] = "N";
                }else if(rgba.size() > 3 && rgba[3]=="0"){
                    rgba << "N";
                    rgba[0] = "N";
                }
                return rgba;
        }
        static QString qColor2RGBStr(const QColor color){
            int a,r,g,b;
            color.getRgb(&r,&g,&b,&a);
            QString out = "rgba(";
            out += QString::number(r);
            out += ',';
            out += QString::number(g);
            out += ',';
            out += QString::number(b);
            out += ',';
            out += QString::number(a);
            out += ')';
            return out;
        }
        static QColor buildColorList(const QString& colorString)
        {
                bool okay; QColor colorFeature;
                QString conversionString = colorString;
                conversionString.replace("transparent", "rgba(0,0,0,0)");
                conversionString.remove(QRegExp("[rgba]")).chop(1);
                conversionString=conversionString.mid(1);
                QStringList rgba = conversionString.split(",");

//		Q_ASSERT(rgba.size()==3 || rgba.size()==4);
                if(rgba.size()==3 || rgba.size()==4){
                    int r = rgba[0].toInt(&okay); Q_ASSERT(okay);
                    int g = rgba[1].toInt(&okay); Q_ASSERT(okay);
                    int b = rgba[2].toInt(&okay); Q_ASSERT(okay);
                    int a = 255;
                    if(rgba.size()==4)
                        a = rgba[3].toInt(&okay); Q_ASSERT(okay);
                    colorFeature.setRgb(r,g,b,a);
                }else{
                    colorFeature.setRgb(0,0,0,0);
                }
                return colorFeature;

        }
        static void rotateColor(QColor *color,int degree)
        {
                int a,h,s,l;
                color->getHsl(&h,&s,&l,&a);
                h+=degree;
                while(h>359){
                    h-=360;
                    qDebug() << h << endl;
                }
                color->setHsl(h,s,l,a);
        }


	static QRect getGeometry(QWebElement& domNode)
	{	
		QRect r = QRect(0,0,0,0);
                r.setX(domNode.geometry().left());
                r.setY(domNode.geometry().top());
		r.setWidth(domNode.evaluateJavaScript("parseInt(this.offsetWidth);").toInt());
		r.setHeight(domNode.evaluateJavaScript("parseInt(this.offsetHeight);").toInt());
		if (domNode.tagName()=="BODY") {
			r.setWidth(std::max(r.width(),domNode.webFrame()->contentsSize().width()));	
			r.setHeight(std::max(r.height(),domNode.webFrame()->contentsSize().height()));	
		}
		return r;
	}
	
	static void getPostOrderList(QList<QWebElement>& postOrderList, QWebElement& domNode)
	{
	    QWebElement domChild = domNode.firstChild();
        while (!domChild.isNull()) {
            getPostOrderList(postOrderList, domChild);
            domChild = domChild.nextSibling();
        }

        postOrderList.append(domNode);
	}
	
	static QVariantMap getAttributes(QWebElement& domNode)
	{
        QVariantMap attributes;
        QStringList attributeNames = domNode.attributeNames();
        foreach(QString attr, attributeNames)
            attributes[attr] = domNode.attribute(attr);
            
        return attributes;
	}
	
        static  QHash<QString, QString> getStyles(QWebElement& domNode){
            //QVariantMap
             QHash<QString, QString> styles;
            QStringList styleNames;
            QStringList subElements;
            subElements << "a"; //fill in with other


            styleNames << "direction" << "display" << "font" << "font-family" << "font-size" << "font-style" << "font-variant" <<
                          "font-weight"  <<"text-rendering"<< "background" <<  "background-attachment" << "background-clip" <<
                          "background-image" << "background-origin" << "background-position" << "background-position-x" << "background-position-y" <<
                          "background-repeat" << "background-repeat-x" << "background-repeat-y" << "background-size" << "border" <<
                          "border-bottom" <<  "border-bottom-left-radius" << "border-bottom-right-radius" <<
                          "border-bottom-style" << "border-bottom-width" << "border-collapse" <<  "border-image" <<
                          "border-image-outset" << "border-image-repeat" << "border-image-slice" << "border-image-source" << "border-image-width" <<
                          "border-left" <<  "border-left-style" << "border-left-width" << "border-radius" << "border-right" <<
                          "border-right-style" << "border-right-width" << "border-spacing" << "border-style" <<
                          "border-top" <<  "border-top-left-radius" << "border-top-right-radius" << "border-top-style" <<
                          "border-top-width" << "border-width" << "bottom" << "box-shadow" << "box-sizing" << "clear" << "clip" << "content" <<
                          "counter-increment" << "counter-reset" << "cursor" << "empty-cells" << "float" << "font-stretch" << "image-rendering" <<
                          "letter-spacing" << "list-style" << "list-style-image" << "list-style-position" << "list-style-type" << "margin" <<
                          "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "max-height" << "max-width" << "min-height" <<
                          "min-width" << "opacity" << "orphans" << "outline"  << "outline-offset" << "outline-style" <<
                          "outline-width" << "overflow" << "overflow-x" << "overflow-y" << "padding" << "padding-bottom" << "padding-left" <<
                          "padding-right" << "padding-top" << "page" << "page-break-after" << "page-break-before" << "page-break-inside" <<
                          "pointer-events" << "position" << "quotes" << "resize" << "size" << "src" << "speak" << "table-layout" << "tab-size" <<
                          "text-align" << "text-decoration" << "text-indent" << "text-line-through" <<
                          "text-line-through-mode" << "text-line-through-style" << "text-line-through-width" << "text-overflow" << "text-overline" <<
                          "text-overline-mode" << "text-overline-style" << "text-overline-width" << "text-shadow" <<
                          "text-transform" << "text-underline"  << "text-underline-mode" << "text-underline-style" <<
                          "text-underline-width" << "unicode-bidi" << "unicode-range" << "vertical-align" << "visibility" << "white-space" <<
                          "widows" << "word-break" << "word-spacing" << "word-wrap" << "z-index" << "left" << "height"<<"line-height"<<"right"<<"top" << "voice-family";

//            QStringList newStyle;
//            newStyle <<"margin" << "padding" << "background" << "color" << "text-align" << "font" << "voice-family" << "font-size" << "position" << "width" << "margin" << "text-align" << "border" << "border-width" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "background" << "text-indent" << "white-space" << "overflow" << "height" << "margin" << "background" << "border-width" << "position" << "top" << "right" << "width" << "margin" << "font-size" << "line-height" << "text-align" << "color" << "background-color" << "color" << "background-color" << "font-weight" << "text-decoration" << "color" << "background-color" << "font-weight" << "text-decoration" << "position" << "top" << "right" << "width" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "background-image" << "margin" << "font-size" << "font-style" << "line-height" << "color" << "background-color" << "border-width" << "border-width" << "margin" << "font-size" << "font-style" << "line-height" << "color" << "background-color" << "border-width" << "border-width" << "margin" << "font-size" << "font-style" << "line-height" << "color" << "background-color" << "margin" << "border" << "border-width" << "padding-bottom" << "border-bottom" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "border" << "border-width" << "background-color" << "background-image" << "border-top-width" << "margin" << "line-height" << "padding" << "border-width" << "border-width" << "border-width" << "margin" << "line-height" << "padding" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "border" << "border-width" << "background-color" << "background-image" << "margin" << "line-height" << "padding" << "border-width" << "border-width" << "color" << "background-color" << "font-weight" << "text-decoration" << "border-width" << "border-width" << "margin" << "line-height" << "padding" << "border-width" << "margin" << "line-height" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "color" << "background-color" << "font-weight" << "text-decoration" << "border-width" << "color" << "background-color" << "font-weight" << "text-decoration" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "border" << "border-width" << "background-color" << "background-image" << "margin" << "line-height" << "padding" << "border-width" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "border" << "border-width" << "background-color" << "background-image" << "margin" << "line-height" << "padding" << "border-width" << "border-width" << "border-width" << "border-width" << "margin" << "line-height" << "padding" << "border-width" << "margin" << "line-height" << "padding" << "margin" << "line-height" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "border-width" << "color" << "background-color" << "font-weight" << "text-decoration" << "margin" << "line-height" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "color" << "background-color" << "font-weight" << "text-decoration" << "color" << "background-color" << "font-weight" << "text-decoration" << "background" << "color" << "margin" << "padding" << "border" << "border-width" << "font" << "text-align" << "color" << "background-color" << "font-weight" << "text-decoration" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "padding" << "color" << "background-color" << "font-weight" << "text-decoration" << "padding" << "position" << "top" << "left" << "width" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "background-image" << "margin" << "padding" << "font-size" << "list-style" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "display" << "text-transform" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "background-image" << "margin" << "padding" << "font-size" << "list-style" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "margin" << "background-repeat" << "background-position" << "text-indent" << "white-space" << "overflow" << "height" << "background-image" << "margin" << "padding" << "font-size" << "list-style" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "border-width" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "border-width" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "border-width" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family" << "text-transform" << "background" << "margin" << "padding" << "line-height" << "color" << "background-color" << "font-weight" << "text-decoration" << "font-family";
//            newStyle.removeDuplicates();
//            newStyle.sort();
//            styleNames.sort();
//            foreach (QString str1, styleNames) {
//                qDebug() << " 1: " << str1 <<endl;
//            }
//            foreach (QString str1, newStyle) {
//                qDebug() << " 2: " << str1 <<endl;
//            }


//                        styleNames << "color" << "direction" << "display" << "font" << "font-family" << "font-size" << "font-style" << "font-variant" <<
//                                      "font-weight" << "text-rendering" << "background" << "background-attachment" << "background-clip" << "background-color" <<
//                                      "background-image" << "background-origin" << "background-position" << "background-position-x" << "background-position-y" <<
//                                      "background-repeat" << "background-repeat-x" << "background-repeat-y" << "background-size" << "border" <<
//                                      "border-bottom" << "border-bottom-color" << "border-bottom-left-radius" << "border-bottom-right-radius" <<
//                                      "border-bottom-style" << "border-bottom-width" << "border-collapse" << "border-color" << "border-image" <<
//                                      "border-image-outset" << "border-image-repeat" << "border-image-slice" << "border-image-source" << "border-image-width" <<
//                                      "border-left" << "border-left-color" << "border-left-style" << "border-left-width" << "border-radius" << "border-right" <<
//                                      "border-right-color" << "border-right-style" << "border-right-width" << "border-spacing" << "border-style" <<
//                                      "border-top" << "border-top-color" << "border-top-left-radius" << "border-top-right-radius" << "border-top-style" <<
//                                      "border-top-width" << "border-width" << "bottom" << "box-shadow" << "box-sizing" << "clear" << "clip" << "content" <<
//                                      "counter-increment" << "counter-reset" << "cursor" << "empty-cells" << "float" << "font-stretch" << "image-rendering" <<
//                                      "letter-spacing" << "list-style" << "list-style-image" << "list-style-position" << "list-style-type" << "margin" <<
//                                      "margin-bottom" << "margin-left" << "margin-right" << "margin-top" << "max-height" << "max-width" << "min-height" <<
//                                      "min-width" << "opacity" << "orphans" << "outline" << "outline-color" << "outline-offset" << "outline-style" <<
//                                      "outline-width" << "overflow" << "overflow-x" << "overflow-y" << "padding" << "padding-bottom" << "padding-left" <<
//                                      "padding-right" << "padding-top" << "page" << "page-break-after" << "page-break-before" << "page-break-inside" <<
//                                      "pointer-events" << "position" << "quotes" << "resize" << "size" << "src" << "speak" << "table-layout" << "tab-size" <<
//                                      "text-align" << "text-decoration" << "text-indent" << "text-line-through" << "text-line-through-color" <<
//                                      "text-line-through-mode" << "text-line-through-style" << "text-line-through-width" << "text-overflow" << "text-overline" <<
//                                      "text-overline-color" << "text-overline-mode" << "text-overline-style" << "text-overline-width" << "text-shadow" <<
//                                      "text-transform" << "text-underline" << "text-underline-color" << "text-underline-mode" << "text-underline-style" <<
//                                      "text-underline-width" << "unicode-bidi" << "unicode-range" << "vertical-align" << "visibility" << "white-space" <<
//                                      "widows" << "word-break" << "word-spacing" << "word-wrap" << "z-index" << "left" << "height"<<"line-height"<<"right"<<"top" << "voice-family";


            foreach(QString style, styleNames){
                QString s = domNode.styleProperty(style, QWebElement::ComputedStyle);
                if(s != "")
                    styles[style] = s;
            }
//            foreach(QString element, subElements){
//                foreach(QString style, styleNames){
//                    QString s = domNode.findFirst(element).styleProperty(style, QWebElement::ComputedStyle);
//                    if(s != "")
//                        styles[element + "_" + style] = s;
//                }
//            }
            
            return styles;
	}

	static int fontWeightToInt(QString conversionString)
	{
		int fontWeight; bool okay; 
		if (conversionString=="normal" || conversionString=="") conversionString = "400";
		else if (conversionString=="bold") conversionString = "700";

		fontWeight = conversionString.toInt(&okay);
        qDebug() << "fontWeight: " + QString::number(fontWeight);
        qDebug() << "conversionString: " + conversionString;
        qDebug() << "ok: " + QString::number(okay);
		Q_ASSERT(okay);
		return fontWeight;
	}
				
//#####################################################################
};
	
//#####################################################################
} // namespace bricolage

#endif // _DOM_UTILS_H_
