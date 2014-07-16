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

#include "ScriptInterface.h"
#include <QDebug>
#include "VirtualTip.h"

ScriptInterface::ScriptInterface( void )
{
  // Save the original global object so we can still search for
  // objects, even if the global object is changed.
  m_global = m_scriptEngine.globalObject( );
}

ScriptInterface::~ScriptInterface()
{
	//Make sure that any lingering virtual marking
	//context is destroyed.
	VirtualTip::destroyOpenGLContext();
}

void ScriptInterface::PushThis( QString thisName )
{
  //  Find our object, then set it as 'this' in our new context
  QScriptValue thisObject = m_global.property( thisName );
  QScriptContext* context = m_scriptEngine.pushContext( );
  context->setThisObject( thisObject );
}

void ScriptInterface::PopThis( void )
{
  //  Pop the current context and restore the 'this' object
  m_scriptEngine.popContext( );
}

void ScriptInterface::RegisterStaticFunction( QString name , 
	QScriptEngine::FunctionSignature fun)
{
  m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( fun ) );
}

void ScriptInterface::RegisterExistingQObject(
	QString name, QObject* someObject)
{
	QScriptValue objectValue = m_scriptEngine.newQObject(someObject);
	m_scriptEngine.globalObject().setProperty(name,
		objectValue);
}

void ScriptInterface::RunScript(QString filename)
{
  QFile scriptFile(filename);
  if ( !scriptFile.open( QIODevice::ReadOnly ) )
  {
	qDebug() << filename << "did not open.";
	return;
  }
  QTextStream stream(&scriptFile);
  //  Not the most efficent way but it will work for now
  QString contents = stream.readAll();
  scriptFile.close();

  m_scriptEngine.evaluate(contents, filename);
  if( m_scriptEngine.hasUncaughtException( ) )
  {
	int line = m_scriptEngine.uncaughtExceptionLineNumber();
	QScriptValue exception = m_scriptEngine.uncaughtException();
	//TODO: Is this a good error message?
	qDebug() << "Uncaught exception at line: " << line << " " 
		<< exception.toString();
  }
}

void ScriptInterface::Run()
{
	RunScript(m_scriptFilename);
	emit scriptFinished();
}
