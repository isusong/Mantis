/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 *
 * Authors: Nikolaus Karpinsky, Laura Ekstrand (ldmil@iastate.edu)
 */

/**
 @file
 @author		Nikolaus Karpinsky
 *
 * Laura Ekstrand, June 2013:
 * Modified for Mantis:  Removed C++ 2011 code, removed extra dependencies,
 * added qDebug error messages for errors in RunScript().
 * Added RegisterStaticFunction, to allow you to add an arbitrary factory
 * function.
 */

#ifndef _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
#define _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_

#include <QObject>
#include <QScriptEngine>
#include <QMetaType>
#include <QFile>
#include <QTextStream>

class ScriptInterface : public QObject
{
  Q_OBJECT

private:
  QScriptValue	m_global;

  QScriptEngine m_scriptEngine;

  QString m_scriptFilename; ///Holds the filename of the script.
  
public:
  ScriptInterface( void );
  virtual ~ScriptInterface();
  
  void PushThis( QString thisName );
  void PopThis( void );

  template <typename objectType>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType> ) );
  }

  template <typename objectType, typename arg0>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0> ) );
  }

  template <typename objectType, typename arg0, typename arg1>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0, arg1> ) );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0, arg1, arg2> ) );
  }

  template <typename objectType>
	void RegisterMetaObjectType( void )
  { qScriptRegisterMetaType( &m_scriptEngine, ScriptInterface::Object2ScriptValue<objectType>, ScriptInterface::ScriptValue2Object<objectType> ); }

  ///Allows you to call fun by name name in the script.
  void RegisterStaticFunction( QString name , 
	QScriptEngine::FunctionSignature fun);
  ///Allows you to manipulate existing someObject by name name in the script.
  void RegisterExistingQObject(QString name, QObject* someObject);

public slots:
  void	RunScript(QString filename);
  void Run(); ///< Runs the script with the filename m_scriptFilename.
  inline void setScriptFilename(QString filename)
	{m_scriptFilename = filename;}

private:
  template <typename objectType>
	static QScriptValue Object2ScriptValue(QScriptEngine* engine, objectType* const &in)
  { return engine->newQObject(in); }

  template <typename objectType>
	static void ScriptValue2Object(const QScriptValue &object, objectType* &out)
  { out = qobject_cast<objectType*>( object.toQObject( ) ); }

  template <typename objectType>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	return engine->newQObject( new objectType( ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
  	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	
	return engine->newQObject( new objectType( argument0 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0, typename arg1>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	arg1 argument1 = qscriptvalue_cast<arg1>( context->argument(1) );

	return engine->newQObject( new objectType( argument0, argument1 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	arg1 argument1 = qscriptvalue_cast<arg1>( context->argument(1) );
	arg2 argument2 = qscriptvalue_cast<arg2>( context->argument(2) );

	return engine->newQObject( new objectType( argument0, argument1, argument2 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

signals:
	void scriptFinished();
};
#endif	// _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
