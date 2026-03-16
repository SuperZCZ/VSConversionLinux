/*!
 * FileName: EasyDomOpt.h
 *
 * Author:   ZhangChaoZe
 * Date:     2021-10-13
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: xml便捷函数(QDomDocument)
 */

#ifndef _EASY_DOM_OPT_H_
#define _EASY_DOM_OPT_H_

#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomElement>
#include <QDomNode>

namespace comm
{
	//************************************
	// Method:    getDomElementText
	// FullName:  getDomElementText
	// Access:    public 
	// Returns:   QT_NAMESPACE::QString
	// Qualifier: 获取指定element的text 
	// Parameter: const QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString attrKey  在符合element名称的所有 元素中找 第一个 attrKey==attrValue的
	// Parameter: const QString attr_value
	//************************************
	QString getDomElementText(const QDomNode &src_dom, const QString &ele_path, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    setDomElementText
	// FullName:  setDomElementText
	// Access:    public 
	// Returns:   QT_NAMESPACE::QString
	// Qualifier: 设置存在的指定节点的Text
	// Parameter: const QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString & text_value
	// Parameter: const QString & attrKey 在符合element名称的所有 元素中找 第一个 attrKey==attrValue的
	// Parameter: const QString & attr_value
	//************************************
	bool setDomElementText(QDomNode &src_dom, const QString &ele_path, const QString &text_value, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    getDomElementAttributeValue
	// FullName:  getDomElementAttributeValue
	// Access:    public 
	// Returns:   QT_NAMESPACE::QString
	// Qualifier: 获取指定元素的指定属性的属性值
	// Parameter: const QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString & attr_key
	// Parameter: const QString attrKey 在符合element名称的所有 元素中找 第一个 attrKey==attrValue的
	// Parameter: const QString attr_value
	//************************************
	QString getDomElementAttributeValue(const QDomNode &src_dom, const QString &ele_path, const QString &attr_key, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    setDomElementAttributeValue
	// FullName:  setDomElementAttributeValue
	// Access:    public 
	// Returns:   bool
	// Qualifier: 设置存在的指定元素的指定属性的值
	// Parameter: const QString & ele_path
	// Parameter: const QString & attr_key
	// Parameter: const QString & key_value
	// Parameter: const QString attrKey 在符合element名称的所有 元素中找 第一个 attrKey==attrValue的
	// Parameter: const QString attr_value
	//************************************
	bool setDomElementAttributeValue(QDomNode &src_dom, const QString &ele_path, const QString &attr_key, const QString &key_value, const QString &attrKey = "", const QString &attr_value = "");


	//************************************
	// Method:    getDomElement
	// FullName:  getDomElement
	// Access:    public 
	// Returns:   QT_NAMESPACE::QDomElement
	// Qualifier: 获取element  在获取的element上操作将不会影响src_dom
	// Parameter: const QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString attrKey
	// Parameter: const QString attr_value
	//************************************
	 QDomElement getDomElement(const QDomNode &src_dom, const QString &ele_path, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    getDomElement
	// FullName:  getDomElement
	// Access:    public 
	// Returns:   QT_NAMESPACE::QDomElement
	// Qualifier: 获取element  在获取的element上操作将会影响src_dom
	// Parameter: QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString attrKey
	// Parameter: const QString attr_value
	//************************************
	QDomElement getDomElement(QDomNode &src_dom, const QString &ele_path, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    setDomElement
	// FullName:  setDomElement
	// Access:    public 
	// Returns:   bool
	// Qualifier: 设置不存在的element
	// Parameter: QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QDomElement & dst_element
	// Parameter: bool useDstName
	// Parameter: const QString attrKey
	// Parameter: const QString attr_value
	//************************************
	bool setDomElement(QDomNode &src_dom, const QString &ele_path, const QDomElement &dst_element, bool useDstName = false, const QString &attrKey = "", const QString &attr_value = "");



	//************************************
	// Method:    removeDomElement
	// FullName:  removeDomElement
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString attrKey
	// Parameter: const QString attr_value
	//************************************
	bool removeDomElement(QDomNode &src_dom, const QString &ele_path, const QString &attrKey = "", const QString &attr_value = "");

	//************************************
	// Method:    domElementToString
	// FullName:  comm::domElementToString
	// Access:    public 
	// Returns:   QString
	// Qualifier: 将指定的element转换为QString 如果element为空则默认为firstChildElement
	// Parameter: const QDomNode & src_dom
	// Parameter: const QString & ele_path
	// Parameter: const QString & attrKey
	// Parameter: const QString & attr_value
	//************************************
	QString domElementToString(const QDomNode &src_dom, const QString &ele_path = "", const QString &attrKey = "", const QString &attr_value = "");

	bool loadDomDocument(QDomDocument &doc, const QString &path, QString &errMsg);
	bool loadDomDocument(QDomDocument &doc, const QString &path);

	bool saveDomDocument(const QDomDocument &doc, const QString &path, QString &errMsg, bool with_bom = false);
	bool saveDomDocument(const QDomDocument &doc, const QString &path, bool with_bom = false);
}
#endif