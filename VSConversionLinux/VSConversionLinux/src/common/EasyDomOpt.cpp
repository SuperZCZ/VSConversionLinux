/*!
 * FileName: EasyDomOpt.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2021-10-13
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: xml便捷函数(QDomDocument)
 */

#include <QDebug>
#include <QTextStream>
#include <QFile>

#include "common/EasyDomOpt.h"

#define DOM_DOCUMENT_INDENT 2

namespace comm
{
	QDomElement getDomElement(const QDomNode &src_dom, const QString &ele_path, const QString &attrKey, const QString &attr_value)
	{
		QDomElement res;
		QStringList path_list = ele_path.split("/");

		if (src_dom.isNull())
		{
			return res;
		}

		QDomElement root;
		if (src_dom.isDocument())
		{
			root = src_dom.toDocument().documentElement();
		}
		else
		{
			root = src_dom.toElement();
		}


		if (root.isNull())
		{
			return res;
		}
		QDomElement element = root;

		if (path_list.size() > 0)
		{
			if (path_list[0] == element.tagName())
			{
				if (path_list.size() == 1)
				{
					res = element;
					return res;
				}
			}
			else
			{
				return res;
			}
		}
		else
		{
			return res;
		}
		for (int i = 1; i < path_list.size(); i++)
		{
			//qDebug() << "=======" << path_list[i];
			if (i == (path_list.size() - 1))
			{
				if (attrKey.isEmpty())
				{
					//不进行关键值的匹配 取第一个
					element = element.firstChildElement(path_list[i]);
				}
				else
				{
					//进行关键值的匹配
					QDomNodeList nodeList = element.elementsByTagName(path_list[i]);
					if (nodeList.isEmpty())
					{
						element = QDomElement();
					}
					else
					{
						int j = 0;
						for (j = 0; j < nodeList.size(); j++)
						{
							if (nodeList.at(j).parentNode() != element)
							{
								continue;
							}

							QString attr = nodeList.at(j).toElement().attribute(attrKey);

							if (attr == attr_value)
							{
								element = nodeList.at(j).toElement();
								break;
							}
						}
						if (j >= nodeList.size())
						{
							element = QDomElement();
						}
					}
				}
			}
			else
			{
				element = element.firstChildElement(path_list[i]);
			}

			if (element.isNull())
			{
				break;
			}
		}
		if (!element.isNull())
		{
			res = element;
		}
		return res;
	}

	QDomElement getDomElement(QDomNode &src_dom, const QString &ele_path, const QString &attrKey, const QString &attr_value)
	{
		QDomElement res;
		QStringList path_list = ele_path.split("/");

		if (src_dom.isNull())
		{
			return res;
		}
		QDomElement root;

		if (src_dom.isDocument())
		{
			root = src_dom.toDocument().documentElement();
		}
		else
		{
			root = src_dom.toElement();;
		}

		if (root.isNull())
		{
			return res;
		}
		QDomElement element = root;

		if (path_list.size() > 0)
		{
			if (path_list[0] == element.tagName())
			{
				if (path_list.size() == 1)
				{
					res = element;
					return res;
				}
			}
			else
			{
				return res;
			}
		}
		else
		{
			return res;
		}
		for (int i = 1; i < path_list.size(); i++)
		{
			//qDebug() << "=======" << path_list[i];
			if (i == (path_list.size() - 1))
			{
				if (attrKey.isEmpty())
				{
					//不进行关键值的匹配 取第一个
					element = element.firstChildElement(path_list[i]);
				}
				else
				{
					//进行关键值的匹配
					QDomNodeList nodeList = element.elementsByTagName(path_list[i]);
					if (nodeList.isEmpty())
					{
						element = QDomElement();
					}
					else
					{
						int j = 0;
						for (j = 0; j < nodeList.size(); j++)
						{
							if (nodeList.at(j).parentNode() != element)
							{
								continue;
							}

							QString attr = nodeList.at(j).toElement().attribute(attrKey);

							if (attr == attr_value)
							{
								element = nodeList.at(j).toElement();
								break;
							}
						}
						if (j >= nodeList.size())
						{
							element = QDomElement();
						}
					}
				}
			}
			else
			{
				element = element.firstChildElement(path_list[i]);
			}

			if (element.isNull())
			{
				break;
			}
		}
		if (!element.isNull())
		{
			res = element;
		}
		return res;
	}


	QString getDomElementText(const QDomNode &src_dom, const QString &ele_path, const QString &attrKey, const QString &attr_value)
	{
		QString res = "";

		QDomElement element = getDomElement(src_dom, ele_path, attrKey, attr_value);

		if (!element.isNull())
		{
			res = element.text();
		}
		return res;
	}

	bool setDomElementText(QDomNode &src_dom, const QString &ele_path, const QString &text_value, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		bool res = false;

		if (src_dom.ownerDocument().isNull())
		{
			return res;
		}

		QDomElement element = getDomElement(src_dom, ele_path, attrKey, attr_value);

		if (!element.isNull())
		{
			QDomNode text_node = element.firstChild();
			if (text_node.isNull())
			{
				QDomText dom_text = src_dom.ownerDocument().createTextNode(text_value);
				element.appendChild(dom_text);
				res = true;
			}
			else if (text_node.isText())
			{
				text_node.setNodeValue(text_value);
				res = true;
			}
		}
		return res;
	}

	QString getDomElementAttributeValue(const QDomNode &src_dom, const QString &ele_path, const QString &attr_key, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		QString res = "";

		QDomElement element = getDomElement(src_dom, ele_path, attrKey, attr_value);

		if (!element.isNull())
		{
			res = element.attribute(attr_key);
		}
		return res;
	}

	bool setDomElementAttributeValue(QDomNode &src_dom, const QString &ele_path, const QString &attr_key, const QString &key_value, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		bool res = false;

		QDomElement element = getDomElement(src_dom, ele_path, attrKey, attr_value);

		if (!element.isNull())
		{
			element.setAttribute(attr_key, key_value);
			res = true;
		}
		return res;
	}

	bool setDomElement(QDomNode &src_dom, const QString &ele_path, const QDomElement &dst_element, bool useDstName /*= false*/, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		bool res = false;
		QStringList path_list = ele_path.split("/");
		QDomDocument dom_tmp = src_dom.ownerDocument(); //获区整个树的Document 以便创建element
		QDomElement tmp_element;
		QDomElement root;
		if (src_dom.isDocument())
		{
			//qDebug() << "src is document";
			root = src_dom.toDocument().documentElement();
		}
		else
		{
			//qDebug() << "src is element";
			root = src_dom.toElement();
		}

		if (path_list.size() <= 0)
		{
			return false;
		}

		if (root.isNull())
		{
			//qDebug() << "root is null";
			if (!dst_element.isNull())
			{
				tmp_element = dom_tmp.createElement(path_list[0]);
				dom_tmp.appendChild(tmp_element);
				root = dom_tmp.firstChildElement();
			}
			else
			{
				//对于一个空的element 删除任何节点都 返回true 因为本来就不存在
				return true;
			}
		}
		else
		{
			if (root.tagName() != path_list[0])
			{
				return false;
			}
			else
			{
				if (path_list.size() == 1)
				{
					//置换起始节点
					if (dst_element.isNull())
					{
						root.parentNode().removeChild(root);
						return true;
					}
					else
					{
						QDomElement tmp_element = dst_element.cloneNode().toElement();
						if (!useDstName)
						{
							tmp_element.setTagName(root.tagName());
						}
						root.parentNode().replaceChild(tmp_element, root);
						return true;
					}
				}
			}
		}

		QDomElement element = root;

		for (int i = 1; i < path_list.size(); i++)
		{
			//qDebug() << "=======" << path_list[i];
			if (i == (path_list.size() - 1))
			{
				if (attrKey.isEmpty())
				{
					//不进行关键值的匹配 取第一个
					tmp_element = element.firstChildElement(path_list[i]);
				}
				else
				{
					//进行关键值的匹配
					QDomNodeList nodeList = element.elementsByTagName(path_list[i]);
					if (nodeList.isEmpty())
					{
						tmp_element = QDomElement();
					}
					else
					{
						int j = 0;
						for (j = 0; j < nodeList.size(); j++)
						{
							if (nodeList.at(j).parentNode() != element)
							{
								continue;
							}

							QString attr = nodeList.at(j).toElement().attribute(attrKey);

							if (attr == attr_value)
							{
								tmp_element = nodeList.at(j).toElement();
								break;
							}
						}
						if (j >= nodeList.size())
						{
							tmp_element = QDomElement();
						}
					}
				}
			}
			else
			{
				tmp_element = element.firstChildElement(path_list[i]);
			}

			if (tmp_element.isNull())
			{
				if (!dst_element.isNull())
				{
					tmp_element = dom_tmp.createElement(path_list[i]);
					element.appendChild(tmp_element);
					element = tmp_element;
					continue;
				}
				else
				{
					//需要被置空的 不存在直接返回true
					return true;
				}
			}
			else
			{
				element = tmp_element;
			}
		}

		if (!element.isNull())
		{
			QDomNode parent_Node = element.parentNode();
			if (dst_element.isNull())
			{
				parent_Node.removeChild(element);
			}
			else
			{
				QDomElement tmp_element = dst_element.cloneNode().toElement();
				if (!useDstName)
				{
					tmp_element.setTagName(element.tagName());
				}
				parent_Node.replaceChild(tmp_element, element);
			}
			res = true;
		}
		return res;
	}

	bool removeDomElement(QDomNode &src_dom, const QString &ele_path, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		return setDomElement(src_dom, ele_path, QDomElement(), false, attrKey, attr_value);
	}

	QString domElementToString(const QDomNode &src_dom, const QString &ele_path /*= ""*/, const QString &attrKey /*= ""*/, const QString &attr_value /*= ""*/)
	{
		QString res;
		QTextStream textStream(&res);
		if (ele_path.isEmpty())
		{
			src_dom.save(textStream, DOM_DOCUMENT_INDENT);
		}
		else
		{
			QDomElement element = getDomElement(src_dom, ele_path, attrKey, attr_value);
			if (!element.isNull())
			{
				element.save(textStream, DOM_DOCUMENT_INDENT);
			}
		}
		return res;
	}


	bool loadDomDocument(QDomDocument &doc, const QString &path, QString &errMsg)
	{
		bool res = false;
		doc.clear();
		errMsg.clear();
		QFile inFile(path);
		if (inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString msg;
			int errLine,errColumn;
			QTextStream textStream(&inFile);
			textStream.setCodec("UTF-8");
			res = doc.setContent(textStream.readAll(), &msg, &errLine, &errColumn);
			if (!res)
			{
				errMsg = QString("XML error: %1 Line:%2,Column:%3").arg(msg).arg(errLine).arg(errColumn);
			}
			inFile.close();
		}
		return res;
	}

	bool loadDomDocument(QDomDocument &doc, const QString &path)
	{
		QString errMsg;
		return loadDomDocument(doc, path, errMsg);
	}

	bool saveDomDocument(const QDomDocument &doc, const QString &path, QString &errMsg, bool with_bom /*= false*/)
	{
		errMsg.clear();
		QFile outFile(path);
		if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		{
			errMsg = QString("创建存储文件失败");
			return false;
		}
		QTextStream textStream(&outFile);
		textStream.setGenerateByteOrderMark(with_bom);
		doc.save(textStream, DOM_DOCUMENT_INDENT);
		outFile.close();
		return true;
	}

	bool saveDomDocument(const QDomDocument &doc, const QString &path, bool with_bom /*= false*/)
	{
		QString errMsg;
		return saveDomDocument(doc, path, errMsg, with_bom);
	}

}