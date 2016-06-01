#ifndef LUAGRAPHOBJECT_H
#define LUAGRAPHOBJECT_H

#include "Diluculum/LuaValue.hpp"

#include "QString"
#include "Q_LONG"

namespace Lua {

/**
 * @brief The LuaGraphObject class for common features of lodes, edges and incidences
 *
 * @author Frantisek Nagy
 * @date 19.5.2014
 */
class LuaGraphObject
{
public:
	/**
	 * @brief getId getter for id
	 * @return id
	 */
	qlonglong getId() const;

	/**
	 * @brief setId setter for id
	 * @param value ne id
	 */
	void setId( qlonglong value );

	/**
	 * @brief getLabel getter for label
	 * @return label
	 */
	QString getLabel() const;

	/**
	 * @brief setLabel setter for label
	 * @param value new label
	 */
	void setLabel( const QString& value );

	/**
	 * @brief getParams getter for params
	 * @return params
	 */
	Diluculum::LuaValue getParams() const;

	/**
	 * @brief setParams setter for params
	 * @param value new params
	 */
	void setParams( const Diluculum::LuaValue& value );

	/**
	 * @brief getFloatParam return float value from params table
	 * @param name key in the params table
	 * @param defaultValue value to be returned if key is not in the table
	 * @return value as float
	 */
	float getFloatParam( std::string name, float defaultValue );

	void setIdentifier( QString identifier )
	{
		this->identifier = identifier;
	}

	QString getIdentifier()
	{
		return this->identifier;
	}

private:
	qlonglong id;
	QString label;

	QString identifier;
	/**
	 * @brief params Lua table
	 */
	Diluculum::LuaValue params;
};

}

#endif // LUAGRAPHOBJECT_H
