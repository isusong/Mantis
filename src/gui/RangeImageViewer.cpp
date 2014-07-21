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
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#include "RangeImageViewer.h"
#include <QIcon>
#include <QColorDialog>

//=======================================================================
//=======================================================================
RangeImageViewer::RangeImageViewer(PRangeImage rangeImage, bool useViewTools, int flatDimension, int w, int h, QWidget* parent):
	QWidget(parent)
{
    _useViewTools = useViewTools;

    _width = w;
    _height = h;
    _mainGrid = new QGridLayout(this);
    _graphics = new GraphicsWidget(GraphicsWidget::None, this);
    _renderer = new RangeImageRenderer(rangeImage, this);

    _viewTools = NULL;
    _actionGroup = NULL;
    _shaded = NULL;
    _wireframe = NULL;
    _textured = NULL;
    _heightMapped = NULL;
    _signalMapper = NULL;
    _localCsys = NULL;
    _worldCsys = NULL;
    _background = NULL;

    if (_useViewTools)
    {
        _viewTools = new QToolBar(this);
        _actionGroup = new QActionGroup(_viewTools);
        _shaded = new QAction(this);
        _wireframe = new QAction(this);
        _textured = new QAction(this);
        _heightMapped = new QAction(this);
        _signalMapper = new QSignalMapper(this);
        _localCsys = new QAction(this);
        _worldCsys = new QAction(this);
        _background = new QAction(this);
    }

	//Adjust for flat dimension.
    setFlatDimension(flatDimension);

	initActions();
	makeConnections();
	assemble();

	//Some start up defaults.
    _graphics->setDrawCS(false);
    _renderer->setDrawCS(false);
    _renderer->setDrawMode(2); //start in textured.
    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
RangeImageViewer::~RangeImageViewer()
{
	//Everything is parented.
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setModel(PRangeImage ri)
{
    if (!_renderer) return;

    _renderer->setModel(ri);
    _graphics->setModel(_renderer); // need to refresh values, like bounding box
    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setWindowSelected(bool sel)
{
    if (!_graphics) return;

    _graphics->setWindowSelected(sel);
}

//=======================================================================
//=======================================================================
bool RangeImageViewer::getIsWindowSelected()
{
    if (!_graphics) return false;

    return _graphics->getWindowSelected();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::initActions()
{
    if (!_useViewTools) return;

	//Set up the actions.
	//Shaded button.
    _shaded->setCheckable(true);
    _shaded->setActionGroup(_actionGroup);
    _shaded->setIcon(QIcon(":/shading_modes/Icons/smooth.png"));
    _shaded->setToolTip(tr("Shaded view"));
    _signalMapper->setMapping(_shaded, 0);

	//Wireframe button
    _wireframe->setCheckable(true);
    _wireframe->setActionGroup(_actionGroup);
    _wireframe->setIcon(QIcon(":/shading_modes/Icons/wire.png"));
    _wireframe->setToolTip(tr("Wireframe view"));
    _signalMapper->setMapping(_wireframe, 1);
	
	//Textured button.
    _textured->setCheckable(true);
    _textured->setActionGroup(_actionGroup);
    _textured->setIcon(QIcon(":/shading_modes/Icons/textures.png"));
    _textured->setToolTip(tr("Textured view"));
    _signalMapper->setMapping(_textured, 2);
    _textured->setChecked(true);

	//Height map button.
    _heightMapped->setCheckable(true);
    _heightMapped->setActionGroup(_actionGroup);
    _heightMapped->setIcon(QIcon(":/shading_modes/Icons/color.png"));
    _heightMapped->setToolTip(tr("Color-mapped view"));
    _signalMapper->setMapping(_heightMapped, 3);

	//Local Csys button.
    _localCsys->setCheckable(true);
    _localCsys->setIcon(QIcon(":/controls/Icons/local-csys.png"));
    _localCsys->setToolTip(tr("Local coordinate system on/off"));
    _localCsys->setChecked(false);

	//World Csys button.
    _worldCsys->setCheckable(true);
    _worldCsys->setIcon(QIcon(":/controls/Icons/world-csys.png"));
    _worldCsys->setToolTip(tr("World coordinate system on/off"));
    _worldCsys->setChecked(false);

	//Background selection button
    _background->setIcon(QIcon(":/controls/Icons/color-wheel.png"));
    _background->setToolTip(tr("Change background color"));
}

//=======================================================================
//=======================================================================
void RangeImageViewer::makeConnections()
{
    if (_useViewTools)
    {
        connect(_shaded, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_wireframe, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_textured, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_heightMapped, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_localCsys, SIGNAL(toggled(bool)), _renderer, SLOT(setDrawCS(bool)));
        connect(_worldCsys, SIGNAL(toggled(bool)), _graphics, SLOT(setDrawCS(bool)));
        connect(_background, SIGNAL(triggered()), this, SLOT(selectBackgroundColor()));
        connect(_signalMapper, SIGNAL(mapped(int)), _renderer, SLOT(setDrawMode(int)));
    }

    connect(_renderer, SIGNAL(updateGL()), _graphics, SLOT(updateGL()));
    connect(_graphics, SIGNAL(doubleClicked(int, int)), this, SIGNAL(doubleClicked(int, int)));
    connect(_graphics, SIGNAL(onLButtonDown(int, int)), this, SLOT(onLButtonDownGraphicsSlot(int, int)));
}

//=======================================================================
//=======================================================================
void RangeImageViewer::assemble()
{
    _graphics->setModel(_renderer);
    _graphics->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    if (_useViewTools)
    {
        _viewTools->setOrientation(Qt::Vertical);
        _viewTools->addAction(_shaded);
        _viewTools->addAction(_wireframe);
        _viewTools->addAction(_textured);
        _viewTools->addAction(_heightMapped);
        _viewTools->addSeparator();
        _viewTools->addAction(_localCsys);
        _viewTools->addAction(_worldCsys);
        _viewTools->addSeparator();
        _viewTools->addAction(_background);
        _mainGrid->addWidget(_viewTools, 0, 0);
        _mainGrid->addWidget(_graphics, 0, 1);
    }
    else
    {
        setContentsMargins(0,0,0,0);
        _mainGrid->addWidget(_graphics, 0, 0);
        _mainGrid->setSpacing(0);
        _mainGrid->setContentsMargins(0,0,0,0);
    }

    setLayout(_mainGrid);
    resize(_width, _height);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::selectBackgroundColor()
{
    QColor selection = QColorDialog::getColor("black", this, tr("Select Background Color"));
    if (selection.isValid()) _graphics->setBackgroundColor(selection);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setFlatDimension(int flatDimension)
{
	//Adjust for flat dimension.
	switch (flatDimension)
	{
		case 0:
            _graphics->setMouseMultipliers(QVector3D(6, 1, 1));
			break;
		case 1:
            _graphics->setMouseMultipliers(QVector3D(1, 6, 1));
			break;
		case 2:
            _graphics->setMouseMultipliers(QVector3D(1, 1, 6));
	}
}

//=======================================================================
//=======================================================================
void RangeImageViewer::rotate(float xAxis, float yAxis, float zAxis)
{
    _renderer->setTransformMatrix(QMatrix4x4());
    _renderer->rotate(zAxis, QVector3D(0, 0, 1)); //z roll
    _renderer->rotate(yAxis, QVector3D(0, 1, 0)); //y yaw
    _renderer->rotate(xAxis, QVector3D(1, 0, 0)); //x pitch
    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape)
    {
        setWindowSelected(false);
    }
}

//=======================================================================
//=======================================================================
void RangeImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    emit onLButtonDown(event->pos().x(), event->pos().y());
}

//=======================================================================
//=======================================================================
void RangeImageViewer::onLButtonDownGraphicsSlot(int x, int y)
{
    emit onLButtonDown(x, y);
}
